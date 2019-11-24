#ifndef _UI_APP_H_
#define _UI_APP_H_

#include "cengine/types/types.h"

extern void app_ui_init (void);

extern void app_ui_end (void);

/*** images ***/

extern void app_ui_images_move_up (u32 movement);

extern void app_ui_images_move_down (u32 movement);

// prepare the ui for the images to be displayed
extern void app_ui_images_set_ui_elements (void);

extern void app_ui_image_display (const char *filename);

#endif