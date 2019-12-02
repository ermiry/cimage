#ifndef _STATE_APP_H_
#define _STATE_APP_H_

#include "cengine/types/string.h"
#include "cengine/collections/dlist.h"

#include "cengine/manager/state.h"

#include "cengine/ui/image.h"

extern State *app_state;
extern State *app_state_new (void);

// general information of the things we are working on
struct _Cimage {

    String *opened_folder_name;
    DoubleList *images;
    DoubleList *selected_images;

};

typedef struct _Cimage Cimage;

extern Cimage *cimage;

struct _ImageItem {

    Image *image;
    String *filename;
    String *path;
    bool selected;

};

typedef struct _ImageItem ImageItem;

extern void images_folder_select (void *args);

extern void images_folder_close (void *args);

extern void images_search (void *args);

extern void main_screen_input (void *win_ptr);

#endif