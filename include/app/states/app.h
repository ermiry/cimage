#ifndef _STATE_APP_H_
#define _STATE_APP_H_

#include "cengine/types/string.h"
#include "cengine/collections/dlist.h"

#include "cengine/manager/state.h"

#include "cengine/ui/image.h"

extern State *app_state;
extern State *app_state_new (void);

struct _ImageItem {

    Image *image;
    String *filename;
    String *path;

};

typedef struct _ImageItem ImageItem;

extern void images_folder_select (void *args);

extern void main_screen_input (void *win_ptr);

#endif