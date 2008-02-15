#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netdb.h>
#include <time.h>
#include <sys/time.h>
#include <assert.h>
#include <errno.h>
#include <signal.h>

#define DEFAULTPORT 10301

/* sockaddr.c */
extern int parse_ip(const char *s, struct sockaddr_in *addr);

/* util.c */
int xwrite(int sd, const void *buf, size_t len);
int xread(int sd, void *buf, size_t len);

enum { MODE_SAVE, MODE_RESTORE, MODE_ACCESS, MODE_REMOVE };
enum { REPLY_OK, REPLY_FAIL };

/* FIXME: Not endian-proof */
struct req {
	int mode;
	char id[1024];
};

struct rep {
	int status;
};

