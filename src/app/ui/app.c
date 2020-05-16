#include <stdlib.h>

#include "cengine/types/types.h"
#include "cengine/types/string.h"

#include "cengine/video.h"
#include "cengine/renderer.h"

#include "cengine/ui/ui.h"
#include "cengine/ui/font.h"
#include "cengine/ui/textbox.h"
#include "cengine/ui/button.h"
#include "cengine/ui/image.h"
#include "cengine/ui/panel.h"
#include "cengine/ui/inputfield.h"
#include "cengine/ui/tooltip.h"
#include "cengine/ui/layout/grid.h"

#include "cengine/utils/utils.h"

#include "cimage.h"
#include "app/states/app.h"
#include "app/media.h"

#include "app/ui/sidebar.h"

static Panel *background_panel = NULL;
Panel *images_panel = NULL;

static Button *open_folder_button = NULL;
static TextBox *open_folder_text = NULL;

static SDL_Texture *overlay_texture = NULL;
static SDL_Texture *selected_texture = NULL;

/*** actions bar ***/

#pragma region actions

#define ACTIONSBAR_HEIGHT           50

static Panel *actionsbar_panel = NULL;
static InputField *actionsbar_search_input = NULL;
static Button *actionsbar_close_button = NULL;

static void actionsbar_init (void) {

    Renderer *main_renderer = renderer_get_by_name ("main");

    Font *font = ui_font_get_default ();

    u32 screen_width = main_renderer->window->window_size.width;
    u32 screen_height = main_renderer->window->window_size.height;

    RGBA_Color statusbar_color = { .r = 37, .g = 44, .b = 54, .a = 245 };

    actionsbar_panel = ui_panel_create (sidebar->ui_element->transform->rect.w, 0, 
        screen_width - sidebar->ui_element->transform->rect.w, ACTIONSBAR_HEIGHT,
        UI_POS_LEFT_UPPER_CORNER, main_renderer);
    ui_panel_set_bg_colour (actionsbar_panel, main_renderer, statusbar_color);
    ui_element_set_layer (main_renderer->ui, actionsbar_panel->ui_element, "top");
    ui_element_toggle_active (actionsbar_panel->ui_element);

    actionsbar_search_input = ui_input_field_create (0, 0, 400, 40, UI_POS_FREE, main_renderer);
    ui_input_field_set_pos (actionsbar_search_input, &actionsbar_panel->ui_element->transform->rect, UI_POS_MIDDLE_CENTER, main_renderer);
    actionsbar_search_input->ui_element->transform->rect.x -= (SIDEBAR_WIDTH / 2);
    ui_input_field_placeholder_text_set (actionsbar_search_input, main_renderer, "Search", font, 24, RGBA_WHITE);
    ui_input_field_placeholder_text_pos_set (actionsbar_search_input, UI_POS_MIDDLE_CENTER);
    ui_input_field_text_set (actionsbar_search_input, main_renderer, "", font, 24, RGBA_WHITE, false);
    ui_input_field_text_pos_set (actionsbar_search_input, UI_POS_MIDDLE_CENTER);
    ui_input_field_ouline_set_colour (actionsbar_search_input, RGBA_WHITE);
    ui_input_field_set_on_key_input (actionsbar_search_input, media_search, actionsbar_search_input);
    ui_element_set_layer (main_renderer->ui, actionsbar_search_input->ui_element, "top");
    ui_element_toggle_active (actionsbar_search_input->ui_element);

    actionsbar_close_button = ui_button_create (0, 0, 48, 48, UI_POS_FREE, main_renderer);
    ui_button_set_pos (actionsbar_close_button, &actionsbar_panel->ui_element->transform->rect, UI_POS_RIGHT_CENTER, main_renderer);
    actionsbar_close_button->ui_element->transform->rect.x -= 20;
    // ui_button_set_ouline_colour (actionsbar_close_button, RGBA_WHITE);
    ui_button_set_sprite (actionsbar_close_button, main_renderer, BUTTON_STATE_MOUSE_OUT, "./assets/icons/close.png");
    ui_element_set_layer (main_renderer->ui, actionsbar_close_button->ui_element, "top");
    ui_button_set_action (actionsbar_close_button, media_folder_close, NULL);
    ui_element_toggle_active (actionsbar_close_button->ui_element);

}

void app_ui_actionsbar_show (void) {

    ui_element_set_active (actionsbar_panel->ui_element, true);
    ui_element_set_active (actionsbar_search_input->ui_element, true);
    ui_element_set_active (actionsbar_close_button->ui_element, true);

}

