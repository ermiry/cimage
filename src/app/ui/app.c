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

#include "app/states/app.h"

static Panel *background_panel = NULL;
static Panel *sidebar = NULL;
static Panel *images_panel = NULL;

static Button *photos_button = NULL;
static Button *presentation_button = NULL;
static Button *settings_button = NULL;

static Button *open_folder_button = NULL;
static TextBox *open_folder_text = NULL;

static SDL_Texture *overlay_texture = NULL;

// FIXME: set actions
static void sidebar_init (u32 screen_height) {

    // FIXME:
    Renderer *main_renderer = renderer_get_by_name ("main");

    RGBA_Color blue_night = { 53, 59, 72, 255 };

    sidebar = ui_panel_create (0, 0, 100, screen_height, UI_POS_LEFT_UPPER_CORNER, main_renderer);
    ui_panel_set_bg_colour (sidebar, main_renderer, blue_night);
    ui_element_set_layer (main_renderer->ui, sidebar->ui_element, "top");

    photos_button = ui_button_create (18, 18, 64, 64, UI_POS_LEFT_UPPER_CORNER, main_renderer);
    // ui_button_set_action (photos_button, NULL, NULL);
    // ui_button_set_ouline_colour (photos_button, RGBA_WHITE);
    ui_button_set_sprite (photos_button, main_renderer, BUTTON_STATE_MOUSE_OUT, "./assets/icons/gallery.png");
    ui_element_set_layer (main_renderer->ui, photos_button->ui_element, "top");

    presentation_button = ui_button_create (18, 118, 64, 64, UI_POS_LEFT_UPPER_CORNER, main_renderer);
    // ui_button_set_action (photos_button, NULL, NULL);
    // ui_button_set_ouline_colour (photos_button, RGBA_WHITE);
    ui_button_set_sprite (presentation_button, main_renderer, BUTTON_STATE_MOUSE_OUT, "./assets/icons/presentation.png");
    ui_element_set_layer (main_renderer->ui, presentation_button->ui_element, "top");

    settings_button = ui_button_create (18, -18, 64, 64, UI_POS_LEFT_BOTTOM_CORNER, main_renderer);
    // ui_button_set_action (settings_button, game_state_change, change_main_state_settings);
    // ui_button_set_ouline_colour (settings_button, RGBA_WHITE);
    ui_button_set_sprite (settings_button, main_renderer, BUTTON_STATE_MOUSE_OUT, "./assets/icons/settings.png");
    ui_element_set_layer (main_renderer->ui, settings_button->ui_element, "top");

}

static void sidebar_end (void) {

    if (sidebar) ui_element_destroy (sidebar->ui_element);

    if (photos_button) ui_element_destroy (photos_button->ui_element);
    if (presentation_button) ui_element_destroy (presentation_button->ui_element);
    if (settings_button) ui_element_destroy (settings_button->ui_element);

}

void app_ui_init (void) {

    // FIXME:
    Renderer *main_renderer = renderer_get_by_name ("main");

    Font *font = ui_font_get_default ();

    u32 screen_width = main_renderer->window->window_size.width;
    u32 screen_height = main_renderer->window->window_size.height;

    /*** background ***/
    RGBA_Color electromagnetic = { 47, 54, 64, 255 };
    background_panel = ui_panel_create (0, 0, screen_width, screen_height, UI_POS_MIDDLE_CENTER, main_renderer);
    ui_panel_set_bg_colour (background_panel, main_renderer, electromagnetic);
    ui_element_set_layer (main_renderer->ui, background_panel->ui_element, "back");

    sidebar_init (screen_height);

    open_folder_button = ui_button_create (0, 0, 128, 128, UI_POS_MIDDLE_CENTER, main_renderer);
    ui_button_set_action (open_folder_button, images_folder_select, NULL);
    // ui_button_set_ouline_colour (photos_button, RGBA_WHITE);
    ui_button_set_sprite (open_folder_button, main_renderer, BUTTON_STATE_MOUSE_OUT, "./assets/icons/folder.png");
    ui_element_set_layer (main_renderer->ui, open_folder_button->ui_element, "top");

    open_folder_text = ui_textbox_create (0, 100, 200, 50, UI_POS_MIDDLE_CENTER, main_renderer);
    ui_textbox_set_text (open_folder_text, main_renderer, "Open a photos folder", font, 24, RGBA_WHITE, false);
    ui_textbox_set_text_pos (open_folder_text, UI_POS_MIDDLE_CENTER);

    SDL_Rect rect = { .x = 0, .y = 0, .w = screen_width, .h = screen_height };
    RGBA_Color overlay_colour = { 255, 255, 255, 70 };
    render_complex_transparent_rect (main_renderer, &overlay_texture, &rect, overlay_colour); 

}

void app_ui_end (void) {

    sidebar_end ();

    ui_element_destroy (background_panel->ui_element);

    if (overlay_texture) SDL_DestroyTexture (overlay_texture);

}

