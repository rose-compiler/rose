#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <errno.h>
#include <assert.h>

#define __USE_GNU  /* F_GETSIG */
#include <fcntl.h>


/* checkpoints of file descriptors:
   
   transplantation:

   the file is slurped into the checkpoint and reconstructed on the new
   host.  what should the file be named?

   - home directory renaming

   remote i/o:

   a remote I/O configuration is established, and I/O persists to the file
   that was left behind.

   terminal:
   alas, we still have terminals!
   inherit the terminal in which we started, reconnect standard i/o.
   this is pretty much what happens now with foo printing back to the console.
 
fd to name issues:
-plan 9 fd2name
-unix ttyname(fd) -> tty name

 */

enum {
	OPEN_MAX = 8192,
	SELF = ~0
};

typedef enum {
	FD_REGULAR,
	FD_DEV,
	FD_PIPE,
	FD_TCPSOCK,
	FD_UDPSOCK, 
	FD_UNIXSOCK,
	FD_UNKNOWN,
	FD_MAX_TYPES
} fdtype;

struct tcpsock {
	unsigned long inode;
	int state;
	struct sockaddr_in addr;
	struct sockaddr_in peer_addr;
	struct tcpsock *peer;
	int blackhole;
};

struct udpsock {
	unsigned long inode;
	int state;
	struct sockaddr_in addr;
	struct sockaddr_in peer_addr;
	struct udpsock *peer;
};

struct unixsock {
	unsigned long inode;
	unsigned long peer_inode;
	int flags;
	int type;
	int state;
	char path[PATH_MAX]; /* FIXME: Find a constant */
};

struct pipe {
	unsigned long inode;
};

struct fil {
	char path[PATH_MAX]; /* FIXME: Find a constant */
	int flags;
	off_t offset;
};

struct fd {
	int fd;
	fdtype type;
	int flags;
	int owner;
	int sig;
	union {
		struct tcpsock *tcp;   /* FD_TCPSOCK */
		struct udpsock *udp;   /* FD_UDPSOCK */
		struct unixsock *un;   /* FD_UNIXSOCK */
		struct pipe *pip;      /* FD_PIPE */
		struct fil *fil;       /* FD_REGULAR */
	} f;
};

static struct fd *fds[OPEN_MAX];
static int numfds;

static void *
xmalloc(size_t size)
{
	void *p;
	p = malloc(size);
	if (!p) {
		fprintf(stderr, "Out of memory\n");
		assert(0);
	}
	bzero(p, size);
	return p;
}

static struct tcpsock *
new_tcpsock()
{
	struct tcpsock *tcp;
	tcp = xmalloc(sizeof(*tcp));
	tcp->inode = 0;
	tcp->state = 0;
	bzero(&tcp->addr, sizeof(tcp->addr));
	bzero(&tcp->peer_addr, sizeof(tcp->peer_addr));
	tcp->peer = NULL;
	tcp->blackhole = 0;
	return tcp;
}

static void
free_tcpsock(struct tcpsock *tcp)
{
	free(tcp);
}

static struct udpsock *
new_udpsock()
{
	struct udpsock *udp;
	udp = xmalloc(sizeof(*udp));
	udp->inode = 0;
	udp->state = 0;
	bzero(&udp->addr, sizeof(udp->addr));
	bzero(&udp->peer_addr, sizeof(udp->peer_addr));
	udp->peer = NULL;
	return udp;
}

static void
free_udpsock(struct udpsock *udp)
{
	free(udp);
}

static struct unixsock *
new_unixsock()
{
	struct unixsock *un;
	un = xmalloc(sizeof(*un));
	un->inode = 0;
	un->state = 0;
	strncpy(un->path, "???", sizeof(un->path));
	return un;
}

static void
free_unixsock(struct unixsock *un)
{
	free(un);
}

static struct pipe *
new_pipe()
{
	struct pipe *p;
	p = xmalloc(sizeof(*p));
	p->inode = 0;
	return p;
}

static void
free_pipe(struct pipe *p)
{
	free(p);
}

struct fil *
new_fil()
{
	struct fil *fil;
	fil = xmalloc(sizeof(*fil));
	strncpy(fil->path, "???", sizeof(fil->path));
	return fil;
}

static void
free_fil(struct fil *f)
{
	free(f);
}