void app_ui_actionsbar_toggle (void) {

    ui_element_toggle_active (actionsbar_panel->ui_element);
    ui_element_toggle_active (actionsbar_search_input->ui_element);
    ui_element_toggle_active (actionsbar_close_button->ui_element);

}

void app_ui_actionsbar_hide (void) {

    ui_element_set_active (actionsbar_panel->ui_element, false);
    ui_element_set_active (actionsbar_search_input->ui_element, false);
    ui_element_set_active (actionsbar_close_button->ui_element, false);

}

static void actionsbar_end (void) {

    // if (actionsbar_panel) ui_element_destroy (actionsbar_panel->ui_element);   
    // if (actionsbar_search_input) ui_element_destroy (actionsbar_search_input->ui_element);
    // if (actionsbar_close_button)  ui_element_destroy (actionsbar_close_button->ui_element);

}

#pragma endregion

#pragma region status

static Panel *statusbar = NULL;
static TextBox *statusbar_foldername = NULL;
static TextBox *statusbar_selected = NULL;
static TextBox *statusbar_zoom = NULL;
static TextBox *statusbar_total = NULL;

#define STATUS_BAR_HEIGHT           50

// sets the total number of images being displayed in the status bar text
int app_ui_statusbar_total_set (u32 total) {

    int retval = 1;

    if (statusbar_total) {
        char *status = c_string_create ("Total: %d", total);
        if (status) {
            // printf ("%s\n", status);
            ui_textbox_update_text (statusbar_total, renderer_get_by_name ("main"), status);
            // ui_textbox_set_text_pos (statusbar_total, UI_POS_RIGHT_CENTER);

            free (status);

            retval = 0 ;
        }
    }

    return retval;

}

// displays the current zoom level in the status bar
int app_ui_statusbar_zoom_set (int zoom) {

    int retval = 1;

    if (statusbar_zoom) {
        char *status = c_string_create ("Zoom: %d", zoom);
        if (status) {
            // printf ("%s\n", status);
            ui_textbox_update_text (statusbar_zoom, renderer_get_by_name ("main"), status);
            // ui_textbox_set_text_pos (statusbar_zoom, UI_POS_RIGHT_CENTER);

            free (status);

            retval = 0 ;
        }
    }

    return retval;

}

