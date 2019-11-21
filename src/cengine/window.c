#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <SDL2/SDL_video.h>
#include <SDL2/SDL_surface.h>

#include "cengine/types/types.h"
#include "cengine/types/string.h"

#include "cengine/renderer.h"
#include "cengine/window.h"

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
Window *window_create (const char *title, WindowSize window_size, Uint32 window_flags) {

    Window *window = NULL;

    if (title) {
        window = window_new ();

        // FIXME: get maximum size possible
        // creates a window of the size of the screen
        window->window = SDL_CreateWindow (title,
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
            window_size.width, window_size.height, window_flags);
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

// FIXME:
// toggles window full screen on and off
int window_toggle_full_screen (Window *window) {

    int retval = 1;

    // if (renderer) {
    //     renderer->fullscreen = SDL_GetWindowFlags (renderer->window) & SDL_WINDOW_FULLSCREEN;
    //     retval = SDL_SetWindowFullscreen (renderer->window, renderer->fullscreen ? 0 : SDL_WINDOW_FULLSCREEN);
    //     renderer->fullscreen = SDL_GetWindowFlags (renderer->window) & SDL_WINDOW_FULLSCREEN;
    // }

    return retval;

}

// FIXME:
// FIXME: do we need to update the renderer?
// resizes the window asscoaited with a renderer
int window_resize (Window *window, u32 new_width, u32 new_height) {

    int retval = 1;

    // if (renderer) {
        // check if we have a valid new size
        // if (new_width <= renderer->display_mode.w && new_width > 0 &&
        //     new_height <= renderer->display_mode.h && new_height > 0) {
        //     SDL_SetWindowSize (renderer->window, new_width, new_height);
        //     window_get_size (renderer->window, &renderer->window_size);
        //     retval = 0;
        // }
    // }

    return retval;

}

// sets the window's icon, the surface gets destroyed when the window is destroyed
void window_set_icon (Window *window, SDL_Surface *icon_surface) {

    if (window && icon_surface) {
        window->icon = icon_surface;
        SDL_SetWindowIcon (window->window, window->icon);
    }

}