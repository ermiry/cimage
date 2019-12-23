#include <unistd.h>

#define _XOPEN_SOURCE 700
#include <sys/types.h>
#include <dirent.h>

#include "cengine/os.h"

#include "app/states/app.h"

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
                    app_ui_image_create (((ImageItem *) le->data));
                    app_ui_image_display (((ImageItem *) le->data)->image);
                }
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

            thread_create_detachable (images_load, cimage->opened_folder_name);
        }

        free (command);
    }
    #endif

}