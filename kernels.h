/* this file is part of Libav and distributed under GPLv3lu0 */
#ifndef AVSCALE_KERNELS_H
#define AVSCALE_KERNELS_H

#include "avscale.h"

typedef struct AVSKernel {
    const char *name; // for convenience

    // init worker-specific contexts and set working functions
    // e.g. for YUV2RGB it will init do_common_ctx with YUV2RGB tables
    // and set do_common = convert_yuv2rgb
    int (*kernel_init)(AVSContext *ctx, const struct AVSKernel *kern, AVSFilterStage *stage /*, options? */);
} AVSKernel;

const AVSKernel* avs_find_kernel(const char *name);
int avs_apply_kernel(AVSContext *ctx, const char *name, AVSFilterStage *stage);

#endif /* AVSCALE_KERNELS_H */

