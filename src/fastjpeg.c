/*--------------------------------------------------------------------------*/

#include "fastjpeg_internal.h"

/*--------------------------------------------------------------------------*/

fastjpeg_t *fastjpeg_new(void)
{
	return (fastjpeg_t *) fastjpeg_malloc_zero(sizeof(fastjpeg_t));
}

/*--------------------------------------------------------------------------*/

void fastjpeg_delete(fastjpeg_t *fastjpeg)
{
	if(fastjpeg->io != NULL) fastjpeg_io_delete(fastjpeg->io);
	if(fastjpeg->marker != NULL) fastjpeg_marker_delete(fastjpeg->marker);

	fastjpeg_free(fastjpeg);
}

/*--------------------------------------------------------------------------*/

bool fastjpeg_read_byte(struct fastjpeg_s *fastjpeg, uint8_t *value)
{
	bool result;

	if(fastjpeg->io->read(fastjpeg->io->descr, value, 1) == 1) {
		result = true;
	}
	else {
		result = false;
	}

	return result;
}

/*--------------------------------------------------------------------------*/

bool fastjpeg_read_word(struct fastjpeg_s *fastjpeg, uint16_t *value)
{
	bool result;

	if(fastjpeg->io->read(fastjpeg->io->descr, (uint8_t *) value, 2) == 2)
	{
		*value = endian_word(*value);
		result = true;
	}
	else {
		result = false;
	}

	return result;
}

/*--------------------------------------------------------------------------*/

bool fastjpeg_read_dword(struct fastjpeg_s *fastjpeg, uint32_t *value)
{
	bool result;

	if(fastjpeg->io->read(fastjpeg->io->descr, (uint8_t *) value, 4) == 4)
	{
		*value = endian_dword(*value);
		result = true;
	}
	else {
		result = false;
	}

	return result;
}

/*--------------------------------------------------------------------------*/

bool fastjpeg_read_qword(struct fastjpeg_s *fastjpeg, uint64_t *value)
{
	bool result;

	if(fastjpeg->io->read(fastjpeg->io->descr, (uint8_t *) value, 8) == 8)
	{
		*value = endian_qword(*value);
		result = true;
	}
	else {
		result = false;
	}

	return result;
}

/*--------------------------------------------------------------------------*/

bool fastjpeg_read_buffer(struct fastjpeg_s *fastjpeg, uint8_t *buff, size_t size)
{
	bool result;

	if(fastjpeg->io->read(fastjpeg->io->descr, buff, size) == size) {
		result = true;
	}
	else {
		result = false;
	}

	return result;
}

/*--------------------------------------------------------------------------*/

