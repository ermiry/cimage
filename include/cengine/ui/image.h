#ifndef _CENGINE_UI_IMAGE_H_
#define _CENGINE_UI_IMAGE_H_

#include <stdbool.h>

#include "cengine/types/types.h"
#include "cengine/sprites.h"

#include "cengine/ui/components/transform.h"

typedef struct Image {

    UIElement *ui_element;

    UITransform *transform;

    Sprite *sprite;

    SpriteSheet *sprite_sheet;
    u32 x_sprite_offset, y_sprite_offset;
    
    bool ref_sprite;
    Flip flip;

} Image;

extern void ui_image_delete (void *image_ptr);

// sets the image's scale factor
extern void ui_image_set_scale (Image *image, int x_scale, int y_scale);

// sets the image's sprite to be rendered and loads its
// returns 0 on success loading sprite, 1 on error
extern u8 ui_image_set_sprite (Image *image, const char *filename);

// sets the image's sprite sheet to be rendered and loads it
// returns 0 on success loading sprite sheet, 1 on error
extern u8 ui_image_set_sprite_sheet (Image *image, const char *filename) ;

// sets the image's sprite from a refrence from an already loaded sprite
extern void ui_image_ref_sprite (Image *image, Sprite *sprite);

// sets the image's sprite sheet from a refrence from an already loaded sprite sheet
extern void ui_image_ref_sprite_sheet (Image *image, SpriteSheet *sprite_sheet);

// sets the image's sprite sheet offset
extern void ui_image_set_sprite_sheet_offset (Image *image, u32 x_offset, u32 y_offset);

// creates a new image
// x and y for position
extern Image *ui_image_create (u32 x, u32 y);

// draws the image to the screen
extern void ui_image_draw (Image *image);

#endif