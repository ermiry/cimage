#ifndef _CIMAGE_INPUT_H_
#define _CIMAGE_INPUT_H_

extern int zoom_level;

extern void zoom_more (void *args);

extern void zoom_less (void *args);

extern void main_screen_input (void *win_ptr);

extern void cimage_input_init (void);

extern void cimage_input_end (void);

#endif