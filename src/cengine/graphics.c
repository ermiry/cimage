#include <stdlib.h>

#include "cengine/types/types.h"
#include "cengine/types/string.h"

#include "cengine/graphics.h"

ImageData *image_data_new (u32 w, u32 h, String *filename) {

    ImageData *img_data = (ImageData *) malloc (sizeof (ImageData));
    if (img_data) {
        img_data->w = w;
        img_data->h = h;

        img_data->filename = filename ? filename : NULL;
    }

    return img_data;

}

void image_data_delete (void *img_data_ptr) { 
    
    if (img_data_ptr) {
        ImageData *img_data = (ImageData *) img_data_ptr;
        str_delete (img_data->filename);
        free (img_data_ptr);
    }  
    
}