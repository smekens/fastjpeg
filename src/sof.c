/*--------------------------------------------------------------------------*/

#include <string.h>

#include "fastjpeg_internal.h"

/*--------------------------------------------------------------------------*/

fastjpeg_sof_t *fastjpeg_sof_new(void)
{
	return (fastjpeg_sof_t *) fastjpeg_malloc_zero(sizeof(fastjpeg_sof_t));
}

/*--------------------------------------------------------------------------*/

fastjpeg_sof_t *fastjpeg_sof_extract(uint8_t *buffer, size_t size)
{
	fastjpeg_sof_t *result = fastjpeg_sof_new();

	result->precision = *(buffer + 0);
	result->height = *((uint16_t *) (buffer + 1));
	result->width = *((uint16_t *) (buffer + 3));
	result->nb_component = *(buffer + 5);

	result->components = (fastjpeg_sof_component_t *)
		fastjpeg_malloc(result->nb_component * sizeof(fastjpeg_sof_component_t));

	int i;
	uint8_t *ptr = &buffer[6];

	for(i = 0; i < result->nb_component; i++)
	{
		result->components[i].id = *ptr++;

		uint8_t sampling = *ptr++;
		result->components[i].h_sampling = (sampling >> 4) & 0x0F;
		result->components[i].v_sampling = (sampling >> 0) & 0x0F;

		result->components[i].dqt_id = *ptr++;
	}

	return result;
}

/*--------------------------------------------------------------------------*/

void fastjpeg_sof_delete(fastjpeg_sof_t *sof)
{
	if(sof->components != NULL) fastjpeg_free((void *) sof->components);
	fastjpeg_free(sof);
}

/*--------------------------------------------------------------------------*/

void fastjpeg_sof_dump(fastjpeg_sof_t *sof)
{
	printf("DUMP: ===== Start Of Frame\n");
	printf("DUMP:    precision: %d\n", sof->precision);
	printf("DUMP:    width: %d\n", sof->width);
	printf("DUMP:    height: %d\n", sof->height);
	printf("DUMP:    nb_component: %d\n", sof->nb_component);

	int i;
	for(i = 0; i < sof->nb_component; i++)
	{
		printf("DUMP:\n");
		printf("DUMP:    -- Component %d\n", sof->components[i].id);
		printf("DUMP:       h_sampling: %d\n", sof->components[i].h_sampling);
		printf("DUMP:       v_sampling: %d\n", sof->components[i].v_sampling);
		printf("DUMP:       dqt_id: %d\n", sof->components[i].dqt_id);
	}

	printf("DUMP:\n");
}

/*--------------------------------------------------------------------------*/

