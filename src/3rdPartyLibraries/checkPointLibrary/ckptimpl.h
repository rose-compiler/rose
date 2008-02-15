#ifndef PRENPTL
/*
 * The name of this structure varies among Linux versions.
 * It was `modify_ldt_ldt_s' on some, then changed to
 * `user_desc'.  Always in /usr/include/asm/ldt.h.
 * We re-define it so that we don't have to figure out
 * the right name.
 */
struct linux_ldt{
	unsigned int  entry_number;
	unsigned long base_addr;
	unsigned int  limit;
	unsigned int  seg_32bit:1;
	unsigned int  contents:2;
	unsigned int  read_exec_only:1;
	unsigned int  limit_in_pages:1;
	unsigned int  seg_not_present:1;
	unsigned int  useable:1;
};
#endif

/* FIXME: Perhaps we should have a richer model of
   memory to cope with these hacks for pages with
   special semantics.*/
enum {
	DEBUG =		0,
	STACKHACK =	0xb0000000,	/* how we guess which pages are part of the stack */
	TRAPPAGEHACK =	0xffffe000,	/* we do not ckpt this pseudo page, which
					   is used for syscall trap on some kernels */
	MAXREGIONS =	512,		/* Maximum num of noncontiguous memory regions */
	REGION_HEAP =	0x8,		/* Memory region is for heap; not equal to any PROT_* */
};

struct ckpt_header {
	char cmd[1024];    /* command name for ps and /proc */
	int num_regions;
	jmp_buf jbuf;
	unsigned long brk;
#ifndef PRENPTL
	/* thread-local storage state */
	struct linux_ldt tls; /* tls segment descriptor */
	unsigned long gs;            /* gs register */
#endif
};

typedef
struct memregion{
	unsigned long addr;
	unsigned long len;
	unsigned flags;  /* bitmask of PROT_* and REGION_HEAP */
} memregion_t;

struct ckpt_restore{
	int fd;
	struct ckpt_header head;
	unsigned long argv0; /* address of argv[0] */
	memregion_t orig_regions[MAXREGIONS];
};

/* ckpt.c */
void ckpt_init();

/* mem.c */
void heap_extension(memregion_t *region);
int read_self_regions(memregion_t *regions, int *num_regions);
void print_regions(const memregion_t *regions, int num_regions, const char *msg);
int map_orig_regions(const struct ckpt_restore *restbuf);
int addr_in_regions(unsigned long addr, const memregion_t *regions, int num_regions);
int set_writeable(const memregion_t *regions, int num_regions);
int set_orig_mprotect(const memregion_t *orig, int num_orig);
int call_with_new_stack(unsigned long num_pages,
			const memregion_t *verboten,
			int num_verboten,
			void(*fn)(void *), void *arg);
int unmap_ifnot_orig(const memregion_t *orig, int num_orig);

/* signal.c */
int ckpt_signals();
int restore_signals();

/* util.c */
int xwrite(int sd, const void *buf, size_t len);
int xread(int sd, void *buf, size_t len);
void call_if_present(char *name, char *lib);
void fatal(char *fmt, ...);
void *xmalloc(size_t size);
char *xstrdup(char *s);

/* csclt.c */
int request_ckpt_save(char *serveraddr,  char *id);
int request_ckpt_restore(char *serveraddr, char *id);
int request_ckpt_remove(char *serveraddr, char *id);
int request_ckpt_access(char *serveraddr, char *id);

/* restart.c */
void restart();

/* config.c */
void ckpt_initconfig();
int ckpt_shouldcontinue();
char *ckpt_ckptname();

/* fd.c */
void getfds(void *);
void restore_fds(void *ignored);

/* elf.c */
int ckpt_open_elfstream(char *name, unsigned long ckptsize);
int ckpt_close_elfstream(int fd);
int ckpt_elf_openckpt(char *name);

/* signal.c */
void ckpt_async(int sig);
void ckpt_cancelasync(int sig);

/* refun.c */
typedef struct symtab *symtab_t;
struct symlist {
	Elf32_Sym *sym;       /* symbols */
	char *str;            /* symbol strings */
	unsigned num;         /* number of symbols */
};
struct symtab {
	struct symlist *st;    /* "static" symbols */
	struct symlist *dyn;   /* dynamic symbols */
};
/* process memory map */
enum {
	RF_MAXLEN = 1024
};
#define MEMORY_ONLY  "[memory]"
struct mm {
	char name[RF_MAXLEN];
	unsigned long start, end; /* where it is mapped */
	unsigned long base;       /* where it thinks it is mapped,
				     according to in-process dl data */
	struct symtab *st;
};

struct modulelist {
	int pid;
	int is_dynamic;
	char exe[RF_MAXLEN];
	int num_mm;
	struct mm *mm;
	int exe_mm; /* index into mm */
};

struct modulelist *rf_parse(int pid);
void rf_free_modulelist(struct modulelist *ml);
unsigned long rf_find_libc_function(struct modulelist *ml, char *name);
int rf_replace_libc_function(struct modulelist *ml, char *name, void *to);
unsigned long rf_find_function(struct modulelist *ml, char *name);
unsigned char *rf_find_address(struct modulelist *ml, unsigned long addr);
