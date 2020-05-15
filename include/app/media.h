#ifndef _CIMAGE_MEDIA_H_
#define _CIMAGE_MEDIA_H_

#include "cengine/ui/image.h"

struct _MediaItem {

    Image *image;
    String *filename;
    String *path;
    bool selected;

};

typedef struct _MediaItem MediaItem;

extern void media_item_delete_dummy (void *item_ptr);

// media folder

extern void media_folder_select (void *args);

extern void media_folder_close (void *args);

// media operations

// gets executed on every input on the search input
extern void media_search (void *args);

#endif