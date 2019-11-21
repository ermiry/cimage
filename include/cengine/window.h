#ifndef _CENGINE_WINDOW_H_
#define _CENGINE_WINDOW_H_

#include <stdbool.h>

#include <SDL2/SDL_video.h>

#include "cengine/types/types.h"
#include "cengine/types/string.h"

#include "cengine/renderer.h"

struct _Renderer;

typedef struct WindowSize {

    u32 width, height;

} WindowSize;

struct _Window {

    SDL_Window *window;
    Uint32 window_flags;

    String *window_title;
    WindowSize window_size;
    bool fullscreen;

};

typedef struct _Window Window;

// gets window size into renderer data struct
// returns 0 on success, 1 on error
extern int window_get_size (SDL_Window *window, WindowSize *window_size);

// toggle full screen on and off
// returns 0 on success, 1 on error
extern int window_toggle_full_screen (struct _Renderer *renderer);

// resizes the window asscoaited with a renderer
// returns 0 on success, 1 on error
extern int window_resize (struct _Renderer *renderer, u32 new_width, u32 new_height);

// sets the window's icon
extern void window_set_icon (SDL_Window *window, SDL_Surface *icon_surface);

#endif