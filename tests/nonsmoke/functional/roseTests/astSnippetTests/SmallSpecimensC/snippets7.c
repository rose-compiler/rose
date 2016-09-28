#include <stdio.h>
#include <stdlib.h>

void output_value(char * value) {
    if (value == NULL) {
        printf("output_value: value = NULL\n");
    } else {
        printf("output_value: value = %s\n", value);
    }
}

void weakness(char * taintedValue) {
    output_value(taintedValue);
}

int main(int argc,char *argv[])
{
    if (argc < 2) {
        printf("Error: requires a single command-line argument\n");
        exit(1);
    }
    char* tainted_buff = argv[1];
    if (tainted_buff != NULL) {
        weakness(tainted_buff);
    }
}
