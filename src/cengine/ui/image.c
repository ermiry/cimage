#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "cengine/types/types.h"

#include "cengine/sprites.h"
#include "cengine/renderer.h"

#include "cengine/ui/ui.h"
#include "cengine/ui/image.h"

static Image *ui_image_new (void) {

    Image *image = (Image *) malloc (sizeof (Image));
    if (image) {
        memset (image, 0, sizeof (Image));
        image->ui_element = NULL;
        image->transform = NULL;
        image->sprite = NULL;
        image->sprite_sheet = NULL;
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

        free (image);
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
u8 ui_image_set_sprite (Image *image, const char *filename) {

    u8 retval = 1;

    if (image && filename) {
        if (!image->ref_sprite) sprite_destroy (image->sprite);

        image->sprite = sprite_load (filename, main_renderer);
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
u8 ui_image_set_sprite_sheet (Image *image, const char *filename) {

    u8 retval = 1;

    if (image && filename) {
        if (!image->ref_sprite) sprite_sheet_destroy (image->sprite_sheet);

        image->sprite_sheet = sprite_sheet_load (filename, main_renderer);
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

// creates a new image
// x and y for position
Image *ui_image_create (u32 x, u32 y) {

    Image *image = NULL;

    UIElement *ui_element = ui_element_new (UI_IMAGE);
    if (ui_element) {
        image = ui_image_new ();
        if (image) {
            image->ui_element = ui_element;
            image->transform = ui_transform_component_create (x, y, 0, 0);
            ui_element->element = image;
        }
    }

    return image;

}

// draws the image to the screen
void ui_image_draw (Image *image) {

    if (image) {
        if (image->sprite) {
            SDL_RenderCopyEx (main_renderer->renderer, image->sprite->texture, 
                &image->sprite->src_rect, &image->transform->rect, 
                0, 0, image->flip);
        }
        
        else if (image->sprite_sheet) {
            image->sprite_sheet->src_rect.x = image->sprite_sheet->sprite_w * image->x_sprite_offset;
            image->sprite_sheet->src_rect.y = image->sprite_sheet->sprite_h * image->y_sprite_offset;

            SDL_RenderCopyEx (main_renderer->renderer, image->sprite_sheet->texture, 
                &image->sprite_sheet->src_rect, &image->transform->rect, 
                0, 0, image->flip);
        }
    }

}