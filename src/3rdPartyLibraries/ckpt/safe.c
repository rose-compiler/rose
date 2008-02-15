#include "sys.h"
#include "ckptimpl.h"

static struct ckpt_restore safe_cr;

static void
die()
{
	*((int *)0) = 0;
}

void
tls_restore(struct ckpt_header *h)
{
#ifndef PRENPTL
	if (0 > syscall(SYS_set_thread_area, &h->tls))
		die();
	asm("movw %w0,%%gs" :: "q" (h->gs));
#endif
}

int
xread(int sd, void *buf, size_t len)
{
	char *p = (char *)buf;
	size_t nrecv = 0;
	ssize_t rv;
	while (nrecv < len) {
		rv = read(sd, p, len - nrecv);
		if (0 > rv)
			return -1;
		if (0 == rv)
			return 0;
		nrecv += rv;
		p += rv;
	}
	return nrecv;
}

void
continuesafe(struct ckpt_restore *cr)
{
	int i;
	char *p, *q;

	safe_cr = *cr;
	cr = 0; /* This pointer is in unsafe memory */
	for (i = 0; i < safe_cr.head.num_regions; i++) {
		if (safe_cr.orig_regions[i].len
		    != xread(safe_cr.fd,
			    (void *)safe_cr.orig_regions[i].addr,
			    safe_cr.orig_regions[i].len))
			return;
	}
	close(safe_cr.fd);
	/* copy the command */
	p = (char *)safe_cr.argv0;
	q = safe_cr.head.cmd;
	while (*q)
		*p++ = *q++;
	*p = '\0';

	/* restoring tls here restores ability to call
	   the libc tls-based system call trap (through
	   GS register) ... which the code after longjmp
	   expects to use. */
	tls_restore(&safe_cr.head);
	longjmp(safe_cr.head.jbuf, 1);
	die();
}
