/*--------------------------------------------------------------------------*/

#ifndef __FASTJPEG_H__
#define __FASTJPEG_H__

/*--------------------------------------------------------------------------*/

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/*--------------------------------------------------------------------------*/

#undef DLL_PUBLIC
#define DLL_PUBLIC

#undef DLL_HIDDEN
#define DLL_HIDDEN

/*--------------------------------------------------------------------------*/

struct fastjpeg_io_s;
struct fastjpeg_marker_s;

/*--------------------------------------------------------------------------*/

typedef struct fastjpeg_s
{
	size_t width;				/* Image with */
	size_t height;				/* Image height */
	size_t bpp;					/* Byte per pixel */

	struct fastjpeg_io_s *io;

	bool found_soi;
	bool found_eoi;

	struct fastjpeg_jfif_header_s *jfif_header;
	struct fastjpeg_dqt_s *dqt[4];

} fastjpeg_t;

/*--------------------------------------------------------------------------*/

fastjpeg_t *fastjpeg_new(void);
void fastjpeg_delete(fastjpeg_t *fastjpeg);

bool fastjpeg_prepare(struct fastjpeg_s *fastjpeg);
bool fastjpeg_terminate(struct fastjpeg_s *fastjpeg);

void fastjpeg_posix_src(struct fastjpeg_s *fastjpeg, int fd);
void fastjpeg_stream_src(struct fastjpeg_s *fastjpeg, FILE *fp);
void fastjpeg_buffer_src(struct fastjpeg_s *fastjpeg, uint8_t *buff, size_t size);

bool fastjpeg_read_scanline(struct fastjpeg_s *fastjpeg, uint8_t *buff);
bool fastjpeg_write_scanline(struct fastjpeg_s *fastjpeg, uint8_t *buff);

/*--------------------------------------------------------------------------*/
/* MEMORY																    */
/*--------------------------------------------------------------------------*/

#define buff_t void *

#define fastjpeg_malloc(x) \
		__fastjpeg_memory_malloc(x, __FILE__, __LINE__)
#define fastjpeg_malloc_zero(x) \
		__fastjpeg_memory_malloc_zero(x, __FILE__, __LINE__)

#define fastjpeg_realloc(x, y) \
		__fastjpeg_memory_realloc(x, y, __FILE__, __LINE__)
#define fastjpeg_realloc_zero(x, y) \
		__fastjpeg_memory_realloc_zero(x, y, __FILE__, __LINE__)

#define fastjpeg_free(x) \
		__fastjpeg_memory_free(x, __FILE__, __LINE__)

buff_t __fastjpeg_memory_malloc(size_t size, const char *, int);
buff_t __fastjpeg_memory_malloc_zero(size_t size, const char *, int);
buff_t __fastjpeg_memory_realloc(buff_t buff, size_t size, const char *, int);
buff_t __fastjpeg_memory_realloc_zero(buff_t buff, size_t size, const char *, int);
size_t __fastjpeg_memory_free(buff_t buff, const char *, int);

/*--------------------------------------------------------------------------*/

#endif /* __FASTJPEG_H__ */

/*--------------------------------------------------------------------------*/