static void statusbar_init (void) {

    Renderer *main_renderer = renderer_get_by_name ("main");

    Font *font = ui_font_get_default ();

    u32 screen_width = main_renderer->window->window_size.width;
    u32 screen_height = main_renderer->window->window_size.height;

    // background
    RGBA_Color statusbar_color = { .r = 37, .g = 44, .b = 54, .a = 245 };
    statusbar = ui_panel_create (sidebar->ui_element->transform->rect.w, 0, 
        screen_width - sidebar->ui_element->transform->rect.w, STATUS_BAR_HEIGHT,
        UI_POS_LEFT_BOTTOM_CORNER, main_renderer);
    ui_panel_set_bg_colour (statusbar, main_renderer, statusbar_color);
    ui_element_set_layer (main_renderer->ui, statusbar->ui_element, "top");
    ui_element_toggle_active (statusbar->ui_element);

    // path
    statusbar_foldername = ui_textbox_create (0, 0, 600, 50, UI_POS_FREE, main_renderer);
    ui_textbox_set_pos (statusbar_foldername, &statusbar->ui_element->transform->rect, UI_POS_LEFT_CENTER, NULL);
    statusbar_foldername->ui_element->transform->rect.x += 20;
    ui_textbox_set_text (statusbar_foldername, main_renderer, "", font, 24, RGBA_WHITE, false);
    // ui_textbox_set_text_pos (statusbar_foldername, UI_POS_LEFT_CENTER);
    // ui_textbox_set_ouline_colour (statusbar_foldername, RGBA_WHITE);
    // ui_element_set_layer (main_renderer->ui, statusbar_foldername->ui_element, "top");
    ui_element_toggle_active (statusbar_foldername->ui_element);

    // selected
    statusbar_selected = ui_textbox_create (0, 0, 600, 50, UI_POS_FREE, main_renderer);
    ui_textbox_set_pos (statusbar_selected, &statusbar->ui_element->transform->rect, UI_POS_MIDDLE_CENTER, NULL);
    statusbar_selected->ui_element->transform->rect.x -= sidebar->ui_element->transform->rect.w / 2;
    // ui_textbox_set_text (statusbar_selected, main_renderer, "", font, 24, RGBA_WHITE, false);
    // ui_textbox_set_text_pos (statusbar_selected, UI_POS_MIDDLE_CENTER);
    // ui_textbox_set_ouline_colour (statusbar_selected, RGBA_WHITE);
    ui_element_set_layer (main_renderer->ui, statusbar_selected->ui_element, "top");
    ui_element_toggle_active (statusbar_selected->ui_element);

    // total
    statusbar_total = ui_textbox_create (0, 0, 160, 50, UI_POS_FREE, main_renderer);
    ui_textbox_set_pos (statusbar_total, &statusbar->ui_element->transform->rect, UI_POS_RIGHT_CENTER, NULL);
    statusbar_total->ui_element->transform->rect.x -= 20;
    ui_textbox_set_text (statusbar_total, main_renderer, "Total: 0", font, 24, RGBA_WHITE, false);
    ui_textbox_set_text_pos (statusbar_total, UI_POS_RIGHT_CENTER);
    // ui_textbox_set_ouline_colour (statusbar_total, RGBA_WHITE);
    ui_element_set_layer (main_renderer->ui, statusbar_total->ui_element, "top");
    ui_element_toggle_active (statusbar_total->ui_element);

    // zoom
    statusbar_zoom = ui_textbox_create (0, 0, 120, 50, UI_POS_FREE, main_renderer);
    ui_textbox_set_pos (statusbar_zoom, &statusbar->ui_element->transform->rect, UI_POS_RIGHT_CENTER, NULL);
    statusbar_zoom->ui_element->transform->rect.x -= (20 + statusbar_total->ui_element->transform->rect.w);
    ui_textbox_set_text (statusbar_zoom, main_renderer, 
        "Zoom: 0", 
        font, 24, RGBA_WHITE, false);
    ui_textbox_set_text_pos (statusbar_zoom, UI_POS_RIGHT_CENTER);
    // ui_textbox_set_ouline_colour (statusbar_zoom, RGBA_WHITE);
    ui_element_set_layer (main_renderer->ui, statusbar_zoom->ui_element, "top");
    ui_element_toggle_active (statusbar_zoom->ui_element);

}

void app_ui_statusbar_show (const char *foldername) {

    Renderer *main_renderer = renderer_get_by_name ("main");
    Font *font = ui_font_get_default ();

    ui_textbox_set_text (statusbar_foldername, main_renderer, foldername, font, 24, RGBA_WHITE, false);
    ui_textbox_set_text_pos (statusbar_foldername, UI_POS_LEFT_CENTER);
    ui_element_set_layer (main_renderer->ui, statusbar_foldername->ui_element, "top");
    ui_element_toggle_active (statusbar_foldername->ui_element);
    
    ui_element_toggle_active (statusbar_total->ui_element);

    ui_element_toggle_active (statusbar_zoom->ui_element);

    ui_element_toggle_active (statusbar->ui_element);

}

void app_ui_statusbar_hide (void) {

    ui_element_toggle_active (statusbar_foldername->ui_element);
    ui_element_toggle_active (statusbar_total->ui_element);
    ui_element_toggle_active (statusbar_zoom->ui_element);
    ui_element_toggle_active (statusbar->ui_element);

}

void app_ui_statusbar_set_selected_text (const char *text) {

    if (text) {
        Renderer *main_renderer = renderer_get_by_name ("main");
        Font *font = ui_font_get_default ();

        ui_textbox_set_text (statusbar_selected, main_renderer, text, font, 24, RGBA_WHITE, false);
        // ui_textbox_update_text (statusbar_selected, main_renderer, text);
        ui_textbox_set_text_pos (statusbar_selected, UI_POS_MIDDLE_CENTER);
        ui_element_set_active (statusbar_selected->ui_element, true);
    }

    else ui_element_set_active (statusbar_selected->ui_element, false);

}

static void statusbar_end (void) {

    // if (statusbar) ui_element_destroy (statusbar->ui_element);

    // if (statusbar_foldername) ui_element_destroy (statusbar_foldername->ui_element);
    // if (statusbar_selected) ui_element_destroy (statusbar_selected->ui_element);
    // if (statusbar_total) ui_element_destroy (statusbar_total->ui_element);

}

#pragma endregion

#include "cengine/ui/dropdown.h"

