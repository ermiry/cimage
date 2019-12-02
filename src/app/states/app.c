#include <stdlib.h>

#ifdef CIMAGE_DEBUG
#include <errno.h>
#endif

#include <unistd.h>

#define _XOPEN_SOURCE 700
#include <sys/types.h>
#include <dirent.h>

#include "cengine/types/types.h"
#include "cengine/types/string.h"

#include "cengine/collections/dlist.h"

#include "cengine/input.h"
#include "cengine/files.h"

#include "cengine/threads/thread.h"
#include "cengine/manager/manager.h"
// #include "cengine/game/go.h"

#include "cengine/ui/ui.h"
#include "cengine/ui/image.h"
#include "cengine/ui/inputfield.h"

#include "cengine/utils/utils.h"
#include "cengine/utils/log.h"

#include "app/states/app.h"
#include "app/ui/app.h"

#define DEFAULT_N_COLS          5
#define DEFAULT_N_ROWS          4

static void image_item_delete_dummy (void *img_ptr);

Cimage *cimage = NULL;

static Cimage *cimage_new (void) {

    Cimage *cimage = (Cimage *) malloc (sizeof (cimage));
    if (cimage) {
        cimage->opened_folder_name = NULL;
        cimage->images = NULL;
        cimage->selected_images = NULL;
    }

    return cimage;

}

static void cimage_delete (void *cimage_ptr) {

    if (cimage_ptr) {
        Cimage *cimage = (Cimage *) cimage_ptr;

        str_delete (cimage->opened_folder_name);
        dlist_delete (cimage->images);
        // FIXME: segfault if list is empty
        // dlist_delete (cimage->selected_images);

        free (cimage_ptr);
    }

}

static Cimage *cimage_create (void) {

    Cimage *cimage = cimage_new ();
    if (cimage) cimage->selected_images = dlist_init (image_item_delete_dummy, NULL);
    return cimage;

}

#pragma region images

static ImageItem *image_item_new (void) {

    ImageItem *img = (ImageItem *) malloc (sizeof (ImageItem));
    if (img) {
        img->image = NULL;
        img->filename = NULL;
        img->path = NULL;
        img->selected = false;
    }

    return img;

}

static void image_item_delete (void *img_ptr) {

    if (img_ptr) {
        ImageItem *img = (ImageItem *) img_ptr;

        str_delete (img->filename);
        str_delete (img->path);

        free (img);
    }

}

static void image_item_delete_dummy (void *img_ptr) {}

static int image_item_comparator (const void *a, const void *b) {

    if (a && b) {
        ImageItem *img_a = (ImageItem *) a;
        ImageItem *img_b = (ImageItem *) b;

        return strcmp (img_a->filename->str, img_b->filename->str);
    }

}

// TODO: better file type check!!
static bool is_image_file (const char *filename) {

    int result;
    bool retval = false;

    if (filename) {
        char *ext = files_get_file_extension (filename);
        if (ext) {
            if (!strcmp (ext, "png") || !strcmp (ext, "jpg") || !strcmp (ext, "jpeg"))
                retval = true;

            free (ext);
        }
    }

    return retval;

}

// get a list of valid images names from the directory
static DoubleList *images_folder_read (const char *images_dir) {

    DoubleList *images = NULL;

    if (images_dir) {
        images = dlist_init (image_item_delete, image_item_comparator);
        
        struct dirent *ep = NULL;
        DIR *dp = opendir (images_dir);
        if (dp) {
            #ifdef CIMAGE_DEBUG
            cengine_log_msg (stdout, LOG_SUCCESS, LOG_NO_TYPE, "Starting to read images...");
            #endif

            while ((ep = readdir (dp)) != NULL) {
                if (strcmp (ep->d_name, ".") && strcmp (ep->d_name, "..")) {
                    if (is_image_file (ep->d_name)) {
                        ImageItem *img = image_item_new ();
                        img->filename = str_new (ep->d_name);
                        img->path = str_create ("%s/%s", images_dir, ep->d_name);

                        dlist_insert_after (images, dlist_end (images), img);
                    }
                }
            }

            (void) closedir (dp);

            #ifdef CIMAGE_DEBUG
            cengine_log_msg (stdout, LOG_SUCCESS, LOG_NO_TYPE, "Done reading images!");
            #endif
        }

        else {
            char *status = c_string_create ("Failed to open dir: %s", images_dir);
            if (status) {
                cengine_log_msg (stderr, LOG_ERROR, LOG_NO_TYPE, status);
                free (status);
            }

            #ifdef CIMAGE_DEBUG
            perror ("Error");
            #endif
        }
    }

    return images;

}

