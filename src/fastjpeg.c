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

	if(fastjpeg->jfif_header != NULL) fastjpeg_jfif_header_delete(fastjpeg->jfif_header);

	if(fastjpeg->dqt[0] != NULL) fastjpeg_dqt_delete(fastjpeg->dqt[0]);
	if(fastjpeg->dqt[1] != NULL) fastjpeg_dqt_delete(fastjpeg->dqt[1]);
	if(fastjpeg->dqt[2] != NULL) fastjpeg_dqt_delete(fastjpeg->dqt[2]);
	if(fastjpeg->dqt[3] != NULL) fastjpeg_dqt_delete(fastjpeg->dqt[3]);

	if(fastjpeg->sof != NULL) fastjpeg_sof_delete(fastjpeg->sof);

	if(fastjpeg->dht != NULL) fastjpeg_dht_delete(fastjpeg->dht);

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

	/* Read sections */
	int i;
	for(i = 0; ; i++)
	{
		uint16_t size;
		uint16_t marker;
		uint8_t *buffer;

		/* Read marker */
		if(fastjpeg_read_word(fastjpeg, &marker) == false) return false;

		printf("DUMP: --- Marker 0x%04x (%d)\n", marker, i);

		switch(marker)
		{
			case FASTJPEG_MARKER_SOI:
				fastjpeg->found_soi = true;
				break;

			case FASTJPEG_MARKER_EOI:
				fastjpeg->found_eoi = true;
				goto END_LOOP;
				break;

			case FASTJPEG_MARKER_APP0:
				if(fastjpeg_read_word(fastjpeg, &size) == false) return false;
				size -= 2;

				buffer = (uint8_t *) fastjpeg_malloc(size);
				if(fastjpeg_read_buffer(fastjpeg, buffer, size) == false) return false;
				fastjpeg->jfif_header = fastjpeg_jfif_header_extract(buffer, size);
				fastjpeg_free(buffer);
				break;

			case FASTJPEG_MARKER_DQT:
				if(fastjpeg_read_word(fastjpeg, &size) == false) return false;
				size -= 2;

				buffer = (uint8_t *) fastjpeg_malloc(size);
				if(fastjpeg_read_buffer(fastjpeg, buffer, size) == false) return false;
				fastjpeg_dqt_t *dqt = fastjpeg_dqt_extract(buffer, size);
				fastjpeg->dqt[dqt->id] = dqt;
				fastjpeg_free(buffer);
				break;

			case FASTJPEG_MARKER_SOF0:
				if(fastjpeg_read_word(fastjpeg, &size) == false) return false;
				size -= 2;

				buffer = (uint8_t *) fastjpeg_malloc(size);
				if(fastjpeg_read_buffer(fastjpeg, buffer, size) == false) return false;
				fastjpeg->sof = fastjpeg_sof_extract(buffer, size);
				fastjpeg_free(buffer);
				break;

			case FASTJPEG_MARKER_DHT:
				if(fastjpeg_read_word(fastjpeg, &size) == false) return false;
				size -= 2;

				buffer = (uint8_t *) fastjpeg_malloc(size);
				if(fastjpeg_read_buffer(fastjpeg, buffer, size) == false) return false;
				fastjpeg->dht = fastjpeg_dht_extract(fastjpeg->dht, buffer, size);
				fastjpeg_free(buffer);
				break;

			case FASTJPEG_MARKER_SOS:
				fastjpeg->found_sos = true;
				goto END_LOOP;

			default:
				printf("Error: marker not valid 0x%04x\n", marker);
				goto END_LOOP;
				return false;
		}
	}

END_LOOP:
	fastjpeg_dump(fastjpeg);

	return true;
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

bool fastjpeg_terminate(fastjpeg_t *fastjpeg)
{
	return false;
}

/*--------------------------------------------------------------------------*/

void fastjpeg_dump(fastjpeg_t *fastjpeg)
{
	int i;

	printf("DUMP:\n");
	if(fastjpeg->found_soi == true)
	{
		printf("DUMP: ===== SOI\n");
		printf("DUMP:\n");
	}

	fastjpeg_jfif_header_dump(fastjpeg->jfif_header);
	for(i = 0; i < 4; i++)
	{
		if(fastjpeg->dqt[i] != NULL) {
			fastjpeg_dqt_dump(fastjpeg->dqt[i]);
		}
	}

	fastjpeg_sof_dump(fastjpeg->sof);
	fastjpeg_dht_dump(fastjpeg->dht);

	if(fastjpeg->found_sos == true)
	{
		printf("DUMP: ===== SOS\n");
		printf("DUMP:\n");
	}

	if(fastjpeg->found_eoi == true)
	{
		printf("DUMP: ===== EOI\n");
		printf("DUMP:\n");
	}
}

/*--------------------------------------------------------------------------*/

