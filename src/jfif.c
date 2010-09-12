/*--------------------------------------------------------------------------*/

#include <string.h>

#include "fastjpeg_internal.h"

/*--------------------------------------------------------------------------*/

fastjpeg_jfif_header_t *fastjpeg_jfif_header_new(void)
{
	return (fastjpeg_jfif_header_t *) fastjpeg_malloc_zero(sizeof(fastjpeg_jfif_header_t));
}

/*--------------------------------------------------------------------------*/

struct fastjpeg_jfif_header_s *fastjpeg_jfif_header_extract(uint8_t *buff, size_t size)
{
	fastjpeg_jfif_header_t *jfif_header = fastjpeg_jfif_header_new();

	memcpy(jfif_header, buff, sizeof(fastjpeg_jfif_header_t));

	if(jfif_header->x_thumbnail == 0 || jfif_header->y_thumbnail == 0) {
		jfif_header->thumbnail = NULL;
	}
	else {
		size_t thumb_size = 3 * jfif_header->x_thumbnail * jfif_header->y_thumbnail;

		jfif_header->thumbnail = (uint8_t *) fastjpeg_malloc(thumb_size);
		memcpy(jfif_header->thumbnail,
			&((fastjpeg_jfif_header_t *) buff)->thumbnail[0], thumb_size);
	}

	return jfif_header;
}

/*--------------------------------------------------------------------------*/

void fastjpeg_jfif_header_delete(struct fastjpeg_jfif_header_s *jfif_header)
{
	if(jfif_header->thumbnail != NULL) fastjpeg_free(jfif_header->thumbnail);

	fastjpeg_free(jfif_header);
}

/*--------------------------------------------------------------------------*/

void fastjpeg_jfif_header_dump(fastjpeg_jfif_header_t *jfif_header)
{
	printf("DUMP: ===== JFIF Header\n");
	printf("DUMP:    id: %c%c%c%c%c\n", jfif_header->id[0], jfif_header->id[1],
		jfif_header->id[2], jfif_header->id[3], jfif_header->id[4]);
	printf("DUMP:    version: %d.%d\n", jfif_header->version_major, jfif_header->version_minor);
	printf("DUMP:    units: %d\n", jfif_header->units);
	printf("DUMP:    density: %d, %d\n", jfif_header->x_density, jfif_header->y_density);
	printf("DUMP:    thumbnail size: %dx%d\n", jfif_header->x_thumbnail, jfif_header->y_thumbnail);
	printf("DUMP:\n");
}

/*--------------------------------------------------------------------------*/

