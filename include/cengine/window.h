#ifndef _CENGINE_WINDOW_H_
#define _CENGINE_WINDOW_H_

#include <stdbool.h>

#include <SDL2/SDL_video.h>
#include <SDL2/SDL_surface.h>

#include "cengine/types/types.h"
#include "cengine/types/string.h"

#include "cengine/renderer.h"

struct _Renderer;

typedef struct WindowSize {

    u32 width, height;

} WindowSize;

struct _Window {

    int display_index;
    SDL_DisplayMode display_mode;

    SDL_Window *window;
    Uint32 window_flags;

    String *window_title;
    WindowSize window_size;
    bool fullscreen;

    SDL_Surface *icon;

    // reference to the window renderer
    struct _Renderer *renderer;

};

typedef struct _Window Window;

extern void window_delete (void *window_ptr);

// creates a new window with the requested arguments
extern Window *window_create (const char *title, WindowSize window_size, Uint32 window_flags);

// gets window size into renderer data struct
extern int window_get_size (Window *window, WindowSize *window_size);

// toggles window full screen on and off
extern int window_toggle_full_screen (Window *window);

// resizes the window asscoaited with a renderer
extern int window_resize (Window *window, u32 new_width, u32 new_height);

// sets the window's icon, the surface gets destroyed when the window is destroyed
extern void window_set_icon (Window *window, SDL_Surface *icon_surface);

#endif