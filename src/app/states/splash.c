#include <stdlib.h>

#include "cengine/manager/manager.h"

State *splash_state = NULL;

static void splash_update (void) {

	// game_object_update_all ();

}

static void splash_on_enter (void) { 

	splash_state->update = splash_update;

	// app_ui_init ();

}

static void splash_on_exit (void) { 

	// splash_ui_end ();

}

State *splash_state_new (void) {

	State *new_splash_state = (State *) malloc (sizeof (State));
	if (new_splash_state) {
		// new_game_state->state = IN_GAME;

		new_splash_state->update = NULL;

		new_splash_state->on_enter = splash_on_enter;
		new_splash_state->on_exit = splash_on_exit;
	}

	return new_splash_state;

}