static struct fd *
new_fd()
{
	struct fd *fd;
	fd = xmalloc(sizeof(*fd));
	fd->fd = -1;
	fd->type = FD_UNKNOWN;
	fd->f.tcp = NULL;
	return fd;
}

static void
free_fd(struct fd *fd)
{
	switch (fd->type) {
	case FD_REGULAR:
		free_fil(fd->f.fil);
		break;
	case FD_DEV:
		break;
	case FD_PIPE:
		free_pipe(fd->f.pip);
		break;
	case FD_TCPSOCK:
		free_tcpsock(fd->f.tcp);
		break;
	case FD_UDPSOCK:
		free_udpsock(fd->f.udp);
		break;
	case FD_UNIXSOCK:
		free_unixsock(fd->f.un);
		break;
	case FD_UNKNOWN:
		break;
	default:
		assert(0);
	}
	free(fd);
}

static int
inode_to_tcp(unsigned long inode, struct tcpsock *tcp)
{
	FILE *fp;
	char buf[1024];
	int state;
	unsigned lp, pp;
	unsigned long lh, ph;
	unsigned long i;
	int n = 0;
	int ret = 0;
	fp = fopen("/proc/net/tcp", "r");
	if (!fp) {
		perror("open /proc/net/tcp");
		assert(0);
	}
	while (fgets(buf, sizeof(buf), fp)) {
		if (!n++)
			continue; /* skip first line */
		sscanf(buf, "%*s %lx:%x %lx:%x %x %*s %*s %*s %*s %*s %lu",
		       &lh, &lp, &ph, &pp, &state, &i);
		/* the kernel prints ports in host order;
		   see /usr/src/linux/net/ipv4/tcp_ipv4.c:^get_tcp_sock */
		lp = htons(lp);
		pp = htons(pp);
		if (i == inode) {
			tcp->addr.sin_addr.s_addr = lh;
			tcp->addr.sin_port = lp;
			tcp->peer_addr.sin_addr.s_addr = ph;
			tcp->peer_addr.sin_port = pp;
			tcp->state = state;
			ret = 1;
			break;
		}
	}
	fclose(fp);
	return ret;
}

static int
inode_to_udp(unsigned long inode, struct udpsock *udp)
{
	FILE *fp;
	char buf[1024];
	int n = 0;
	int ret = 0;
	int state;
	unsigned lp, pp;
	unsigned long lh, ph, i;

	fp = fopen("/proc/net/udp", "r");
	if (!fp) {
		perror("open /proc/net/udp");
		assert(0);
	}
	while (fgets(buf, sizeof(buf), fp)) {
		if (!n++)
			continue; /* skip first line */
		sscanf(buf, "%*s %lx:%x %lx:%x %x %*s %*s %*s %*s %*s %lu",
		       &lh, &lp, &ph, &pp, &state, &i);
		if (i == inode) {
			udp->addr.sin_addr.s_addr = lh;
			udp->addr.sin_port = lp;
			udp->peer_addr.sin_addr.s_addr = ph;
			udp->peer_addr.sin_port = pp;
			udp->state = state;
			ret = 1;
			break;
		}
	}
	fclose(fp);
	return ret;
}

static int
inode_to_unix(unsigned long inode, struct unixsock *un)
{
	FILE *fp;
	char buf[1024];
	char pth[1024];
	int n = 0;
	int ret = 0;
	int flags, type, state;
	int rv;
	unsigned long i;
	fp = fopen("/proc/net/unix", "r");
	if (!fp) {
		perror("open /proc/net/unix");
		assert(0);
	}
	un->path[0] = '\0';
	un->peer_inode = -1UL;
	while (fgets(buf, sizeof(buf), fp)) {
		if (!n++)
			continue; /* skip first line */
		rv = sscanf(buf, "%*s %*s %*s %x %x %x %lu %s",
			    &flags, &type, &state, &i, pth);
		if (i == inode) {
			un->flags = flags;
			un->type = type;
			un->state = state;
			if (rv == 5)
				strcpy(un->path, pth);
			ret = 1;
		}
		if (i == inode-1 || i == inode+1) {
			/* FIXME: guesswork */
			un->peer_inode = i;
		}
			
	}
	fclose(fp);
	return ret;
}

static struct tcpsock *
tcpsock(unsigned long inode)
{
	struct tcpsock *sk;

	/* make a new one */
	sk = new_tcpsock();
	sk->inode = inode;
	if (!inode_to_tcp(inode, sk)) {
		free(sk);
		return NULL;
	}
	return sk;
}

