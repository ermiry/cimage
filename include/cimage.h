#ifndef _CIMAGE_H_
#define _CIMAGE_H_

#define CIMAGE_VERSION          "0.1"

#define FPS_LIMIT               30

#define DEFAULT_SCREEN_WIDTH    1280    
#define DEFAULT_SCREEN_HEIGHT   720

typedef unsigned char uchar;

extern void cimage_quit (void);
extern void cimage_die (const char *error);

extern int cimage_init (void);
extern int cimage_end (void);

#endif