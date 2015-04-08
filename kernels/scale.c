/* this file is part of Libav and distributed under GPLv3lu0 */
#include <string.h>
#include "kernels.h"

typedef struct ScaleContext {
    int dst_w, dst_h;
} ScaleContext;

static void component_scale(void *ctx_,
                            uint8_t *src, int sstride,
                            uint8_t *dst, int dstride,
                            int w, int h)
{
    ScaleContext *ctx = ctx_;
    int i, j;
    int x0, y0;
    if (!src || !dst)
        return;

    for (j = 0; j < ctx->dst_h; j++) {
        y0 = (j * h / ctx->dst_h) * sstride;
        for (i = 0; i < ctx->dst_w; i++) {
            x0 = i * w / ctx->dst_w;
            dst[i] = src[x0 + y0];
        }
        dst += dstride;
    }
}

static void scale_deinit(AVSFilterStage *stage)
{
    int i;

    for (i = 0; i < AVS_MAX_COMPONENTS; i++) {
        av_freep(&stage->dst[i]);
        stage->dst_stride[i] = 0;
        av_freep(&stage->do_component_ctx[i]);
    }
}

static int scale_kernel_init(AVSContext *ctx, const AVSKernel *kern, AVSFilterStage *stage)
{
    ScaleContext *sc;
    int i, n, dstride;
    n = ctx->dstfmt->components;
    if (ctx->cur_fmt.component_desc[0].bpp > 8){printf("returning %d\n",AVERROR(ENOSYS));
        return AVERROR(ENOSYS);}
    for (i = 0; i < n; i++) {
        stage->do_component[i] = component_scale;

        stage->do_component_ctx[i] = av_malloc(sizeof(ScaleContext));
        sc = (ScaleContext*)stage->do_component_ctx[i];
        if (!sc)
            return AVERROR(ENOMEM);
        sc->dst_w = ctx->dst_w >> ctx->cur_fmt.component_desc[i].h_sub_log;
        sc->dst_h = ctx->dst_h >> ctx->cur_fmt.component_desc[i].h_sub_log;

        dstride = (sc->dst_w + 31) & ~31;
        stage->dst[i] = av_mallocz(sc->dst_h * dstride);
        if (!stage->dst[i])
            return AVERROR(ENOMEM);
        stage->dst_stride[i] = dstride;
    }

    ctx->cur_w = ctx->dst_w;
    ctx->cur_h = ctx->dst_h;

    return 0;
}

const AVSKernel avs_scale_kernel = {
    .name = "scale",
    .kernel_init = scale_kernel_init,
};
