#include "avscale.h"

const AVPixelFormaton rgb24_desc = {
    .name           = "rgb24",
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

const AVPixelFormaton r210_desc = {
    .name           = "r210",
    .colourspace    = AVS_RGB,
    .flags          = AVPF_FLAG_BE,
    .entry_size     = 4,
    .components     = 3,
    .component_desc = {
         {
             .plane     = 0,
             .h_sub_log = 0,
             .v_sub_log = 0,
             .off       = 0,
             .shift     = 20,
             .bpp       = 10,
             .packed    = 1,
             .next      = 0,
         },
         {
             .plane     = 0,
             .h_sub_log = 0,
             .v_sub_log = 0,
             .off       = 0,
             .shift     = 10,
             .bpp       = 10,
             .packed    = 1,
             .next      = 0,
         },
         {
             .plane     = 0,
             .h_sub_log = 0,
             .v_sub_log = 0,
             .off       = 0,
             .shift     = 0,
             .bpp       = 10,
             .packed    = 1,
             .next      = 0,
         }
    },
};

const AVPixelFormaton r10k_desc = {
    .name           = "r10k",
    .colourspace    = AVS_RGB,
    .flags          = AVPF_FLAG_BE,
    .entry_size     = 4,
    .components     = 3,
    .component_desc = {
         {
             .plane     = 0,
             .h_sub_log = 0,
             .v_sub_log = 0,
             .off       = 0,
             .shift     = 22,
             .bpp       = 10,
             .packed    = 1,
             .next      = 0,
         },
         {
             .plane     = 0,
             .h_sub_log = 0,
             .v_sub_log = 0,
             .off       = 0,
             .shift     = 12,
             .bpp       = 10,
             .packed    = 1,
             .next      = 0,
         },
         {
             .plane     = 0,
             .h_sub_log = 0,
             .v_sub_log = 0,
             .off       = 0,
             .shift     = 2,
             .bpp       = 10,
             .packed    = 1,
             .next      = 0,
         }
    },
};
