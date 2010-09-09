/*-------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include "fastjpeg_internal.h"

/*-------------------------------------------------------------------------*/

void fastjpeg_panic(const char *format, ...)
{
	va_list ap;

	printf("* PANIC *\n");

	va_start(ap, format);
	vfprintf(stderr, format, ap);
	va_end(ap);

	exit(1);
}

/*-------------------------------------------------------------------------*/

