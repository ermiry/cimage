#include <stdlib.h>

#include "cengine/types/types.h"
#include "cengine/types/string.h"

#include "cengine/renderer.h"

#include "cengine/ui/ui.h"
#include "cengine/ui/font.h"
#include "cengine/ui/textbox.h"
#include "cengine/ui/button.h"
#include "cengine/ui/image.h"
#include "cengine/ui/panel.h"

#include "cengine/ui/layout/grid.h"

static Panel *background_panel = NULL;

static Panel *sidebar = NULL;

static Button *photos_button = NULL;
static Button *settings_button = NULL;

// FIXME: set actions
static void sidebar_init (u32 screen_height) {

    // FIXME:
    Renderer *main_renderer = renderer_get_by_name ("main");

    RGBA_Color blue_night = { 53, 59, 72, 255 };

    sidebar = ui_panel_create (0, 0, 100, screen_height, UI_POS_LEFT_UPPER_CORNER, main_renderer);
    ui_panel_set_bg_colour (sidebar, main_renderer, blue_night);
    ui_element_set_layer (sidebar->ui_element, "top");

    photos_button = ui_button_create (18, 18, 64, 64, UI_POS_LEFT_UPPER_CORNER, main_renderer);
    // ui_button_set_action (photos_button, NULL, NULL);
    // ui_button_set_ouline_colour (photos_button, RGBA_WHITE);
    ui_button_set_sprite (photos_button, main_renderer, BUTTON_STATE_MOUSE_OUT, "./assets/icons/gallery.png");
    ui_element_set_layer (photos_button->ui_element, "top");

    settings_button = ui_button_create (18, -18, 64, 64, UI_POS_LEFT_BOTTOM_CORNER, main_renderer);
    // ui_button_set_action (settings_button, game_state_change, change_main_state_settings);
    // ui_button_set_ouline_colour (settings_button, RGBA_WHITE);
    ui_button_set_sprite (settings_button, main_renderer, BUTTON_STATE_MOUSE_OUT, "./assets/icons/settings.png");
    ui_element_set_layer (settings_button->ui_element, "top");

}

static void sidebar_end (void) {

    if (sidebar) ui_element_destroy (sidebar->ui_element);

    if (photos_button) ui_element_destroy (photos_button->ui_element);
    if (settings_button) ui_element_destroy (settings_button->ui_element);

}

// FIXME:
GridLayout *grid = NULL;

void app_ui_init (void) {

    // FIXME:
    Renderer *main_renderer = renderer_get_by_name ("main");

    Font *font = ui_font_get_default ();

    u32 screen_width = main_renderer->window_size.width;
    u32 screen_height = main_renderer->window_size.height;

    /*** background ***/
    RGBA_Color electromagnetic = { 47, 54, 64, 255 };
    background_panel = ui_panel_create (0, 0, screen_width, screen_height, UI_POS_MIDDLE_CENTER, main_renderer);
    ui_panel_set_bg_colour (background_panel, main_renderer, electromagnetic);

    sidebar_init (screen_height);

    grid = ui_layout_grid_create (100, 0, screen_width - 100, screen_height);
    ui_layout_grid_set_grid (grid, 5, 4);

}

void app_ui_end (void) {

    sidebar_end ();

    ui_element_destroy (background_panel->ui_element);

}

#pragma region images

void app_ui_image_display_in_window (void *img_ptr) {

    if (img_ptr) {
        Image *image = (Image *) img_ptr;

        // FIXME: check if this size if possible
        // TODO: also we dont want to cover the full screen, we need to constraint to, lets say, max 80% of the full screen
        WindowSize window_size = { .width = image->sprite->img_data->w, .height =  image->sprite->img_data->h };
        Renderer *renderer = renderer_create_with_window ("test", 0, SDL_RENDERER_SOFTWARE | SDL_RENDERER_ACCELERATED,
            image->sprite->img_data->filename->str, window_size, 0);
    }

}

// TODO: first reduce the image and then render what we want
void app_ui_image_display (const char *filename) {

    if (filename) {
        // FIXME:
        Renderer *main_renderer = renderer_get_by_name ("main");

        Image *image = ui_image_create_static (0, 0);
        ui_image_set_pos (image, NULL, UI_POS_MIDDLE_CENTER, main_renderer);
        ui_image_set_sprite (image, main_renderer, filename);
        ui_image_set_ouline_colour (image, RGBA_WHITE);
        ui_image_set_outline_scale (image, 2, 2);
        ui_image_toggle_active (image);
        // ui_image_set_action (image, app_ui_image_display_in_window, image);
        RGBA_Color overlay_colour = { 255, 255, 255, 70 };
        ui_image_set_overlay (image, main_renderer, overlay_colour);

        ui_layout_grid_add_element (grid, image->ui_element);
    }

}

#pragma endregion