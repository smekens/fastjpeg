/*--------------------------------------------------------------------------*/

#include <stdlib.h>

#include <fastjpeg/decompress.h>

/*--------------------------------------------------------------------------*/

fj_decompress_t *fj_decompress_new(void)
{
	fj_decompress_t *fj_decompress = (fj_decompress_t *) malloc(sizeof(fj_decompress_t));

	return fj_decompress;
}

/*--------------------------------------------------------------------------*/

void fj_decompress_delete(fj_decompress_t *fj_decompress)
{
	if(fj_decompress) {
		free(fj_decompress);
	}
}

/*--------------------------------------------------------------------------*/

