#include <stdlib.h>
#include <string.h>

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