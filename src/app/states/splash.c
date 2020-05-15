#include <stdlib.h>

#include "cengine/manager/manager.h"

#include "cengine/timer.h"

#include "app/states/app.h"
#include "app/ui/splash.h"

State *splash_state = NULL;

static Timer *timer = NULL;
static u32 time = 2000;

static void splash_update (void) {

	if (timer_get_ticks (timer) > time) {
		// go to main screen
		app_state = app_state_new ();
    	manager_state_change_state (app_state);
	}

}

static void splash_on_enter (void) { 

	splash_state->update = splash_update;

	splash_ui_init ();

	timer = timer_new ();
    timer_start (timer);

}

static void splash_on_exit (void) { 

	splash_ui_end ();

	timer_destroy (timer);

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