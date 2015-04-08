#ifndef INTERNAL_H
#define INTERNAL_H

#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define HAVE_BIGENDIAN 0

#define AVERROR(err) -(err)

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

#define AV_RL16(src) (((uint8_t*)(src))[0] | (((uint8_t*)(src))[1] << 8))
#define AV_RB16(src) (((uint8_t*)(src))[1] | (((uint8_t*)(src))[0] << 8))
#define AV_RL32(src) ( ((uint8_t*)(src))[0]        | \
                      (((uint8_t*)(src))[1] << 8)  | \
                      (((uint8_t*)(src))[2] << 16) | \
                      (((uint8_t*)(src))[3] << 24))
#define AV_RB32(src) ( ((uint8_t*)(src))[3]        | \
                      (((uint8_t*)(src))[2] << 8)  | \
                      (((uint8_t*)(src))[1] << 16) | \
                      (((uint8_t*)(src))[0] << 24))

#if HAVE_BIGENDIAN
#define AV_RN16 AV_RB16
#define AV_RN32 AV_RB32
#else
#define AV_RN16 AV_RL16
#define AV_RN32 AV_RL32
#endif

static inline void AV_WL16(uint8_t *dst, unsigned val)
{
    *dst++ = val;
    *dst++ = val >> 8;
}
static inline void AV_WB16(uint8_t *dst, unsigned val)
{
    *dst++ = val >> 8;
    *dst++ = val;
}
static inline void AV_WL32(uint8_t *dst, unsigned val)
{
    *dst++ = val;
    *dst++ = val >> 8;
    *dst++ = val >> 16;
    *dst++ = val >> 24;
}
static inline void AV_WB32(uint8_t *dst, unsigned val)
{
    *dst++ = val >> 24;
    *dst++ = val >> 16;
    *dst++ = val >> 8;
    *dst++ = val;
}

#if HAVE_BIGENDIAN
#define AV_WN16 AV_WB16
#define AV_WN32 AV_WB32
#else
#define AV_WN16 AV_WL16
#define AV_WN32 AV_WL32
#endif

#endif
