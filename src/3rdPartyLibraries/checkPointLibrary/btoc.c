#include <stdio.h>
#include <stdlib.h>

int
main(int argc, char *argv[])
{
	int i, fill = 15, rv, cnt;
	unsigned long len;
	unsigned char buf[1024];

	if (argc != 2) {
		fprintf(stderr, "usage: %s NAME\n", argv[0]);
		exit(1);
	}

	printf("char %s[] = {\n", argv[1]);
	len = cnt = 0;
	while (0 != (rv = fread(buf, 1, sizeof(buf), stdin))) {
		for (i = 0; i < rv; i++) {
			printf("0x%.2x,", buf[i]);
			if (++cnt == fill) {
				printf("\n");
				cnt = 0;
			}
		}
		len += rv;
	}
	printf("\n};\n");
	printf("unsigned long %slen = %ld;\n", argv[1], len);
	return 0;
}
