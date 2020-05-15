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
#include "app/input.h"
#include "app/ui/app.h"

#pragma region cimage

Cimage *cimage = NULL;

static Cimage *cimage_new (void) {

    Cimage *cimage = (Cimage *) malloc (sizeof (Cimage));
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

State *app_state = NULL;

static void app_update (void) {

    // game_object_update_all ();

}

static void app_on_enter (void) { 

    input_command_register (SDLK_EQUALS, zoom_more, NULL);
    input_command_register (SDLK_MINUS, zoom_less, NULL);

    app_state->update = app_update;

    app_ui_init ();

    Renderer *main_renderer = renderer_get_by_name ("main");
    renderer_set_update (main_renderer, main_renderer_update, main_renderer);

    cimage_input_init ();

}

static void app_on_exit (void) { 

    cimage_input_end ();

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