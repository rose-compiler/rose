/* Dump the specified futex table. */
#include "rose.h"
#include "RSIM_Private.h"

#ifdef ROSE_ENABLE_SIMULATOR   /* protects this whole file */

#include "RSIM_Linux32.h"

int
main(int argc, char *argv[])
{
    RSIM_Linux32 sim;           /* only used to access the futex table */
    assert(argc==2);
    sim.set_semaphore_name(argv[1]);

    RSIM_FutexTable ftab(sim.get_semaphore(), argv[1], false);
    ftab.dump(stdout);
}


#endif
