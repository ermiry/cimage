#ifndef _CENGINE_UI_LAYOUT_VERTICAL_H_
#define _CENGINE_UI_LAYOUT_VERTICAL_H_

#include "cengine/types/types.h"

#include "cengine/collections/dlist.h"

#include "cengine/ui/ui.h"
#include "cengine/ui/components/transform.h"

typedef struct VerticalLayout {

    UITransform *transform;

    DoubleList *ui_elements;
    u32 max_n_ui_elements;
    u32 curr_n_ui_elements;

    u32 cell_width;
    u32 cell_height;

} VerticalLayout;

extern void ui_layout_vertical_delete (void *horizontal_ptr);

// creates a new horizontal layout
// x and y for position, w and h for dimensions
extern VerticalLayout *ui_layout_vertical_create (i32 x, i32 y, u32 w, u32 h);

#endif