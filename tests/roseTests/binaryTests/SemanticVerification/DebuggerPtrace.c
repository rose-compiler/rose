/* This program is part of ROSE but is meant to run on a system that doesn't have the ROSE libraries.  Its purpose is to
 * execute a subordinate program one instruction at a time as a sort of debugger and provide register and memory information
 * across the network. */


#define _GNU_SOURCE
#define __STDC_FORMAT_MACROS

#include "Debugger.h"

#include <arpa/inet.h>
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <unistd.h>

#define FALSE 0
#define TRUE 1
#define MIN(X,Y) ((X)<(Y)?(X):(Y))
#define MAX(X,Y) ((X)>(Y)?(X):(Y))
static const char *arg0;                /* Base name of this executable. */
static char *exename;                   /* Name of executable being traced. */
static unsigned short port=32002;       /* TCP port on which to listen. */
static int verbose;                     /* If true then show actions on stderr */
static int trace;                       /* If true then show instruction pointer on stderr */

static struct bprange_t {
    uint64_t    begin;                  /* Beginning address of break point range */
    uint64_t    size;                   /* Size of range (never zero) */
} *bp_range;                            /* Dynamic array of break point ranges */
static size_t bp_nalloc;                /* Number of break point ranges allocated */
static size_t bp_nused;                 /* Number of inital break point range entries used */

/** Print usage message to the specified file. */
static void usage(FILE *f) {
    fprintf(f, "usage: %s [--port=N] [--verbose] [--trace] EXECUTABLE [ARGS]\n", arg0);
}

/* Works like hexdump -C to display N bytes of DATA. */
static void hexdump(int fd, uint64_t base_addr, const unsigned char *data, size_t n) {
    int i, j;
    for (i=0; i<n; i+=16) {
        char *line = NULL;
        size_t line_size = 0;
        FILE *f = open_memstream(&line, &line_size);
        fprintf(f, "0x%08"PRIx64": ", base_addr+i);
        for (j=0; j<16; j++) {
            if (8==j) fputc(' ', f);
            if (i+j<n) {
                fprintf(f, " %02x", data[i+j]);
            } else {
                fputs("   ", f);
            }
        }
        fprintf(f, "  |");
        for (j=0; j<16 && i+j<n; j++) {
            if (isprint(data[i+j])) {
                fputc(data[i+j], f);
            } else {
                fputc('.', f);
            }
        }
        fputs("|\n", f);
        fclose(f);
        write(fd, line, line_size);
        free(line);
    }
}

