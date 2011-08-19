/* Dump the specified futex table. */
#include "rose.h"
#include "RSIM_Private.h"

#ifdef ROSE_ENABLE_SIMULATOR   /* protects this whole file */

#include "RSIM_Linux32.h"

int
main(int argc, char *argv[])
{

    if (argc<3) {
        fprintf(stderr, "usage: %s TABLE_NAME COMMAND...\n", argv[0]);
        exit(1);
    }
    RSIM_Linux32 sim;           /* only used to access the futex table */
    sim.set_semaphore_name(argv[1]);
    RSIM_FutexTable ftab(sim.get_semaphore(), argv[1], false);

    int argno=2;
    if (!strcmp(argv[argno], "show")) {
        if (argno+1!=argc) {
            fprintf(stderr, "usage: %s TABLE_NAME show", argv[0]);
            exit(1);
        }
        ftab.dump(stdout);
        
    } else if (!strcmp(argv[argno], "signal")) {
        if (argno+2!=argc) {
            fprintf(stderr, "usage: %s TABLE_NAME signal KEY\n", argv[0]);
            exit(1);
        }
        char *rest;
        rose_addr_t key = strtoull(argv[argno+1], &rest, 0);
        if (!argv[argno+1][0] || *rest) {
            fprintf(stderr, "usage: invalid key: %s\n", argv[argno+1]);
            exit(1);
        }
        ftab.signal(key, -1, 1, RSIM_FutexTable::UNLOCKED);

    } else {
        fprintf(stderr, "%s: unknown command: %s\n", argv[0], argv[argno]);
        exit(1);
    }

    return 0;
}


#endif
