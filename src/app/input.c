#include "cengine/timer.h"

#include "cengine/ui/layout/grid.h"
#include "cengine/ui/panel.h"

#include "app/ui/app.h"

#pragma region general

int zoom_level = 0;

void zoom_more (void *args) {

    // get window with keyboard focus
    Window *window = NULL;
    for (ListElement *le = dlist_start (windows); le; le = le->next) {
        window = (Window *) le->data;
        if (window->keyboard) break;
    }

    if (!strcmp (window->renderer->name->str, "main")) {
        if (images_panel) {
            GridLayout *grid =  (GridLayout *) images_panel->layout;

            // FIXME: 14/05/2020 -- scroll not working with (grid->cols - 1) > 1
            if ((grid->cols - 1) > 2) {
                Renderer *main_renderer = renderer_get_by_name ("main");

                u32 window_width = main_renderer->window->window_size.width;
                u32 window_height = main_renderer->window->window_size.height;

                zoom_level -= 1;

                u32 cols = (grid->cols - 1);
                u32 rows = 4 + zoom_level;
                // printf ("\n\ncols %d - rows %d\n\n", cols, rows);

                u32 cell_width = (window_width - 100) / cols;
                u32 cell_height = (window_height / rows);

                ui_layout_grid_update_size (grid, cols, cell_width, cell_height);

                app_ui_statusbar_zoom_set (zoom_level);

                // printf ("+\n");
            }
        }
    }

}

void zoom_less (void *args) {

    // get window with keyboard focus
    Window *window = NULL;
    for (ListElement *le = dlist_start (windows); le; le = le->next) {
        window = (Window *) le->data;
        if (window->keyboard) break;
    }

    if (!strcmp (window->renderer->name->str, "main")) {
        if (images_panel) {
            GridLayout *grid =  (GridLayout *) images_panel->layout;

            if ((grid->cols + 1) < 20) {
                Renderer *main_renderer = renderer_get_by_name ("main");

                u32 window_width = main_renderer->window->window_size.width;
                u32 window_height = main_renderer->window->window_size.height;
                
                zoom_level += 1;

                u32 cols = (grid->cols + 1);
                u32 rows = 4 + zoom_level;
                // printf ("\n\ncols %d - rows %d\n\n", cols, rows);

                u32 cell_width = (window_width - 100) / cols;
                u32 cell_height = (window_height / rows);

                ui_layout_grid_update_size (grid, cols, cell_width, cell_height);

                app_ui_statusbar_zoom_set (zoom_level);

                // printf ("-\n");
            }
        }
    }

}

#pragma endregion

#pragma region main screen

static bool once = false;

static Timer *alt_key_timer = NULL;
static u32 alt_key_cooldown = 500;

void main_screen_input (void *win_ptr) {

    if (win_ptr) {
        Window *win = (Window *) win_ptr;

        if (win->keyboard) {
            if (input_is_key_down (SDL_SCANCODE_F11)) {
                // make window fullscreen
                if (!once) {
                    window_toggle_fullscreen_soft (win);
                    once = true;
                }
            }

            else if (input_is_key_down (SDL_SCANCODE_LALT) || input_is_key_down (SDL_SCANCODE_RALT)) {
                if (timer_get_ticks (alt_key_timer) > alt_key_cooldown) {
                    // toggle display actions menu
                    if (images_panel) app_ui_actionsbar_toggle ();

                    timer_start (alt_key_timer);
                }
            }

            else if (input_is_key_down (SDL_SCANCODE_UP)) {
                // move images up
                // app_ui_images_move_down (10);
                ui_layout_grid_scroll_up ((GridLayout *) images_panel->layout, 1);
            }

            else if (input_is_key_down (SDL_SCANCODE_DOWN)) {
                // move images down
                // app_ui_images_move_up (10);
                ui_layout_grid_scroll_down ((GridLayout *) images_panel->layout, -1);
            }
        }
    }

}

#pragma endregion

#pragma region main

void cimage_input_init (void) {

    alt_key_timer = timer_new ();
    timer_start (alt_key_timer);

}

void cimage_input_end (void) {

    timer_destroy (alt_key_timer);

}

#pragma endregion