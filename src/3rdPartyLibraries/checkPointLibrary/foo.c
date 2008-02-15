#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

static void
segv(int sig)
{
	while (1)
		;
}

int
main(int argc, char *argv[])
{
	struct sigaction sa;
	int i = 0;

	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = segv;
	if (0 > sigaction(SIGSEGV, &sa, NULL)) {
		fprintf(stderr, "cannot install sighandler\n");
		exit(1);
	}
	printf("%s: ckpt test program (pid = %d)\n",
	       argv[0], getpid());
	while (1) {
		unsigned j;
		for (j = 0; j < 100000000; j++)
			;
		malloc(100000);
		fprintf(stderr, "%d\n", ++i);
	}
	return 0;
}
