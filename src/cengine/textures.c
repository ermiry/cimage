#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "types/myTypes.h"

// #include "game/camera.h"

#include "cengine/renderer.h"
#include "cengine/sprites.h"

#include "utils/log.h"
#include "utils/myUtils.h"

/*** TEXTURE MANAGER ***/

SDL_Texture *texture_load (const char *filename, SDL_Renderer *renderer) {

    if (filename && renderer) {
        SDL_Surface *tmpSurface = IMG_Load (filename);
        if (tmpSurface) {
            SDL_Texture *texture = SDL_CreateTextureFromSurface (renderer, tmpSurface);
            SDL_FreeSurface (tmpSurface);

            return texture;
        }

        logMsg (stderr, ERROR, NO_TYPE, createString ("Failed to load asset: %s!", filename));
    }

    return NULL;

}

void texture_get_dimensions (SDL_Texture *texture, u32 *w, u32 *h) {

    if (texture) SDL_QueryTexture (texture, NULL, NULL, w, h);

}

// FIXME:
/* void texture_draw (Camera *cam, Sprite *sprite, i32 x, i32 y, SDL_RendererFlip flip) {

    if (cam && sprite) {
        sprite->dest_rect.x = x;
        sprite->dest_rect.y = y;

        CamRect screenRect = camera_world_to_screen (cam, sprite->dest_rect);

        SDL_RenderCopyEx (main_renderer, sprite->texture, &sprite->src_rect, &screenRect, 
            0, 0, flip);    
    }

}

void texture_draw_frame (Camera *cam, SpriteSheet *spriteSheet, 
    i32 x, i32 y, u32 col, u32 row, SDL_RendererFlip flip) {

    if (cam && spriteSheet) {
        spriteSheet->src_rect.x = spriteSheet->sprite_w * col;
        spriteSheet->src_rect.y = spriteSheet->sprite_h * row;

        spriteSheet->dest_rect.x = x;
        spriteSheet->dest_rect.y = y;

        CamRect screenRect = camera_world_to_screen (cam, spriteSheet->dest_rect);

        SDL_RenderCopyEx (main_renderer, spriteSheet->texture, 
            &spriteSheet->src_rect, &screenRect,
            0, 0, flip);
    }

} */