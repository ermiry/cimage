#ifndef _UI_APP_H_
#define _UI_APP_H_

#include "cengine/types/types.h"

#include "app/states/app.h"

struct _ImageItem;

extern void app_ui_init (void);

extern void app_ui_end (void);

/*** actionsbar ***/

extern void app_ui_actionsbar_show (void);

extern void app_ui_actionsbar_hide (void);

/*** statusbar ***/

extern void app_ui_statusbar_show (const char *foldername, u32 total);

extern void app_ui_statusbar_hide (void);

/*** images ***/

extern void app_ui_images_move_up (u32 movement);

extern void app_ui_images_move_down (u32 movement);

// prepare the ui for the images to be displayed
extern void app_ui_images_set_ui_elements (u32 n_images, u32 n_cols, u32 n_rows);

// we have closed the images folde,r so hide images ui items
extern void app_ui_images_remove_ui_elements (void);

extern void app_ui_image_display (struct _ImageItem *image_item);

#endif