/*--------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef __HAS_PTHREAD
  #include <pthread.h>

  #define thread_mutex_lock(x) pthread_mutex_lock(x)
  #define thread_mutex_unlock(x) pthread_mutex_unlock(x)
#else
  #define thread_mutex_lock(x)
  #define thread_mutex_unlock(x)
#endif

#include "fastjpeg_internal.h"
#include "ctnr_list.h"

#undef malloc
#undef realloc
#undef free

/*-------------------------------------------------------------------------*/

#ifdef __HAS_PTHREAD
  static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
#endif

/*-------------------------------------------------------------------------*/

typedef struct fastjpeg_memory_leak_s
{
	size_t size;
	const char *file;
	int line;

	struct fastjpeg_memory_leak_s *prev, *next;

} fastjpeg_memory_leak_t;

/*-------------------------------------------------------------------------*/

static fastjpeg_memory_leak_t *fastjpeg_memory_leak_list = NULL;

/*-------------------------------------------------------------------------*/

#define CHECK(leak) \
		if((leak) == NULL)				\
		{						\
			fastjpeg_panic("Out of memory !\n");	\
		}						\

/*-------------------------------------------------------------------------*/

buff_t __fastjpeg_memory_malloc(size_t size, const char *file, int line)
{
	fastjpeg_memory_leak_t *leak;

	thread_mutex_lock(&mutex);

	/**/	CHECK(leak = (fastjpeg_memory_leak_t *) malloc(sizeof(fastjpeg_memory_leak_t) + size));
	/**/	ctnr_list_add(fastjpeg_memory_leak_list, leak);
	/**/
	/**/	leak->size = size;
	/**/	leak->file = file;
	/**/	leak->line = line;

	thread_mutex_unlock(&mutex);

	return leak + 1;
}

/*-------------------------------------------------------------------------*/

buff_t __fastjpeg_memory_malloc_zero(size_t size, const char *file, int line)
{
	buff_t result = __fastjpeg_memory_malloc(size, file, line);

	memset(result, 0x00, size);

	return result;
}

/*-------------------------------------------------------------------------*/

buff_t __fastjpeg_memory_realloc(buff_t buff, size_t size, const char *file, int line)
{
	if(buff == NULL) {
		return __fastjpeg_memory_malloc(size, file, line);
	}

	fastjpeg_memory_leak_t *leak = (fastjpeg_memory_leak_t *) buff - 1;

	thread_mutex_lock(&mutex);

	/**/	ctnr_list_del(fastjpeg_memory_leak_list, leak);
	/**/	CHECK(leak = (fastjpeg_memory_leak_t *) realloc(leak, sizeof(fastjpeg_memory_leak_t) + size));
	/**/	ctnr_list_add(fastjpeg_memory_leak_list, leak);
	/**/
	/**/	leak->size = size;
	/**/	leak->file = file;
	/**/	leak->line = line;

	thread_mutex_unlock(&mutex);

	return leak + 1;
}

/*-------------------------------------------------------------------------*/

buff_t __fastjpeg_memory_realloc_zero(buff_t buff, size_t size, const char *file, int line)
{
	buff_t result = __fastjpeg_memory_realloc(buff, size, file, line);

	memset(result, 0x00, size);

	return result;
}

/*-------------------------------------------------------------------------*/

size_t __fastjpeg_memory_free(buff_t buff, const char *file, int line)
{
	fastjpeg_memory_leak_t *leak = (fastjpeg_memory_leak_t *) buff - 1;

	size_t size = leak->size;

	thread_mutex_lock(&mutex);

	/**/	ctnr_list_del(fastjpeg_memory_leak_list, leak);
	/**/	free(leak);

	thread_mutex_unlock(&mutex);

	return size;
}

/*-------------------------------------------------------------------------*/

size_t fastjpeg_memory_get_size(buff_t buff) {
	return ((fastjpeg_memory_leak_t *) buff - 1)->size;
}

const char *fastjpeg_memory_get_file(buff_t buff) {
	return ((fastjpeg_memory_leak_t *) buff - 1)->file;
}

int fastjpeg_memory_get_line(buff_t buff) {
	return ((fastjpeg_memory_leak_t *) buff - 1)->line;
}

/*-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/

static void __attribute__ ((destructor)) __fastjpeg_memory_dtor(void)
{
	fastjpeg_memory_leak_t *leak;

	thread_mutex_lock(&mutex);

	/**/	if(fastjpeg_memory_leak_list != NULL)
	/**/	{
	/**/		fprintf(stderr, "Leak list:\n");
	/**/
	/**/		while((leak = ctnr_list_get_head(fastjpeg_memory_leak_list)) != NULL)
	/**/		{
	/**/			fprintf(stderr, " -> size = '%lu byte(s)', file = '%s', line = '%d'\n", leak->size, leak->file, leak->line);
	/**/			ctnr_list_del(fastjpeg_memory_leak_list, leak);
	/**/			free(leak);
	/**/		}
	/**/	}

	thread_mutex_unlock(&mutex);
}

/*-------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

