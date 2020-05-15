#include "cengine/version.h"
#include "cengine/cengine.h"
#include "cengine/manager/manager.h"

#include "cimage.h"
#include "version.h"
#include "app/states/splash.h"
#include "app/states/app.h"

int main (void) {

    cengine_version_print_full ();

    cimage_version_print_full ();

    running = !cimage_init () ? true : false;

    // splash_state = splash_state_new ();
    // manager = manager_new (splash_state);

    app_state = app_state_new ();
    manager = manager_new (app_state);

    cengine_start (FPS_LIMIT);

    return cimage_end ();

}