#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

void handleFiles(char * otherFileName) {
    int value = 0;
    value = isupper(*otherFileName);
}

void weakness(char * taintedValue) {
    int inSnippet = isupper(*taintedValue);
}
