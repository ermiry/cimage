#ifndef _CENGINE_UTILS_H_
#define _CENGINE_UTILS_H_

#include <stdbool.h>
#include <stdint.h>

/*** misc ***/

extern bool system_is_little_endian (void);

/*** math ***/

extern int clamp_int (int val, int min, int max);

int abs_int (int value);

extern float lerp (float first, float second, float by);

/*** random ***/

// init psuedo random generator based on our seed
extern void random_set_seed (unsigned int seed);

extern int random_int_in_range (int min, int max);

/*** converters ***/

// convert a string representing a hex to a string
extern int xtoi (char *hexString);

extern char *itoa (int i, char *b);

extern uint32_t convert_rgba_to_hex (uint8_t r, uint8_t g, uint8_t b, uint8_t a);

/*** c strings ***/

// creates a new c string with the desired format, as in printf
extern char *c_string_create (const char *format, ...);

// splits a c string into tokens based on a delimiter
extern char **c_string_split (char *string, const char delim, int *n_tokens);

// copies a c string into another one previuosly allocated
extern void c_string_copy (char *to, const char *from);

// revers a c string
extern char *c_string_reverse (char *str);

// removes all ocurrances of a char from a string
extern void c_string_remove_char (char *string, char garbage);

// removes the exact sub string from the main one
// returns a newly allocated copy of the original str but withput the sub
extern char *c_string_remove_sub (char *str, const char *sub);

// creates a newly allocated string using the data between the two pointers of the SAME string
// returns a new string, NULL on error
extern char *c_string_create_with_ptrs (char *first, char *last);

// removes a substring from a c string that is defined after a token
// returns a newly allocated string without the sub,
// and option to retrieve the actual substring
extern char *c_string_remove_sub_after_token (char *str, const char token, char **sub);

// removes a substring from a c string delimited by two equal tokens
// takes the first and last appearance of the token
// example: test_20191118142101759__TEST__.png - token: '_'
// result: test.png
// returns a newly allocated string, and a option to get the substring
extern char *c_string_remove_sub_simetric_token (char *str, const char token, char **sub);

// removes a substring from a c string delimitied by two equal tokens
// and you can select the idx of the token; use -1 for last token
// example: test_20191118142101759__TEST__.png - token: '_' - idx (first: 1,  last: 3)
// result: testTEST__.png
// returns a newly allocated string, and a option to get the substring
extern char *c_string_remove_sub_range_token (char *str, const char token, unsigned int first, int last,
	char **sub);

#endif