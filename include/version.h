#ifndef _CIMAGE_VERSION_H_
#define _CIMAGE_VERSION_H_

#define CIMAGE_VERSION                  "0.1"
#define CIMAGE_VERSION_NAME             "Pre-Release 0.1"
#define CIMAGE_VERSION_DATE			    "14/05/2020"
#define CIMAGE_VERSION_TIME			    "19:47 CST"
#define CIMAGE_VERSION_AUTHOR			"Erick Salas"

// print full cimage version information 
extern void cimage_version_print_full (void);

// print the version id
extern void cimage_version_print_version_id (void);

// print the version name
extern void cimage_version_print_version_name (void);

#endif