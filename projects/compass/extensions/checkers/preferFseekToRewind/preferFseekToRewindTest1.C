
//Your test file code goes here.
#include <stdio.h>

void foo()
{
	FILE* fptr = fopen("file.ext", "r");
	if (fptr == NULL) {
		/* handle open error */
	}

	/* read data */

	rewind(fptr);
}
