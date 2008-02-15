#include "sys.h"

char *
ipstr(struct sockaddr_in *sa)
{
	static char buf[128];
	struct servent *servent;

	static char addr[32];
	static char serv[32];

	strcpy(addr, inet_ntoa(sa->sin_addr));
	servent = getservbyport(sa->sin_port, "tcp");
	if (servent) {
		strcpy(serv, servent->s_name);
		sprintf(buf, "%s:%s", addr, serv);
	}
	else
		sprintf(buf, "%s:%d", addr, ntohs(sa->sin_port));
	return buf;
}

static void
nomem()
{
	fprintf(stderr, "Out of memory\n");
	exit(1);
}

static int
parse_addr(const char *s, struct in_addr *addr)
{
	struct hostent* h;
	h = gethostbyname(s);
	if (!h)
		return -1;
	*addr = *((struct in_addr *) h->h_addr); /* network order */
	return 0;
}

static int
parse_port(const char *s, short *port)
{
	char *p;
	struct servent *se;
	unsigned long l;

	se = getservbyname(s, "tcp");
	if (se) {
		*port = se->s_port;
		return 0;
	}
	l = strtoul(s, &p, 10);
	if (*p != '\0')
		return -1;
	*port = (short) htons(l);
	return 0;
}

int
parse_ip(const char *s, struct sockaddr_in *addr)
{
	char *buf = NULL;
	char *p;
	int ret = -1;

	buf = strdup(s);
	if (!buf)
		nomem();
	addr->sin_family = AF_INET;
	addr->sin_addr.s_addr = INADDR_ANY;
	addr->sin_port = htons(0);
	if ((p = strchr(buf, ':'))) {
		/* HOST:PORT */
		*p++ = '\0';
		if (0 > parse_addr(buf, &addr->sin_addr))
			goto out;
		if (0 > parse_port(p, &addr->sin_port))
			goto out;
	} else if ((p = strchr(buf, '.'))) {
		/* HOST */
		if (0 > parse_addr(buf, &addr->sin_addr))
			goto out;
	} else {
		/* PORT or HOST? */
		if (0 > parse_port(buf, &addr->sin_port)
		    && 0 > parse_addr(buf, &addr->sin_addr))
			goto out;
	}
	ret = 0;
out:
	free(buf);
	return ret;
}
