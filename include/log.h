#ifndef LOG_H
#define LOG_H

#include <stdio.h>

/*** LOG ***/

#define COLOR_RED       "\x1b[31m"
#define COLOR_GREEN     "\x1b[32m"
#define COLOR_YELLOW    "\x1b[33m"
#define COLOR_BLUE      "\x1b[34m"
#define COLOR_MAGENTA   "\x1b[35m"
#define COLOR_CYAN      "\x1b[36m"
#define COLOR_RESET     "\x1b[0m"

typedef enum LogMsgType {

	NO_TYPE = 0,

    ERROR = 1,
    WARNING,
    SUCCESS,
    DEBUG,
    TEST,

    IMAGE,

} LogMsgType;

void logMsg (FILE *__restrict __stream, LogMsgType firstType, LogMsgType secondType,
    const char *msg);

#endif