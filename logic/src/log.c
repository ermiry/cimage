#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"

char *getMsgType (LogMsgType type) {

    char temp[10];

    switch (type) {
        case ERROR: strcpy (temp, "[ERROR]"); break;
        case WARNING: strcpy (temp, "[WARNING]"); break;
        case SUCCESS: strcpy (temp, "[SUCCESS]"); break;
        case DEBUG: strcpy (temp, "[DEBUG]"); break;
        case TEST: strcpy (temp, "[TEST]"); break;

        default: break;
    }

    char *retval = (char *) calloc (strlen (temp) + 1, sizeof (temp));
    strcpy (retval, temp);

    return retval;

}

void logMsg (FILE *__restrict __stream, LogMsgType firstType, LogMsgType secondType,
    const char *msg) {

    char *first = getMsgType (firstType);
    char *second = NULL;

    char *message = NULL;

    if (secondType != 0) {
        second = getMsgType (secondType);
        message = createString ("%s%s: %s\n", first, second, msg);
    }

    else message = createString ("%s: %s\n", first, msg);

    // log messages with color
    switch (firstType) {
        case ERROR: fprintf (__stream, COLOR_RED "%s" COLOR_RESET, message); break;
        case WARNING: fprintf (__stream, COLOR_YELLOW "%s" COLOR_RESET, message); break;
        case SUCCESS: fprintf (__stream, COLOR_GREEN "%s" COLOR_RESET, message); break;

        default: fprintf (__stream, "%s", message); break;
    }

    if (!first) free (first);
    if (!second) free (second);

}