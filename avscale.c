/* this file is part of Libav and distributed under GPLv3lu0 */
#include "avscale.h"
#include "kernels.h"

uint8_t* avs_get_component_ptr(AVFrame *src, int component_id)
{ // currently a simple hack - it has to be extended for e.g. NV12
    if (component_id >= src->pixfmt->components)
        return 0;
    if (!src->pixfmt->component_desc[component_id].packed)
        return src->data[src->pixfmt->component_desc[component_id].plane];
    else
        return src->data[0] + src->pixfmt->component_desc[component_id].off;
}

int avs_get_component_stride(AVFrame *src, int component_id)
{
    if (src->linesize[component_id])
        return src->linesize[component_id];
    else
        return src->linesize[0];
}

static int prepare_next_stage(AVSContext *ctx, AVSFilterStage **stage, const char *name)
{
    int ret;
    AVSFilterStage *s;
    int i;

    s = av_mallocz(sizeof(*s));
    if (!s)
        return AVERROR(ENOMEM);

    if (!ctx->head)
        ctx->head = s;

    for (i = 0; i < AVS_MAX_COMPONENTS; i++) {
        s->w[i] = ctx->cur_w >> ctx->cur_fmt.component_desc[i].h_sub_log;
        s->h[i] = ctx->cur_h >> ctx->cur_fmt.component_desc[i].v_sub_log;
    }

    if ((ret = avs_apply_kernel(ctx, name, s)) < 0)
        return ret;

    if (*stage)
        (*stage)->next = s;
    *stage = s;
    return 0;
err:
    if (s->deinit)
        s->deinit(s);
    av_free(s);
    return ret;
}

int avs_build_chain(AVSContext *ctx, AVFrame *src, AVFrame *dst)
{ // xxx proof of a concept, not a real thing
    AVSFilterStage *stage = 0;
    int ret;

    ctx->srcfmt  = src->pixfmt;
    ctx->dstfmt  = dst->pixfmt;
    ctx->cur_w   = src->width;
    ctx->cur_h   = src->height;
    ctx->dst_w   = dst->width;
    ctx->dst_h   = dst->height;
    ctx->cur_fmt = *ctx->srcfmt;

    if (ctx->srcfmt->colourspace == ctx->dstfmt->colourspace) {
        if ( ctx->srcfmt->component_desc[0].packed &&
            !ctx->dstfmt->component_desc[0].packed) {
            if ((ret = prepare_next_stage(ctx, &stage, "rgbunp")) < 0)
                return ret;
        } else {
            if ((ret = prepare_next_stage(ctx, &stage, "murder")) < 0)
                return ret;
        }
    } else if (ctx->srcfmt->colourspace == AVS_RGB &&
               ctx->dstfmt->colourspace == AVS_YUV) {
        if ((ret = prepare_next_stage(ctx, &stage, "rgbunp")) < 0)
            return ret;
        if (ctx->cur_w != ctx->dst_w || ctx->cur_h != ctx->dst_h)
            if ((ret = prepare_next_stage(ctx, &stage, "scale")) < 0)
                return ret;
        if ((ret = prepare_next_stage(ctx, &stage, "rgb2yuv")) < 0)
            return ret;
    } else
        return AVERROR(ENOSYS);

    ctx->tail = stage;

    return 0;
}

int avs_process_frame(AVSContext *ctx, AVFrame *srcf, AVFrame *dstf)
{
    int ret;
    const AVSFilterStage *stage;

    int i;

    uint8_t *src[AVS_MAX_COMPONENTS];
    int  sstride[AVS_MAX_COMPONENTS];
    uint8_t *dst[AVS_MAX_COMPONENTS];
    int  dstride[AVS_MAX_COMPONENTS];
    int w[AVS_MAX_COMPONENTS], h[AVS_MAX_COMPONENTS];
    uint8_t *src2[AVS_MAX_COMPONENTS];
    uint8_t *dst2[AVS_MAX_COMPONENTS];

    if (!ctx->head) {
        if ((ret = avs_build_chain(ctx, srcf, dstf)) < 0)
            return ret;
    }

    stage = ctx->head;

    for (i = 0; i < AVS_MAX_COMPONENTS; i++) {
        src[i]     = avs_get_component_ptr(srcf, i);
        sstride[i] = avs_get_component_stride(srcf, i);
    }
    while (stage) {
        for (i = 0; i < AVS_MAX_COMPONENTS; i++) {
            if (stage->src[i]) {
                src[i]     = stage->src[i];
                sstride[i] = stage->src_stride[i];
            }
            if (stage->dst[i]) {
                dst[i]     = stage->dst[i];
                dstride[i] = stage->dst_stride[i];
            } else {
                dst[i]     = avs_get_component_ptr(dstf, i);
                dstride[i] = avs_get_component_stride(dstf, i);
            }
        }
        memcpy(src2, src, sizeof(src2));
        memcpy(dst2, dst, sizeof(dst2));
        if (stage->do_common)
            stage->do_common(stage->do_common_ctx,
                             src2, sstride, dst2, dstride,
                             stage->w[0], stage->h[0]);
        for (i = 0; i < AVS_MAX_COMPONENTS; i++)
            if (stage->do_component[i])
                stage->do_component[i](stage->do_component_ctx[i],
                                       src2[i], sstride[i],
                                       dst2[i], dstride[i],
                                       stage->w[i], stage->h[i]);

        // this stage output buffers are likely to be next stage input
        for (i = 0; i < AVS_MAX_COMPONENTS; i++) {
            src[i]     = dst[i];
            sstride[i] = dstride[i];
        }
        stage = stage->next;
    }
}