bool fastjpeg_prepare_in(fastjpeg_t *fastjpeg)
{
	printf("Start preparing\n");

	/* Allocate memory for marker */
	if(fastjpeg->marker != NULL) fastjpeg_marker_delete(fastjpeg->marker);
	fastjpeg->marker = fastjpeg_marker_new();

	/* Read sections */
	int i;
	for(i = 0; ; i++)
	{
		uint16_t size;
		uint16_t marker;

		/* Read marker */
		if(fastjpeg_read_word(fastjpeg, &marker) == false) return false;

		printf("%02d, marker: 0x%04x\n", i, marker);

		switch(marker)
		{
			case FASTJPEG_MARKER_SOI:
				fastjpeg->marker->soi = true;
				printf("SOI ok \n");
				break;

			case FASTJPEG_MARKER_EOI:
				fastjpeg->marker->eoi = true;
				printf("EOI ok \n");
				goto END_LOOP;
				break;

			case FASTJPEG_MARKER_APP0:
				if(fastjpeg_read_word(fastjpeg, &size) == false) return false;
				size -= 2;

				printf("Reading APP0 section. Size: %d\n", size);

				fastjpeg->marker->data[0] = (uint8_t *) fastjpeg_malloc(size);
				if(fastjpeg->marker->data[0] == NULL) return false;
				if(fastjpeg_read_buffer(fastjpeg, fastjpeg->marker->data[0], size) == false) return false;
				break;

			case FASTJPEG_MARKER_DQT:
				if(fastjpeg_read_word(fastjpeg, &size) == false) return false;
				size -= 2;

				printf("Reading DQT section. Size: %d\n", size);

				fastjpeg->marker->data[1] = (uint8_t *) fastjpeg_malloc(size);
				if(fastjpeg->marker->data[1] == NULL) return false;
				if(fastjpeg_read_buffer(fastjpeg, fastjpeg->marker->data[1], size) == false) return false;
				break;

			case FASTJPEG_MARKER_SOF0:
				if(fastjpeg_read_word(fastjpeg, &size) == false) return false;
				size -= 2;

				printf("Reading SOF0 section. Size: %d\n", size);

				fastjpeg->marker->data[2] = (uint8_t *) fastjpeg_malloc(size);
				if(fastjpeg->marker->data[2] == NULL) return false;
				if(fastjpeg_read_buffer(fastjpeg, fastjpeg->marker->data[2], size) == false) return false;
				break;

			case FASTJPEG_MARKER_DHT:
				if(fastjpeg_read_word(fastjpeg, &size) == false) return false;
				size -= 2;

				printf("Reading DHT section. Size: %d\n", size);

				fastjpeg->marker->data[3] = (uint8_t *) fastjpeg_malloc(size);
				if(fastjpeg->marker->data[3] == NULL) return false;
				if(fastjpeg_read_buffer(fastjpeg, fastjpeg->marker->data[3], size) == false) return false;
				break;

			case FASTJPEG_MARKER_SOS:
				if(fastjpeg_read_word(fastjpeg, &size) == false) return false;
				size -= 2;

				printf("Reading SOS section. Size: %d\n", size);

				fastjpeg->marker->data[4] = (uint8_t *) fastjpeg_malloc(size);
				if(fastjpeg->marker->data[4] == NULL) return false;
				if(fastjpeg_read_buffer(fastjpeg, fastjpeg->marker->data[4], size) == false) return false;
				break;

//			default:
//				if(marker < 0xFFC0) return false;
//
//				if(fastjpeg_read_word(fastjpeg, &size) == false) return false;
//
//				buff = (uint8_t *) fastjpeg_malloc(size);
//				fastjpeg->marker->app_size[marker & 0x3F] = size;
//				fastjpeg->marker->app_data[marker & 0x3F] = buff;
//
//				break;

			default:
				printf("Error: marker not valid 0x%04x\n", marker);
				return false;
		}
	}

END_LOOP:
	return true;

//	/* Read JFIF header */
//	fastjpeg->io->read(fastjpeg->io->descr, (uint8_t *) &marker, sizeof(marker));
//	if(endian_word(marker) != FASTJPEG_MARKER_APP0) return false;
//	fastjpeg->io->read(fastjpeg->io->descr, (uint8_t *) &length, sizeof(length));
//
//	buff = (uint8_t *) fastjpeg_realloc(buff, length);
//	fastjpeg->io->read(fastjpeg->io->descr, buff, length);
//
//	fastjpeg_jfif_header_t *jfif_header = fastjpeg_jfif_header_extract(buff, length);
//	if(jfif_header == NULL) return false;
//
//	fastjpeg->jfif_header = jfif_header;
//
//	printf("Identifier: %c%c%c%c%c\n", jfif_header->id[0], jfif_header->id[1],
//		jfif_header->id[2], jfif_header->id[3], jfif_header->id[4]);
//	printf("Version: %d.%d\n", jfif_header->version_major, jfif_header->version_minor);
//	printf("Units: %d\n", jfif_header->units);
//	printf("Density: %d, %d\n", jfif_header->x_density, jfif_header->y_density);
//	printf("Thumb: %d, %d\n", jfif_header->x_thumbnail, jfif_header->y_thumbnail);
//
//	fastjpeg_free(buff);
//
//	return true;
}

/*--------------------------------------------------------------------------*/

bool fastjpeg_prepare_out(fastjpeg_t *fastjpeg)
{
	return true;
}

/*--------------------------------------------------------------------------*/

bool fastjpeg_prepare(fastjpeg_t *fastjpeg)
{
	bool result;

	if(fastjpeg->io == NULL) return false;

	/* Initialize io */
	fastjpeg->io->init(fastjpeg->io->descr);

	switch(fastjpeg->io->type)
	{
		case FASTJPEG_IO_IN:
			result = fastjpeg_prepare_in(fastjpeg);
			break;

		case FASTJPEG_IO_OUT:
			result = fastjpeg_prepare_out(fastjpeg);
			break;

		default:
			fastjpeg_panic("fastjpeg_prepare() - io type not valid !\n");
	}

	return result;
}

/*--------------------------------------------------------------------------*/

bool fastjpeg_terminate(struct fastjpeg_s *fastjpeg)
{
	return false;
}

/*--------------------------------------------------------------------------*/

