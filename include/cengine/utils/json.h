/* vim: set et ts=3 sw=3 sts=3 ft=c:
 *
 * Copyright (C) 2012, 2013, 2014 James McLaughlin et al.  All rights reserved.
 * https://github.com/udp/json-parser
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _CENGIN_JSON_H_
#define _CENGIN_JSON_H_

#ifndef json_char
   #define json_char char
#endif

#ifndef json_int_t
   #ifndef _MSC_VER
      #include <inttypes.h>
      #define json_int_t int64_t
   #else
      #define json_int_t __int64
   #endif
#endif

#include <stdlib.h>

#ifdef __cplusplus

   #include <string.h>

   extern "C"
   {

#endif

typedef struct {

   unsigned long max_memory;
   int settings;

   /* Custom allocator support (leave null to use malloc/free)
    */

   void * (* mem_alloc) (size_t, int zero, void * user_data);
   void (* mem_free) (void *, void * user_data);

   void * user_data;  /* will be passed to mem_alloc and mem_free */

   size_t value_extra;  /* how much extra space to allocate for values? */

} json_settings;

#define json_enable_comments  0x01

typedef enum {

   json_none,
   json_object,
   json_array,
   json_integer,
   json_double,
   json_string,
   json_boolean,
   json_null

} json_type;

extern const struct _json_value json_value_none;
       
typedef struct _json_object_entry {

    json_char * name;
    unsigned int name_length;
    
    struct _json_value * value;
    
} json_object_entry;

typedef struct _json_value {

   struct _json_value * parent;

   json_type type;

   union {

      int boolean;
      json_int_t integer;
      double dbl;

      struct {

		unsigned int length;
		json_char * ptr; /* null terminated */

      } string;

      struct {
		unsigned int length;

		json_object_entry * values;

      } object;

      struct
      {
         unsigned int length;
         struct _json_value ** values;

         #if defined(__cplusplus) && __cplusplus >= 201103L
         decltype(values) begin () const
         {  return values;
         }
         decltype(values) end () const
         {  return values + length;
         }
         #endif

      } array;

   } u;

   union
   {
      struct _json_value * next_alloc;
      void * object_mem;

   } _reserved;

   #ifdef JSON_TRACK_SOURCE

      /* Location of the value in the source JSON
       */
      unsigned int line, col;

   #endif

} json_value;
       
json_value * json_parse (const json_char * json,
                         size_t length);

#define json_error_max 128
json_value * json_parse_ex (json_settings * settings,
                            const json_char * json,
                            size_t length,
                            char * error);

void json_value_free (json_value *);


/* Not usually necessary, unless you used a custom mem_alloc and now want to
 * use a custom mem_free.
 */
void json_value_free_ex (json_settings * settings,
                         json_value *);


#ifdef __cplusplus
   } /* extern "C" */
#endif

#endif