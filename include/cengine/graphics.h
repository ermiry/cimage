#ifndef _CENGINE_GRAPHICS_H_
#define _CENGINE_GRAPHICS_H_

#include "cengine/types/types.h"
#include "cengine/types/string.h"

typedef enum Flip {

    NO_FLIP             = 0x00000000,
    FLIP_HORIZONTAL     = 0x00000001,
    FLIP_VERTICAL       = 0x00000002

} Flip;

struct _ImageData {

    u32 w, h;
    String *filename;

};

typedef struct _ImageData ImageData;

extern ImageData *image_data_new (u32 w, u32 h, String *filename);

extern void image_data_delete (void *img_data_ptr);

#endif