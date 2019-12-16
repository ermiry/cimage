#ifndef _CENGINE_SPRITES_H_
#define _CENGINE_SPRITES_H_

#include <SDL2/SDL.h>

#include "cengine/types/types.h"

#include "cengine/renderer.h"
#include "cengine/graphics.h"
#include "cengine/textures.h"

struct _Sprite {

    ImageData *img_data;

    u32 w, h;
    SDL_Texture *texture;
    i32 scale_factor;
    SDL_Rect src_rect, dest_rect;

};

typedef struct _Sprite Sprite;

extern Sprite *sprite_new (void);

extern void sprite_destroy (Sprite *sprite);

extern Sprite *sprite_load (const char *filename, Renderer *renderer);

typedef struct IndividualSprite {

    u32 col, row;

} IndividualSprite;

struct _SpriteSheet {

    struct _ImageData *img_data;

    u32 w, h;
    SDL_Texture *texture;

    SDL_Rect src_rect, dest_rect;

    u32 sprite_w, sprite_h;

    i32 scale_factor;

    IndividualSprite ***individual_sprites;

};

typedef struct _SpriteSheet SpriteSheet;

extern void sprite_sheet_destroy (SpriteSheet *sprite_sheet);

extern SpriteSheet *sprite_sheet_load (const char *filename, Renderer *renderer);

extern void sprite_sheet_set_sprite_size (SpriteSheet *sprite_sheet, u32 w, u32 h);

extern void sprite_sheet_set_scale_factor (SpriteSheet *sprite_sheet, i32 scale_factor);

extern void sprite_sheet_crop (SpriteSheet *sprite_sheet);

#endif