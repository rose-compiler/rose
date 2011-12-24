#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <map>

void print_array(unsigned int* array, unsigned int size) {
	
	printf("printing array - begin\n");
	for(unsigned int index = 0; index < size; index++) {
		printf("%u: %u\n", index, array[index]);
	}
	printf("printing array - end\n");

	return;
}
		
