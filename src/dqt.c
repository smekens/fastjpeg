/*--------------------------------------------------------------------------*/

#include <string.h>

#include "fastjpeg_internal.h"

/*--------------------------------------------------------------------------*/

fastjpeg_dqt_t *fastjpeg_dqt_new(void)
{
	return (fastjpeg_dqt_t *) fastjpeg_malloc_zero(sizeof(fastjpeg_dqt_t));
}

/*--------------------------------------------------------------------------*/

fastjpeg_dqt_t *fastjpeg_dqt_extract(uint8_t *buffer, size_t size)
{
	fastjpeg_dqt_t *result = fastjpeg_dqt_new();

	result->id   = (buffer[0] >> 0) & 0x0F;
	result->size = (buffer[0] >> 4) & 0x0F;

	switch(result->size)
	{
		case FASTJPEG_DQT_SIZE_BYTE:
			result->data.byte = fastjpeg_malloc(64 * sizeof(uint8_t));
			memcpy(result->data.byte, buffer + 1, 64 * sizeof(uint8_t));
			break;

		case FASTJPEG_DQT_SIZE_WORD:
			result->data.word = fastjpeg_malloc(64 * sizeof(uint16_t));
			memcpy(result->data.word, buffer + 1, 64 * sizeof(uint16_t));
			break;

		default:
			fastjpeg_panic("DQT size not valid\n");
	}

	return result;
}

/*--------------------------------------------------------------------------*/

void fastjpeg_dqt_delete(fastjpeg_dqt_t *dqt)
{
	switch(dqt->size)
	{
		case FASTJPEG_DQT_SIZE_BYTE:
			fastjpeg_free(dqt->data.byte);
			break;

		case FASTJPEG_DQT_SIZE_WORD:
			fastjpeg_free(dqt->data.word);
			break;
	}



	fastjpeg_free(dqt);
}

/*--------------------------------------------------------------------------*/

void fastjpeg_dqt_dump(fastjpeg_dqt_t *dqt)
{
	printf("DUMP: ===== Define Quantization Table\n");
	printf("DUMP:    id: %d\n", dqt->id);
	printf("DUMP:    size: %d\n", dqt->size);
	printf("DUMP:\n");

	int i, j;
	for(j = 0; j < 8; j++)
	{
		printf("DUMP:    ");

		for(i = 0; i < 8; i++)
		{
			switch(dqt->size)
			{
				case FASTJPEG_DQT_SIZE_BYTE:
					printf("%2X ", dqt->data.byte[j * 8 + i]);
					break;

				case FASTJPEG_DQT_SIZE_WORD:
					printf("%4X ", dqt->data.word[j * 8 + i]);
					break;
			}
		}

		printf("\n");
	}

	printf("DUMP:\n");
}

/*--------------------------------------------------------------------------*/

