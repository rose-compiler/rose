#include "sys.h"
#include "ckpt.h"
#include "ckptimpl.h"

/* FIXME: Why is this missing from unistd.h? */
extern ssize_t pread(int fd, void *buf, size_t count, off_t offset);

static void
free_syms(struct symlist *sl)
{
	if (!sl)
		return;
	free(sl->sym);
	free(sl->str);
	free(sl);
}

static void
free_symtab(symtab_t symtab)
{
	if (symtab->dyn)
		free_syms(symtab->dyn);
	if (symtab->st)
		free_syms(symtab->st);
	free(symtab);
}

static struct symlist *
get_syms(int fd, Elf32_Shdr *symh, Elf32_Shdr *strh)
{
	struct symlist *sl, *ret;
	int rv;

	ret = NULL;
	sl = (struct symlist *) xmalloc(sizeof(struct symlist));
	sl->str = NULL;
	sl->sym = NULL;

	/* sanity */
	if (symh->sh_size % sizeof(Elf32_Sym)) { 
		fprintf(stderr, "elf_error\n");
		goto out;
	}

	/* symbol table */
	sl->num = symh->sh_size / sizeof(Elf32_Sym);
	sl->sym = (Elf32_Sym *) xmalloc(symh->sh_size);
	rv = pread(fd, sl->sym, symh->sh_size, symh->sh_offset);
	if (0 > rv) {
		perror("read");
		goto out;
	}
	if (rv != symh->sh_size) {
		fprintf(stderr, "elf error\n");
		goto out;
	}

	/* string table */
	sl->str = (char *) xmalloc(strh->sh_size);
	rv = pread(fd, sl->str, strh->sh_size, strh->sh_offset);
	if (0 > rv) {
		perror("read");
		goto out;
	}
	if (rv != strh->sh_size) {
		fprintf(stderr, "elf error");
		goto out;
	}

	ret = sl;
out:
	return ret;
}

static int
do_load(int fd, symtab_t symtab)
{
	int rv;
	size_t size;
	Elf32_Ehdr ehdr;
	Elf32_Shdr *shdr = NULL, *p;
	Elf32_Shdr *dynsymh, *dynstrh;
	Elf32_Shdr *symh, *strh;
	char *shstrtab = NULL;
	int i;
	int ret = -1;
	
	/* elf header */
	rv = read(fd, &ehdr, sizeof(ehdr));
	if (0 > rv) {
		perror("read");
		goto out;
	}
	if (rv != sizeof(ehdr)) {
		fprintf(stderr, "elf error\n");
		goto out;
	}
	if (strncmp(ELFMAG, ehdr.e_ident, SELFMAG)) { /* sanity */
		fprintf(stderr, "not an elf (magic)\n");
		goto out;
	}
	if (sizeof(Elf32_Shdr) != ehdr.e_shentsize) { /* sanity */
		fprintf(stderr, "elf error\n");
		goto out;
	}

	/* section header table */
	size = ehdr.e_shentsize * ehdr.e_shnum;
	shdr = (Elf32_Shdr *) xmalloc(size);
	rv = pread(fd, shdr, size, ehdr.e_shoff);
	if (0 > rv) {
		perror("read");
		goto out;
	}
	if (rv != size) {
		fprintf(stderr, "elf error");
		goto out;
	}
	
	/* section header string table */
	size = shdr[ehdr.e_shstrndx].sh_size;
	shstrtab = (char *) xmalloc(size);
	rv = pread(fd, shstrtab, size, shdr[ehdr.e_shstrndx].sh_offset);
	if (0 > rv) {
		perror("read");
		goto out;
	}
	if (rv != size) {
		fprintf(stderr, "elf error\n");
		goto out;
	}

	/* symbol table headers */
	symh = dynsymh = NULL;
	strh = dynstrh = NULL;
	for (i = 0, p = shdr; i < ehdr.e_shnum; i++, p++)
		if (SHT_SYMTAB == p->sh_type) {
			if (symh) {
				fprintf(stderr, "too many symbol tables\n");
				goto out;
			}
			symh = p;
		} else if (SHT_DYNSYM == p->sh_type) {
			if (dynsymh) {
				fprintf(stderr, "too many symbol tables\n");
				goto out;
			}
			dynsymh = p;
		} else if (SHT_STRTAB == p->sh_type
			   && !strncmp(shstrtab+p->sh_name, ".strtab", 7)) {
			if (strh) {
				fprintf(stderr, "too many string tables\n");
				goto out;
			}
			strh = p;
		} else if (SHT_STRTAB == p->sh_type
			   && !strncmp(shstrtab+p->sh_name, ".dynstr", 7)) {
			if (dynstrh) {
				fprintf(stderr, "too many string tables\n");
				goto out;
			}
			dynstrh = p;
		}
	/* sanity checks */
	if ((!dynsymh && dynstrh) || (dynsymh && !dynstrh)) {
		fprintf(stderr, "bad dynamic symbol table");
		goto out;
	}
	if ((!symh && strh) || (symh && !strh)) {
		fprintf(stderr, "bad symbol table");
		goto out;
	}
	if (!dynsymh && !symh) {
		fprintf(stderr, "no symbol table");
		goto out;
	}

	/* symbol tables */
	if (dynsymh)
		symtab->dyn = get_syms(fd, dynsymh, dynstrh);
	if (symh)
		symtab->st = get_syms(fd, symh, strh);
	ret = 0;
out:
	free(shstrtab);
	free(shdr);
	return ret;
}

