/*
stream logic:

open file/service stream
open elf stream
write ckpt
close elf stream
close file/service stream
*/

#include "sys.h"
#include "ckpt.h"
#include "ckptimpl.h"


enum {
	MAX_CALLBACKS = 1000
};
static fn_t on_preckpt[MAX_CALLBACKS];
static void *on_preckpt_arg[MAX_CALLBACKS];
static unsigned num_on_preckpt;
static fn_t on_postckpt[MAX_CALLBACKS];
static void *on_postckpt_arg[MAX_CALLBACKS];
static unsigned num_on_postckpt;
static fn_t on_restart[MAX_CALLBACKS];
static void *on_restart_arg[MAX_CALLBACKS];
static unsigned num_on_restart;

void
ckpt_on_preckpt(fn_t f, void *arg)
{
	if (num_on_preckpt >= MAX_CALLBACKS) {
		fprintf(stderr, "Warning: too many pre-ckpt callbacks\n");
		return;
	}
	on_preckpt[num_on_preckpt] = f;
	on_preckpt_arg[num_on_preckpt++] = arg;
}

void
ckpt_on_postckpt(fn_t f, void *arg)
{
	if (num_on_postckpt >= MAX_CALLBACKS) {
		fprintf(stderr, "Warning: too many post-ckpt callbacks\n");
		return;
	}
	on_postckpt[num_on_postckpt] = f;
	on_postckpt_arg[num_on_postckpt++] = arg;
}

void
ckpt_on_restart(fn_t f, void *arg)
{
	if (num_on_restart >= MAX_CALLBACKS) {
		fprintf(stderr, "Warning: too many restart callbacks\n");
		return;
	}
	on_restart[num_on_restart] = f;
	on_restart_arg[num_on_restart++] = arg;
}

static int
get_ckpt_stream(char *name, unsigned long ckptsize)
{
	return ckpt_open_elfstream(name, ckptsize);
}

static void
close_ckpt_stream(int fd)
{
	int rv;
	rv = ckpt_close_elfstream(fd);
	if(0 > rv)
		fatal("cannot write checkpoint file\n");
}

static unsigned long
ckpt_size(const struct ckpt_header *head,
	  const memregion_t *regions)
{
	unsigned long sz = 0;
	int i;

	sz += sizeof(*head);
	sz += head->num_regions*sizeof(*regions);
	for(i = 0; i < head->num_regions; i++)
		sz += regions[i].len;
	return sz;
}

static int
ckpt_save(char *name,
	  const struct ckpt_header *head,
	  const memregion_t *regions)
{
	int fd;
	int i;
	unsigned long total = 0;

	fd = get_ckpt_stream(name, ckpt_size(head, regions));
	if (0 > fd) {
		fprintf(stderr, "cannot obtain a checkpoint stream\n");
		return -1;
	}

	if (0 > xwrite(fd, head, sizeof(*head))) {
		fprintf(stderr, "head write failed\n");
		goto err;
	}
	total += sizeof(struct ckpt_header);
	if (0 > xwrite(fd, regions, head->num_regions*sizeof(*regions))) {
		fprintf(stderr, "head count failed\n");
		goto err;
	}
	total += head->num_regions*sizeof(*regions);
	for (i = 0; i < head->num_regions; i++) {
		if (0 > xwrite(fd, (void*)regions[i].addr, regions[i].len)) {
			fprintf(stderr, "region %d failed (%s)\n", i,
				strerror(errno));
			goto err;
		}
		total += regions[i].len;
	}
	close_ckpt_stream(fd);
	return 0;
err:
	fprintf(stderr, "Checkpoint write error\n");
	return -1;
}

static int
getcmd(char *cmd, int max)
{
	int fd;
	int rv;

	fd = open("/proc/self/cmdline", O_RDONLY);
	if (0 > fd)
		return -1;
	rv = read(fd, cmd, max);
	close(fd);
	if (0 >= rv)
		return -1;
	if (rv >= max)
		cmd[max-1] = '\0';
	else
		cmd[rv] = '\0';
	return 0;
}

void
tls_save(struct ckpt_header *h)
{
#ifndef PRENPTL
	int gs;
	h->tls.entry_number = 6; /* magic: see linux/include/asm-i386/segment.h */
	if (0 > syscall(SYS_get_thread_area, &h->tls)) {
		fprintf(stderr, "cannot get tls segment\n");
		exit(1);
	}
	asm("movw %%gs,%w0" : "=q" (gs));
	h->gs = gs&0xffff;
#endif
}

int
ckpt_ckpt(char *name)
{
	struct ckpt_header head;
	memregion_t regions[MAXREGIONS];
	int i;

/*	getfds(NULL); */

	for (i = 0; i < num_on_preckpt; i++)
		on_preckpt[i](on_preckpt_arg[i]);

	if (0 > getcmd(head.cmd, sizeof(head.cmd))) {
		fprintf(stderr, "cannot read my command\n");
		return -1;
	}

	if (0 > read_self_regions(regions, &head.num_regions)) {
		fprintf(stderr, "cannot read my memory map\n");
		return -1;
	}

	if (0 == setjmp(head.jbuf)) {
		/* Checkpoint */
		if (0 > ckpt_signals()) {
			fprintf(stderr, "cannot save the signal state\n");
			return -1;
		}
		head.brk = (unsigned long) sbrk(0);
		tls_save(&head);

		if (0 > ckpt_save(name, &head, regions)) {
			fprintf(stderr, "cannot save the ckpt image\n");
			return -1;
		}

		if (!ckpt_shouldcontinue())
			_exit(0); /* do not call atexit functions */
		for (i = 0; i < num_on_postckpt; i++)
			on_postckpt[i](on_postckpt_arg[i]);
		return 0;
	}		

	/* Restart */
	if (0 > unmap_ifnot_orig(regions, head.num_regions)) {
		fprintf(stderr,
			"cannot purge restart code from address space\n");
		return -1;
	}
	if (0 > set_orig_mprotect(regions, head.num_regions)) {
		fprintf(stderr, "cannot restore address space protection\n");
	}
	if (0 > restore_signals()) {
		fprintf(stderr, "cannot restore signal disposition\n");
		return -1;
	}

/*	restore_fds(NULL); */
	for (i = num_on_restart-1; i >= 0; i--)
		on_restart[i](on_restart_arg[i]);
	return 0;
}

static void
ckpt_banner()
{
	return;
	fprintf(stderr, "libckpt of %s %s\n", __DATE__, __TIME__);
}

static void
unlinklib()
{
	char *p;
	p = getenv("LD_PRELOAD");
	if(p == NULL)
		return;
	if(strstr(p, "tmplibckpt") == NULL)
		return;
	unlink(p);
}

void
ckpt_init()
{
	ckpt_banner();
	unlinklib();
	ckpt_initconfig();
}