#pragma region images

void app_ui_images_move_up (u32 movement) {

    if (images_panel) {
        Renderer *main_renderer = renderer_get_by_name ("main");
        u32 screen_height = main_renderer->window->window_size.height;

        GridLayout *grid = (GridLayout *) images_panel->layout;
        if ((abs (images_panel->transform->rect.y) + grid->cell_height) < (images_panel->transform->rect.h)) {
            images_panel->transform->rect.y -= movement;
            grid->transform->rect.y -= movement;

            for (u32 i = 0; i < grid->cols; i++) {
                for (u32 j = 0; j < grid->rows; j++) {
                    if (grid->ui_elements[i][j]) {
                        Image *image = (Image *) grid->ui_elements[i][j]->element;
                        image->transform->rect.y -= movement;
                    }
                }
            }
        }
    }

}

void app_ui_images_move_down (u32 movement) {

    if (images_panel) {
        Renderer *main_renderer = renderer_get_by_name ("main");
        u32 screen_height = main_renderer->window->window_size.height;

        if (images_panel->transform->rect.y < 0) {
            images_panel->transform->rect.y += movement;
            GridLayout *grid = (GridLayout *) images_panel->layout;
            grid->transform->rect.y += movement;

            for (u32 i = 0; i < grid->cols; i++) {
                for (u32 j = 0; j < grid->rows; j++) {
                    if (grid->ui_elements[i][j]) {
                        Image *image = (Image *) grid->ui_elements[i][j]->element;
                        image->transform->rect.y += movement;
                    }
                }
            }
        }

        else {
            images_panel->transform->rect.y = 0;
            GridLayout *grid = (GridLayout *) images_panel->layout;
            grid->transform->rect.y = 0;
        }
    }

}

// prepare the ui for the images to be displayed
void app_ui_images_set_ui_elements (u32 n_images, u32 n_cols, u32 n_rows) {

    // FIXME:
    Renderer *main_renderer = renderer_get_by_name ("main");

    u32 screen_width = main_renderer->window->window_size.width;
    u32 screen_height = main_renderer->window->window_size.height;

    // remove past buttons
    open_folder_button->ui_element->active = false;
    open_folder_text->ui_element->active = false;

    u32 n_actual_rows = n_images / n_cols;         // total rows for the layout
    n_actual_rows += 1;

    u32 panel_width = (screen_width - 100);
    u32 panel_height = (screen_height / n_rows) * n_actual_rows;

    images_panel = ui_panel_create (100, 0, panel_width, panel_height, UI_POS_FREE, main_renderer);
    ui_panel_layout_set (images_panel, LAYOUT_TYPE_GRID);

    u32 cell_width = (screen_width - 100) / n_cols;
    u32 cell_height = (screen_height / n_rows);

    GridLayout *grid = (GridLayout *) images_panel->layout;
    ui_layout_grid_set_grid (grid, n_cols, n_actual_rows);
    ui_layout_grid_set_cell_size (grid, cell_width, cell_height);

}

void app_ui_image_display_in_window (void *img_ptr) {

    if (img_ptr) {
        Image *image = (Image *) img_ptr;

        // FIXME: check if this size if possible
        // TODO: also we dont want to cover the full screen, we need to constraint to, lets say, max 80% of the full screen
        WindowSize window_size = { .width = image->sprite->img_data->w, .height =  image->sprite->img_data->h };
        Renderer *renderer = renderer_create_with_window ("test", 0, SDL_RENDERER_SOFTWARE | SDL_RENDERER_ACCELERATED,
            image->sprite->img_data->filename->str, window_size, 0);

        // load a new image
        Image *new_image = ui_image_create_static (0, 0, renderer);
        ui_image_set_pos (new_image, NULL, UI_POS_LEFT_UPPER_CORNER, renderer);
        ui_image_set_sprite (new_image, renderer, image->sprite->img_data->filename->str);
        // ui_image_set_dimensions (new_image, renderer->window->window_size.width, renderer->window->window_size.height);
    }

}

// TODO: first reduce the image and then render what we want
void app_ui_image_display (const char *filename) {

    if (filename) {
        // FIXME:
        Renderer *main_renderer = renderer_get_by_name ("main");

        Image *image = ui_image_create_static (0, 0, main_renderer);
        ui_image_set_pos (image, NULL, UI_POS_MIDDLE_CENTER, main_renderer);
        ui_image_set_sprite (image, main_renderer, filename);
        ui_image_set_ouline_colour (image, RGBA_WHITE);
        ui_image_set_outline_scale (image, 2, 2);
        ui_image_toggle_active (image);
        ui_image_set_overlay_ref (image, overlay_texture);
        ui_image_set_action (image, app_ui_image_display_in_window, image);

        ui_panel_layout_add_element (images_panel, image->ui_element);
    }

}

#pragma endregion