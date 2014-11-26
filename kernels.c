/* this file is part of Libav and distributed under GPLv3lu0 */
#include <string.h>
#include "kernels.h"

extern const AVSKernel avs_rgbunp_kernel;
extern const AVSKernel avs_rgb2yuv_kernel;
extern const AVSKernel avs_scale_kernel;

static void component_murder(void *ctx,
                             uint8_t *src, int sstride,
                             uint8_t *dst, int dstride,
                             int w, int h)
{
    if (!src || !dst)
        return;
    memcpy(dst, src, sstride * h);
}

static int murder_kernel_init(AVSContext *ctx, const AVSKernel *kern, AVSFilterStage *stage)
{
    int i, n;
    n = ctx->cur_fmt.components;
    if (ctx->cur_fmt.component_desc[0].packed)
        n = 1;
    for (i = 0; i < n; i++)
        stage->do_component[i] = component_murder;
    return 0;
}

static const AVSKernel avs_murder_kernel = {
    .name = "murder",
    .kernel_init = murder_kernel_init,
};

static const AVSKernel* avs_kernels[] = {
    &avs_murder_kernel,
    &avs_rgbunp_kernel,
    &avs_rgb2yuv_kernel,
    &avs_scale_kernel,
    0
};

const AVSKernel* avs_find_kernel(const char *name)
{
    const AVSKernel** k = avs_kernels;

    while (*k) {
        if (!strcmp((*k)->name, name))
            return *k;
        k++;
    }

    return 0;
}

int avs_apply_kernel(AVSContext *ctx, const char *name, AVSFilterStage *stage)
{
    const AVSKernel* k = avs_find_kernel(name);
    if (!k)
        return AVERROR(EINVAL);
    return k->kernel_init(ctx, k, stage);
}
