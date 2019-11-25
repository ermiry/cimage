#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_rwops.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_image.h>

#include "cengine/types/types.h"
#include "cengine/types/vector2d.h"

#include "cengine/sprites.h"
#include "cengine/renderer.h"
#include "cengine/input.h"

#include "cengine/ui/ui.h"
#include "cengine/ui/image.h"

static Image *ui_image_new (void) {

    Image *image = (Image *) malloc (sizeof (Image));
    if (image) {
        memset (image, 0, sizeof (Image));
        image->ui_element = NULL;
        image->transform = NULL;
        image->sprite = NULL;
        image->texture = NULL;
        image->sprite_sheet = NULL;

        image->active = NULL;
        image->pressed = NULL;
        image->action = NULL;
        image->args = NULL;

        image->overlay_texture = NULL;
    }

    return image;

}

void ui_image_delete (void *image_ptr) {

    if (image_ptr) {
        Image *image = (Image *) image_ptr;

        image->ui_element = NULL;
        ui_transform_component_delete (image->transform);

        if (image->ref_sprite) {
            image->sprite = NULL;
            image->sprite_sheet = NULL;
        }

        else {
            sprite_destroy (image->sprite);
            sprite_sheet_destroy (image->sprite_sheet);
        }

        if (image->texture) SDL_DestroyTexture (image->texture);

        if (image->overlay_texture) SDL_DestroyTexture (image->overlay_texture);

        free (image);
    }

}

// sets the image's UI position
void ui_image_set_pos (Image *image, UIRect *ref_rect, UIPosition pos, Renderer *renderer) {

    if (image) ui_transform_component_set_pos (image->transform, renderer, ref_rect, pos, false);

}

// sets the image's render dimensions
void ui_image_set_dimensions (Image *image, unsigned int width, unsigned int height) {

    if (image) {
        image->transform->rect.w = width;
        image->transform->rect.h = height;
    }

}

// sets the image's scale factor
void ui_image_set_scale (Image *image, int x_scale, int y_scale) {

    if (image) {
        image->transform->x_scale = x_scale;
        image->transform->y_scale = y_scale;

        image->transform->rect.w *= image->transform->x_scale;
        image->transform->rect.h *= image->transform->y_scale;
    }

}

// sets the image's sprite to be rendered and loads its
// returns 0 on success loading sprite, 1 on error
u8 ui_image_set_sprite (Image *image, Renderer *renderer, const char *filename) {

    u8 retval = 1;

    if (image && renderer && filename) {
        if (!image->ref_sprite) sprite_destroy (image->sprite);

        image->sprite = sprite_load (filename, renderer);
        if (image->sprite) {
            image->transform->rect.w = image->sprite->w;
            image->transform->rect.h = image->sprite->h;
            retval = 0;
        }
    }

    return retval;

}

// sets the image's sprite sheet to be rendered and loads it
// returns 0 on success loading sprite sheet, 1 on error
u8 ui_image_set_sprite_sheet (Image *image, Renderer *renderer, const char *filename) {

    u8 retval = 1;

    if (image && renderer && filename) {
        if (!image->ref_sprite) sprite_sheet_destroy (image->sprite_sheet);

        image->sprite_sheet = sprite_sheet_load (filename, renderer);
        if (image->sprite_sheet) retval = 0;
    }

    return retval;

}

// sets the image's sprite from a refrence from an already loaded sprite
void ui_image_ref_sprite (Image *image, Sprite *sprite) {

    if (image && sprite) {
        if (!image->ref_sprite) sprite_destroy (image->sprite);

        image->sprite = sprite;
        image->ref_sprite = true;
    }

}

// sets the image's sprite sheet from a refrence from an already loaded sprite sheet
void ui_image_ref_sprite_sheet (Image *image, SpriteSheet *sprite_sheet) {

    if (image && sprite_sheet) {
        if (!image->ref_sprite) sprite_sheet_destroy (image->sprite_sheet);

        image->sprite_sheet = sprite_sheet;
        image->ref_sprite = true;
    }

}

