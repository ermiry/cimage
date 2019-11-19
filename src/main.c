#include "cengine/cengine.h"
#include "cengine/manager/manager.h"

#include "cimage.h"
#include "app/states/app.h"

int main (void) {

    running = !cimage_init () ? true : false;

    app_state = app_state_new ();
    manager = manager_new (app_state);

    cengine_start (FPS_LIMIT);

    return cimage_end ();

}