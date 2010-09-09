/*--------------------------------------------------------------------------*/

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "../include/fastjpeg.h"

/*--------------------------------------------------------------------------*/

int main(int argc, char **argv)
{
	if(argc != 2)
	{
		printf("No image given !\n");
		exit(1);
	}

	printf("Fast Jpeg Library\n");

	fastjpeg_t *fastjpeg = fastjpeg_new();

	FILE *fp = fopen(argv[1], "r");
	fastjpeg_stream_src(fastjpeg, fp);

	if(fastjpeg_prepare(fastjpeg) == false)
	{
		printf("File not valid !\n");
		fastjpeg_delete(fastjpeg);
		fclose(fp);
		exit(1);
	}

	uint8_t *buff = (uint8_t *) fastjpeg_malloc(fastjpeg->width * fastjpeg->bpp);

	int i;
	for(i = 0; i < fastjpeg->height; i++)
	{
		fastjpeg_read_scanline(fastjpeg, buff);
	}

	fastjpeg_terminate(fastjpeg);


	/* Cleaning */
	fastjpeg_free(buff);
	fastjpeg_delete(fastjpeg);
	fclose(fp);

	return 0;
}

/*--------------------------------------------------------------------------*/


