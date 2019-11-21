#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <SDL2/SDL_video.h>
#include <SDL2/SDL_surface.h>

#include "cengine/types/types.h"
#include "cengine/types/string.h"

#include "cengine/renderer.h"
#include "cengine/window.h"
#include "cengine/video.h"

#ifdef CENGINE_DEBUG
#include "cengine/utils/utils.h"
#include "cengine/utils/log.h"
#endif

int window_get_size (Window *window, WindowSize *window_size);

static Window *window_new (void) {

    Window *window = (Window *) malloc (sizeof (Window));
    if (window) {
        memset (window, 0, sizeof (Window));
        window->window = NULL;  
        window->window_title = NULL;

        window->icon = NULL;
    }

    return window;

}

void window_delete (void *window_ptr) {

    if (window_ptr) {
        Window *window = (Window *) window_ptr;
        if (window->window) SDL_DestroyWindow (window->window);
        str_delete (window->window_title);

        surface_delete (window->icon);

        free (window);
    }

}

// creates a new window with the requested arguments
Window *window_create (const char *title, WindowSize window_size, Uint32 window_flags,
    int display_idx) {

    Window *window = NULL;

    if (title) {
        window = window_new ();

        window->display_index = display_idx;

        if (!video_get_display_mode (window->display_index, &window->display_mode)) {
            u32 width = window_size.width;
            u32 height = window_size.height;

            // cap to maximum display size
            if (width > window->display_mode.w) width = window->display_mode.w;
            if (height > window->display_mode.h) height = window->display_mode.h;

            // creates a window of the size of the screen
            window->window = SDL_CreateWindow (title,
                SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                width, height, window_flags);

            // sets the actual window size in the correct places
            window_get_size (window, &window->window_size);

            window->window_title = title ? str_new (title) : NULL;
            window->window_flags = window_flags;
            window->fullscreen = window_flags & SDL_WINDOW_FULLSCREEN;
        }

        else {
            window_delete (window);
            window = NULL;
        }
    }

    return window;

}

// gets window size into renderer data struct
int window_get_size (Window *window, WindowSize *window_size) {

    int retval = 1;

    if (window) {
        SDL_GetWindowSize (window->window, &window_size->width, &window_size->height);
        #ifdef CENGINE_DEBUG
        cengine_log_msg (stdout, LOG_DEBUG, LOG_NO_TYPE, 
            c_string_create ("Window size: %dx%dpx.", window_size->width, window_size->height));
        #endif
        retval = 0;
    }

    return retval;

}

// FIXME: do we need to update the renderer?
// toggles window full screen on and off
int window_toggle_full_screen (Window *window) {

    int retval = 1;

    if (window) {
        window->fullscreen = SDL_GetWindowFlags (window->window) & SDL_WINDOW_FULLSCREEN;
        retval = SDL_SetWindowFullscreen (window->window, window->fullscreen ? 0 : SDL_WINDOW_FULLSCREEN);
        window->fullscreen = SDL_GetWindowFlags (window->window) & SDL_WINDOW_FULLSCREEN;
    }

    return retval;

}

// resizes the window asscoaited with a renderer
int window_resize (Window *window, u32 new_width, u32 new_height) {

    int retval = 1;

    if (window) {
        // cap to maximum display size
        u32 width = new_width;
        u32 height = new_height;

        if (width > window->display_mode.w) width = window->display_mode.w;
        if (height > window->display_mode.h) height = window->display_mode.h;

        SDL_SetWindowSize (window->window, new_width, new_height);

        // sets the actual window size in the correct places
        window_get_size (window, &window->window_size);

        SDL_RenderSetLogicalSize (window->renderer->renderer, 
            window->window_size.width, window->window_size.height);

        retval = 0;
    }

    return retval;

}

// sets the window's icon, the surface gets destroyed when the window is destroyed
void window_set_icon (Window *window, SDL_Surface *icon_surface) {

    if (window && icon_surface) {
        window->icon = icon_surface;
        SDL_SetWindowIcon (window->window, window->icon);
    }

}