// sets the image's sprite sheet offset
void ui_image_set_sprite_sheet_offset (Image *image, u32 x_offset, u32 y_offset) {

    if (image) {
        image->x_sprite_offset = x_offset;
        image->y_sprite_offset = y_offset;
    }

}

// sets the image's outline colour
void ui_image_set_ouline_colour (Image *image, RGBA_Color colour) {

    if (image) {
        image->outline = true;
        image->outline_colour = colour;
    }

}

// sets the image's outline scale
void ui_image_set_outline_scale (Image *image, float x_scale, float y_scale) {

    if (image) {
        image->outline_scale_x = x_scale;
        image->outline_scale_y = y_scale;
    }

}

// removes the ouline form the image
void ui_image_remove_outline (Image *image) {

    if (image) {
        memset (&image->outline_colour, 0, sizeof (RGBA_Color));
        image->outline = false;
    }

}

// sets the image to be active depending on value
// action listerner working
void ui_image_set_active (Image *image, bool active) {

    if (image) image->active = active;

}

// toggles the image to be active or not
// action listerner working
void ui_image_toggle_active (Image *image) {

    if (image) image->active = !image->active;

}

// sets an action to be triggered when the image is clicked
void ui_image_set_action (Image *image, Action action, void *args) {

    if (image) {
        image->action = action;
        image->args = args;
    } 

}

// sets an overlay to the image that only renders when you hover the image
void ui_image_set_overlay (Image *image, Renderer *renderer, RGBA_Color color) {

    if (image) {
        if (image->overlay_texture) {
            if (!image->overlay_reference) {
                SDL_DestroyTexture (image->overlay_texture);
                image->overlay_texture = NULL;
            }
        }

        render_complex_transparent_rect (renderer, &image->overlay_texture, &image->transform->rect, color); 
        image->overlay_reference = false;   
    }

}

// sets an overlay to the image that only renders when you hover the image
// you need to pass a refrence to the texture
void ui_image_set_overlay_ref (Image *image, SDL_Texture *overlay_ref) {

    if (image && overlay_ref) {
        if (image->overlay_texture) {
            if (!image->overlay_reference) {
                SDL_DestroyTexture (image->overlay_texture);
                image->overlay_texture = NULL;
            }
        }

        image->overlay_texture = overlay_ref;
        image->overlay_reference = true;
    }

}

// removes the overlay from the image
void ui_image_remove_overlay (Image *image) {

    if (image) {
        if (image->overlay_texture) {
            if (!image->overlay_reference) {
                SDL_DestroyTexture (image->overlay_texture);
                image->overlay_texture = NULL;
            }
        }
    }

}

static Image *ui_image_create_common (Renderer *renderer) {

    Image *image = NULL;

    UIElement *ui_element = ui_element_create (renderer->ui, UI_IMAGE);
    if (ui_element) {
        image = ui_image_new ();
        if (image) {
            image->ui_element = ui_element;
            ui_element->element = image;

            image->outline_scale_x = image->outline_scale_y = 1;
        }
    }

    return image;

}

// creates a new image to be displayed from a constant source, like using a sprite loaded from a file
// x and y for position
Image *ui_image_create_static (u32 x, u32 y, Renderer *renderer) {

    Image *image = ui_image_create_common (renderer);
    if (image) {
        image->transform = ui_transform_component_create (x, y, 0, 0);
    }

    return image;

}

// manually creates a streaming access texture, usefull for constant updates
u8 ui_image_create_streaming_texture (Image *image, Renderer *renderer, Uint32 sdl_pixel_format) {

    u8 retval = 1;

    if (image && renderer) {
        image->texture = SDL_CreateTexture (renderer->renderer, sdl_pixel_format,
            SDL_TEXTUREACCESS_STREAMING, image->transform->rect.w, image->transform->rect.h);
        if (image->texture) retval = 0;
    }

    return retval;

}