void app_ui_init (void) {

    Renderer *main_renderer = renderer_get_by_name ("main");

    Font *font = ui_font_get_default ();

    u32 screen_width = main_renderer->window->window_size.width;
    u32 screen_height = main_renderer->window->window_size.height;

    /*** background ***/
    RGBA_Color electromagnetic = { 47, 54, 64, 255 };
    background_panel = ui_panel_create (0, 0, screen_width, screen_height, UI_POS_MIDDLE_CENTER, main_renderer);
    ui_panel_set_bg_colour (background_panel, main_renderer, electromagnetic);
    ui_element_set_layer (main_renderer->ui, background_panel->ui_element, "back");

    sidebar_init (screen_height);

    open_folder_button = ui_button_create (0, 0, 128, 128, UI_POS_MIDDLE_CENTER, main_renderer);
    ui_button_set_action (open_folder_button, media_folder_select, NULL);
    // ui_button_set_ouline_colour (photos_button, RGBA_WHITE);
    ui_button_set_sprite (open_folder_button, main_renderer, BUTTON_STATE_MOUSE_OUT, "./assets/icons/folder.png");
    ui_element_set_layer (main_renderer->ui, open_folder_button->ui_element, "top");

    open_folder_text = ui_textbox_create (0, 100, 200, 50, UI_POS_MIDDLE_CENTER, main_renderer);
    ui_textbox_set_text (open_folder_text, main_renderer, "Open a photos folder", font, 24, RGBA_WHITE, false);
    ui_textbox_set_text_pos (open_folder_text, UI_POS_MIDDLE_CENTER);
    ui_element_set_layer (main_renderer->ui, open_folder_text->ui_element, "top");

    SDL_Rect rect = { .x = 0, .y = 0, .w = screen_width, .h = screen_height };
    RGBA_Color overlay_colour = { 255, 255, 255, 70 };
    render_complex_transparent_rect (main_renderer, &overlay_texture, &rect, overlay_colour); 

    RGBA_Color selected_colour = { 210, 77, 87, 180 };
    render_complex_transparent_rect (main_renderer, &selected_texture, &rect, selected_colour); 

    actionsbar_init ();
    statusbar_init ();

    Tooltip *right_click_menu = ui_tooltip_create (200, 100, main_renderer);
    ui_element_set_layer (main_renderer->ui, right_click_menu->ui_element, "top");
    ui_tooltip_set_bg_colour (right_click_menu, main_renderer, RGBA_BLACK);
    input_set_right_click_menu (main_renderer->window->input, right_click_menu);

    TextBox *test_text = ui_textbox_create (0, 0, 200, 50, UI_POS_UPPER_CENTER, main_renderer);
    ui_textbox_set_text (test_text, main_renderer, "Test!", font, 24, RGBA_WHITE, false);
    ui_textbox_set_text_pos (test_text, UI_POS_MIDDLE_CENTER);
    ui_textbox_set_ouline_colour (test_text, RGBA_WHITE);
    ui_element_set_layer (main_renderer->ui, test_text->ui_element, "top");
    // ui_element_toggle_active (test_text->ui_element);

    ui_tooltip_add_element_at_end (right_click_menu, test_text->ui_element);

    TextBox *hola_text = ui_textbox_create (0, 0, 500, 50, UI_POS_UPPER_CENTER, main_renderer);
    ui_textbox_set_pos_offset (hola_text, 0, 50);
    ui_textbox_set_text (hola_text, main_renderer, "Hola!", font, 24, RGBA_WHITE, false);
    ui_textbox_set_text_pos (hola_text, UI_POS_MIDDLE_CENTER);
    ui_textbox_set_ouline_colour (hola_text, RGBA_WHITE);
    ui_element_set_layer (main_renderer->ui, hola_text->ui_element, "top");

    ui_tooltip_add_element_at_end (right_click_menu, hola_text->ui_element);

    // FIXME: 21/12/2019 -- dropdwon is still broken!!!
    // Dropdown *projects_update_type_dropdown = ui_dropdown_create (0, 225, 200, 50, UI_POS_MIDDLE_CENTER, main_renderer);
    // ui_dropdown_set_options (projects_update_type_dropdown, 0, 5, 200, 200, UI_POS_BOTTOM_CENTER, main_renderer);
    // ui_dropdown_set_ouline_colour (projects_update_type_dropdown, RGBA_WHITE);
    // ui_dropdown_set_placeholder (projects_update_type_dropdown, main_renderer, "Type", font, 32, RGBA_WHITE);
    // ui_dropdown_set_placeholder_pos (projects_update_type_dropdown, main_renderer, UI_POS_MIDDLE_CENTER);
    // ui_dropdown_extened_set_bg_colour (projects_update_type_dropdown, main_renderer, RGBA_BLACK);

    // DropdownOption *option_type_app = ui_dropdown_option_create (main_renderer, "App", font, 32, RGBA_WHITE);
    // ui_dropdown_option_set_ouline_colour (option_type_app, RGBA_WHITE);
    // ui_dropdown_option_add (projects_update_type_dropdown, option_type_app);
    // DropdownOption *option_type_frame = ui_dropdown_option_create (main_renderer, "Framework", font, 32, RGBA_WHITE);
    // ui_dropdown_option_set_ouline_colour (option_type_frame, RGBA_WHITE);
    // ui_dropdown_option_add (projects_update_type_dropdown, option_type_frame);
    // DropdownOption *option_type_idea = ui_dropdown_option_create (main_renderer, "Idea", font, 32, RGBA_WHITE);
    // ui_dropdown_option_set_ouline_colour (option_type_idea, RGBA_WHITE);
    // ui_dropdown_option_add (projects_update_type_dropdown, option_type_idea);
    // DropdownOption *option_type_other = ui_dropdown_option_create (main_renderer, "Other", font, 32, RGBA_WHITE);
    // ui_dropdown_option_set_ouline_colour (option_type_other, RGBA_WHITE);
    // ui_dropdown_option_add (projects_update_type_dropdown, option_type_other);

}

