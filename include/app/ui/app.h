#ifndef _UI_APP_H_
#define _UI_APP_H_

extern void app_ui_init (void);

extern void app_ui_end (void);

/*** images ***/

// prepare the ui for the images to be displayed
extern void app_ui_images_set_ui_elements (void);

extern void app_ui_image_display (const char *filename);

#endif