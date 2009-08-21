#include <stdlib.h>

// This routine is strictly for testing the visit_next function; it should
// visit nodes in the order of i = 1, 2, 3 ...

void foo() {
	char* x = NULL;
	int i = 0;
	while (i == -1) {
		i = -2;
	}
		
	for (i = -3; i == -4; i++) {
		i = -5;
	}

	for (i = -3; i == 5; i++) {
		i = 6;
		do {
			i = 3;
			free(x);
			i = 1;
		} while (i == 2);
		i = 4;
	}

	while (i == 7) {
		i = 8;
		if (i == 9) {
			i = 10;
			return;
		}
		i == 11;
	}


	for (i = 12; i == 13; i++) {
		i = 14;
	}
}
