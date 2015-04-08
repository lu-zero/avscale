/* this file is part of Libav and distributed under GPLv3lu0 */
#include <string.h>
#include "kernels.h"

typedef struct RGBPackContext {
    int off[3], shift[3];
    int step;
    int inbpp;
    int be;
} RGBPackContext;

static void rgbpack_fields(void *ctx_,
                           uint8_t *src[AVS_MAX_COMPONENTS],
                           int sstrides[AVS_MAX_COMPONENTS],
                           uint8_t *dst[AVS_MAX_COMPONENTS],
                           int dstrides[AVS_MAX_COMPONENTS],
                           int w, int h)
{
    RGBPackContext *ctx = ctx_;
    uint8_t *rgb[3], *dest;
    unsigned val;
    int i, j, c;

    rgb[0] = src[0];
    rgb[1] = src[1];
    rgb[2] = src[2];
    dest   = dst[0];

    for (j = 0; j < h; j++) {
        for (i = 0; i < w; i++) {
            val = 0;
            if (ctx->inbpp <= 8) {
                for (c = 0; c < 3; c++)
                    val |= rgb[c][i] << ctx->shift[c];
            } else {
                for (c = 0; c < 3; c++)
                    val |= AV_RN16(rgb[c] + i * 2) << ctx->shift[c];
            }
            switch (ctx->step) {
            case 1:
                dest[i] = val;
                break;
            case 2:
                if (ctx->be) AV_WB16(dest + i * 2, val);
                else         AV_WL16(dest + i * 2, val);
                break;
            case 4:
                if (ctx->be) AV_WB32(dest + i * 4, val);
                else         AV_WL32(dest + i * 4, val);
                break;
            }
        }
        for (c = 0; c < 3; c++)
            rgb[c] += sstrides[0];
        dest += dstrides[0];
    }
}

static void rgbpack24(void *ctx_,
                      uint8_t *src[AVS_MAX_COMPONENTS],
                      int sstrides[AVS_MAX_COMPONENTS],
                      uint8_t *dst[AVS_MAX_COMPONENTS],
                      int dstrides[AVS_MAX_COMPONENTS],
                      int w, int h)
{
    RGBPackContext *ctx = ctx_;
    uint8_t *rgb[3], *dest;
    unsigned val;
    int i, j, c;

    rgb[0] = src[0];
    rgb[1] = src[1];
    rgb[2] = src[2];
    dest   = dst[0];

    for (j = 0; j < h; j++) {
        for (i = 0; i < w; i++) {
            for (c = 0; c < 3; c++)
                dest[ctx->off[c]] = (ctx->inbpp <= 8) ? rgb[c][i] : AV_RN16(rgb[c] + i * 2) >> (ctx->inbpp - 8);
            dest += ctx->step;
        }
        for (c = 0; c < 3; c++)
            rgb[c] += sstrides[0];
        dest += dstrides[0] - w * ctx->step;
    }
}

static void rgbpck_free(AVSFilterStage *stage)
{
    av_freep(&stage->do_common_ctx);
}

static int rgbpck_kernel_init(AVSContext *ctx, const AVSKernel *kern, AVSFilterStage *stage)
{
    RGBPackContext *rc;
    int i;
    int dstride = (ctx->cur_w + 31) & ~31;

    if (!ctx->dstfmt->component_desc[0].next)
        stage->do_common = rgbpack_fields;
    else
        stage->do_common = rgbpack24;
    stage->deinit    = rgbpck_free;
    stage->do_common_ctx = av_malloc(sizeof(RGBPackContext));
    if (!stage->do_common_ctx)
        return AVERROR(ENOMEM);

    rc = stage->do_common_ctx;
    for (i = 0; i < 3; i++) {
        rc->off[i]   = ctx->dstfmt->component_desc[i].off;
        rc->shift[i] = ctx->dstfmt->component_desc[i].shift;
    }
    rc->step  = ctx->dstfmt->entry_size;
    rc->be    = ctx->dstfmt->flags & AVPF_FLAG_BE;

    rc->inbpp = ctx->cur_fmt.component_desc[0].bpp;

    return 0;
}

const AVSKernel avs_rgbpck_kernel = {
    .name = "rgbpck",
    .kernel_init = rgbpck_kernel_init,
};
