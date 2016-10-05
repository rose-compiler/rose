/* This is the stuff related to snippets, to be included by the target when the injection process is told not to copy related
 * global declarations, #includes, and imports. */
#ifndef snippetRelated_H
#define snippetRelated_H

/* From snippets2.c */
extern int snippet_usage_counter;

/* From snippets3.c */
#include <stdio.h>
int copy_me_too();

/* From snippets4.c */
#include <stdlib.h>
#include <string.h>

/* Added by DQ to avoid interpretation of assert macro as implicit function */
#include <assert.h>

struct Struct1 {
    int int_member;
    char char_member;
    const char *const_string_member;
    char* string_member;
    double double_member;
};

/* From snippets5.c */

/* From snippets6.c */
#include <stdlib.h>
#include <string.h>

void randomOffByOne(int addend1);
void addWithError(int addend1, int addend2, int result);
void copy_string10(char *dst, const char *src);
void allocate_string(const char *s);

#endif
