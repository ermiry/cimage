#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>

#include "cengine/types/string.h"

static inline void char_copy (char *to, char *from) {

    while (*from) *to++ = *from++;
    *to = '\0';

}

String *str_new (const char *str) {

    String *string = (String *) malloc (sizeof (String));
    if (string) {
        memset (string, 0, sizeof (String));

        if (str) {
            string->len = strlen (str);
            string->str = (char *) calloc (string->len + 1, sizeof (char));
            char_copy (string->str, (char *) str);
        }
    }

    return string;

}

String *str_create (const char *format, ...) {

    String *string = NULL;

    if (format) {
        char *fmt = strdup (format);

        va_list argp;
        va_start (argp, format);
        char oneChar[1];
        int len = vsnprintf (oneChar, 1, fmt, argp);
        va_end (argp);

        char *str = (char *) calloc (len + 1, sizeof (char));

        va_start (argp, format);
        vsnprintf (str, len + 1, fmt, argp);
        va_end (argp);

        string = str_new (str);

        free (str);
        free (fmt);
    }

    return string;

}

void str_delete (String *string) {

    if (string) {
        if (string->str) free (string->str);
        free (string);
    }

}

void str_copy (String *to, String *from) {

    if (to && from) {
        while (*from->str)
            *to->str++ = *from->str++;

        *to->str = '\0';
        to->len = from->len;
    }

}

void str_concat (String *des, String *s1, String *s2) {

    if (des && s1 && s2) {
        while (*s1->str) *des->str++ = *s1->str++;
        while (*s2->str) *des->str++ = *s2->str++;

        *des->str = '\0';

        des->len = s1->len + s2->len;
    }

}

void str_to_upper (String *string) {

    if (string) for (int i = 0; i < string->len; i++) string->str[i] = toupper (string->str[i]);

}

void str_to_lower (String *string) {

    if (string) for (int i = 0; i < string->len; i++) string->str[i] = tolower (string->str[i]);

}

int str_compare (const String *s1, const String *s2) { return strcmp (s1->str, s2->str); }

char **str_split (String *string, const char delim, int *n_tokens) {

    char **result = 0;
    size_t count = 0;
    char *temp = string->str;
    char *last = 0;
    char dlm[2];
    dlm[0] = delim;
    dlm[1] = 0;

    // count how many elements will be extracted
    while (*temp) {
        if (delim == *temp) {
            count++;
            last = temp;
        }

        temp++;
    }

    count += last < (string->str + strlen (string->str) - 1);

    count++;

    result = (char **) calloc (count, sizeof (char *));
    *n_tokens = count;

    if (result) {
        size_t idx = 0;
        char *token = strtok (string->str, dlm);

        while (token) {
            // assert (idx < count);
            *(result + idx++) = strdup (token);
            token = strtok (0, dlm);
        }

        // assert (idx == count - 1);
        *(result + idx) = 0;
    }

    return result;

}

void str_remove_char (String *string, char garbage) {

    char *src, *dst;
    for (src = dst = string->str; *src != '\0'; src++) {
        *dst = *src;
        if (*dst != garbage) dst++;
    }
    *dst = '\0';

}

int str_contains (String *string, char *to_find) {

    int slen = string->len;
    int tFlen = strlen (to_find);
    int found = 0;

    if (slen >= tFlen) {
        for (unsigned int s = 0, t = 0; s < slen; s++) {
            do {
                if (string->str[s] == to_find[t]) {
                    if (++found == tFlen) return 0;
                    s++;
                    t++;
                }
                else { s -= found; found = 0; t = 0; }

              } while(found);
        }

        return 1;
    }

    else return -1;

}