/** Initializes a RegisterSet struct from system info. */
static void initRegisterSet(const struct user_regs_struct *regs, struct RegisterSet *rs) {
    memset(rs, 0, sizeof(*rs));
#if __WORDSIZE == 64
#define RIP_MEMBER rip
    rs->reg[X86_REG_r15]      = regs->r15;
    rs->reg[X86_REG_r14]      = regs->r14;
    rs->reg[X86_REG_r13]      = regs->r13;
    rs->reg[X86_REG_r12]      = regs->r12;
    rs->reg[X86_REG_rbp]      = regs->rbp;
    rs->reg[X86_REG_rbx]      = regs->rbx;
    rs->reg[X86_REG_r11]      = regs->r11;
    rs->reg[X86_REG_r10]      = regs->r10;
    rs->reg[X86_REG_r9]       = regs->r9;
    rs->reg[X86_REG_r8]       = regs->r8;
    rs->reg[X86_REG_rax]      = regs->rax;
    rs->reg[X86_REG_rcx]      = regs->rcx;
    rs->reg[X86_REG_rdx]      = regs->rdx;
    rs->reg[X86_REG_rsi]      = regs->rsi;
    rs->reg[X86_REG_rdi]      = regs->rdi;
    rs->reg[X86_REG_orig_rax] = regs->orig_rax;
    rs->reg[X86_REG_rip]      = regs->rip;
    rs->reg[X86_REG_cs]       = regs->cs;
    rs->reg[X86_REG_eflags]   = regs->eflags;
    rs->reg[X86_REG_rsp]      = regs->rsp;
    rs->reg[X86_REG_ss]       = regs->ss;
    rs->reg[X86_REG_fs_base]  = regs->fs_base;
    rs->reg[X86_REG_gs_base]  = regs->gs_base;
    rs->reg[X86_REG_ds]       = regs->ds;
    rs->reg[X86_REG_es]       = regs->es;
    rs->reg[X86_REG_fs]       = regs->fs;
    rs->reg[X86_REG_gs]       = regs->gs;
#else
#define RIP_MEMBER eip
    /* Note that regs data members are signed, so they will be sign extended to 64 bits. */
    rs->reg[X86_REG_ebx]      = regs->ebx;
    rs->reg[X86_REG_ecx]      = regs->ecx;
    rs->reg[X86_REG_edx]      = regs->edx;
    rs->reg[X86_REG_esi]      = regs->esi;
    rs->reg[X86_REG_edi]      = regs->edi;
    rs->reg[X86_REG_ebp]      = regs->ebp;
    rs->reg[X86_REG_eax]      = regs->eax;
    rs->reg[X86_REG_xds]      = regs->xds;
    rs->reg[X86_REG_xes]      = regs->xes;
    rs->reg[X86_REG_xfs]      = regs->xfs;
    rs->reg[X86_REG_xgs]      = regs->xgs;
    rs->reg[X86_REG_orig_eax] = regs->orig_eax;
    rs->reg[X86_REG_eip]      = regs->eip;
    rs->reg[X86_REG_xcs]      = regs->xcs;
    rs->reg[X86_REG_eflags]   = regs->eflags;
    rs->reg[X86_REG_esp]      = regs->esp;
    rs->reg[X86_REG_xss]      = regs->xss;
#endif
}

/** Reads @p size bytes of memory beginning at address @p addr and returns the result in the heap memory buffer pointed to
 *  by @p buf. The @p bufp and @bufnallocp arguments are similar to those of the GNU readline function. Returns non-negative
 *  if successful, negative if memory cannot be read from the address. */
static int read_memory(pid_t child, size_t addr, size_t size, unsigned char **bufp, size_t *bufnallocp) {
    if (*bufnallocp < size) {
        *bufnallocp = size;
        *bufp = realloc(*bufp, *bufnallocp);
        assert(*bufp);
    }

    size_t i;
    unsigned char *ptr = *bufp;
    while (size>0) {
        size_t nbytes = MIN(size, sizeof(long));
        unsigned long word = ptrace(PTRACE_PEEKDATA, child, (void*)addr, NULL);
        if (errno!=0)
            return -1;
        for (i=0; i<nbytes; i++) {
            *ptr++ = (word >> (i*8)) & 0xff;
        }
        size -= nbytes;
        addr += nbytes;
    }
    return 0;
}

/* Search through all mapped memory for the child, looking for a particular value. */
static size_t search_memory(pid_t child, int client, uint32_t value, uint32_t mask, int ascii) {
    char *line=NULL, outbuf[256];
    unsigned char *buf=NULL;
    size_t linesz=0, bufsz=0, i, found=0;
    FILE *f;
    uint32_t needle;
    size_t nfound=0;

    if (0==mask)
        mask = 0xffffffff;
    needle = value & mask;

    sprintf(outbuf, "/proc/%d/maps", child);
    f = fopen(outbuf, "r");
    assert(f);
    
    while (getline(&line, &linesz, f)>0) {
        char *rest;
        uint64_t memlo = strtoll(line, &rest, 16);
        uint64_t memhi = strtoll(rest+1, NULL, 16);
        uint64_t memsz = memhi-memlo;
        if (read_memory(child, memlo, memsz, &buf, &bufsz)>=0) {
            for (i=0; i+4<memsz; i++) {
                if ((*(uint32_t*)(buf+i) & mask) == needle) {
                    nfound++;
                    if (ascii) {
                        if (memlo!=found) {
                            write(client, line, strlen(line));
                            found = memlo;
                        }
                        sprintf(outbuf, "    at 0x%08"PRIx64" (0x%08"PRIx64"+0x%zu): 0x%08"PRIx32"\n",
                                memlo+i, memlo, i, *(uint32_t*)(buf+i));
                        write(client, outbuf, strlen(outbuf));
                    } else {
                        uint64_t result = memlo+i;
                        write(client, &result, sizeof result);
                    }
                }
            }
        }
    }
    fclose(f);
    return nfound;
}

