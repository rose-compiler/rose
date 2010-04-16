#include "sys.h"
#include "ckptimpl.h"

#define PAGEUP(a) ( ((a)&PAGE_MASK) + (((a)%PAGE_SIZE) ? PAGE_SIZE : 0) )

enum {
	MAXSHDR=50,		/* maximum number of elf sections */
	MAXSTRTBL=MAXSHDR*100,	/* maximum string table size */
};

typedef
struct ElfStream{
	int fd;

	unsigned long pad1;		/* bytes following end of restart*/
	unsigned long lenshstrtbl;	/* bytes of string table */
	unsigned long pad2;		/* bytes following end of string table */
	unsigned long lenprogbits;	/* bytes in checkpoint image */
	unsigned long pad3;		/* bytes following checkpoint image */ 
	unsigned long lenshdr;		/* bytes in section header */

	Elf32_Shdr shdr[MAXSHDR];
	char strtbl[MAXSTRTBL];
} ElfStream;

static ElfStream e = { .fd = -1 };

extern char restartbin[];
extern unsigned long restartbinlen;
static char sname[] = ".ckptimage";

/*
 * prepare to write out an executable restart file;
 * write a customized restart executable up to the
 * offset where the checkpoint image data goes;
 * ckpt_close_elfstream will write out the rest. *
 */
int
ckpt_open_elfstream(char *name, unsigned long ckptsize)
{
	Elf32_Ehdr *ehdr, xehdr;
	Elf32_Shdr *shdr, *p;
	char *strtbl, *q;
	char *base;
	int rv;
	char zero[PAGE_SIZE];

	e.fd = open(name, O_CREAT|O_TRUNC|O_WRONLY, 0700);
	if(0 > e.fd){
		fprintf(stderr, "cannot open checkpoint file %s: %s\n",
			name, strerror(errno));
		return -1;
	}

	base = &restartbin[0];
	ehdr = (Elf32_Ehdr*)base;
	if(0 != strncmp(ELFMAG, ehdr->e_ident, SELFMAG))
		fatal("no elf inside\n");
	shdr = (Elf32_Shdr *)(base+ehdr->e_shoff);
	strtbl = base+shdr[ehdr->e_shstrndx].sh_offset;

	/* padding and string table */
	e.pad1 = PAGEUP(restartbinlen)-restartbinlen;
	e.lenshstrtbl = shdr[ehdr->e_shstrndx].sh_size + strlen(sname) + 1;

	/* padding and checkpoint image */
	e.pad2 = PAGEUP(e.lenshstrtbl)-e.lenshstrtbl;
	e.lenprogbits = ckptsize;

	/* padding to beginning of new section header table */
	e.pad3 = PAGEUP(e.lenprogbits)-e.lenprogbits;

	/* allocate new string table for section headers,
	   fill with old entries, and add new one */
	if(sizeof(e.strtbl) < e.lenshstrtbl)
		fatal("string table too small\n");
	memcpy(e.strtbl, strtbl, shdr[ehdr->e_shstrndx].sh_size);
	q = e.strtbl+shdr[ehdr->e_shstrndx].sh_size;
	strcpy(q, sname);

	/* allocate new section hdr table, fill with
	   old entries, update string table entry, and
	   add new one */
	e.lenshdr = sizeof(Elf32_Shdr)*(ehdr->e_shnum+1);
	if(sizeof(e.shdr) < e.lenshdr)
		fatal("string table too small\n");
	memcpy(e.shdr, shdr, sizeof(Elf32_Shdr)*ehdr->e_shnum);
	e.shdr[ehdr->e_shstrndx].sh_size = e.lenshstrtbl;
	e.shdr[ehdr->e_shstrndx].sh_offset = restartbinlen+e.pad1;
	p = &e.shdr[ehdr->e_shnum];
	p->sh_name = q-e.strtbl;
	p->sh_type = SHT_PROGBITS;
	p->sh_flags = 0;
	p->sh_addr = 0;
	p->sh_offset = restartbinlen+e.pad1+e.lenshstrtbl+e.pad2;
	p->sh_size = e.lenprogbits;
	p->sh_link = 0;
	p->sh_info = 0;
	p->sh_addralign = 0;
	p->sh_entsize = 0;

	/* new header */
	memcpy(&xehdr, ehdr, sizeof(Elf32_Ehdr));
	xehdr.e_shnum++;
	xehdr.e_shoff = (restartbinlen+e.pad1
			 +e.lenshstrtbl+e.pad2
			 +e.lenprogbits+e.pad3);

	/* write out new executable up to where the checkpoint bits go */
	memset(zero, 0, sizeof(zero));
	rv = xwrite(e.fd, &xehdr, sizeof(Elf32_Ehdr));
	if(0 > rv)
		fatal("cannot write checkpoint file\n");
	rv = xwrite(e.fd, base+sizeof(Elf32_Ehdr), restartbinlen-sizeof(Elf32_Ehdr));
	if(0 > rv)
		fatal("cannot write checkpoint file\n");
	rv = xwrite(e.fd, zero, e.pad1);
	if(0 > rv)
		fatal("cannot write checkpoint file\n");
	rv = xwrite(e.fd, e.strtbl, e.lenshstrtbl);
	if(0 > rv)
		fatal("cannot write checkpoint file\n");
	rv = xwrite(e.fd, zero, e.pad2);
	if(0 > rv)
		fatal("cannot write checkpoint file\n");

	return e.fd;
}

int
ckpt_close_elfstream(int fd)
{
	int rv;
	char zero[PAGE_SIZE];

	if(fd != e.fd){
		fprintf(stderr, "bad elfstream reference\n");
		return -1;
	}

	/* write out rest of executable */
	rv = xwrite(e.fd, zero, e.pad3);
	if(0 > rv)
		fatal("cannot write checkpoint file\n");
	rv = xwrite(e.fd, e.shdr, e.lenshdr);
	if(0 > rv)
		fatal("cannot write checkpoint file\n");

	close(e.fd);
	return 0;
}