symtab_t
load_symtab(char *filename)
{
	int fd;
	symtab_t symtab;

	symtab = (symtab_t) xmalloc(sizeof(*symtab));
	bzero(symtab, sizeof(*symtab));

	fd = open(filename, O_RDONLY);
	if (0 > fd) {
		perror("open");
		return NULL;
	}
	if (0 > do_load(fd, symtab)) {
		fprintf(stderr, "Error ELF parsing %s\n", filename);
		free(symtab);
		symtab = NULL;
	}
	close(fd);
	return symtab;
}

static int
lookup_sym2(struct symlist *sl, unsigned char type,
	char *name, unsigned long *val)
{
	Elf32_Sym *p;
	int i;

	for (i = 0, p = sl->sym; i < sl->num; i++, p++)
		if (!strcmp(sl->str+p->st_name, name)
		    && p->st_shndx != 0 /* UNDEFINED */
		    && (type == STT_NOTYPE
			|| ELF32_ST_TYPE(p->st_info) == type)) {
			*val = p->st_value;
			return 0;
		}
	return -1;
}

static int
lookup_sym(symtab_t s, unsigned char type,
	   char *name, unsigned long *val)
{
	if (s->dyn && !lookup_sym2(s->dyn, type, name, val))
		return 0;
	if (s->st && !lookup_sym2(s->st, type, name, val))
		return 0;
	return -1;
}

static int
lookup_func_sym(symtab_t s, char *name, unsigned long *val)
{
	return lookup_sym(s, STT_FUNC, name, val);
}

int
lookup_sym_notype(symtab_t s, char *name, unsigned long *val)
{
	return lookup_sym(s, STT_NOTYPE, name, val);
}

static int
lookup_addr2(struct symlist *sl, unsigned char type,
	unsigned long addr, char **name)
{
	Elf32_Sym *p;
	int i;

	for (i = 0, p = sl->sym; i < sl->num; i++, p++)
		if (addr >= p->st_value && addr < p->st_value+p->st_size
		    && p->st_shndx != 0 /* UNDEFINED */
		    && ELF32_ST_TYPE(p->st_info) == type) {
			*name = sl->str+p->st_name;
			return 0;
		}
	return -1;
}

static int
lookup_addr(symtab_t s, unsigned long addr, char **name)
{
	if (s->dyn && !lookup_addr2(s->dyn, STT_FUNC, addr, name))
		return 0;
	if (s->st && !lookup_addr2(s->st, STT_FUNC, addr, name))
		return 0;
	return -1;
}

static char raw[100000];

