#include "sys.h"
#include "ckptimpl.h"

static struct ckpt_restore globalrestbuf;
extern char librestart[];
extern long librestartlen;

static int
open_ckpt_file(struct ckpt_restore *cr)
{
	int ret;
	char exe[1024];

//	if (!OPTIONS.ckpt_server) {
//		cr->fd = open(OPTIONS.ckpt_filename, O_RDONLY);
//		if (0 > cr->fd) {
//			fprintf(stderr,
//				"cannot open checkpoint %s for reading\n",
//				OPTIONS.ckpt_filename);
//			ret = -1;
//			goto out;
//		}
//	}
//	else {
//		cr->fd = request_ckpt_restore(OPTIONS.ckpt_server,
//					      OPTIONS.ckpt_id);
//		if (0 > cr->fd) {
//			fprintf(stderr,
//				"cannot open checkpoint %s for reading\n",
//				OPTIONS.ckpt_id);
//			ret = -1;
//			goto out;
//		}
//	}
	readlink("/proc/self/exe", exe, sizeof(exe));
	cr->fd = ckpt_elf_openckpt(exe);
	if(0 > cr->fd){
		fprintf(stderr, "cannot open checkpoint %s\n", "/tmp/ckpt");
		ret = -1;
		goto out;
	}
	ret = xread(cr->fd, &cr->head, sizeof(cr->head));
	if (sizeof(cr->head) != ret) {
		fprintf(stderr, "cannot read checkpoint file (header)\n");
		ret = -1;
		goto out;
	}
	ret = xread(cr->fd, cr->orig_regions,
		    sizeof(memregion_t) * cr->head.num_regions);
	if (sizeof(memregion_t) * cr->head.num_regions != ret) {
		fprintf(stderr, "cannot read checkpoint file (memory map)\n");
		ret = -1;
		goto out;
	}
	ret = 0; /* All's well */
out:
	if (ret && cr->fd >= 0)
		/* Normally leave it open; we still need to read the pages */
		close(cr->fd);
	return ret;
}

static void
close_ckpt_file(struct ckpt_restore *cr)
{
	close(cr->fd);
}

static int
load_safe_restart_code(void **f, char *libname)
{
	void *hdl = NULL;
	void *fn;

	hdl = dlopen(libname, RTLD_NOW);
	if(!hdl){
		fprintf(stderr, "restart: cannot load restart helper code\n");
		return -1;
	}
	unlink(libname);
	fn = dlsym(hdl, "continuesafe");
	if (!fn) {
		fprintf(stderr,
			"restart: missing symbols in restart library: %s\n",
			dlerror());
		return -1;
	}
	*f = fn;
	return 0;
}

/* This function expects to be called on a fresh stack */
static void
continue_restart(void *arg)
{
	void *funcp;
	char *libname = arg;
	void (*continuesafe)(struct ckpt_restore *);

	if (0 > map_orig_regions(&globalrestbuf)) {
		fprintf(stderr, "Error blocking regions of ckpt\n");
		return;
	}
	if (0 > load_safe_restart_code(&funcp, libname)) {
		fprintf(stderr, "cannot load safe restart code\n");
		return;
	}
	/* Loading the library may grow the heap.  Undo that. */
	if (0 > brk((void*)globalrestbuf.head.brk)) {
		fprintf(stderr, "cannot restore brk\n");
		return;
	}
	continuesafe = (void(*)(struct ckpt_restore*))funcp;
	continuesafe(&globalrestbuf);  /* Does not return */
	fprintf(stderr, "Unexpected return from safe restart code\n");
}

/* Compare two memregion_t structures. */
static int
mcmp(const void *ap, const void *bp)
{
	memregion_t *a = (memregion_t *)ap;
	memregion_t *b = (memregion_t *)bp;
	unsigned long a_addr = a->addr;
	unsigned long b_addr = b->addr;
	unsigned long a_len = a->len;
	unsigned long b_len = b->len;

	if (a_addr < b_addr)
		return -1;
	if (a_addr > b_addr)
		return 1;
	if (a_len < b_len)
		return -1;
	if (a_len > b_len)
		return 1;
	return 0;
}

/* If regions intersect they will be merged into one.
   The state of the flags is undefined in that case. */
static void
munion(memregion_t *a, int lena,
       memregion_t *b, int lenb,
       memregion_t *c, int *lenc)
{
	int i, lent;
	memregion_t t[lena + lenb];
	memregion_t *tp, *end, *cp;

	if (lena + lenb == 0) {
		*lenc = 0;
		return;
	}

	/* Copy all regions into one buffer */
	lent = 0;
	for (i = 0; i < lena; i++)
		t[lent++] = a[i];
	for (i = 0; i < lenb; i++)
		t[lent++] = b[i];
	
	/* Sort by start address */
	qsort(t, lent, sizeof(memregion_t), mcmp);
	
	/* Fold overlapping regions */
	tp = t;
	cp = c;
	end = &t[lent];
	while (tp < end) {
		memregion_t *np = tp + 1;

		while (np < end
		       && tp->addr + tp->len >= np->addr) {
			if (np->addr + np->len > tp->addr + tp->len)
				tp->len = np->addr + np->len - tp->addr;
			++np;
		}
		*cp++ = *tp;
		tp = np;
	}
	*lenc = cp - &c[0];
}

static int
getargs(int *argc, char ***argv)
{
	int fd, rv;
	char buf[2048];
	unsigned long stackstart;

	fd = open("/proc/self/stat", O_RDONLY);
	if (0 > fd)
		return -1;
	rv = read(fd, buf, sizeof(buf));
	close(fd);
	if (0 >= rv)
		return -1;
	rv = sscanf(buf,
		    "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %*d %*d %*d %*d %*d %*d %*u %*u %*d %*u %*u %*u %*u %*u %lu",
		    &stackstart);
	if (rv != 1)
		return -1;
	*argc = *(int*)stackstart;
	*argv = (char**)(stackstart+4);
	return 0;
}

void
restart(char *safelib)
{
	int argc;
	char **argv;
	memregion_t selfregions[MAXREGIONS];
	int num_selfregions;
	memregion_t verboten[MAXREGIONS];
	int num_verboten;
	char tmpl[1024];
	int fd, rv;

	/* unpack restart helper code */
	snprintf(tmpl, sizeof(tmpl), "/tmp/librestartXXXXXX");
	fd = mkstemp(tmpl);
	if(0 > fd){
		fprintf(stderr, "cannot create temporary file\n");
		exit(1);
	}
	rv = xwrite(fd, librestart, librestartlen);
	if(0 > rv){
		fprintf(stderr, "cannot unpack restart helper code: %s\n",
			strerror(errno));
		exit(1);
	}
	close(fd);

	getargs(&argc, &argv);
	globalrestbuf.argv0 = (unsigned long)argv[0];
	globalrestbuf.fd = -1;
	if (0 > open_ckpt_file(&globalrestbuf)) {
		fprintf(stderr, "Error reading the checkpoint file\n");
		return;
	}
	if (0 > read_self_regions(selfregions, &num_selfregions)) {
		fprintf(stderr, "cannot read my memory map\n");
		return;
	}
	if (0 > set_writeable(selfregions, num_selfregions)) {
		fprintf(stderr, "cannot set my memory to writable\n");
		return;
	}

	munion(globalrestbuf.orig_regions, 
	       globalrestbuf.head.num_regions,
	       selfregions,
	       num_selfregions,
	       verboten,
	       &num_verboten);

	call_with_new_stack(5, verboten, num_verboten, continue_restart, tmpl);
	/* We should not be here */
	close_ckpt_file(&globalrestbuf);
}