// TODO: what about the lock texture method?
// updates the streaming texture using an in memory buffer representing an image
// NOTE: buffer is not freed
u8 ui_image_update_streaming_texture_mem (Image *image, void *mem, int mem_size) {

    u8 retval = 1;

    if (image && mem) {
        SDL_RWops *rw = SDL_RWFromConstMem (mem, mem_size);
        // printf ("Is jpeg? %d\n", IMG_isJPG (rw));
        // SDL_Surface *surface = IMG_Load_RW (rw, 1);
        SDL_Surface *surface = IMG_LoadTyped_RW (rw, 0, "JPG");
        // printf ("Pixel format: %s\n", SDL_GetPixelFormatName (surface->format->format));
        // Uint32 format = 0;
        // SDL_QueryTexture (image->texture, &format, NULL, NULL, NULL);
        // printf ("%s\n", SDL_GetPixelFormatName (format));
        // printf ("surface pitch: %d\n", surface->pitch);

        // update the texture
        if (!SDL_UpdateTexture (image->texture, NULL, surface->pixels, surface->pitch)) {
            // SDL_RenderClear (main_renderer->renderer);
            // SDL_RenderCopy (main_renderer->renderer, image->texture, NULL, &image->transform->rect);
            // SDL_RenderPresent (main_renderer->renderer);

            retval = 0;
        }

        // SDL_FreeSurface (surface);
    }

    return retval;

}

// creates an image that is ment to be updated directly and constantly using its texture
// usefull for streaming video
// x and y for position
// w and h for dimensions
Image *ui_image_create_dynamic (u32 x, u32 y, u32 w, u32 h, Renderer *renderer) {

    Image *image = ui_image_create_common (renderer);
    if (image) {
        image->transform = ui_transform_component_create (x, y, w, h);
    }

    return image;

}

// draws the image to the screen
void ui_image_draw (Image *image, Renderer *renderer) {

    if (image && renderer) {
        if (SDL_HasIntersection (&image->transform->rect, &renderer->window->screen_rect)) {
            if (image->texture) {
                SDL_RenderCopyEx (renderer->renderer, image->texture, 
                    NULL, &image->transform->rect, 
                    0, 0, image->flip);
            }

            else {
                if (image->sprite) {
                    SDL_RenderCopyEx (renderer->renderer, image->sprite->texture, 
                        &image->sprite->src_rect, &image->transform->rect, 
                        0, 0, image->flip);
                }
                
                else if (image->sprite_sheet) {
                    image->sprite_sheet->src_rect.x = image->sprite_sheet->sprite_w * image->x_sprite_offset;
                    image->sprite_sheet->src_rect.y = image->sprite_sheet->sprite_h * image->y_sprite_offset;

                    SDL_RenderCopyEx (renderer->renderer, image->sprite_sheet->texture, 
                        &image->sprite_sheet->src_rect, &image->transform->rect, 
                        0, 0, image->flip);
                }
            }

            // render the outline border
            if (image->outline) 
                render_basic_outline_rect (renderer, &image->transform->rect, image->outline_colour, 
                    image->outline_scale_x, image->outline_scale_y);

            // check for action listener
            if (image->active) {
                // check if the mouse is in the image
                if (mousePos.x >= image->transform->rect.x && mousePos.x <= (image->transform->rect.x + image->transform->rect.w) && 
                    mousePos.y >= image->transform->rect.y && mousePos.y <= (image->transform->rect.y + image->transform->rect.h)) {
                    if (image->overlay_texture) {
                        SDL_RenderCopyEx (renderer->renderer, image->overlay_texture, 
                            NULL, &image->transform->rect, 
                            0, 0, image->flip);
                    }

                    // check if the user pressed the left button over the image
                    if (input_get_mouse_button_state (MOUSE_LEFT)) {
                        image->pressed = true;
                    }
                    
                    else if (!input_get_mouse_button_state (MOUSE_LEFT)) {
                        if (image->pressed) {
                            image->pressed = false;
                            if (image->action) image->action (image->args);
                            // printf ("Pressed!\n");
                        }
                    }
                }
            
                else image->pressed = false;
            }

            renderer->render_count += 1;
        }
    }

}