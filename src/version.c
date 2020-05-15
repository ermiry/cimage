#include <stdio.h>

#include "version.h"

// print full cimage version information 
void cimage_version_print_full (void) {

    printf ("\nCimage Version: %s\n", CIMAGE_VERSION_NAME);
    printf ("Release Date & time: %s - %s\n", CIMAGE_VERSION_DATE, CIMAGE_VERSION_TIME);
    printf ("Author: %s\n\n", CIMAGE_VERSION_AUTHOR);

}

// print the version id
void cimage_version_print_version_id (void) {

    printf ("\n\nCimage Version ID: %s\n", CIMAGE_VERSION);

}

// print the version name
void cimage_version_print_version_name (void) {

    printf ("\n\nCimage Version: %s\n", CIMAGE_VERSION_NAME);

}