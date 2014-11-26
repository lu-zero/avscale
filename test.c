#include <string.h>
#include <stdio.h>

#include "avscale.h"

const AVPixelFormaton rgb_desc = {
    .name           = "rgb",
    .colourspace    = AVS_RGB,
    .entry_size     = 3,
    .components     = 3,
    .component_desc = {
         {
             .plane     = 0,
             .h_sub_log = 0,
             .v_sub_log = 0,
             .off       = 0,
             .shift     = 0,
             .bpp       = 8,
             .packed    = 1,
             .next      = 3,
         },
         {
             .plane     = 0,
             .h_sub_log = 0,
             .v_sub_log = 0,
             .off       = 1,
             .shift     = 0,
             .bpp       = 8,
             .packed    = 1,
             .next      = 3,
         },
         {
             .plane     = 0,
             .h_sub_log = 0,
             .v_sub_log = 0,
             .off       = 2,
             .shift     = 0,
             .bpp       = 8,
             .packed    = 1,
             .next      = 3,
         }
    },
};

const AVPixelFormaton yuv420_desc = {
    .name           = "yuv420",
    .colourspace    = AVS_YUV,
    .entry_size     = 1,
    .components     = 3,
    .component_desc = {
         {
             .plane     = 0,
             .h_sub_log = 0,
             .v_sub_log = 0,
             .off       = 0,
             .shift     = 0,
             .bpp       = 8,
             .packed    = 0,
             .next      = 1,
         },
         {
             .plane     = 1,
             .h_sub_log = 1,
             .v_sub_log = 1,
             .off       = 0,
             .shift     = 0,
             .bpp       = 8,
             .packed    = 0,
             .next      = 1,
         },
         {
             .plane     = 2,
             .h_sub_log = 1,
             .v_sub_log = 1,
             .off       = 0,
             .shift     = 0,
             .bpp       = 8,
             .packed    = 0,
             .next      = 1,
         }
    },
};

int main(int argc, char **argv)
{
    int w, h;
    FILE *in, *out;
    AVFrame src = {0}, dst = {0};
    AVSContext avsctx = {0};
    int copy;
    int ret;

    if (argc < 3) {
        printf("usage: %s infile.pnm outfile.{ppm,pgm}\n", argv[0]);
        return 0;
    }
    in  = fopen(argv[1], "rb");
    out = fopen(argv[2], "wb");
    copy = !!strstr(argv[2], ".ppm");

    fscanf(in, "P6\n%d %d\n255\n", &w, &h);
    printf("converting %dx%d pic...\n", w, h);

    src.version = 1;
    src.width = w;
    src.height = h;
    src.data[0] = av_malloc(w * h * 3);
    src.linesize[0] = w * 3;
    src.pixfmt = &rgb_desc;

    fread(src.data[0], w * 3, h, in);

    dst.version = 1;
    dst.width = w;
    dst.height = h;
    if (!copy && w > 550) {
        dst.width  = (dst.width  / 3 + 3) & ~3;
        dst.height = (dst.height / 3 + 3) & ~3;
    }
    if (copy) {
        dst.data[0] = av_malloc(dst.width * dst.height * 3);
        dst.linesize[0] = dst.width * 3;
        dst.pixfmt = &rgb_desc;
    } else {
        dst.data[0] = av_malloc(dst.width * dst.height);
        dst.linesize[0] = dst.width;
        dst.data[1] = av_malloc(dst.width * dst.height / 4);
        dst.linesize[1] = dst.width / 2;
        dst.data[2] = av_malloc(dst.width * dst.height / 4);
        dst.linesize[2] = dst.width / 2;
        dst.pixfmt = &yuv420_desc;
    }
    ret = avs_process_frame(&avsctx, &src, &dst);
    printf(ret ? "Failed\n" : "Succeeded\n");
    w = dst.width;
    h = dst.height;
    if (copy) {
        fprintf(out, "P6\n%d %d\n255\n",w,h);
        fwrite(dst.data[0], w * 3, h, out);
    } else {
        int i;
        fprintf(out, "P5\n%d %d\n255\n",w,h+h/2);
        fwrite(dst.data[0], w, h, out);
        for (i = 0; i < h / 2; i++) {
            fwrite(dst.data[1] + i * dst.linesize[1], w/2, 1, out);
            fwrite(dst.data[2] + i * dst.linesize[2], w/2, 1, out);
        }
    }
    fclose(in);
    fclose(out);

    return 0;
}