static int
restore_tcpsock(struct tcpsock *sk)
{
	fprintf(stderr, "no tcp socks!\n");
	assert(0);
	return 0;
}

static struct udpsock *
udpsock(unsigned long inode)
{
	struct udpsock *sk;

	sk = new_udpsock();
	sk->inode = inode;
	if (!inode_to_udp(inode, sk)) {
		free(sk);
		return NULL;
	}
	return sk;
}

static int
restore_udpsock(struct udpsock *sk)
{
	fprintf(stderr, "no udp sockets!\n");
	assert(0);
	return 0;
}

static struct unixsock *
unixsock(unsigned long inode)
{
	struct unixsock *sk;

	sk = new_unixsock();
	sk->inode = inode;
	if (!inode_to_unix(inode, sk)) {
		free(sk);
		return NULL;
	}
	return sk;
}

static int
restore_unixsock(struct unixsock *sk)
{
	int sd;
	struct sockaddr_un addr;

	if (!sk->path[0]) {
		fprintf(stderr, "cannot restore unixsock with no path\n");
		return -1;
	}
	assert(sk->type == SOCK_DGRAM || sk->type == SOCK_STREAM);
	sd = socket(AF_UNIX, SOCK_DGRAM, 0);
	if (0 > sd) {
		perror("socket");
		assert(0);
	}
	bzero(&addr, sizeof(addr));
	addr.sun_family = AF_LOCAL;
	unlink(sk->path);
	strcpy(addr.sun_path, sk->path);
	if (0 > bind(sd, (struct sockaddr *)&addr, SUN_LEN(&addr))) {
		perror("bind");
		assert(0);
	}
	return sd;
}

static void
type_socket(struct fd *fd, unsigned long inode)
{
	struct tcpsock *tcp;
	struct udpsock *udp;
	struct unixsock *un;

	if ((tcp = tcpsock(inode))) {
		fd->type = FD_TCPSOCK;
		fd->f.tcp = tcp;
	} else if ((udp = udpsock(inode))) {
		fd->type = FD_UDPSOCK;
		fd->f.udp = udp;
	} else if ((un = unixsock(inode))) {
		fd->type = FD_UNIXSOCK;
		fd->f.un = un;
	} else
		fd->type = FD_UNKNOWN;
}

static void
type_pipe(struct fd *fd, unsigned long inode)
{
	struct pipe *pip;

	fd->type = FD_PIPE;
	pip = new_pipe();
	pip->inode = inode;
	fd->f.pip = pip;
}

static int
restore_pipe(struct pipe *pip)
{
	fprintf(stderr, "no pipes!\n");
	assert(0);
	return 0;
}

static void
type_dev(struct fd *fd)
{
	fd->type = FD_DEV;
}

static void
type_filename(struct fd *fd, char *filename)
{
	struct fil *fil;
	char buf[1024];
	struct stat st;
	static int st_to_flags[] = {
		0, 0,
		O_WRONLY, O_WRONLY,
		O_RDONLY, O_RDONLY,
		O_RDWR, O_RDWR
	};

	fd->type = FD_REGULAR;
	fil = new_fil();
	strncpy(fil->path, filename, sizeof(fil->path));
	sprintf(buf, "/proc/%d/fd/%d", getpid(), fd->fd);
	if (0 > stat(buf, &st)) {
		perror("stat");
		assert(0);
	}
	fil->flags = st_to_flags[(st.st_mode & 0700) >> 6];
	fil->offset = lseek(fd->fd, 0, SEEK_CUR);
	if (fil->offset == (off_t)-1) {
		perror("lseek");
		assert(0);
	}
	fd->f.fil = fil;
}

static int
restore_file(struct fil *fil)
{
	int fd;

	if (!strncmp(fil->path, "/dev", 4)) {
		fprintf(stderr, "skipping devices\n");
		return -1;
	}

	fd = open(fil->path, fil->flags);
	if (0 > fd) {
		perror("open");
		assert(0);
	}
	if ((off_t)-1 == lseek(fd, fil->offset, SEEK_SET)) {
		perror("lseek");
		assert(0);
	}
	return fd;
}