/** Adds a new break point range to the list. A size of zero is the same as a size of one. This is a very simple function that
 *  doesn't try to minimize the number of entries by combining overlaps. */
static void bp_add(uint64_t begin, uint64_t size) {
    if (bp_nused>=bp_nalloc) {
        bp_nalloc = MAX(64, 2*bp_nalloc);
        bp_range = realloc(bp_range, bp_nalloc*sizeof(*bp_range));
        assert(bp_range);
    }
    bp_range[bp_nused].begin = begin;
    bp_range[bp_nused].size = MAX(size, 1);
    bp_nused++;
}

/** Read one command and return it along with any argument. Returns CMD_QUIT at EOF, -1 for errors. */
static int get_command(int fd, uint64_t *arg1, uint64_t *arg2) {
    char cmd;
    ssize_t nread = read(fd, &cmd, 1);
    if (nread<0) {
        fprintf(stderr, "%s: read from client failed: %s\n", arg0, strerror(errno));
        return -1;
    }
    if (cmd<CMD_NCOMMANDS) {
        int nargs;
        switch (cmd) {
            case CMD_QUIT:
            case CMD_STEP:
            case CMD_REGS:
            case CMD_NOOP:
                nargs = 0;
                break;
            case CMD_CONT:
                nargs = 1;
                break;
            case CMD_BREAK:
            case CMD_MEM:
                nargs = 2;
                break;
            default:
                abort();
        }
        if (nargs>=1) {
            nread = read(fd, arg1, sizeof(*arg1));
            if (nread!=sizeof(*arg1)) {
                fprintf(stderr, "%s: short read from client\n", arg0);
                return -1;
            }
        }
        if (nargs>=2) {
            nread = read(fd, arg2, sizeof(*arg1));
            if (nread!=sizeof(*arg2)) {
                fprintf(stderr, "%s: short read from client\n", arg0);
                return -1;
            }
        }
    } else if (isprint(cmd)) {
        size_t at=0;
        char ch, *rest, line[1024];
        while (1==read(fd, &ch, 1) && '\n'!=ch) {
            assert(at+2<sizeof line);
            line[at++] = ch;
        }
        line[at] = '\0';
        *arg1 = strtoull(line, &rest, 0);
        *arg2 = strtoull(rest, NULL, 0);
    }
    return cmd;
}
        
