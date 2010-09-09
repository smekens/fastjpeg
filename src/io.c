/*--------------------------------------------------------------------------*/

#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "fastjpeg_internal.h"

/*--------------------------------------------------------------------------*/

fastjpeg_io_t *fastjpeg_io_new(
	fastjpeg_io_descr_t descr,
	fastjpeg_io_type_t type,
	void (* init) (fastjpeg_io_descr_t descr),
	long (* read) (fastjpeg_io_descr_t descr, uint8_t *buff, size_t size),
	bool (* skip) (fastjpeg_io_descr_t descr, size_t size),
	bool (* seek) (fastjpeg_io_descr_t descr, size_t size),
	void (* close) (fastjpeg_io_descr_t descr))
{
	fastjpeg_io_t *io = (fastjpeg_io_t *) fastjpeg_malloc(sizeof(fastjpeg_io_t));
	if(io == NULL) return NULL;

	io->type = type;

	io->descr = descr;
	io->init = init;
	io->read = read;
	io->skip = skip;
	io->seek = seek;
	io->close = close;

	return io;
}

/*--------------------------------------------------------------------------*/

void fastjpeg_io_delete(fastjpeg_io_t *io)
{
	fastjpeg_free(io);
}

/*--------------------------------------------------------------------------*/

static void posix_init(fastjpeg_io_descr_t descr)
{
	/* Nothing to do */
}

static long posix_read(fastjpeg_io_descr_t descr, uint8_t *buff, size_t size)
{
	return read((int) descr.data[0], buff, size);
}

static bool posix_skip(fastjpeg_io_descr_t descr, size_t size)
{
	return lseek((int) descr.data[0], size, SEEK_CUR) >= 0;
}

static bool posix_seek(fastjpeg_io_descr_t descr, size_t size)
{
	return lseek((int) descr.data[0], size, SEEK_SET) >= 0;
}

static void posix_close(fastjpeg_io_descr_t descr)
{
	/* Nothing to do */
}

/*--------------------------------------------------------------------------*/

void fastjpeg_posix_src(fastjpeg_t *fastjpeg, int fd)
{
	if(fastjpeg->io != NULL) fastjpeg_io_delete(fastjpeg->io);

	fastjpeg_io_descr_t descr = {{(long) fd, 0, 0, 0}};

	fastjpeg->io = fastjpeg_io_new(descr, FASTJPEG_IO_IN,
		posix_init, posix_read, posix_skip, posix_seek, posix_close);
}

/*--------------------------------------------------------------------------*/

static void stream_init(fastjpeg_io_descr_t descr)
{
	/* Nothing to do */
}

static long stream_read(fastjpeg_io_descr_t descr, uint8_t *buff, size_t size)
{
	return fread(buff, size, 1, (FILE *) descr.data[0]);
}

static bool stream_skip(fastjpeg_io_descr_t descr, size_t size)
{
	return fseek((FILE *) descr.data[0], size, SEEK_CUR) == 0 ;
}

static bool stream_seek(fastjpeg_io_descr_t descr, size_t size)
{
	return fseek((FILE *) descr.data[0], size, SEEK_SET) == 0;
}

static void stream_close(fastjpeg_io_descr_t descr)
{
	/* Nothing to do */
}

/*--------------------------------------------------------------------------*/

void fastjpeg_stream_src(fastjpeg_t *fastjpeg, FILE *fp)
{
	if(fastjpeg->io != NULL) fastjpeg_io_delete(fastjpeg->io);

	fastjpeg_io_descr_t descr = {{(long) fp, 0, 0, 0}};

	fastjpeg->io = fastjpeg_io_new(descr, FASTJPEG_IO_IN,
		stream_init, stream_read, stream_skip, stream_seek, stream_close);
}

/*--------------------------------------------------------------------------*/

static void buffer_init(fastjpeg_io_descr_t descr)
{
	descr.data[1] = 0;
}

static long buffer_read(fastjpeg_io_descr_t descr, uint8_t *buff, size_t size)
{
	memcpy(buff, (uint8_t *) descr.data[0] + descr.data[1], size);

	descr.data[1] += size;

	return size;
}

static bool buffer_skip(fastjpeg_io_descr_t descr, size_t size)
{
	descr.data[1] += size;

	return true;
}

static bool buffer_seek(fastjpeg_io_descr_t descr, size_t size)
{
	descr.data[1] = size;

	return true;
}

static void buffer_close(fastjpeg_io_descr_t descr)
{
	/* Nothing to do */
}

/*--------------------------------------------------------------------------*/

void fastjpeg_buffer_src(fastjpeg_t *fastjpeg, uint8_t *buff, size_t size)
{
	if(fastjpeg->io != NULL) fastjpeg_io_delete(fastjpeg->io);

	fastjpeg_io_descr_t descr = {{(long) buff, (long) size, 0, 0}};

	fastjpeg->io = fastjpeg_io_new(descr, FASTJPEG_IO_IN,
		buffer_init, buffer_read, buffer_skip, buffer_seek, buffer_close);
}

/*--------------------------------------------------------------------------*/