static void *images_load (void *folder_name_ptr) {

    if (folder_name_ptr) {
        String *folder_name = (String *) folder_name_ptr;

        // get images from directory
        cimage->images = images_folder_read (folder_name->str);
        if (cimage->images) {
            if (dlist_size (cimage->images) > 0) {
                // prepare ui for images
                app_ui_images_set_ui_elements (dlist_size (cimage->images), DEFAULT_N_COLS, DEFAULT_N_ROWS);
                app_ui_actionsbar_show ();
                app_ui_statusbar_show (folder_name->str, cimage->images->size);

                for (ListElement *le = dlist_start (cimage->images); le; le = le->next) {
                    // printf ("%s\n", ((String *) le->data)->str);
                    app_ui_image_display (((ImageItem *) le->data));
                }
            }
            
            else {
                // TODO: display error message in ui
                #ifdef CIMAGE_DEBUG
                char *status = c_string_create ("No images found in dir: %s!", folder_name->str);
                if (status) {
                    cengine_log_msg (stderr, LOG_WARNING, LOG_NO_TYPE, status);
                    free (status);
                }
                #endif

                dlist_delete (cimage->images);
                cimage->images = NULL; 

                str_delete (cimage->opened_folder_name);
                cimage->opened_folder_name = NULL;
            }
        }
    }

    return NULL;

}

// TODO: implement cross-platform code
void images_folder_select (void *args) {

    cimage = cimage_create ();

    #ifdef OS_LINUX
    char *command = NULL;
    char *username = getlogin ();
    if (username) {
        // printf ("%s\n", username);
        command = c_string_create ("zenity  --file-selection --title=\"Choose a photos directory\" --filename=/home/%s/ --save --directory", 
            username);
    }

    else {
        // if for whatever reason we can get the username, just open in root folder
        command = c_string_create ("zenity  --file-selection --title=\"Choose a photos directory\" --filename=/ --save --directory");
    }

    if (command) {
        char folder_name[1024];
        FILE *pipe = popen (command, "r");
        if (pipe) {
            fgets (folder_name, 1024, pipe);
            fclose (pipe);
            // printf ("\n%s\n", folder_name);

            c_string_remove_char (folder_name, '\n');
            cimage->opened_folder_name = str_new (folder_name);

            thread_create_detachable (images_load, cimage->opened_folder_name);
        }

        free (command);
    }
    #endif

}

void images_folder_close (void *args) {

    // remove UI
    app_ui_images_remove_ui_elements ();

    cimage_delete (cimage);
    cimage = NULL;

}

// gets executed on every input on the search input
void images_search (void *args) {

    if (args) {
        InputField *search_input = (InputField *) args;

        printf ("search: %s\n", search_input->text->text->str);
    }

}

#pragma endregion

static char *last_sub = NULL;

// TODO: pass the rendrer also
static void main_renderer_update (void *args) {

    // TODO: make this an option!!
    bool done = false;
    // Renderer *main_renderer = renderer_get_by_name ("main");
    Renderer *main_renderer = (Renderer *) args;
    UIElement *hover_element = ui_element_hover_get (main_renderer->ui);
    if (hover_element) {
        if (hover_element->type == UI_IMAGE) {
            Image *image = (Image *) hover_element->element;
            char *sub = NULL;
            char *retval = c_string_remove_sub_after_token_with_idx (image->sprite->img_data->filename->str, '/', &sub, -1);
            if (retval && sub) {
                if (!last_sub) {
                    last_sub = sub;
                    app_ui_statusbar_set_selected_text (sub);
                }

                else {
                    if (strcmp (last_sub, sub)) {
                        free (last_sub);
                        last_sub = sub;
                        app_ui_statusbar_set_selected_text (sub);
                    }
                }

                free (retval);

                done = true;
            }
        }
    }

    if (!done) {
        app_ui_statusbar_set_selected_text (NULL);
        free (last_sub);
        last_sub = NULL;
    } 

}

#pragma region global input

#include "cengine/ui/layout/grid.h"
#include "cengine/ui/panel.h"

extern Panel *images_panel;

void zoom_more (void *args) {

    // get window with keyboard focus
    Window *window = NULL;
    for (ListElement *le = dlist_start (windows); le; le = le->next) {
        window = (Window *) le->data;
        if (window->keyboard) break;
    }

    if (!strcmp (window->renderer->name->str, "main")) {
        if (images_panel) {
            GridLayout *grid =  (GridLayout *) images_panel->layout;
            ui_layout_grid_update_dimensions (grid, grid->cols - 1, grid->rows - 1);
            // printf ("+\n");
        }
    }

}

