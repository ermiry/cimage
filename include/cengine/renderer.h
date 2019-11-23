#ifndef _CENGINE_RENDERER_H_
#define _CENGINE_RENDERER_H_

#include <stdbool.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_surface.h>

#include "cengine/types/types.h"
#include "cengine/types/string.h"

#include "cengine/collections/dlist.h"
#include "cengine/collections/queue.h"

#include "cengine/threads/thread.h"

#include "cengine/graphics.h"
#include "cengine/window.h"

#include "cengine/ui/ui.h"

#define DEFAULT_BG_LOADING_FACTOR               1

struct _Window;
struct _UI;

// auxiliary structure to map the source surface to a texture
typedef struct SurfaceTexture {

    SDL_Surface *surface;
    SDL_Texture **texture;

} SurfaceTexture;

extern SurfaceTexture *surface_texture_new (SDL_Surface *surface, SDL_Texture **texture);

extern void surface_texture_delete (void *st_ptr);

/*** Renderer ***/

struct _Renderer {

    u64 id;

    String *name;
    pthread_t thread_id;

    SDL_Renderer *renderer;
    Uint32 render_flags;

    queue_t *textures_queue;
    u32 bg_loading_factor;

    struct _Window *window;

    struct _UI *ui;

};

typedef struct _Renderer Renderer;

extern DoubleList *renderers;

extern void renderer_delete (void *ptr);

// gets the renderer by its name
extern Renderer *renderer_get_by_name (const char *name);

// creates a new empty renderer without a window attached to it
extern Renderer *renderer_create_empty (const char *name, int display_idx);

// creates a new renderer with a window attached to it
extern Renderer *renderer_create_with_window (const char *name, int display_idx,
    Uint32 render_flags,
    const char *window_title, WindowSize window_size, Uint32 window_flags);

// attaches a new window to a renderer
// creates a new window and then a new render (SDL_Renderer) for it
// retunrs 0 on success, 1 on error
extern int renderer_window_attach (Renderer *renderer, Uint32 render_flags, int display_idx,
    const char *window_title, WindowSize window_size, Uint32 window_flags);

extern int renderer_init_main (Uint32 flags,
    const char *window_title, WindowSize window_size, Uint32 window_flags);

extern void renderer_delete_main (void);

extern void renderer_queue_push (Renderer *renderer, SurfaceTexture *st);

// sets how many textures the renderer can create in the background every loop
// example: if you have a frame rate of 30, the default loading factor is 1,
// so you will load 30 textures in a second, 1 for each frame
extern void renderer_set_background_texture_loading_factor (Renderer *renderer, u32 bg_loading_factor);

/*** Layers ***/

typedef struct Layer {

    String *name;
    int pos;
    DoubleList *elements;

} Layer;

extern DoubleList *gos_layers;              // render layers for the gameobjects

extern Layer *layer_get_by_pos (DoubleList *layers, int pos);

extern Layer *layer_get_by_name (DoubleList *layers, const char *name);

// creates a new layer; 
// takes the layer name and the layer pos, -1 for last layer
// pos 0 renders first
// returns 0 on success, 1 on error
extern int layer_create (DoubleList *layers, const char *name, int pos, bool gos);

// adds an element to a layer
// returns 0 on succes, 1 on error
extern int layer_add_element (Layer *layer, void *ptr);

// adds an element to a layer that is gotten by its name 
// returns 0 on succes, 1 on error
extern int layer_add_element_by_name (DoubleList *layers, const char *layer_name, void *ptr);

// removes an element from a layer
// returns 0 on succes, 1 on error
extern int layer_remove_element (Layer *layer, void *ptr);

// removes an element from a layer taht is gotten by its name
// returns 0 on succes, 1 on error
extern int layer_remove_element_by_name (DoubleList *layers, const char *layer_name, void *ptr);

// init the ui elements layers
extern DoubleList *ui_layers_init (void);

/*** Surfaces ***/

// creates a new empty surface
extern SDL_Surface *surface_create (int width, int height);

// loads an image into a new surface
extern SDL_Surface *surface_load_image (const char *filename);

// wrapper function to destroy a sdl surface
extern void surface_delete (SDL_Surface *surface);

/*** Render Basic ***/

// renders a dot
extern void render_basic_dot (Renderer *renderer, int x, int y, SDL_Color color,
    float x_scale, float y_scale);

// renders a horizontal line of dots
extern void render_basic_dot_line_horizontal (Renderer *renderer, int start, int end, int y, int offset, SDL_Color color,
    float x_scale, float y_scale);

// renders a vertical line of dots
extern void render_basic_dot_line_vertical (Renderer *renderer, int start, int end, int x, int offset, SDL_Color color,
    float x_scale, float y_scale);

// renders a filled rect
extern void render_basic_filled_rect (Renderer *renderer,SDL_Rect *rect, SDL_Color color);

// renders an outline rect
// scale works better with even numbers
extern void render_basic_outline_rect (Renderer *renderer, SDL_Rect *rect, SDL_Color color, float scale_x, float scale_y);

// renders a line
// scale works better with even numbers
extern void render_basic_line (Renderer *renderer, int x1, int x2, int y1, int y2, SDL_Color color, float scale_x, float scale_y);

/*** Render Complex ***/

// renders a rect with transparency
extern void render_complex_transparent_rect (Renderer *renderer, SDL_Texture **texture, SDL_Rect *rect, SDL_Color color);

/*** Render ***/

extern void render (Renderer *renderer);

/*** Public ***/

// inits cengine render capabilities
extern u8 render_init (void);

extern void render_end (void);

#endif