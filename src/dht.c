/*--------------------------------------------------------------------------*/

#include <string.h>

#include "ctnr_list.h"
#include "fastjpeg_internal.h"

/*--------------------------------------------------------------------------*/

fastjpeg_dht_t *fastjpeg_dht_new(void)
{
	return (fastjpeg_dht_t *) fastjpeg_malloc_zero(sizeof(fastjpeg_dht_t));
}

/*--------------------------------------------------------------------------*/

fastjpeg_dht_t *fastjpeg_dht_extract(fastjpeg_dht_t *dht, uint8_t *buffer, size_t size)
{
	if(dht == NULL) dht = fastjpeg_dht_new();

	fastjpeg_huffman_table_t *table = fastjpeg_huffman_table_new();

	switch((buffer[0] >> 4) & 0x0F)
	{
		case 0:
			table->table_class = FASTJPEG_HUFFMAN_TABLE_CLASS_DC;
			break;

		case 1:
			table->table_class = FASTJPEG_HUFFMAN_TABLE_CLASS_AC;
			break;

		default:
			return NULL;
	}

	table->id = (buffer[0] >> 0) & 0x0F;

	memcpy(table->code_length_list, buffer + 1, 16);

	/* Calculate lengths and build huffman codes */
	int i, nb_code;
	for(i = 0, nb_code = 0; i < 16; i ++) {
		nb_code += table->code_length_list[i];
	}

	/* Check if table currupted */
	if(nb_code + 17 != size)
	{
		fastjpeg_huffman_table_delete(table);
		printf("Error: dht table corrupted\n");
		return dht;
	}

	table->lookup_size = nb_code;
	table->lookup = (fastjpeg_huffman_lookup_t *) fastjpeg_malloc(nb_code * sizeof(fastjpeg_huffman_lookup_t));

	size_t bitlen;
	size_t dht_idx = 0;
	size_t lookup_idx = 0;
	uint16_t code_val = 0;
	uint8_t *data_ptr = buffer + 17;

	for(bitlen = 0; bitlen < 16; bitlen++)
	{
		for(i = 0; i < table->code_length_list[bitlen]; i++)
		{
			table->lookup[lookup_idx].mask = 0xFFFF >> (16 - bitlen);
			table->lookup[lookup_idx].bits = code_val;
			table->lookup[lookup_idx].code = *(data_ptr + dht_idx);
			table->lookup[lookup_idx].bitlen = bitlen;

			lookup_idx++;
			code_val++;
			dht_idx++;
		}

		code_val <<= 1;
	}

	ctnr_list_add(dht->table_list, table);

	return dht;
}

/*--------------------------------------------------------------------------*/

void fastjpeg_dht_delete(fastjpeg_dht_t *dht)
{
	fastjpeg_huffman_table_t *table;

	while((table = ctnr_list_get_head(dht->table_list)) != NULL)
	{
		ctnr_list_del(dht->table_list, table);
		fastjpeg_huffman_table_delete(table);
	}

	fastjpeg_free(dht);
}

/*--------------------------------------------------------------------------*/

void fastjpeg_dht_dump(fastjpeg_dht_t *dht)
{
	int i, nr = 0;
	fastjpeg_huffman_table_t *table;

	printf("DUMP: ===== Describe Huffman Table\n");
	ctnr_list_foreach(dht->table_list, table, nr)
	{
		printf("DUMP:    id: %d\n", table->id);

		switch(table->table_class)
		{
			case FASTJPEG_HUFFMAN_TABLE_CLASS_DC:
				printf("DUMP:    class: DC\n");
				break;

			case FASTJPEG_HUFFMAN_TABLE_CLASS_AC:
				printf("DUMP:    class: AC\n");
				break;
		}

		printf("DUMP:    Lookup codes: (mask, bit, code, bitlen)\n");
		for(i = 0; i < table->lookup_size; i++)
		{
			printf("DUMP:      %4x, %4x, %4x, %4lx\n",
				table->lookup[i].mask, table->lookup[i].bits,
				table->lookup[i].code, table->lookup[i].bitlen);
		}

		printf("DUMP:\n");
	}
}


/*--------------------------------------------------------------------------*/

fastjpeg_huffman_table_t *fastjpeg_huffman_table_new(void)
{
	return (fastjpeg_huffman_table_t *) fastjpeg_malloc_zero(sizeof(fastjpeg_huffman_table_t));
}

void fastjpeg_huffman_table_delete(fastjpeg_huffman_table_t *table)
{
	if(table->lookup != NULL) fastjpeg_free(table->lookup);
	fastjpeg_free(table);
}

/*--------------------------------------------------------------------------*/

