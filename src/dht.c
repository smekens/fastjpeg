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

	/* Calculate lengths */
	int i, length;
	for(i = 0, length = 0; i < 16; i ++) {
		length += table->code_length_list[i];
	}

	printf("---> %ld, %d\n", size, length);

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
		fastjpeg_free(table);
	}

	fastjpeg_free(dht);
}

/*--------------------------------------------------------------------------*/

void fastjpeg_dht_dump(fastjpeg_dht_t *dht)
{
	int nr = 0;
	fastjpeg_huffman_table_t *table;

	printf("DUMP: ===== Describe Huffman Table\n");
	ctnr_list_foreach(dht->table_list, table, nr)
	{
		printf("DUMP:    table class: %d\n", table->table_class);
		printf("DUMP:    id: %d\n", table->id);

		int i;
		printf("DUMP:    code length: ");
		for(i = 0; i < 16; i++) {
			printf("%03d ", table->code_length_list[i]);
		}
		printf("\n");
	}



/*
	printf("DUMP: ===== Start Of Frame\n");
	printf("DUMP:    precision: %d\n", dht->precision);
	printf("DUMP:    width: %d\n", dht->width);
	printf("DUMP:    height: %d\n", dht->height);
	printf("DUMP:    nb_component: %d\n", dht->nb_component);

	int i;
	for(i = 0; i < dht->nb_component; i++)
	{
		printf("DUMP:\n");
		printf("DUMP:    -- Component %d\n", dht->components[i].id);
		printf("DUMP:       h_sampling: %d\n", dht->components[i].h_sampling);
		printf("DUMP:       v_sampling: %d\n", dht->components[i].v_sampling);
		printf("DUMP:       dqt_id: %d\n", dht->components[i].dqt_id);
	}

	printf("DUMP:\n");
*/
}


/*--------------------------------------------------------------------------*/

fastjpeg_huffman_table_t *fastjpeg_huffman_table_new(void)
{
	return (fastjpeg_huffman_table_t *) fastjpeg_malloc_zero(sizeof(fastjpeg_huffman_table_t));
}

void fastjpeg_huffman_table_delete(fastjpeg_huffman_table_t *table)
{

}

/*--------------------------------------------------------------------------*/

