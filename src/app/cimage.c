#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include <signal.h>

#include "cengine/types/types.h"

#include "cengine/cengine.h"
#include "cengine/assets.h"

#include "cengine/threads/thread.h"
#include "cengine/manager/manager.h"

#include "cengine/ui/font.h"
#include "cengine/ui/notification.h"

#include "cengine/utils/utils.h"
#include "cengine/utils/log.h"

#include "cimage.h"
#include "app/settings.h"
#include "app/states/app.h"

void cimage_quit (void) { running = false; }

static void cimage_quit_signal (int dummy) { running = false; }

void cimage_die (const char *error) {

    cengine_log_msg (stderr, LOG_ERROR, LOG_NO_TYPE, error);
    cimage_quit ();

};

// Renderer *main_renderer = NULL;
SDL_Surface *icon_surface = NULL;

NotiCenter *main_noti_center = NULL;

static int cimage_init_ui (Renderer *renderer) {

    u8 retval = 1;

    char *path = c_string_create ("%sui/default/", cengine_assets_path->str);
    if (path) {
        ui_default_assets_load ();
        Font *main_font = ui_font_create ("roboto", "./assets/fonts/Roboto-Regular.ttf");
        if (main_font) {
            ui_font_set_sizes (main_font, 6, 16, 20, 24, 32, 64, 200);
            ui_font_load (main_font, renderer, TTF_STYLE_NORMAL);

            main_noti_center = ui_noti_center_create (renderer->ui, 3, UI_POS_RIGHT_BOTTOM_CORNER, renderer);
            if (main_noti_center) ui_element_set_layer (renderer->ui, main_noti_center->ui_element, "top");

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

    retval = cengine_init ();
    if (retval) cengine_log_msg (stderr, LOG_ERROR, LOG_NO_TYPE, "Failed to init cengine!");
    errors |= retval;

    // load settings
    main_settings = settings_create ();

    // create our main renderer
    WindowSize window_size = { DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT };
    Renderer *main_renderer = renderer_create_with_window ("main", 0, SDL_RENDERER_SOFTWARE | SDL_RENDERER_ACCELERATED,
        "Cimage", window_size, 0);
    window_set_user_input (main_renderer->window, main_screen_input);

    icon_surface = surface_load_image ("./assets/cimage-128.png");
    if (icon_surface) window_set_icon (main_renderer->window, icon_surface);
    else cengine_log_msg (stderr, LOG_ERROR, LOG_NO_TYPE, "Failed to load icon!");

    retval = cimage_init_ui (main_renderer);
    if (retval) cengine_log_msg (stderr, LOG_ERROR, LOG_NO_TYPE, "Failed to init cimage's ui!");
    errors |= retval;

    return errors;

}

int cimage_end (void) {

    settings_delete (main_settings);

    // surface_delete (icon_surface);

    (void) cengine_end ();

    #ifdef CIMAGE_DEBUG
    cengine_log_msg (stdout, LOG_SUCCESS, LOG_NO_TYPE, "Done!");
    #endif

    return 0;

}