static void
readmaps(int fd, char *buf, int max)
{
	int rv, n, len;
	n = 0;
	
	while (1) {
		len = max-n;
		rv = read(fd, buf+n, len);
		if (0 > rv) {
			perror("read");
			assert(0);
		}
		if (0 == rv)
			return;
		n += rv;
		if (n >= max) {
			fprintf(stderr, "Too many memory mappings\n");
			assert(0);
		}
	}
}

static int
isdynexec(struct modulelist *ml)
{
	int i;
	for (i = 0; i < ml->num_mm; i++)
		if (strstr(ml->mm[i].name, "lib/ld"))
			return 1;
	return 0;
}

static unsigned long
vaddr(char *name)
{
	int fd, rv, len, i;
	Elf32_Phdr *p, *q;
	Elf32_Ehdr ehdr;
	unsigned long ret;

	fd = open(name, O_RDONLY);
	if (0 > fd) {
		perror("open");
		return -1UL;
	}

	rv = read(fd, &ehdr, sizeof(ehdr));
	if (rv != sizeof(ehdr)) {
		perror("read");
		return -1UL;
	}
		
	len = ehdr.e_phnum*ehdr.e_phentsize;
	p = q = xmalloc(len);
	rv = read(fd, p, len);
	if (rv != len) {
		perror("read");
		return -1UL;
	}

	for (i = 0; i < ehdr.e_phnum; i++, p++) {
		if (p->p_type != PT_LOAD)
			continue;
		ret = p->p_vaddr;
		break;
	}

	free(q);
	close(fd);
	return ret;
}

struct modulelist *
rf_parse(int pid)
{
	char buf[1024];
	char name[RF_MAXLEN];
	char perm[10];
	char *p;
	unsigned long start, end;
	struct mm *m;
	int fd, rv, i;
	int max_mm;
	struct modulelist *ml;

	ml = xmalloc(sizeof(*ml));
	ml->pid = pid;

	/* executable name */
	snprintf(buf, sizeof(buf), "/proc/%d/exe", pid);
	rv = readlink(buf, ml->exe, sizeof(ml->exe));
	if (0 > rv) {
		fprintf(stderr, "cannot read %s\n", buf);
		return NULL;
	}
	if (rv >= sizeof(ml->exe)) {
		fprintf(stderr, "buffer overflow\n");
		return NULL;
	}
	ml->exe[rv] = '\0';

	/* read memory map */
	snprintf(buf, sizeof(buf), "/proc/%d/maps", pid);
	fd = open(buf, O_RDONLY);
	if (0 > fd) {
		fprintf(stderr, "Cannot open %s\n", buf);
		return NULL;
	}
	bzero(raw, sizeof(raw)); /* ensure data read is null terminated */
	readmaps(fd, raw, sizeof(raw));
	close(fd);

	/* parse memory map */
	ml->mm = xmalloc(sizeof(*ml->mm)*(max_mm = 500));
	ml->num_mm = 0;
	ml->exe_mm = -1;
	p = strtok(raw, "\n");
	while (p) {
		/* parse current map line */
		rv = sscanf(p, "%08lx-%08lx %s %*s %*s %*s %s\n",
			    &start, &end, perm, name);
		p = strtok(NULL, "\n");
		if (perm[2] != 'x')
			continue;

		if (ml->num_mm >= max_mm) {
			fprintf(stderr, "too many memory mappings\n");
			return NULL;
		}

		/* shared memory, devices, etc. */
		if (rv == 2
		    || !strncmp(name, "/dev/zero", strlen("/dev/zero"))
		    || !strncmp(name, "/dev/mem", strlen("/dev/mem"))
		    || !strncmp(name, "/SYSV", strlen("/SYSV"))) {
			m = &ml->mm[ml->num_mm++];
			m->start = start;
			m->end = end;
			strcpy(m->name, MEMORY_ONLY);
			continue;
		}

		/* search backward for other mapping with same name */
		for (i = ml->num_mm-1; i >= 0; i--) {
			m = &ml->mm[i];
			if (!strcmp(m->name, name))
				break;
		}

		if (i >= 0) {
			if (start < m->start)
				m->start = start;
			if (end > m->end)
				m->end = end;
		} else {
			/* new entry */
			m = &ml->mm[ml->num_mm++];
			m->start = start;
			m->end = end;
			strcpy(m->name, name);
			if (!strcmp(m->name, ml->exe))
				/* mark the executable */
				ml->exe_mm = ml->num_mm-1;
			if (access(name, R_OK)) {
				m->st = NULL;
				m->base = 0;
				continue;
			}
				
			m->st = load_symtab(name);
			if (!m->st) {
				fprintf(stderr,
					"cannot read symbol table for %s\n",
					name);
				return NULL;
			}
			/* FIXME: combine the elf cracking of
			   vaddr and load_symtab */
			m->base = m->start-vaddr(name);
		}
	}
	if (-1 == ml->exe_mm) {
		fprintf(stderr, "Cannot find executable in process\n");
		return NULL;
	}

	ml->is_dynamic = isdynexec(ml);
	return ml;
}

