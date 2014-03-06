/* This is the stuff related to snippets, to be included by the target when the injection process is told not to copy related
 * global declarations, #includes, and imports. */
#ifndef snippetRelated_H
#define snippetRelated_H

#define COPY_RELATED_THINGS_no 0
#define COPY_RELATED_THINGS_yes 1

#if COPY_RELATED_THINGS == COPY_RELATED_THINGS_no

/* From snippets2.c */
int snippet_usage_counter = 0;

/* From snippets3.c */
#include <stdio.h>

/* From snipets4.c */
#include <stdlib.h>
#include <string.h>

struct Struct1 {
    int int_member;
    char char_member;
    const char *const_string_member;
    char* string_member;
    double double_member;
};

/* From snippets6.c */
#include <stdlib.h>
#include <string.h>


#endif
#endif
