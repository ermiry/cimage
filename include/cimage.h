#ifndef _CIMAGE_H_
#define _CIMAGE_H_

#include "cengine/ui/notification.h"

#define CIMAGE_VERSION          "0.1"

#define FPS_LIMIT               30

#define DEFAULT_SCREEN_WIDTH    1920    
#define DEFAULT_SCREEN_HEIGHT   1080

typedef unsigned char uchar;

extern NotiCenter *main_noti_center;

extern void cimage_quit (void);
extern void cimage_die (const char *error);

extern int cimage_init (void);
extern int cimage_end (void);

#endif