void app_ui_end (void) {

    actionsbar_end ();
    statusbar_end ();
    sidebar_end ();

    // 28/11/2019 -- gives segfault 
    // if (background_panel) ui_element_destroy (background_panel->ui_element);

    if (overlay_texture) SDL_DestroyTexture (overlay_texture);
    if (selected_texture) SDL_DestroyTexture (selected_texture);

}

#pragma region images

// prepare the ui for the images to be displayed
void app_ui_images_set_ui_elements (u32 n_images, u32 n_cols, u32 n_rows) {

    Renderer *main_renderer = renderer_get_by_name ("main");

    u32 window_width = main_renderer->window->window_size.width;
    u32 window_height = main_renderer->window->window_size.height;

    // remove past buttons
    open_folder_button->ui_element->active = false;
    open_folder_text->ui_element->active = false;

    u32 n_actual_rows = n_images / n_cols;         // total rows for the layout
    n_actual_rows += 1;

    u32 panel_width = (window_width - 100);
    u32 panel_height = window_height - ACTIONSBAR_HEIGHT - STATUS_BAR_HEIGHT;
    printf ("\nSTART: app_ui_images_set_ui_elements () - panel_width: %d - panel_height: %d\n", panel_width, panel_height);

    // set images panel grid layout for images
    images_panel = ui_panel_create (SIDEBAR_WIDTH, ACTIONSBAR_HEIGHT, panel_width, panel_height, UI_POS_FREE, main_renderer);
    ui_panel_layout_set (images_panel, LAYOUT_TYPE_GRID, main_renderer);

    u32 cell_width = (window_width - 100) / n_cols;
    u32 cell_height = (window_height / n_rows);
    //  printf ("\nSTART: cols %d - rows %d\n", n_cols, n_rows);
    // printf ("\nSTART: cell_width %d - cell_height %d\n", cell_width, cell_height);

    // 14/05/2020 - just like when creating an array in c, we only care about the col count (x)
    // the row count is pretty much irrelevant
    GridLayout *grid = (GridLayout *) images_panel->layout;
    ui_layout_grid_set_x_axis_alignment (grid, ALIGN_KEEP_SIZE);
	ui_layout_grid_set_y_axis_alignment (grid, ALIGN_KEEP_SIZE);
    ui_layout_grid_set_grid (grid, n_cols, n_actual_rows);
	ui_layout_grid_set_elements_x_row (grid, n_cols);
	ui_layout_grid_set_elements_x_col (grid, n_actual_rows);
    ui_layout_grid_set_cell_size (grid, cell_width, cell_height);
    ui_layout_grid_set_cell_inner_padding (grid, 0.05, 0.05);
	ui_layout_grid_toggle_scrolling (grid, true);
	ui_layout_grid_set_scroll_sensitivity (grid, 20);

}

// we have closed the images folde,r so hide images ui items
void app_ui_images_remove_ui_elements (void) {

    if (images_panel) {
        ui_element_destroy (images_panel->ui_element);
        images_panel = NULL;
    } 

    app_ui_statusbar_hide ();
    app_ui_actionsbar_hide ();

    open_folder_button->ui_element->active = true;
    open_folder_text->ui_element->active = true;

}