void
rf_free_modulelist(struct modulelist *ml)
{
	int i;
	for (i = 0; i < ml->num_mm; i++)
		if (ml->mm[i].st)
			free_symtab(ml->mm[i].st);
	free(ml->mm);
	free(ml);
}

static int
patch(unsigned long from, unsigned long to)
{
	unsigned char *p;
	int *q;
	size_t pgsize;

	/* twiddle protection */
	pgsize = getpagesize();
	if (0 > mprotect((void *) (from & ~(pgsize - 1)), pgsize,
			 PROT_READ|PROT_WRITE|PROT_EXEC)) {
		perror("mprotect");
		return -1;
	}

	/* opcode */
	p = (unsigned char *) from;
	*p++ = 0xe9;

	/* displacement */
	q = (int *) p;
	*q = to - (from + 5);
	return 0;
}

unsigned long
rf_find_function(struct modulelist *ml, char *name)
{
	int i, rv;
	struct mm *m;
	unsigned long addr;

	for (i = 0, m = ml->mm; i < ml->num_mm; i++, m++) {
		if (!strcmp(m->name, MEMORY_ONLY))
			continue;
		if (!m->st)
			continue;
		rv = lookup_func_sym(m->st, name, &addr);
		if (!rv) {
			addr += m->base;
			return addr;
		}
	}
	return 0;
}

unsigned long
rf_find_libc_function(struct modulelist *ml, char *name)
{
	int i, rv;
	struct mm *m;
	unsigned long addr;

	for (i = 0, m = ml->mm; i < ml->num_mm; i++, m++) {
		if (!strcmp(m->name, MEMORY_ONLY))
			continue;
		if (!strstr(m->name, "/libc"))
			continue;
		if (!m->st)
			continue;
		rv = lookup_func_sym(m->st, name, &addr);
		if (!rv) {
			addr += m->base;
			return addr;
		}
	}
	return 0;
}

/* find function containing ADDR and return its name.
    return NULL if there is no such function or an error.
*/
unsigned char *
rf_find_address(struct modulelist *ml, unsigned long addr)
{
	int i, rv;
	struct mm *m;
	char *name;

	for (i = 0, m = ml->mm; i < ml->num_mm; i++, m++) {
		if (!strcmp(m->name, MEMORY_ONLY))
			continue; /* no symbols */
		if (addr < m->start || addr >= m->end)
			continue;
		if (!m->st)
			continue;
		rv = lookup_addr(m->st, addr-m->base, &name);
		if (!rv)
			return name;
	}
	return NULL;	
}

int
rf_replace_libc_function(struct modulelist *ml, char *name, void *to)
{
	unsigned long addr;
	assert(getpid() == ml->pid);
	addr = rf_find_libc_function(ml, name);
	if (!addr) {
		fprintf(stderr, "cannot find %s\n", name);
		return -1;
	}
	if (0 > patch(addr, (unsigned long) to)) {
		fprintf(stderr, "refun could not patch\n");
		return -1;
	}
	return 0;
}
