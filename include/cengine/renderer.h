#ifndef _CENGINE_RENDERER_H_
#define _CENGINE_RENDERER_H_

#include <stdbool.h>

#include <SDL2/SDL.h>

#include "cengine/types/types.h"
#include "cengine/types/string.h"

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
// returns a new renderer on success, NULL on error
extern Renderer *render_create_renderer (const char *renderer_name, Uint32 flags, int display_index,
    const char *window_title, WindowSize window_size, bool full_screen);

// TODO: as of 03/06/2019 we only have support for one renderer, the main one
// the plan is to have as many as you want in order to support multiple windows 
extern Renderer *main_renderer;

extern int renderer_init_main (Uint32 flags,
    const char *window_title, WindowSize window_size, bool full_screen);

extern void renderer_delete_main (void);

/*** Render func ***/

extern void render (void);

#endif