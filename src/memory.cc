#include <assert.h>
#include <malloc.h>
#include "memory.h"

void *_calloc(size_t member,size_t count)
{
	return calloc(member,count);
}

void *_malloc(size_t size)
{
	return malloc(size);
}

void _free(void* ptr)
{
	if(ptr)
		free(ptr);
	ptr = NULL;
}

void *_realloc(void *ptr,size_t size)
{
	return realloc(ptr,size);
}

