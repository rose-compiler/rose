#include "sys.h"
#include "ckptimpl.h"

int
main(int argc, char *argv[])
{
	restart();
	fprintf(stderr, "restart failed\n");
	exit(1);
}
