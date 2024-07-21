#ifndef _MEMORY_H
#define _MEMORY_H

#include <malloc.h>

void *_calloc(size_t,size_t);
void *_malloc(size_t);
void _free(void*);
void *_realloc(void*,size_t);
char *_strdup(char *);

#endif
