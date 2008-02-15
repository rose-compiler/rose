#include "sys.h"
#include "ckpt.h"
#include "ckptimpl.h"

/* Write NLONG 4 byte words from BUF into PID starting
   at address POS.  Calling process must be attached to PID. */
static int
write_mem(pid_t pid, unsigned long *buf, int nlong, unsigned long pos)
{
	unsigned long *p;
	int i;

	for (p = buf, i = 0; i < nlong; p++, i++)
		if (0 > ptrace(PTRACE_POKEDATA, pid, pos+(i*4), *p))
			return -1;
	return 0;
}

static char code[] = {
	0xbc, 0x0, 0x0, 0x0, 0x0,   /* mov $0, %esp (NEWESP)*/
	0xe8, 0x0, 0x0, 0x0, 0x0,   /* call $0 (DLOPEN)*/
                                    /* (DLRET) */
	0xbc, 0x0, 0x0, 0x0, 0x0,   /* mov $0, %oldesp (OLDESP) */
	0xcc                        /* trap */
};

/* offsets in code for addresses */
enum {
	OFF_NEWESP = 1,
	OFF_DLOPEN = 6,
	OFF_DLRET = 10,
	OFF_OLDESP = 11
};

int
hijack(int pid, char *libname)
{
	struct user_regs_struct regs, oregs;
	unsigned long dlopenaddr, codeaddr, libaddr;
	unsigned long esp;
	unsigned long *p;
	int n;
	char *arg;
	int status, rv;
	struct modulelist *ml;

	if (0 > kill(pid, 0)) {
		fprintf(stderr, "cannot hijack process %d: %s\n",
			pid, strerror(errno));
		return -1;
	}

	/* find dlopen in PID's libc */
	ml = rf_parse(pid);
	if (!ml) {
		fprintf(stderr, "cannot parse %d\n", pid);
		return -1;
	}
	if (!ml->is_dynamic) {
		fprintf(stderr, "cannot hijack static programs\n");
		return -1;
	}

	dlopenaddr = rf_find_libc_function(ml, "_dl_open");
	if (-1UL == dlopenaddr) {
		fprintf(stderr, "cannot find _dl_open in process\n");
		return -1;
	}
	rf_free_modulelist(ml);

	/* Attach */
	if (0 > ptrace(PTRACE_ATTACH, pid, 0, 0)) {
		fprintf(stderr, "cannot attach to %d\n", pid);
		exit(1);
	}
	waitpid(pid, NULL, 0);
	ptrace(PTRACE_GETREGS, pid, 0, &oregs);
	memcpy(&regs, &oregs, sizeof(regs));

	/* push EIP */
	regs.esp -= 4;
	ptrace(PTRACE_POKEDATA, pid, regs.esp, regs.eip-2);

	/* OLDESP */
	esp = regs.esp;

	/* push library name */
	n = strlen(libname)+1;
	n = n/4 + (n%4 ? 1 : 0);
	arg = xmalloc(n*sizeof(unsigned long));
	memcpy(arg, libname, strlen(libname));
	libaddr = esp - n*4;
	if (0 > write_mem(pid, (unsigned long*)arg, n, libaddr)) {
		fprintf(stderr, "cannot write dlopen argument (%s)\n",
			strerror(errno));
		free(arg);
		return -1;
	}
	free(arg);
		
	/* finish code and push it */
	codeaddr = libaddr - sizeof(code);
	p = (unsigned long*)&code[OFF_NEWESP];
	*p = codeaddr; /* stack begins after code */
	p = (unsigned long*)&code[OFF_DLOPEN];
	*p = dlopenaddr-(codeaddr+OFF_DLRET);
	p = (unsigned long*)&code[OFF_OLDESP];
	*p = esp;
	if (0 > write_mem(pid, (unsigned long*)&code,
			  sizeof(code)/sizeof(long), codeaddr)) {
		fprintf(stderr, "cannot write code\n");
		return -1;
	}
	regs.eip = codeaddr;

	/* Setup dlopen call; use internal register calling convention */
	regs.eax = libaddr;             /* library name */
	regs.edx = RTLD_NOW;            /* dlopen mode */
	regs.ecx = codeaddr+OFF_DLRET;  /* caller context used by dlopen */
	ptrace(PTRACE_SETREGS, pid, 0, &regs);

	rv = ptrace(PTRACE_CONT, pid, 0, 0);
	if (0 > rv) {
		perror("PTRACE_CONT");
		return -1;
	}
	while (1) {
		if (0 > waitpid(pid, &status, 0)) {
			perror("waitpid");
			return -1;
		}
		if (WIFSTOPPED(status)) {
			if (WSTOPSIG(status) == SIGTRAP)
				break;
			else if (WSTOPSIG(status) == SIGSEGV
				 || WSTOPSIG(status) == SIGBUS) {
				if (0 > ptrace(PTRACE_CONT, pid, 0,
					       WSTOPSIG(status))) {
					perror("PTRACE_CONT");
					return -1;
				}
			} else if (0 > ptrace(PTRACE_CONT, pid, 0, 0)) {
				/* FIXME: Remember these signals */
				perror("PTRACE_CONT");
				return -1;
			}
		}
	}
		
	if (0 > ptrace(PTRACE_SETREGS, pid, 0, &oregs)) {
		perror("PTRACE_SETREGS");
		return -1;
	}
	rv = ptrace(PTRACE_DETACH, pid, 0, 0);
	if (0 > rv) {
		perror("PTRACE_DETACH");
		return -1;
	}

	return 0;
}
