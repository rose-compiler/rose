#include <stdio.h>

shared int *foo;

#define STRING "Hello World!\n"

main() {
	int i;
	fprintf(stdout, STRING);
	i = *foo;
}
