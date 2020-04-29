#include <stdlib.h>

#include "cengine/manager/manager.h"

SessionType sessionType;

Manager *manager = NULL;

Manager *manager_new (State *init_state) {

    Manager *new_manager = (Manager *) malloc (sizeof (Manager));
    if (new_manager) {
        if (init_state) {
            new_manager->curr_state = init_state;
        
            if (new_manager->curr_state->on_enter)
                new_manager->curr_state->on_enter ();
        }

        else new_manager->curr_state = NULL;
    } 

    return new_manager;

}

void manager_delete (Manager *manager) {

    if (manager) {
        if (manager->curr_state) free (manager->curr_state);
        free (manager);
    }

}

State *manager_state_get_current (void) { return manager->curr_state; }

void manager_state_change_state (State *new_state) { 
    
    if (manager->curr_state->on_exit)
        manager->curr_state->on_exit ();

    manager->curr_state = new_state; 
    if (manager->curr_state->on_enter)
        manager->curr_state->on_enter ();
    
}