static void
type_file(struct fd *fd, char *name)
{
	unsigned long inode;
	if (1 == sscanf(name, "socket:[%lu]", &inode)) {
		type_socket(fd, inode);
		return;
	}
	if (1 == sscanf(name, "pipe:[%lu]", &inode)) {
		type_pipe(fd, inode);
		return;
	}
	if (!strncmp(name, "/dev", 4)) {
		type_dev(fd);
		return;
	}
	type_filename(fd, name);
}

void
getfds(void *ignored)
{
	DIR *dir;
	struct dirent *e;
	struct fd *fd;
	char s[1024];
	char buf[1024];
	int rv;

	snprintf(s, sizeof(s), "/proc/%d/fd", getpid());
	dir = opendir(s);
	if (!dir) {
		fprintf(stderr, "cannot open %s\n", s);
		return;
	}
	while ((e = readdir(dir))) {
		if (atoi(e->d_name) == dirfd(dir))
			continue;
		snprintf(s, sizeof(s), "/proc/%d/fd/%s", getpid(), e->d_name);
		rv = readlink(s, buf, sizeof(buf));
		if (0 > rv)
			continue;
		if (rv >= sizeof(buf)) {
			fprintf(stderr, "buffer overflow\n");
			assert(0);
		}
		buf[rv] = '\0';

		if (numfds >= sizeof(fds)/sizeof(fds[0])) {
			fprintf(stderr, "buffer overflow\n");
			assert(0);
		}
		fd = new_fd();
		fd->fd = atoi(e->d_name);
		errno = 0;
		fd->flags = fcntl(fd->fd, F_GETFL);
		if (errno) {
			perror("fcntl(F_GETFL)");
			assert(0);
		}
		fd->owner = fcntl(fd->fd, F_GETOWN);
		if (errno) {
			perror("fcntl(F_GETOWN)");
			assert(0);
		}
		if (fd->owner == getpid())
			fd->owner = SELF;
		fd->sig = fcntl(fd->fd, F_GETSIG);
		if (errno) {
			perror("fcntl(F_GETSIG)");
			assert(0);
		}
		type_file(fd, buf);
		fds[numfds++] = fd;
	}
	closedir(dir);
}

void
reset_fds()
{
	int i;
	for (i = 0; i < numfds; i++)
		free_fd(fds[i]);
}

/* If FD a dup of some other descriptor < FD? */
int
detect_dup(int fd)
{
	/* The first test is the same dev and inode number. */
	/* For regular files, seek one descriptor and see 
	   if other changes. */
	/* For sockets, change a sockopt. */
	assert(0);
	return 0;
}

static void
restore_fd(struct fd *fd)
{
	int rv, new_fd;

	close(fd->fd);
	switch (fd->type) {
	case FD_REGULAR:
		new_fd = restore_file(fd->f.fil);
		break;
	case FD_DEV:
		new_fd = -1;
		break;
	case FD_PIPE:
		new_fd = restore_pipe(fd->f.pip);
		break;
	case FD_TCPSOCK:
		new_fd = restore_tcpsock(fd->f.tcp);
		break;
	case FD_UDPSOCK:
		new_fd = restore_udpsock(fd->f.udp);
		break;
	case FD_UNIXSOCK:
		new_fd = restore_unixsock(fd->f.un);
		break;
	case FD_UNKNOWN:
		new_fd = -1;
		break;
	default:
		assert(0);
	}

	if (new_fd == -1) {
#if 0
		fprintf(stderr, "warning -1 from restore fd (%d)\n", fd->fd);
#endif
		return;
	}
	if (new_fd != fd->fd) {
		rv = dup2(new_fd, fd->fd);
		if (0 > rv) {
			perror("dup2");
			assert(0);
		}
		close(new_fd);
	}
	rv = fcntl(fd->fd, F_SETFL, fd->flags);
	if (0 > rv) {
		perror("fcntl(F_SETFL)");
		assert(0);
	}
	rv = fcntl(fd->fd, F_SETOWN,
		   fd->owner == SELF ? getpid() : fd->owner);
#if 0
	if (0 > rv) {
		perror("fcntl(F_SETOWNER)");
		assert(0);
	}
#endif
	rv = fcntl(fd->fd, F_SETSIG, fd->sig);
#if 0
	if (0 > rv) {
		perror("fcntl(F_SETSIG)");
		assert(0);
	}
#endif
}

void
restore_fds(void *ignored)
{
	int i;
	for (i = 0; i < numfds; i++)
		restore_fd(fds[i]);
}
