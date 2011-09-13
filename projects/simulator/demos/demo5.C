/* Demonstrates how to watch for memory access.
 *
 * Monitors the specimen stack to look for points at which instructions are executed on the stack.  We assume that the stack is
 * from 0xbffeb000 to 0xc0000000, which is normally the case for single-threaded i386 executables on 32-bit Linux.  Executing
 * an instruction on the stack could be an indication that a buffer overflow vulnerability is being exploited.
 *
 * Run this demo like this:
 *     $ demo5 ./demo5input
 *
 * Output should include lines similar to these:
 *     MemoryAccessWatcher: triggered for read access at 0xbfffef70 for 4 bytes
 *     MemoryAccessWatcher: triggered for read access at 0xbfffef74 for 4 bytes
 *     MemoryAccessWatcher: triggered for read access at 0xbfffef75 for 4 bytes
 * 
 * Documentation can be found by running:
 *    $ doxygen RSIM.dxy
 *    $ $WEB_BROWSER docs/html/index.html
 */
#include "rose.h"
#include "RSIM_Private.h"

#ifdef ROSE_ENABLE_SIMULATOR /* protects this whole file */

#include "RSIM_Linux32.h"

int
main(int argc, char *argv[], char *envp[])
{
    RSIM_Linux32 sim;
    int n = sim.configure(argc, argv, envp);

    /* We might need some instructions for the dynamic linker that aren't part of ROSE yet. */
    sim.install_callback(new RSIM_Tools::UnhandledInstruction);

    /* Watch for stack being executed. */
    rose_addr_t stack_base = 0xbffeb000;
    rose_addr_t stack_size = 0x00015000;
    unsigned operation = MemoryMap::MM_PROT_READ;
    unsigned access_type = MemoryMap::MM_PROT_EXEC;
    RTS_Message facility(stderr, NULL);
    sim.install_callback(new RSIM_Tools::MemoryAccessWatcher(stack_base, stack_size, operation, access_type, &facility));

    /* The usual stuff to run the simulator */
    sim.exec(argc-n, argv+n);
    sim.activate();
    sim.main_loop();
    sim.deactivate();
    sim.describe_termination(stderr);
    sim.terminate_self();
    return 0;
}

#else

int main(int, char *argv[])
{
    std::cerr <<argv[0] <<": not supported on this platform" <<std::endl;
    return 0;
}

#endif /* ROSE_ENABLE_SIMULATOR */
