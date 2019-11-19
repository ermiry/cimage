#include <stdlib.h>

#include "cengine/manager/manager.h"
#include "cengine/game/go.h"

State *app_state = NULL;

static void app_update (void) {

    game_object_update_all ();

}

static void app_on_enter (void) { 

    app_state->update = app_update;


}

static void app_on_exit (void) { 


}

State *app_state_new (void) {

    State *new_app_state = (State *) malloc (sizeof (State));
    if (new_app_state) {
        // new_game_state->state = IN_GAME;

        new_app_state->update = NULL;

        new_app_state->on_enter = app_on_enter;
        new_app_state->on_exit = app_on_exit;
    }

}