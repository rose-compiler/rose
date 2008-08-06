#include <stdio.h>
#include <stdlib.h>

/* Incorrect Code */
void func1() {
	int* x;

	x = (int*) malloc(sizeof(int));
	free(x);
}

/* Correct Code */
void func2() {
	int *x;
	x = (int*) malloc(sizeof(int));
	free(x);
	x = NULL;
}

int main(void) {

	func1();
	func2();

	return 1;
}
