#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include <signal.h>

#include "cengine/types/types.h"

#include "cengine/cengine.h"
#include "cengine/threads/thread.h"
#include "cengine/manager/manager.h"

#include "cengine/utils/utils.h"
#include "cengine/utils/log.h"

#include "cimage.h"

void cimage_quit (void) { running = false; }

static void cimage_quit_signal (int dummy) { running = false; }

void cimage_die (const char *error) {

    cengine_log_msg (stderr, LOG_ERROR, LOG_NO_TYPE, error);
    cimage_quit ();

};

static int cimage_init_ui (void) {

    u8 retval = 1;

    char *path = c_string_create ("%sui/default/", cengine_assets_path->str);
    if (path) {
        ui_default_assets_load ();
        Font *main_font = ui_font_create ("roboto", "./assets/fonts/Roboto-Regular.ttf");
        if (main_font) {
            ui_font_set_sizes (main_font, 6, 16, 20, 24, 32, 64, 200);
            ui_font_load (main_font, TTF_STYLE_NORMAL);

            retval = 0;
        }

        else cengine_log_msg (stderr, LOG_ERROR, LOG_NO_TYPE, "Failed to load main font!");

        free (path);
    }

    return retval;

}

int cimage_init (void) {

    int errors = 0;
    int retval = 0;

    // register to some signals
    signal (SIGINT, cimage_quit_signal);
    signal (SIGSEGV, cimage_quit_signal);

    cengine_set_quit (cimage_quit);
    cengine_assets_set_path ("./assets");

    // FIXME: load settings

    WindowSize window_size = { DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT };
    retval = cengine_init ("Cimage", window_size, false);
    // FIXME: we need better logs!!
    // FIXME: add custom log types and add integrate c_string_create
    if (retval) cengine_log_msg (stderr, LOG_ERROR, LOG_NO_TYPE, "Failed to init cengine!");
    errors |= retval;

    // TODO: init UI
    retval = cimage_init_ui ();
    if (retval) cengine_log_msg (stderr, LOG_ERROR, LOG_NO_TYPE, "Failed to init cimage's ui!");
    errors |= retval;

    return errors;

}

int cimage_end (void) {

    (void) cengine_end ();

    #ifdef CIMAGE_DEBUG
    cengine_log_msg (stdout, LOG_SUCCESS, LOG_NO_TYPE, "Done!");
    #endif

    return 0;

}