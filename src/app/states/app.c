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

#pragma region state

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

#pragma endregion