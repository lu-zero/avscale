/* this file is part of Libav and distributed under GPLv3lu0 */
#ifndef AVSCALE_H
#define AVSCALE_H

#include <stdint.h>
#include "internal.h"

#define AVS_MAX_COMPONENTS 5

typedef struct AVChromaton {
    int plane;
    int h_sub_log, v_sub_log; // subsampling information
    int off; // offset to the starting element - e.g. 0 for Y, 1 for U and 3 for V in YUYV
    int shift; // component shift for packed, e.g. for RGB565 it will be 11,5,0
    int bpp;   // bits per component, e.g. for packed RGB565 you'll have 5,6,5
    int packed; // if component is packed with others (e.g. RGB24 - 1,1,1, NV12 - 0,1,1)
    int next; // offset to the next element - e.g. 2 for Y and 4 for U and V in YUYV
} AVChromaton;

typedef struct AVPixelFormaton {
    const char *name; //maybe
    enum { AVS_GREY, AVS_RGB, AVS_YUV, AVS_XYZ, AVS_LAB, AVS_CMYK } colourspace;
    unsigned flags; // has alpha, uses BE order, uses palette etc
    int entry_size; // might serve useful for packed formats - e.g. 4 or 2 bytes per entry

    //optional
//    enum gamma;
//    enum colourspace_params; // e.g. BT.709 or linear RGB/XYZ
    //optional end

    int components;
    AVChromaton component_desc[AVS_MAX_COMPONENTS];
} AVPixelFormaton;

typedef struct AVFrame {
    int version;
    int width, height;
    uint8_t* data[AVS_MAX_COMPONENTS];
    int      linesize[AVS_MAX_COMPONENTS];
    const AVPixelFormaton *pixfmt;
} AVFrame;

struct AVSContext;

typedef struct AVSFilterStage {
    void (*do_common)(void *ctx,
                      uint8_t *src[AVS_MAX_COMPONENTS],
                      int sstrides[AVS_MAX_COMPONENTS],
                      uint8_t *dst[AVS_MAX_COMPONENTS],
                      int dstrides[AVS_MAX_COMPONENTS],
                      int w, int h);
    void (*do_component[AVS_MAX_COMPONENTS])(void *ctx,
                                             uint8_t *src, int sstride,
                                             uint8_t *dst, int dstride,
                                             int w, int h);

    void *do_common_ctx;
    void *do_component_ctx[AVS_MAX_COMPONENTS];

    uint8_t *src[AVS_MAX_COMPONENTS]; // null if current input should be used
    int      src_stride[AVS_MAX_COMPONENTS];
    uint8_t *dst[AVS_MAX_COMPONENTS]; // null if default output should be used
    int      dst_stride[AVS_MAX_COMPONENTS];
    int      w[AVS_MAX_COMPONENTS], h[AVS_MAX_COMPONENTS];

    struct AVSFilterStage *next;
    void (*deinit)(struct AVSFilterStage *stage);
} AVSFilterStage;

typedef struct AVSContext {
    const AVPixelFormaton *srcfmt, *dstfmt;
    AVPixelFormaton cur_fmt;
    int cur_w, cur_h;
    int dst_w, dst_h;
    AVSFilterStage *head, *tail;
} AVSContext;

uint8_t* avs_get_component_ptr(AVFrame *src, int component_id);
int avs_get_component_stride(AVFrame *src, int component_id);

int avs_build_chain(AVSContext *ctx, AVFrame *src, AVFrame *dst);

int avs_process_frame(AVSContext *ctx, AVFrame *src, AVFrame *dst);

#endif /* AVSCALE_H */
