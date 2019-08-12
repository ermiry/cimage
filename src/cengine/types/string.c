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

        else string->str = NULL;
    }

    return string;

}

String *str_create (const char *format, ...) {

    String *string = NULL;

    if (format) {
        char *fmt = strdup (format);

        va_list argp;
        va_start (argp, format);
        char one_char[1];
        int len = vsnprintf (one_char, 1, fmt, argp);
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

void str_delete (void *str_ptr) {

    if (str_ptr) {
        String *string = (String *) str_ptr;
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

void str_replace (String *old, const char *str) {

    if (old && str) {
        free (old->str);
        old->len = strlen (str);
        old->str = (char *) calloc (old->len + 1, sizeof (char));
        char_copy (old->str, (char *) str);
    }

}

// concatenates two strings into a new one
String *str_concat (String *s1, String *s2) {

    String *des = NULL;

    if (s1 && s2) {
        des = str_new (NULL);
        des->str = (char *) calloc (s1->len + s2->len + 1, sizeof (char));
        if (des->str) {
            while (*s1->str) *des->str++ = *s1->str++;
            while (*s2->str) *des->str++ = *s2->str++;

            *des->str = '\0';

            des->len = strlen (des->str);
        }

        else {
            str_delete (des);
            des = NULL;
        } 
    }

    return des;

}

// appends a char to the end of the string
// reallocates the same string
void str_append_char (String *s, const char c) {

    if (s) {
        unsigned int new_len = s->len + 1;   

        s->str = (char *) realloc (s->str, new_len);
        if (s->str) {
            char *des = s->str + (s->len);
            *des = c;
            s->len = new_len;
        }
    }

}

// appends a c string at the end of the string
// reallocates the same string
void str_append_c_string (String *s, const char *c_str) {

    if (s && c_str) {
        unsigned int new_len = s->len + strlen (c_str);

        s->str = (char *) realloc (s->str, new_len);
        if (s->str) {
            char *des = s->str + (s->len);
            char_copy (des, (char *) c_str);
            s->len = new_len;
        }
    }

}

void str_to_upper (String *string) {

    if (string) for (int i = 0; i < string->len; i++) string->str[i] = toupper (string->str[i]);

}

void str_to_lower (String *string) {

    if (string) for (int i = 0; i < string->len; i++) string->str[i] = tolower (string->str[i]);

}

int str_compare (const String *s1, const String *s2) { return strcmp (s1->str, s2->str); }

int str_comparator (const void *a, const void *b) {

    if (a && b) return strcmp (((String *) a)->str, ((String *) b)->str);

}

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

// removes the last char from a string
void str_remove_last_char (String *s) {

    if (s) {
        if (s->len > 0) {
            unsigned int new_len = s->len - 1;

            s->str = (char *) realloc (s->str, s->len);
            if (s->str) {
                s->str[s->len - 1] = '\0';
                s->len = new_len;
            }
        }
    }

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

/*** serialization ***/

// returns a ptr to a serialized string
void *str_serialize (String *string, SStringSize size) {

    void *retval = NULL;

    if (string) {
        switch (size) {
            case SS_SMALL: {
                SStringS *s_small = (SStringS *) malloc (sizeof (SStringS));
                if (s_small) {
                    memset (s_small, 0, sizeof (SStringS));
                    strncpy (s_small->string, string->str, 64);
                    s_small->len = string->len > 64 ? 64 : string->len;
                    retval = s_small;
                }
            } break;
            case SS_MEDIUM: {
                SStringM *s_medium = (SStringM *) malloc (sizeof (SStringM));
                if (s_medium) {
                    memset (s_medium, 0, sizeof (SStringM));
                    strncpy (s_medium->string, string->str, 128);
                    s_medium->len = string->len > 128 ? 128 : string->len;
                    retval = s_medium;
                }
            } break;
            case SS_LARGE: {
                SStringL *s_large = (SStringL *) malloc (sizeof (SStringL));
                if (s_large) {
                    memset (s_large, 0, sizeof (SStringL));
                    strncpy (s_large->string, string->str, 256);
                    s_large->len = string->len > 256 ? 256 : string->len;
                    retval = s_large;
                }
            } break;
            case SS_EXTRA_LARGE: {
                SStringXL *s_xlarge = (SStringXL *) malloc (sizeof (SStringXL));
                if (s_xlarge) {
                    memset (s_xlarge, 0, sizeof (SStringXL));
                    strncpy (s_xlarge->string, string->str, 512);
                    s_xlarge->len = string->len > 512 ? 512 : string->len;
                    retval = s_xlarge;
                }
            } break;

            default: break;
        }
    }

    return retval;

}