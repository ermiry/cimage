#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cengine/utils/utils.h"
#include "cengine/utils/log.h"

static char *cengine_get_msg_type (LogMsgType type) {

    char temp[15];

    switch (type) {
        case LOG_ERROR: strcpy (temp, "[LOG_ERROR]"); break;
        case LOG_WARNING: strcpy (temp, "[WARNING]"); break;
        case LOG_SUCCESS: strcpy (temp, "[SUCCESS]"); break;
        case LOG_DEBUG: strcpy (temp, "[DEBUG]"); break;
        case LOG_TEST: strcpy (temp, "[TEST]"); break;

        default: break;
    }

    char *retval = (char *) calloc (strlen (temp) + 1, sizeof (temp));
    strcpy (retval, temp);

    return retval;

}

void cengine_log_msg (FILE *__restrict __stream, LogMsgType firstType, LogMsgType secondType,
    const char *msg) {

    char *first = cengine_get_msg_type (firstType);
    char *second = NULL;
    char *message = NULL;

    if (secondType != 0) {
        second = cengine_get_msg_type (secondType);

        if (firstType == LOG_DEBUG)
            message = c_string_create ("%s: %s\n", second, msg);
        
        else message = c_string_create ("%s%s: %s\n", first, second, msg);
    }

    else if (firstType != LOG_DEBUG)
        message = c_string_create ("%s: %s\n", first, msg);

    // log messages with color
    switch (firstType) {
        case LOG_DEBUG: 
            fprintf (__stream, COLOR_MAGENTA "%s: " COLOR_RESET "%s\n", first, msg); break;

        case LOG_ERROR: fprintf (__stream, COLOR_RED "%s" COLOR_RESET, message); break;
        case LOG_WARNING: fprintf (__stream, COLOR_YELLOW "%s" COLOR_RESET, message); break;
        case LOG_SUCCESS: fprintf (__stream, COLOR_GREEN "%s" COLOR_RESET, message); break;

        default: fprintf (__stream, "%s", message); break;
    }

    if (message) free (message);

}