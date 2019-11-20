#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "cengine/renderer.h"
#include "cengine/sprites.h"
#include "cengine/textures.h"

#include "cengine/game/camera.h"

#include "cengine/utils/log.h"
#include "cengine/utils/utils.h"

ImageData *image_data_new (u32 w, u32 h) {

    ImageData *img_data = (ImageData *) malloc (sizeof (ImageData));
    if (img_data) {
        img_data->w = w;
        img_data->h = h;
    }

    return img_data;

}

void image_data_delete (void *img_data_ptr) { if (img_data_ptr) free (img_data_ptr); }

ImageData *texture_load (Renderer *renderer, const char *filename, SDL_Texture **texture) {

    ImageData *image_data = NULL;

    if (filename && renderer && texture) {
        SDL_Surface *temp_surface = IMG_Load (filename);
        if (temp_surface) {
            image_data = image_data_new (temp_surface->w, temp_surface->h);

            pthread_t thread_id = pthread_self ();
            // printf ("Loading texture in thread: %ld\n", thread_id);
            if (thread_id == renderer->thread_id) {
                // load texture as always
                *texture = SDL_CreateTextureFromSurface (renderer->renderer, temp_surface);
            }

            else {
                // TODO: send image to renderer queue
            }

            SDL_FreeSurface (temp_surface);
        }

        else {
            cengine_log_msg (stderr, LOG_ERROR, LOG_NO_TYPE, 
                c_string_create ("Failed to load asset: %s!", filename));
        } 
    }

    return image_data;

}

void texture_get_dimensions (SDL_Texture *texture, u32 *w, u32 *h) {

    if (texture) SDL_QueryTexture (texture, NULL, NULL, w, h);

}

void texture_draw (Camera *cam, Sprite *sprite, i32 x, i32 y, SDL_RendererFlip flip) {

    if (cam && sprite) {
        sprite->dest_rect.x = x;
        sprite->dest_rect.y = y;

        CamRect screenRect = camera_world_to_screen (cam, sprite->dest_rect);

        SDL_RenderCopyEx (main_renderer->renderer, sprite->texture, &sprite->src_rect, &screenRect, 
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

        SDL_RenderCopyEx (main_renderer->renderer, spriteSheet->texture, 
            &spriteSheet->src_rect, &screenRect,
            0, 0, flip);
    }

}