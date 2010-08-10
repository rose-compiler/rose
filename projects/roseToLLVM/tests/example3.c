#include <stdio.h>

//
// Test escape characters
//
int main(int argc, char *argv[]) {

    printf("\\n = %i\n", '\n');
    printf("\\t = %i\n", '\t');
    printf("\\b = %i\n", '\b');
    printf("\\r = %i\n", '\r');
    printf("\\f = %i\n", '\f');
    printf("\\v = %i\n", '\v');
    printf("\\\\ = %i\n", '\\') ;
    printf("\\\' = %i\n", '\'') ;
    printf("\\\" = %i\n", '\"') ;
    printf("\\a = %i\n", '\a');
    printf("\\? = %i\n", '\?');

    return 0;
}
