#include "sys.h"
#include "ckptimpl.h"

int
ckpt_elf_openckpt(char *name)
{
	int fd, rv;
	Elf32_Ehdr ehdr;
	Elf32_Shdr shdr;
	unsigned long off;

	fd = open(name, O_RDONLY);
	if(0 > fd)
		goto fail;
	rv = xread(fd, &ehdr, sizeof(Elf32_Ehdr));
	if(0 > rv)
		goto fail;
	if(0 != strncmp(ELFMAG, ehdr.e_ident, SELFMAG))
		fatal("no elf inside\n");	

	/* FIXME: We are assuming that the executable is a checkpoint;
	   we should read the string table to verify this. */

	/* read the last entry in section header table */
	off = ehdr.e_shoff+(ehdr.e_shnum-1)*sizeof(Elf32_Shdr);
	rv = lseek(fd, off, SEEK_SET);
	if(0 > rv)
		goto fail;
	rv = xread(fd, &shdr, sizeof(Elf32_Shdr));
	if(0 > rv)
		goto fail;

	rv = lseek(fd, shdr.sh_offset, SEEK_SET);
	if(0 > rv)
		goto fail;

	return fd;
fail:
	fprintf(stderr, "cannot read elfstream %s\n", name);
	return -1;
}
