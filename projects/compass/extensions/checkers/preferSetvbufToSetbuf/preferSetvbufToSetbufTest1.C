
//Your test file code goes here.
#include <stdio.h>

int main(void) {
	FILE* file;
	char *buf = NULL;
	/* Setup file */
	setbuf(file, buf);

	/* Setup file */
	if (setvbuf(file, buf, buf ? _IOFBF : _IONBF, BUFSIZ) != 0) {
		/* Handle error */
	}
}
