/* client for checkpoint server */
#include "cs.h"

static int
do_request(char *serveraddr, struct req *req)
{
	int fd;
	struct sockaddr_in addr;
	struct rep rep;

	if (0 > parse_ip(serveraddr, &addr)) {
		fprintf(stderr, "cannot resolve %s\n", serveraddr);
		return -1;
	}
	if (addr.sin_port == 0)
		addr.sin_port = htons(DEFAULTPORT);

	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (0 > fd) {
		perror("socket");
		return -1;
	}
	if (0 > connect(fd, (struct sockaddr *)&addr, sizeof(addr))) {
		perror("connect");
		return -1;
	}
	if (0 > xwrite(fd, req, sizeof(*req))) {
		perror("write");
		return -1;
	}
	if (0 > xread(fd, &rep, sizeof(rep))) {
		perror("read");
		return -1;
	}
	if (rep.status != REPLY_OK) {
		close(fd);
		return -1;
	}
	return fd;
}

int
request_ckpt_save(char *serveraddr, char *id)
{
	struct req req;
	req.mode = MODE_SAVE;
	strncpy(req.id, id, sizeof(req.id));
	return do_request(serveraddr, &req);
}

int
request_ckpt_restore(char *serveraddr, char *id)
{
	struct req req;
	req.mode = MODE_RESTORE;
	strncpy(req.id, id, sizeof(req.id));
	return do_request(serveraddr, &req);
}

int
request_ckpt_remove(char *serveraddr, char *id)
{
	int fd;
	struct req req;
	req.mode = MODE_REMOVE;
	strncpy(req.id, id, sizeof(req.id));
	fd = do_request(serveraddr, &req);
	if (fd >= 0);
		close(fd);
	return fd;
}

int
request_ckpt_access(char *serveraddr, char *id)
{
	int fd;
	struct req req;
	req.mode = MODE_ACCESS;
	strncpy(req.id, id, sizeof(req.id));
	fd = do_request(serveraddr, &req);
	if (fd >= 0)
		close(fd);
	return fd;
}