void zoom_less (void *args) {

    // get window with keyboard focus
    Window *window = NULL;
    for (ListElement *le = dlist_start (windows); le; le = le->next) {
        window = (Window *) le->data;
        if (window->keyboard) break;
    }

    if (!strcmp (window->renderer->name->str, "main")) {
        if (images_panel) {
            GridLayout *grid =  (GridLayout *) images_panel->layout;
            ui_layout_grid_update_dimensions (grid, grid->cols + 1, grid->rows + 1);
            // printf ("-\n");
        }
    }

}

#pragma endregion

#pragma region main screen

#include "cengine/timer.h"

static bool once = false;

static Timer *alt_key_timer = NULL;
static u32 alt_key_cooldown = 500;

void main_screen_input (void *win_ptr) {

    if (win_ptr) {
        Window *win = (Window *) win_ptr;

        if (win->keyboard) {
            if (input_is_key_down (SDL_SCANCODE_F11)) {
                // make window fullscreen
                if (!once) {
                window_toggle_fullscreen_soft (win);
                once = true;
                }
            }

            else if (input_is_key_down (SDL_SCANCODE_LALT) || input_is_key_down (SDL_SCANCODE_RALT)) {
                if (timer_get_ticks (alt_key_timer) > alt_key_cooldown) {
                    // toggle display actions menu
                    if (images_panel) app_ui_actionsbar_toggle ();

                    timer_start (alt_key_timer);
                }
            }

            else if (input_is_key_down (SDL_SCANCODE_UP)) {
                // move images up
                app_ui_images_move_down (10);
            }

            else if (input_is_key_down (SDL_SCANCODE_DOWN)) {
                // move images down
                app_ui_images_move_up (10);
            }
        }
    }

}

#pragma endregion

// TODO: make this an option
int mouse_sensitivity = 50;

void images_move_up_scroll_wheel (void *amount_ptr) {

    if (amount_ptr) {
        int *amount = (int *) amount_ptr;

        if (images_panel) {
            Renderer *main_renderer = renderer_get_by_name ("main");
            u32 screen_height = main_renderer->window->window_size.height;

            GridLayout *grid = (GridLayout *) images_panel->layout;
            if ((abs (images_panel->ui_element->transform->rect.y) + grid->cell_height) < (images_panel->ui_element->transform->rect.h)) {
                // this is positive because when moving the scroll wheel down, we get negative numbers
                images_panel->ui_element->transform->rect.y += (*amount * mouse_sensitivity);
                grid->transform->rect.y += (*amount * mouse_sensitivity);

                GridElement *grid_element = NULL;
                for (ListElement *le = dlist_start (grid->elements); le; le = le->next) {
                    grid_element = (GridElement *) le->data;
                    grid_element->ui_element->transform->rect.y += (*amount * mouse_sensitivity);
                }
            }
        }
    }

}

void images_move_down_scroll_wheel (void *amount_ptr) {

    if (amount_ptr) {
        int *amount = (int *) amount_ptr;

        if (images_panel) {
            Renderer *main_renderer = renderer_get_by_name ("main");
            u32 screen_height = main_renderer->window->window_size.height;

            GridLayout *grid = (GridLayout *) images_panel->layout;
            if ((abs (images_panel->ui_element->transform->rect.y) + grid->cell_height) < (images_panel->ui_element->transform->rect.h)) {
                images_panel->ui_element->transform->rect.y += (*amount * mouse_sensitivity);
                grid->transform->rect.y += (*amount * mouse_sensitivity);

                GridElement *grid_element = NULL;
                for (ListElement *le = dlist_start (grid->elements); le; le = le->next) {
                    grid_element = (GridElement *) le->data;
                    grid_element->ui_element->transform->rect.y += (*amount * mouse_sensitivity);
                }
            }
        }
    }

}

State *app_state = NULL;

static void app_update (void) {

    // game_object_update_all ();

}

static void app_on_enter (void) { 

    input_command_register (SDLK_EQUALS, zoom_more, NULL);
    input_command_register (SDLK_MINUS, zoom_less, NULL);

    input_set_on_mouse_wheel_scroll_up (images_move_up_scroll_wheel);
    input_set_on_mouse_wheel_scroll_down (images_move_down_scroll_wheel);

    app_state->update = app_update;

    app_ui_init ();

    Renderer *main_renderer = renderer_get_by_name ("main");
    renderer_set_update (main_renderer, main_renderer_update, main_renderer);

    alt_key_timer = timer_new ();
    timer_start (alt_key_timer);

}

static void app_on_exit (void) { 

    timer_destroy (alt_key_timer);

    app_ui_end ();

    cimage_delete (cimage);

}

State *app_state_new (void) {

    State *new_app_state = (State *) malloc (sizeof (State));
    if (new_app_state) {
        // new_game_state->state = IN_GAME;

        new_app_state->update = NULL;

        new_app_state->on_enter = app_on_enter;
        new_app_state->on_exit = app_on_exit;
    }

}