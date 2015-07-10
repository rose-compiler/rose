#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// typical 32-bit stack
//
//      0x00 (8 bytes)
//      abspath(argv[0])
//      ...
//      envp[0]
//      argv[argc-1]
//      ...
//      argv[0]
//      0x69 .. 0x00            "i686"
//      0x5b
//      0xe7
//      0xf6
//      0x423ecbf6
//      0x184c6400
//      0x9af84fe5
//      0x6b000000
//      0x00000000 0x00000000
//      0x0000000f 0xffffcbdb
//      0x0000001f 0xffffdfb6
//      0x00000019 0xffffcbcb
//      0x00000017 0x00000000
//      0x0000000e 0x000003e8
//      0x00000003 0x000003e8
//      0x0000000c 0x000003e8
//      0x0000000b 0x000003e8
//      0x00000009 0x08048400
//      0x00000008 0x00000000
//      0x00000007 0xf7fe0000
//      0x00000005 0x00000007
//      0x00000004 0x00000020
//      0x00000003 0x08048034
//      0x00000011 0x00000064
//      0x00000006 0x00001000
//      0x00000010 0xbfebfbff
//      0x00000021 0xf7fdf000
//      0x00000020 0xf7fdf420
//      0
//      ...
//      envp+1
//      envp+0
//      0
//      ...
//      argv+1
//      argv+0
//      argc

static void
showAuxv(char **envp) {
    struct auxv_t {
        unsigned long type;
        unsigned long val;
    } *auxvp;
    char **p = envp;
    while (*p++) /*void*/;
    for (auxvp=(struct auxv_t*)p; 1; auxvp++) {
        switch (auxvp->type) {
            case 0:  fprintf(stderr, "    0x00  AT_NULL         %lu\n", auxvp->val); break;
            case 1:  fprintf(stderr, "    0x01  AT_IGNORE       %lu\n", auxvp->val); break;
            case 2:  fprintf(stderr, "    0x02  AT_EXECFD       %lu\n", auxvp->val); break;
            case 3:  fprintf(stderr, "    0x03  AT_PHDR         0x%lx\n", auxvp->val); break;
            case 4:  fprintf(stderr, "    0x04  AT_PHENT        0x%lx\n", auxvp->val); break;
            case 5:  fprintf(stderr, "    0x05  AT_PHNUM        %lu\n", auxvp->val); break;
            case 6:  fprintf(stderr, "    0x06  AT_PAGESZ       %lu\n", auxvp->val); break;
            case 7:  fprintf(stderr, "    0x07  AT_BASE         0x%lx\n", auxvp->val); break;
            case 8:  fprintf(stderr, "    0x08  AT_FLAGS        0x%lx\n", auxvp->val); break;
            case 9:  fprintf(stderr, "    0x09  AT_ENTRY        0x%lx\n", auxvp->val); break;
            case 10: fprintf(stderr, "    0x0a AT_NOTELF       %lu\n", auxvp->val); break;
            case 11: fprintf(stderr, "    0x0b AT_UID          %ld\n", auxvp->val); break;
            case 12: fprintf(stderr, "    0x0c AT_EUID         %ld\n", auxvp->val); break;
            case 13: fprintf(stderr, "    0x0d AT_GID          %ld\n", auxvp->val); break;
            case 14: fprintf(stderr, "    0x0e AT_EGID         %ld\n", auxvp->val); break;
            case 15: fprintf(stderr, "    0x0f AT_PLATFORM     0x%lx\n", auxvp->val); break;
            case 16: fprintf(stderr, "    0x10 AT_HWCAP        0x%lx\n", auxvp->val); break;
            case 17: fprintf(stderr, "    0x11 AT_CLKTCK       %lu\n", auxvp->val); break;
            case 18: fprintf(stderr, "    0x12 AT_FPUCW        %lu\n", auxvp->val); break;
            case 19: fprintf(stderr, "    0x13 AT_DCACHEBSIZE  %lu\n", auxvp->val); break;
            case 20: fprintf(stderr, "    0x14 AT_ICACHEBSIZE  %lu\n", auxvp->val); break;
            case 21: fprintf(stderr, "    0x15 AT_UCACHEBSIZE  %lu\n", auxvp->val); break;
            case 22: fprintf(stderr, "    0x16 AT_IGNOREPPC    %lu\n", auxvp->val); break;
            case 23: fprintf(stderr, "    0x17 AT_SECURE       %ld\n", auxvp->val); break;

            case 32: fprintf(stderr, "    0x20 AT_SYSINFO      0x%lx\n", auxvp->val); break;
            case 33: fprintf(stderr, "    0x21 AT_SYSINFO_PHDR 0x%lx\n", auxvp->val); break;
            case 34: fprintf(stderr, "    0x22 AT_L1I_CACHESHAPE 0x%lx\n", auxvp->val); break;
            case 35: fprintf(stderr, "    0x23 AT_L1D_CACHESHAPE 0x%lx\n", auxvp->val); break;
            case 36: fprintf(stderr, "    0x24 AT_L2_CACHESHAPE  0x%lx\n", auxvp->val); break;
            case 37: fprintf(stderr, "    0x25 AT_L3_CACHESHAPE  0x%lx\n", auxvp->val); break;

            default: fprintf(stderr, "    0x%02lx AT_(unknown)   0x%lx\n", auxvp->type, auxvp->val); break;
        }
        if (!auxvp->type)
            break;
    }
}
    

int
main(int argc, char *argv[], char *envp[]) {
    unsigned const char *begin=NULL, *end=NULL, *cur=NULL;
    int fd = -1;

    showAuxv(envp);

    /*  Show the memory map. */
    fd = open("/proc/self/maps", O_RDONLY);
    if (fd != -1) {
        char buffer[8192];
        ssize_t nread = read(fd, buffer, sizeof buffer);
        if (nread != -1)
            write(2, buffer, nread);
    }

    /* Dump a memory area */
    if (argc != 3) {
        fprintf(stderr, "usage: %s start size\n", argv[0]);
        exit(1);
    }

    begin += strtoull(argv[1], NULL, 0);
    end += strtoull(argv[2], NULL, 0);

    for (cur=begin; cur<end; ++cur)
        fputc(*cur, stdout);
    
    return 0;
}