int main(int argc, char *argv[]) {
    char line_ending = isatty(2) ? '\r' : '\n';

    /* Executable base name */
    const char *slash = strrchr(argv[0], '/');
    arg0 = slash ? slash+1 : argv[0];

    /* Parse command-line */
    int argno;
    for (argno=1; argno<argc; argno++) {
        if (!strcmp(argv[argno], "--")) {
            argno++;
            break;
        } else if (!strcmp(argv[argno], "-h") || !strcmp(argv[argno], "--help")) {
            usage(stdout);
            exit(0);
        } else if (!strncmp(argv[argno], "--port=", 7)) {
            char *rest;
            port = strtol(argv[argno]+7, &rest, 0);
            if (!argv[argno][7] || *rest) {
                usage(stderr);
                exit(1);
            }
        } else if (!strcmp(argv[argno], "--trace")) {
            trace = 1;
        } else if (!strcmp(argv[argno], "--verbose")) {
            verbose = 1;
        } else if (argv[argno][0] == '-') {
            usage(stderr);
            exit(1);
        } else {
            break;
        }
    }
    if (argno>=argc) {
        usage(stderr);
        exit(1);
    }
    exename = argv[argno];

    /* Fork. Child prepares itself for tracing and then execs the desired file. */
    pid_t child = fork();
    if (child<0) {
        fprintf(stderr, "%s: fork: %s\n", arg0, strerror(errno));
        exit(1);
    } else if (0==child) {
        /* This is the child */
        if (ptrace(PTRACE_TRACEME, 0, 0, 0)<0) {
            fprintf(stderr, "%s: ptrace(TRACEME): %s\n", arg0, strerror(errno));
            exit(1);
        }
        execvp(argv[argno], argv+argno);
        fprintf(stderr, "%s: execvp: %s\n", arg0, strerror(errno));
        exit(1);
    }

    /* Create the socket and wait for the (single) client to connect. */
    int server = socket(AF_INET, SOCK_STREAM, 0);
    if (server<0) {
        fprintf(stderr, "%s: socket: %s\n", arg0, strerror(errno));
        exit(1);
    }
    int optval = 1;
    if (setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval)<0) {
        fprintf(stderr, "%s: setsockopt: %s\n", arg0, strerror(errno));
        exit(1);
    }
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(server, (struct sockaddr*)&server_addr, sizeof server_addr)<0) {
        fprintf(stderr, "%s: bind: %s\n", arg0, strerror(errno));
        exit(1);
    }
    if (verbose)
        fprintf(stderr, "%s: listening on TCP port %hu\n", arg0, port);
    if (listen(server, 1)<0) {
        fprintf(stderr, "%s: listen: %s\n", arg0, strerror(errno));
        exit(1);
    }
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int client = accept(server, (struct sockaddr*)&client_addr, &client_addr_len);
    if (client<0) {
        fprintf(stderr, "%s: accept: %s\n", arg0, strerror(errno));
        exit(1);
    }
    if (verbose)
        fprintf(stderr, "%s: connection from host %s, port %hu\n",
                arg0, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    /* Allow the subordinate process to run. */
    int command = CMD_NOOP;                             /* debugger command last issued */
    uint64_t cmd_arg1=0, cmd_arg2=0;                    /* arguments for last debugger command */
    unsigned char *data=NULL;                           /* heap-allocated command result buffer */
    size_t data_nalloc=0;                               /* allocated size of result buffer */

    while (1) {
        int wstat;
        pid_t pid = waitpid(child, &wstat, 0);
        if (pid<0) {
            fprintf(stderr, "%s: waitpid: %s\n", arg0, strerror(errno));
            kill(child, SIGKILL);
            exit(1);
        } else if (WIFEXITED(wstat)) {
            if (verbose)
                fprintf(stderr, "%s: child %d exited with status %d\n", arg0, child, WEXITSTATUS(wstat));
            break;
        } else if (WIFSIGNALED(wstat)) {
            if (verbose)
                fprintf(stderr, "%s: child %d exited from signal: %s%s\n", arg0, child, strsignal(WTERMSIG(wstat)),
                        WCOREDUMP(wstat)?" (core dumped)":"");
            break;
        } else if (WIFSTOPPED(wstat)) {
            struct user_regs_struct regs;
            if (ptrace(PTRACE_GETREGS, child, 0, &regs)<0) {
                fprintf(stderr, "%s: ptrace(GETREGS): %s\n", arg0, strerror(errno));
                kill(child, SIGKILL);
                exit(1);
            }

            if (trace) {
                fprintf(stderr, "0x%016lx%c", regs.RIP_MEMBER, line_ending);
            }

            /* If the last command was "continue" then continue execution until we hit a break point.  If the continue command
             * was given an argument then that address is the only break point we use, otherwise use the break point range
             * list set with the "b" commands. */
            if (CMD_CONT==command || CMD_CONT_A==command) {
                if (cmd_arg1!=0) {
                    if (cmd_arg1!=regs.RIP_MEMBER) {
                        if (ptrace(PTRACE_SINGLESTEP, child, 0, 0)<0) {
                            fprintf(stderr, "%s: ptrace(STEP): %s\n", arg0, strerror(errno));
                            kill(child, SIGKILL);
                            exit(1);
                        }
                        continue;
                    }
                } else {
                    int broke=0;
                    size_t i;
                    for (i=0; !broke && i<bp_nused; i++)
                        broke = bp_range[i].begin<=regs.RIP_MEMBER && regs.RIP_MEMBER<bp_range[i].begin+bp_range[i].size;
                    if (!broke) {
                        if (ptrace(PTRACE_SINGLESTEP, child, 0, 0)<0) {
                            fprintf(stderr, "%s: ptrace(STEP): %s\n", arg0, strerror(errno));
                            kill(child, SIGKILL);
                            exit(1);
                        }
                        continue;
                    }
                }
            }

            if (verbose)
                fprintf(stderr, "%s: child %d stopped at 0x%016lx %s\n", arg0, child, regs.RIP_MEMBER, strsignal(WSTOPSIG(wstat)));

            /* Delayed status report */
            char buf[256];
            uint64_t result;
            switch (command) {
                case CMD_STEP:
                case CMD_CONT:
                    result = regs.RIP_MEMBER;
                    write(client, &result, sizeof result);
                    break;

                case CMD_STEP_A:
                case CMD_CONT_A:
                    sprintf(buf, "OK; stopped at 0x%016lx\n", regs.RIP_MEMBER);
                    write(client, buf, strlen(buf));
                    break;
            }

            /* Process client commands */
            while (1) {
                if ((command = get_command(client, &cmd_arg1, &cmd_arg2))<0) {
                    kill(child, SIGKILL);
                    exit(0);
                }
                switch (command) {
                    case CMD_STEP_A:
                    case CMD_STEP:
                    case CMD_CONT_A:
                    case CMD_CONT:
                        /* Status will be reported next time we stop. */
                        goto resume;

                    case CMD_BREAK_A:
                        bp_add(cmd_arg1, cmd_arg2);
			sprintf(buf, "OK; breakpoint at 0x%"PRIx64" for 0x%"PRIx64" byte%s\n",
                                cmd_arg1, cmd_arg2, 1==cmd_arg2?"":"s");
                        write(client, buf, strlen(buf));
                        break;
                        
                    case CMD_BREAK:
                        if (0==cmd_arg2) cmd_arg2 = 1;
                        bp_add(cmd_arg1, cmd_arg2);
                        result = 0;
                        write(client, &result, sizeof result);
                        break;
                        
                    case CMD_NOOP:
                        abort();

                    case CMD_REGS: {
		        struct RegisterSet mesg;
		        initRegisterSet(&regs, &mesg);
                        result = sizeof(mesg);
                        write(client, &result, sizeof result);
                        write(client, &mesg, sizeof mesg);
                        break;
		    }

                    case CMD_MEM:
                        if (0==cmd_arg2) cmd_arg2 = 1;
                        if (0<=read_memory(child, cmd_arg1, cmd_arg2, &data, &data_nalloc)) {
                            write(client, &cmd_arg2, sizeof cmd_arg2);
                            write(client, data, cmd_arg2);
                        } else {
                            result = 0;
                            write(client, &result, sizeof result); /*error*/
                        }
                        break;

                    case CMD_MEM_A:
                        if (0==cmd_arg2) cmd_arg2 = 1;
                        read_memory(child, cmd_arg1, cmd_arg2, &data, &data_nalloc);
                        sprintf(buf, "OK; memory dump: %"PRIu64" byte%s at 0x%"PRIx64"\n",
                                cmd_arg2, 1==cmd_arg2?"":"s beginning", cmd_arg1);
                        write(client, buf, strlen(buf));
                        hexdump(client, cmd_arg1, data, cmd_arg2);
                        break;

                    case CMD_SRCH:
                        result = search_memory(child, client, cmd_arg1, cmd_arg2, FALSE);
                        write(client, &result, sizeof result);
                        break;

                    case CMD_SRCH_A:
                        result = search_memory(child, client, cmd_arg1, cmd_arg2, TRUE);
                        sprintf(buf, "OK; found %"PRIu64" instance%s\n", result, 1==result?"":"s");
                        write(client, buf, strlen(buf));
                        break;
                        
                    case CMD_REGS_A: {
                        char *s = NULL;
                        size_t ssz = 0;
			struct RegisterSet rs;
			initRegisterSet(&regs, &rs);
                        FILE *out = open_memstream(&s, &ssz);
                        assert(out);
                        fprintf(out, "OK; register dump:\n");
#if __WORDSIZE == 64
                        fprintf(out, "r15      = 0x%016"PRIx64"\n", rs.reg[X86_REG_r15]);
                        fprintf(out, "r14      = 0x%016"PRIx64"\n", rs.reg[X86_REG_r14]);
                        fprintf(out, "r13      = 0x%016"PRIx64"\n", rs.reg[X86_REG_r13]);
                        fprintf(out, "r12      = 0x%016"PRIx64"\n", rs.reg[X86_REG_r12]);
                        fprintf(out, "rbp      = 0x%016"PRIx64"\n", rs.reg[X86_REG_rbp]);
                        fprintf(out, "rbx      = 0x%016"PRIx64"\n", rs.reg[X86_REG_rbx]);
                        fprintf(out, "r11      = 0x%016"PRIx64"\n", rs.reg[X86_REG_r11]);
                        fprintf(out, "r10      = 0x%016"PRIx64"\n", rs.reg[X86_REG_r10]);
                        fprintf(out, "r9       = 0x%016"PRIx64"\n", rs.reg[X86_REG_r9]);
                        fprintf(out, "r8       = 0x%016"PRIx64"\n", rs.reg[X86_REG_r8]);
                        fprintf(out, "rax      = 0x%016"PRIx64"\n", rs.reg[X86_REG_rax]);
                        fprintf(out, "rcx      = 0x%016"PRIx64"\n", rs.reg[X86_REG_rcx]);
                        fprintf(out, "rdx      = 0x%016"PRIx64"\n", rs.reg[X86_REG_rdx]);
                        fprintf(out, "rsi      = 0x%016"PRIx64"\n", rs.reg[X86_REG_rsi]);
                        fprintf(out, "rdi      = 0x%016"PRIx64"\n", rs.reg[X86_REG_rdi]);
                        fprintf(out, "orig_rax = 0x%016"PRIx64"\n", rs.reg[X86_REG_orig_rax]);
                        fprintf(out, "rip      = 0x%016"PRIx64"\n", rs.reg[X86_REG_rip]);
                        fprintf(out, "cs       = 0x%016"PRIx64"\n", rs.reg[X86_REG_cs]);
                        fprintf(out, "eflags   = 0x%016"PRIx64"\n", rs.reg[X86_REG_eflags]);
                        fprintf(out, "rsp      = 0x%016"PRIx64"\n", rs.reg[X86_REG_rsp]);
                        fprintf(out, "ss       = 0x%016"PRIx64"\n", rs.reg[X86_REG_ss]);
                        fprintf(out, "fs_base  = 0x%016"PRIx64"\n", rs.reg[X86_REG_fs_base]);
                        fprintf(out, "gs_base  = 0x%016"PRIx64"\n", rs.reg[X86_REG_gs_base]);
                        fprintf(out, "ds       = 0x%016"PRIx64"\n", rs.reg[X86_REG_ds]);
                        fprintf(out, "es       = 0x%016"PRIx64"\n", rs.reg[X86_REG_es]);
                        fprintf(out, "fs       = 0x%016"PRIx64"\n", rs.reg[X86_REG_fs]);
                        fprintf(out, "gs       = 0x%016"PRIx64"\n", rs.reg[X86_REG_gs]);
#else
                        fprintf(out, "ebp      = 0x%016"PRIx64"\n", rs.reg[X86_REG_ebp]);
                        fprintf(out, "ebx      = 0x%016"PRIx64"\n", rs.reg[X86_REG_ebx]);
                        fprintf(out, "eax      = 0x%016"PRIx64"\n", rs.reg[X86_REG_eax]);
                        fprintf(out, "ecx      = 0x%016"PRIx64"\n", rs.reg[X86_REG_ecx]);
                        fprintf(out, "edx      = 0x%016"PRIx64"\n", rs.reg[X86_REG_edx]);
                        fprintf(out, "esi      = 0x%016"PRIx64"\n", rs.reg[X86_REG_esi]);
                        fprintf(out, "edi      = 0x%016"PRIx64"\n", rs.reg[X86_REG_edi]);
                        fprintf(out, "orig_eax = 0x%016"PRIx64"\n", rs.reg[X86_REG_orig_eax]);
                        fprintf(out, "eip      = 0x%016"PRIx64"\n", rs.reg[X86_REG_eip]);
                        fprintf(out, "xcs      = 0x%016"PRIx64"\n", rs.reg[X86_REG_xcs]);
                        fprintf(out, "eflags   = 0x%016"PRIx64"\n", rs.reg[X86_REG_eflags]);
                        fprintf(out, "esp      = 0x%016"PRIx64"\n", rs.reg[X86_REG_esp]);
                        fprintf(out, "xss      = 0x%016"PRIx64"\n", rs.reg[X86_REG_xss]);
                        fprintf(out, "xds      = 0x%016"PRIx64"\n", rs.reg[X86_REG_xds]);
                        fprintf(out, "xes      = 0x%016"PRIx64"\n", rs.reg[X86_REG_xes]);
                        fprintf(out, "xfs      = 0x%016"PRIx64"\n", rs.reg[X86_REG_xfs]);
                        fprintf(out, "xgs      = 0x%016"PRIx64"\n", rs.reg[X86_REG_xgs]);
#endif

                        fclose(out);
                        write(client, s, ssz);
                        free(s);
                        break;
                    }
                        
                    case CMD_QUIT:
                        result = 0;
                        write(client, &result, sizeof result);
                        break;

                    case CMD_QUIT_A:
                        kill(child, SIGKILL);
                        strcpy(buf, "OK; killed subordinate\n");
                        write(client, buf, strlen(buf));
                        exit(0);

                    case CMD_DEBUG:
                        result = 0;
                        write(client, &result, sizeof result);
                        break;

                    case CMD_DEBUG_A: {
#if __WORDSIZE == 32
                        struct gdt_t {
                            uint16_t limit;
                            uint32_t base;
                        } __attribute__((packed));
#else
                        struct gdt_t {
                            uint64_t base;
                            uint16_t limit;
                        } __attribute__((packed));
#endif
                        struct gdt_t gdt;
                        memset(&gdt, 0, sizeof gdt);
                        asm("sgdt %0" : "=m" (gdt));
                        hexdump(client, 0, (const unsigned char*)&gdt, sizeof gdt);
                        sprintf(buf, "OK; base=0x%0*"PRIx64", limit=0x%04hx\n",
                                2*(int)sizeof(gdt.base), (uint64_t)gdt.base, gdt.limit);
                        write(client, buf, strlen(buf));
                        break;
                    }

                    default:
                        fprintf(stderr, "%s: bad command\n", arg0);
                        kill(child, SIGKILL);
                        exit(0);
                }
            }

            /* Resume the subordinate */
        resume:
            if (verbose)
                fprintf(stderr, "%s: child %d continuing...\n", arg0, child);
            if (ptrace(PTRACE_SINGLESTEP, child, 0, 0)<0) {
                fprintf(stderr, "%s: ptrace(STEP): %s\n", arg0, strerror(errno));
                kill(child, SIGKILL);
                exit(1);
            }
        } else if (WIFCONTINUED(wstat)) {
            if (verbose)
                fprintf(stderr, "%s: child %d continued\n", arg0, child);
        }
    }

    return 0;
}

        
        
    
