#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define _XOPEN_SOURCE 700
#include <sys/types.h>
#include <sys/stat.h>

#include "cengine/utils/utils.h"
#include "cengine/utils/log.h"
#include "cengine/utils/json.h"

// returns an allocated string with the file extensio
// NULL if no file extension
char *files_get_file_extension (const char *filename) {

    char *retval = NULL;

    if (filename) {
        char *ptr = strrchr (filename, '.');
        if (ptr) {
            *ptr++;
            size_t ext_len = 0;
            char *p = ptr;
            while (*p++) ext_len++;

            char *ext = (char *) calloc (ext_len + 1, sizeof (char));
            if (ext) {
                memcpy (ext, ptr, ext_len);
                ext[ext_len] = '\0';

                retval = ext;
            }
        }
        
    }

    return retval;

}

FILE *file_open (const char *filename, const char *modes, struct stat *filestatus) {

    FILE *fp = NULL;

    if (filename) {
        memset (filestatus, 0, sizeof (struct stat));
        if (!stat (filename, filestatus)) 
            fp = fopen (filename, modes);

        else cengine_log_msg (stderr, LOG_ERROR, LOG_NO_TYPE, 
            c_string_create ("File %s not found!", filename));
    }

    return fp;

}

// opens and reads a file into a buffer
char *file_read (const char *filename, int *file_size) {

    char *file_contents = NULL;

    if (filename) {
        struct stat filestatus;
        FILE *fp = file_open (filename, "rt", &filestatus);
        if (fp) {
            *file_size = filestatus.st_size;
            file_contents = (char *) malloc (filestatus.st_size);

            // read the entire file into the buffer
            if (fread (file_contents, filestatus.st_size, 1, fp) != 1) {
                cengine_log_msg (stderr, LOG_ERROR, LOG_NO_TYPE, "Failed to read file contents!");
                free (file_contents);
            }

            fclose (fp);
        }

        else cengine_log_msg (stderr, LOG_ERROR, LOG_NO_TYPE, c_string_create ("Unable to open file %s", filename));
    }

    return file_contents;

}

json_value *file_json_parse (const char *filename) {

    json_value *value = NULL;

    if (filename) {
        int file_size;
        char *file_contents = file_read (filename, &file_size);
        json_char *json = (json_char *) file_contents;
        value = json_parse (json, file_size);

        free (file_contents);
    }

    return value;
    
}