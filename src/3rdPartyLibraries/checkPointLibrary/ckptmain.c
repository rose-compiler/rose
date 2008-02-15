#include "sys.h"
#include "ckptimpl.h"

extern char libckpt[];
extern unsigned long libckptlen;

enum {
	MAXBUF=1024
};

static void
dopreload()
{
	char buf[MAXBUF], *p;
	int fd, rv;
	
	snprintf(buf, sizeof(buf), "LD_PRELOAD=/tmp/tmplibckptXXXXXX");
	p = buf+strlen("LD_PRELOAD=");

	fd = mkstemp(p);
	if(0 > fd){
		fprintf(stderr, "ckpt I/O error\n");
		exit(1);
	}
	rv = xwrite(fd, libckpt, libckptlen);
	if(0 > rv){
		fprintf(stderr, "ckpt I/O error\n");
		exit(1);
	}
	close(fd);

	if(0 > putenv(buf)){
		fprintf(stderr, "ckpt error\n");
		exit(1);
	}
}

static void
usage()
{
	fprintf(stderr,
		"usage: ckpt [-n filename] [-a signal] [-c] <program> <args>\n");
	exit(1);
}

int
main(int argc, char *argv[])
{
	char buf[2*MAXBUF];  /* must be bigger than CKPT_MAXNAME */
	int c;

	opterr = 0;
	optind = 0;
	/* `+' means don't reorder argv */
	while (EOF != (c = getopt(argc, argv, "+n:ca:")))
		switch (c) {
		case 'n':
			snprintf(buf, sizeof(buf), "CKPT_NAME=%s", optarg);
			putenv(buf);
			break;
		case 'c':
			snprintf(buf, sizeof(buf), "CKPT_CONTINUE=1");
			putenv(buf);
			break;
		case 'a':
			snprintf(buf, sizeof(buf), "CKPT_ASYNCSIG=%s", optarg);
			putenv(buf);
			break;
		case '?':
			fprintf(stderr, "Unknown option `-%c'.\n", optopt);
			usage();
			break;
		}
	argc -= optind;
	argv += optind;

	if(argc <= 0)
		usage();

	dopreload();
	execv(argv[0], &argv[0]);
	perror("exec");
	exit(1);
}
