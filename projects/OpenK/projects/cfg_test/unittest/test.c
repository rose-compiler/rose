#include <stdlib.h>

void foo(int * array, int * lower, int* upper)
{
	int i, j, s = 3;
	int *v, *sptr;
	//int &sref = &s; // in C, there is no reference variable
	sptr = &s;
	v = (int *) malloc(10 * sizeof(int));
	
	for (i=0; i<100; i++)  ; // y
	for (i=0,j=0; i<100; i++)  ;
	for (int k=0; k < 10; k++); // y
	for (int s=0, t = 1; s < 10, t < 5; s++, ++t);
	for (i=0; i < 10; i += 3); // y
	for (i=0, j+3; i < 10; i = i + 3);

	for (array = lower; array<upper; array++ ); // y

	for (int s; s < 5; s++);
	int *ip = &i;

	for (*ip = 0; *ip < 3; (*ip)++); // is it canonical ?
	for (int *sp = &s; *sp < 3; ++(*sp)); // ?
	
	for (i = *sptr; i < 3; i++) { // y
		v[i] ++;
	}
	
	for (i = 3; i < 10; i--);
	for (i = 4; 1 < i; i--); // y
	for (i = 0; i < 10; i++) {
		i = s;
	}
	
	for (int i = 0; i < 10; i++) { // y
		for (int j = 0; j < 20; j++) {	
			int i;
			i = 30;
			j = 20;
		}
	}
	
}
