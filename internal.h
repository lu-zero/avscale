#ifndef INTERNAL_H
#define INTERNAL_H

#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define AVERROR(err) err

#define av_malloc(x) malloc(x)
#define av_free(x) free(x)

static inline void* av_mallocz(size_t size)
{
    void *ptr = av_malloc(size);
    if (ptr)
        memset(ptr, 0, size);
    return ptr;
}

static inline void av_freep(void *ptr)
{
    if (ptr) {
        void **arg = (void**)ptr;
        av_free(*arg);
        *arg = 0;
    }
}

#endif
