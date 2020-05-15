#ifndef _UI_APP_H_
#define _UI_APP_H_

#include "cengine/types/types.h"

#include "cengine/ui/panel.h"
#include "cengine/ui/image.h"

#include "app/media.h"

#define DEFAULT_GRID_N_COLS          5
#define DEFAULT_GRID_N_ROWS          4

extern void app_ui_init (void);

extern void app_ui_end (void);

/*** actionsbar ***/

extern void app_ui_actionsbar_show (void);

extern void app_ui_actionsbar_toggle (void);

extern void app_ui_actionsbar_hide (void);

/*** statusbar ***/

// sets the total number of images being displayed in the status bar text
extern int app_ui_statusbar_total_set (u32 total);

// displays the current zoom level in the status bar
extern int app_ui_statusbar_zoom_set (int zoom);

extern void app_ui_statusbar_show (const char *foldername, u32 total);

extern void app_ui_statusbar_hide (void);

extern void app_ui_statusbar_set_selected_text (const char *text);

/*** images ***/

extern Panel *images_panel;

// prepare the ui for the images to be displayed
extern void app_ui_images_set_ui_elements (u32 n_images, u32 n_cols, u32 n_rows);

// we have closed the images folde,r so hide images ui items
extern void app_ui_images_remove_ui_elements (void);

// adds the image into the main grid for display
extern void app_ui_image_display (Image *image);

// creates a ui image element to be displayed
extern void app_ui_image_create (MediaItem *image_item);

#endif