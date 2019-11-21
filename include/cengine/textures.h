#ifndef _CENGINE_TEXTURES_H_
#define _CENGINE_TEXTURES_H_

#include <SDL2/SDL.h>

#include "cengine/types/types.h"
#include "cengine/types/string.h"

#include "cengine/graphics.h"
#include "cengine/renderer.h"
#include "cengine/sprites.h"

struct _Sprite;
struct _SpriteSheet;

// loads an image file into a texture
// NOTE: uses the renderer to create a new texture from a surface
// but only works from the thread where you created the renderer, you can call this method from 
// another thread and you will still get the image data (recovered from the created surface)
// and the texture will be created in the next render loop, given the illusion of a background charge
// the image data msut be freed using image_data_delete
extern ImageData *texture_load (Renderer *renderer, const char *filename, SDL_Texture **texture);

// gets the texture's width and height
extern void texture_get_dimensions (SDL_Texture *texture, u32 *w, u32 *h);

#include "cengine/game/camera.h"

struct _Camera;

extern void texture_draw (struct _Camera *cam, Renderer *renderer, struct _Sprite *sprite, i32 x, i32 y, SDL_RendererFlip flip);

extern void texture_draw_frame (struct _Camera *cam, Renderer *renderer, struct _SpriteSheet *spriteSheet, 
    i32 x, i32 y, u32 col, u32 row, SDL_RendererFlip flip);

#endif