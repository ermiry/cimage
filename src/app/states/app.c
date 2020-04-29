#include <stdlib.h>

#ifdef CIMAGE_DEBUG
#include <errno.h>
#endif

#include "cengine/types/types.h"
#include "cengine/types/string.h"

#include "cengine/collections/dlist.h"

#include "cengine/input.h"

#include "cengine/manager/manager.h"
// #include "cengine/game/go.h"

#include "cengine/ui/ui.h"
#include "cengine/ui/image.h"
#include "cengine/ui/inputfield.h"
#include "cengine/ui/layout/grid.h"

#include "cengine/utils/utils.h"
#include "cengine/utils/log.h"

#include "cimage.h"
#include "app/states/app.h"
#include "app/media.h"
#include "app/ui/app.h"

#pragma region cimage

Cimage *cimage = NULL;

static Cimage *cimage_new (void) {

    Cimage *cimage = (Cimage *) malloc (sizeof (cimage));
    if (cimage) {
        cimage->opened_folder_name = NULL;
        cimage->images = NULL;
        cimage->selected_images = NULL;
    }

    return cimage;

}

void cimage_delete (void *cimage_ptr) {

    if (cimage_ptr) {
        Cimage *cimage = (Cimage *) cimage_ptr;

        str_delete (cimage->opened_folder_name);
        dlist_delete (cimage->images);
        // FIXME: segfault if list is empty
        // dlist_delete (cimage->selected_images);

        free (cimage_ptr);
    }

}

Cimage *cimage_create (void) {

    Cimage *cimage = cimage_new ();
    if (cimage) cimage->selected_images = dlist_init (media_item_delete_dummy, NULL);
    return cimage;

}

#pragma endregion

static char *last_sub = NULL;

// TODO: pass the rendrer also
static void main_renderer_update (void *args) {

    // TODO: make this an option!!
    bool done = false;
    // Renderer *main_renderer = renderer_get_by_name ("main");
    Renderer *main_renderer = (Renderer *) args;
    UIElement *hover_element = ui_element_hover_get (main_renderer->ui);
    if (hover_element) {
        if (hover_element->type == UI_IMAGE) {
            Image *image = (Image *) hover_element->element;
            char *sub = NULL;
            char *retval = c_string_remove_sub_after_token_with_idx (image->sprite->img_data->filename->str, '/', &sub, -1);
            if (retval && sub) {
                if (!last_sub) {
                    last_sub = sub;
                    app_ui_statusbar_set_selected_text (sub);
                }

                else {
                    if (strcmp (last_sub, sub)) {
                        free (last_sub);
                        last_sub = sub;
                        app_ui_statusbar_set_selected_text (sub);
                    }
                }

                free (retval);

                done = true;
            }
        }
    }

    if (!done) {
        app_ui_statusbar_set_selected_text (NULL);
        free (last_sub);
        last_sub = NULL;
    } 

}

#pragma region global input

#include "cengine/ui/layout/grid.h"
#include "cengine/ui/panel.h"

extern Panel *images_panel;

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
            ui_layout_grid_update_dimensions (grid, grid->cols - 1, grid->rows - 1);
            // printf ("+\n");
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
            ui_layout_grid_update_dimensions (grid, grid->cols + 1, grid->rows + 1);
            // printf ("-\n");
        }
    }

}

#pragma endregion

#pragma region main screen

#include "cengine/timer.h"

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
                app_ui_images_move_down (10);
            }

            else if (input_is_key_down (SDL_SCANCODE_DOWN)) {
                // move images down
                app_ui_images_move_up (10);
            }
        }
    }

}

#pragma endregion

// TODO: make this an option
int mouse_sensitivity = 50;

void images_move_up_scroll_wheel (void *amount_ptr) {

    if (amount_ptr) {
        int *amount = (int *) amount_ptr;

        if (images_panel) {
            Renderer *main_renderer = renderer_get_by_name ("main");
            u32 screen_height = main_renderer->window->window_size.height;

            GridLayout *grid = (GridLayout *) images_panel->layout;
            if ((abs (images_panel->ui_element->transform->rect.y) + grid->cell_height) < (images_panel->ui_element->transform->rect.h)) {
                // this is positive because when moving the scroll wheel down, we get negative numbers
                images_panel->ui_element->transform->rect.y += (*amount * mouse_sensitivity);
                grid->transform->rect.y += (*amount * mouse_sensitivity);

                GridElement *grid_element = NULL;
                for (ListElement *le = dlist_start (grid->elements); le; le = le->next) {
                    grid_element = (GridElement *) le->data;
                    grid_element->ui_element->transform->rect.y += (*amount * mouse_sensitivity);
                }
            }
        }
    }

}

void images_move_down_scroll_wheel (void *amount_ptr) {

    if (amount_ptr) {
        int *amount = (int *) amount_ptr;

        if (images_panel) {
            Renderer *main_renderer = renderer_get_by_name ("main");
            u32 screen_height = main_renderer->window->window_size.height;

            GridLayout *grid = (GridLayout *) images_panel->layout;
            if ((abs (images_panel->ui_element->transform->rect.y) + grid->cell_height) < (images_panel->ui_element->transform->rect.h)) {
                images_panel->ui_element->transform->rect.y += (*amount * mouse_sensitivity);
                grid->transform->rect.y += (*amount * mouse_sensitivity);

                GridElement *grid_element = NULL;
                for (ListElement *le = dlist_start (grid->elements); le; le = le->next) {
                    grid_element = (GridElement *) le->data;
                    grid_element->ui_element->transform->rect.y += (*amount * mouse_sensitivity);
                }
            }
        }
    }

}

State *app_state = NULL;

static void app_update (void) {

    // game_object_update_all ();

}

static void app_on_enter (void) { 

    input_command_register (SDLK_EQUALS, zoom_more, NULL);
    input_command_register (SDLK_MINUS, zoom_less, NULL);

    input_set_on_mouse_wheel_scroll_up (images_move_up_scroll_wheel);
    input_set_on_mouse_wheel_scroll_down (images_move_down_scroll_wheel);

    app_state->update = app_update;

    app_ui_init ();

    Renderer *main_renderer = renderer_get_by_name ("main");
    renderer_set_update (main_renderer, main_renderer_update, main_renderer);

    alt_key_timer = timer_new ();
    timer_start (alt_key_timer);

}

static void app_on_exit (void) { 

    timer_destroy (alt_key_timer);

    app_ui_end ();

    cimage_delete (cimage);

}

State *app_state_new (void) {

    State *new_app_state = (State *) malloc (sizeof (State));
    if (new_app_state) {
        // new_game_state->state = IN_GAME;

        new_app_state->update = NULL;

        new_app_state->on_enter = app_on_enter;
        new_app_state->on_exit = app_on_exit;
    }

    return new_app_state;

}