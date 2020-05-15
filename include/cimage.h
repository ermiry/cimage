#ifndef _CIMAGE_H_
#define _CIMAGE_H_

#include "cengine/ui/notification.h"

#define CIMAGE_VERSION          "0.1"

#define FPS_LIMIT               30

#define DEFAULT_SCREEN_WIDTH    1920    
#define DEFAULT_SCREEN_HEIGHT   1080

typedef unsigned char uchar;

// general information of the things we are working on
struct _Cimage {

    String *opened_folder_name;
    DoubleList *images;
    DoubleList *selected_images;

};

typedef struct _Cimage Cimage;

extern Cimage *cimage;

extern void cimage_delete (void *cimage_ptr);

extern Cimage *cimage_create (void);

extern NotiCenter *main_noti_center;

extern void cimage_quit (void);
extern void cimage_die (const char *error);

extern int cimage_init (void);
extern int cimage_end (void);

#endif