/* this file is part of Libav and distributed under GPLv3lu0 */
#include <string.h>
#include "kernels.h"

typedef struct RGBUnpContext {
    int roff, goff, boff;
    int step;
    int shift[3], mask[3];
    int bpp, be;
} RGBUnpContext;

static void rgbunpack_packed(void *ctx_,
                             uint8_t *src[AVS_MAX_COMPONENTS],
                             int sstrides[AVS_MAX_COMPONENTS],
                             uint8_t *dst[AVS_MAX_COMPONENTS],
                             int dstrides[AVS_MAX_COMPONENTS],
                             int w, int h)
{
    RGBUnpContext *ctx = ctx_;
    uint8_t *rgb[3];
    int i, j, c;

    rgb[0] = src[0] + ctx->roff;
    rgb[1] = src[0] + ctx->goff;
    rgb[2] = src[0] + ctx->boff;

    for (j = 0; j < h; j++) {
        for (i = 0; i < w; i++) {
            for (c = 0; c < 3; c++) {
                dst[c][i] = rgb[c][0];
                rgb[c] += ctx->step;
            }
        }
        for (c = 0; c < 3; c++) {
            rgb[c] += sstrides[0] - w * ctx->step;
            dst[c] += dstrides[c];
        }
    }
}

static void rgbunpack_fields(void *ctx_,
                             uint8_t *src[AVS_MAX_COMPONENTS],
                             int sstrides[AVS_MAX_COMPONENTS],
                             uint8_t *dst[AVS_MAX_COMPONENTS],
                             int dstrides[AVS_MAX_COMPONENTS],
                             int w, int h)
{
    RGBUnpContext *ctx = ctx_;
    unsigned val, cval;
    uint8_t *rgb;
    int i, j, c;

    rgb = src[0];

    for (j = 0; j < h; j++) {
        for (i = 0; i < w; i++) {
            switch (ctx->step) {
            case 1: val = rgb[i]; break;
            case 2: val = ctx->be ? AV_RB16(rgb + i * 2) : AV_RL16(rgb + i * 2); break;
            case 4: val = ctx->be ? AV_RB32(rgb + i * 4) : AV_RL32(rgb + i * 4); break;
            default: val = 0;
            }
            for (c = 0; c < 3; c++) {
                cval = (val >> ctx->shift[c]) & ctx->mask[c];
                if (ctx->bpp <= 8)
                    dst[c][i] = cval;
                else
                    AV_WN16(dst[c] + i * 2, cval);
            }
        }
        rgb += sstrides[0];
        for (c = 0; c < 3; c++)
            dst[c] += dstrides[c];
    }
}

static void rgbunp_free(AVSFilterStage *stage)
{
    int i;
    for (i = 0; i < 3; i++) {
        av_freep(&stage->dst[i]);
        stage->dst_stride[i] = 0;
    }
    av_freep(&stage->do_common_ctx);
}

static int rgbunp_kernel_init(AVSContext *ctx, const AVSKernel *kern, AVSFilterStage *stage)
{
    RGBUnpContext *ruc;
    int i;
    int dstride;

    if (ctx->cur_fmt.component_desc[0].next)
        stage->do_common = rgbunpack_packed;
    else
        stage->do_common = rgbunpack_fields;
    stage->deinit    = rgbunp_free;
    stage->do_common_ctx = av_malloc(sizeof(RGBUnpContext));
    if (!stage->do_common_ctx)
        return AVERROR(ENOMEM);

    ruc = stage->do_common_ctx;
    ruc->roff = ctx->cur_fmt.component_desc[0].off;
    ruc->goff = ctx->cur_fmt.component_desc[1].off;
    ruc->boff = ctx->cur_fmt.component_desc[2].off;
    ruc->step = ctx->cur_fmt.entry_size;
    ruc->bpp  = ctx->cur_fmt.component_desc[0].bpp;
    ruc->be   = ctx->cur_fmt.flags & AVPF_FLAG_BE;
    for (i = 0; i < 3; i++) {
        ruc->shift[i] = ctx->cur_fmt.component_desc[i].shift;
        ruc->mask[i]  = (1 << ctx->cur_fmt.component_desc[i].bpp) - 1;
    }
    dstride = (ctx->cur_w * (ruc->bpp + 7 >> 3) + 31) & ~31;

    //todo not allocate temp buffer for planar final output
    for (i = 0; i < 3; i++) {
        stage->dst[i] = av_mallocz(ctx->cur_h * dstride);
        if (!stage->dst[i])
            return AVERROR(ENOMEM);
        stage->dst_stride[i] = dstride;
    }

    if (HAVE_BIGENDIAN)
        ctx->cur_fmt.flags |=  AVPF_FLAG_BE;
    else
        ctx->cur_fmt.flags &= ~AVPF_FLAG_BE;

    return 0;
}

const AVSKernel avs_rgbunp_kernel = {
    .name = "rgbunp",
    .kernel_init = rgbunp_kernel_init,
};
