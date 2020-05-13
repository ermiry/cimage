#include <unistd.h>

#define _XOPEN_SOURCE 700
#include <sys/types.h>
#include <dirent.h>

#include "cengine/os.h"
#include "cengine/threads/thread.h"
#include "cengine/files.h"

#include "cengine/ui/notification.h"

#include "cengine/utils/utils.h"
#include "cengine/utils/log.h"

#include "cimage.h"
#include "app/states/app.h"
#include "app/media.h"

#include "app/ui/app.h"

#pragma region media items

static MediaItem *media_item_new (void) {

    MediaItem *item = (MediaItem *) malloc (sizeof (MediaItem));
    if (item) {
        item->image = NULL;
        item->filename = NULL;
        item->path = NULL;
        item->selected = false;
    }

    return item;

}

static void media_item_delete (void *item_ptr) {

    if (item_ptr) {
        MediaItem *media = (MediaItem *) item_ptr;

        str_delete (media->filename);
        str_delete (media->path);

        free (media);
    }

}

void media_item_delete_dummy (void *item_ptr) {}

static int media_item_comparator (const void *a, const void *b) {

    if (a && b) {
        MediaItem *item_a = (MediaItem *) a;
        MediaItem *item_b = (MediaItem *) b;

        return strcmp (item_a->filename->str, item_b->filename->str);
    }

}

#pragma endregion

#pragma region loading

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
static DoubleList *media_folder_read (const char *images_dir) {

    DoubleList *images = NULL;

    if (images_dir) {
        struct dirent *ep = NULL;
        DIR *dp = opendir (images_dir);
        if (dp) {
            images = dlist_init (media_item_delete, media_item_comparator);

            #ifdef CIMAGE_DEBUG
            cengine_log_msg (stdout, LOG_SUCCESS, LOG_NO_TYPE, "Starting to read images...");
            #endif

            while ((ep = readdir (dp)) != NULL) {
                if (strcmp (ep->d_name, ".") && strcmp (ep->d_name, "..")) {
                    if (is_image_file (ep->d_name)) {
                        MediaItem *media_item = media_item_new ();
                        media_item->filename = str_new (ep->d_name);
                        media_item->path = str_create ("%s/%s", images_dir, ep->d_name);

                        dlist_insert_after (images, dlist_end (images), media_item);
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

static void *media_load (void *folder_name_ptr) {

    if (folder_name_ptr) {
        String *folder_name = (String *) folder_name_ptr;

        // get images from directory
        cimage->images = media_folder_read (folder_name->str);
        if (cimage->images) {
            if (dlist_size (cimage->images) > 0) {
                // prepare ui for images
                app_ui_images_set_ui_elements (dlist_size (cimage->images), DEFAULT_GRID_N_COLS, DEFAULT_GRID_N_ROWS);
                app_ui_actionsbar_show ();
                app_ui_statusbar_show (folder_name->str, cimage->images->size);

                for (ListElement *le = dlist_start (cimage->images); le; le = le->next) {
                    // printf ("%s\n", ((String *) le->data)->str);
                    app_ui_image_create (((MediaItem *) le->data));
                    app_ui_image_display (((MediaItem *) le->data)->image);
                }

                // printf ("\n\n\n");
                // UIElement *ui_element = NULL;
                // for (ListElement *le = dlist_start (images_panel->children); le; le = le->next) {
                //     ui_element = (UIElement *) le->data;
                //     printf ("x %d - y %d\n", ui_element->transform->rect.x, ui_element->transform->rect.y);
                // }
            }
            
            else {
                // TODO: display error message in ui
                char *status = c_string_create ("No images found in dir: %s!", folder_name->str);
                if (status) {
                    #ifdef CIMAGE_DEBUG
                    cengine_log_msg (stderr, LOG_WARNING, LOG_NO_TYPE, status);
                    #endif

                    ui_notification_create_and_display (main_noti_center, renderer_get_by_name ("main"), NOTI_TYPE_ERROR, 4, false,
                        NULL, status);

                    free (status);
                }

                dlist_delete (cimage->images);
                cimage->images = NULL; 

                str_delete (cimage->opened_folder_name);
                cimage->opened_folder_name = NULL;
            }
        }
    }

    return NULL;

}

#pragma endregion

#pragma region folder

void media_folder_close (void *args) {

    // remove UI
    app_ui_images_remove_ui_elements ();

    cimage_delete (cimage);
    cimage = NULL;

}

// TODO: implement cross-platform code
void media_folder_select (void *args) {

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

            thread_create_detachable (media_load, cimage->opened_folder_name);
        }

        free (command);
    }
    #endif

}

#pragma endregion

#pragma region operations

// gets executed on every input on the search input
void media_search (void *args) {

    if (args) {
        InputField *search_input = (InputField *) args;

        // remove all images from the grid
        GridLayout *grid = (GridLayout *) images_panel->layout;
        ui_layout_grid_remove_ui_elements (grid);

        String *query = str_new (search_input->text->text->str);

        // search all the images that matches our query letter by letter
        MediaItem *item = NULL;
        for (ListElement *le = dlist_start (cimage->images); le; le = le->next) {
            item = (MediaItem *) le->data;

            ui_element_set_active (item->image->ui_element, false);

            if (!strncasecmp (query->str, item->filename->str, query->len)) {
                // add this image for display
                ui_layout_grid_add_element_at_end (grid, item->image->ui_element);
                ui_element_set_active (item->image->ui_element, true);
            }
        }

        // update images count in status bar
        app_ui_statusbar_total_set (ui_layout_grid_get_elements_count (grid));

        str_delete (query);
    }

}

#pragma endregion