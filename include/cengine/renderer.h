#ifndef _CENGINE_RENDERER_H_
#define _CENGINE_RENDERER_H_

#include <stdbool.h>

#include <SDL2/SDL.h>

#include "cengine/types/types.h"

typedef struct WindowSize {

    u32 width, height;

} WindowSize;

typedef struct Renderer {

    String *name;
    SDL_Renderer *renderer;
    int index;
    Uint32 flags;

    int display_index;
    SDL_DisplayMode display_mode;

    String *window_title;
    SDL_Window *window;
    WindowSize window_size;
    bool full_screen;

} Renderer;

/*** Window ***/

// gets window size into renderer data struct
// returns 0 on success, 1 on error
extern int window_get_size (SDL_Window *window, WindowSize *window_size);

// toggle full screen on and off
// returns 0 on success, 1 on error
extern int window_toggle_full_screen (Renderer *renderer);

// resizes the window asscoaited with a renderer
// returns 0 on success, 1 on error
extern int window_resize (Renderer *renderer, u32 new_width, u32 new_height);

/*** Renderer ***/

// creates a new renderer
// returns 0 on success, 1 on error
extern int render_create_renderer (const char *renderer_name, Uint32 flags, int display_index,
    const char *window_title, WindowSize window_size, bool full_screen);

#endif