// add image to selected list and sets selected text in status bar
void app_ui_image_select (void *media_item_ptr) {

    if (media_item_ptr) {
        MediaItem *item = (MediaItem *) media_item_ptr;

        // if (img->selected) dlist_remove (cimage->selected_images, img);
        if (item->selected) {
            dlist_remove_element (cimage->selected_images, dlist_get_element (cimage->selected_images, item, NULL));
            item->selected = false;
        } 
        
        else {
            dlist_insert_after (cimage->selected_images, dlist_end (cimage->selected_images), item);
            item->selected = true;
        } 

        if (!cimage->selected_images->size) app_ui_statusbar_set_selected_text (NULL);

        else if (cimage->selected_images->size == 1) {
            MediaItem *media_item = (MediaItem *) (dlist_start (cimage->selected_images)->data);
            if (media_item) 
                app_ui_statusbar_set_selected_text (media_item->filename->str);
        }

        else {
            char *status = c_string_create ("%ld images selected", cimage->selected_images->size);
            if (status) {
                app_ui_statusbar_set_selected_text (status);
                free (status);
            }
        }
    }

}

void app_ui_image_display_in_window (void *item_ptr) {

    if (item_ptr) {
        MediaItem *item = (MediaItem *) item_ptr;

        // check if we already displaying the image
        bool found = false;
        Renderer *renderer = NULL;
        for (ListElement *le = dlist_start (renderers); le; le = le->next) {
            renderer = (Renderer *) le->data;

            if (!strcmp (renderer->name->str, item->filename->str)) {
                window_focus (renderer->window);
                found = true;
                break;
            }
        }

        if (!found) {
            // first we calculate the size of the window
            SDL_DisplayMode display_mode = { 0 };
            video_get_display_mode (0, &display_mode);

            u32 max_width = display_mode.w * 0.80;
            u32 max_height = display_mode.h * 0.80;

            float ratio = 0;
            u32 width = item->image->sprite->img_data->w;
            u32 height = item->image->sprite->img_data->h; 

            WindowSize window_size = { .width = width, .height = height };

            if (width > max_width) {
                ratio = (float) max_width / width;
                window_size.width = max_width;
                window_size.height *= ratio;
                height = height * ratio;
                width = width * ratio;
            }

            if (height > max_height){
                ratio = (float) max_height / height;
                window_size.height = max_height;
                window_size.width *= ratio; 
                width = width * ratio;
                height = height * ratio;
            }

            Renderer *renderer = renderer_create_with_window (item->filename->str, 
                0, SDL_RENDERER_SOFTWARE | SDL_RENDERER_ACCELERATED,
                item->filename->str, window_size, 0);

            // load a new image
            Image *new_image = ui_image_create (0, 0, 0, 0, renderer);
            ui_image_set_pos (new_image, NULL, UI_POS_LEFT_UPPER_CORNER, renderer);
            ui_image_set_sprite (new_image, renderer, item->image->sprite->img_data->filename->str);
            ui_image_set_dimensions (new_image, renderer->window->window_size.width, renderer->window->window_size.height);
        }
    }

}

// adds the image into the main grid for display
void app_ui_image_display (Image *image) {

    if (image) {
        ui_panel_layout_add_element_at_end (images_panel, image->ui_element);
    }

}

// TODO: load a smaller image to us less memory
// creates a ui image element to be displayed
void app_ui_image_create (MediaItem *image_item) {

    if (image_item) {
        Renderer *main_renderer = renderer_get_by_name ("main");

        Image *image = ui_image_create (0, 0, 0, 0, main_renderer);
        ui_image_set_pos (image, NULL, UI_POS_MIDDLE_CENTER, main_renderer);
        ui_image_set_sprite (image, main_renderer, image_item->path->str);
        ui_image_set_ouline_colour (image, RGBA_WHITE);
        ui_image_set_outline_scale (image, 2, 2);
        ui_image_toggle_active (image);
        ui_image_set_overlay_ref (image, overlay_texture);
        ui_image_set_selected_ref (image, selected_texture);
        ui_image_set_action (image, app_ui_image_select, image_item);
        ui_image_set_double_click_action (image, app_ui_image_display_in_window, image_item);
        // ui_element_set_layer (main_renderer->ui, image->ui_element, "middle");

        image_item->image = image;
    }

}

#pragma endregion