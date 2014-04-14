#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

void handleFiles(char * otherFileName) {
    int value = 0;
    // Note that this will become: value = ((*__ctype_b_loc ())[(int) ((*otherFileName))] & (unsigned short int) _ISupper);
    value = isupper(*otherFileName);
}

void weakness(char * taintedValue) {
    int inSnippet = 0;
    // Note that this will become: inSnippet = ((*__ctype_b_loc ())[(int) ((*taintedValue))] & (unsigned short int) _ISupper);
    inSnippet = isupper(*taintedValue);
}
