/* Encodes standard input using a variant of the ROT-13 algorithm and writes it to standard output. */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void
usage(const char* arg0) {
    fprintf(stderr, "usage: %s [--decode|--encode]  <input  >output\n", arg0);
    exit(1);
}

int
main(int argc, char* argv[])
{
    unsigned char shift = 13;

    if (2==argc) {
        if (!strcmp(argv[1], "-d") || !strcmp(argv[1], "--decode")) {
            shift = (unsigned char)-13;
        } else if (!strcmp(argv[1], "-e") || !strcmp(argv[1], "--encode")) {
            shift = 13;
        } else {
            usage(argv[0]);
        }
    } else if (argc>2) {
        usage(argv[0]);
    }
    
    unsigned char buf[4096];
    while (1) {
        ssize_t n = read(0, buf, sizeof buf);
        if (n<0) {
            fprintf(stderr, "%s: read: %s\n", argv[0], strerror(errno));
            exit(1);
        }
        if (0==n)
            break;

        for (ssize_t i=0; i<n; i++)
            buf[i] += shift;

        unsigned char* b2 = buf;
        while (n>0) {
            ssize_t nout = write(1, b2, n);
            if (nout<0) {
                fprintf(stderr, "%s: write: %s\n", argv[0], strerror(errno));
                exit(1);
            }
            b2 += nout;
            n -= nout;
        }
    }        

    return 0;
}

