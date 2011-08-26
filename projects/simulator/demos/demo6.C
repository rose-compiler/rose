/* Demonstrates how to obtain an instruction trace with information about the function call stack interspersed.
 *
 * Run this demo like this:
 *     $ demo6 --debug=insn ./demo6input
 *
 * Documentation can be found by running:
 *    $ doxygen RSIM.dxy
 *    $ $WEB_BROWSER docs/html/index.html
 *
 * Especially look at documentation for the RSIM_Tools name space.  The source code for the various tools is a good way to
 * learn how to do more sophisticated stuff.  There's nothing special about the tools -- you could just as easily write similar
 * classes in your own source code.
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

    /* Parse the ELF container so we can get to the symbol table. */
    char *rose_argv[4];
    rose_argv[0] = argv[0];
    rose_argv[1] = strdup("-rose:read_executable_file_format_only");
    rose_argv[2] = argv[n];
    rose_argv[3] = NULL;
    SgProject *project = frontend(3, rose_argv);

    /* Find the address of "main" and trigger a complete disassembly when we hit that point. By disassembling after we hit
     * main, we've given the dynamic linker a chance to do its work (sucking in shared objects and linking them into the
     * executable) so we have more information.  The disassembler will also organize instructions into basic blocks and
     * functions. */
    rose_addr_t main_va = RSIM_Tools::FunctionFinder().address(project, "main");
    assert(main_va!=0);
    sim.install_callback(new RSIM_Tools::MemoryDisassembler(main_va));

    /* Print the names and addresses of all known functions after we disassemble. */
    sim.install_callback(new RSIM_Tools::FunctionIndex(main_va));

    /* Install the function call stack analysis. */
    sim.install_callback(new RSIM_Tools::FunctionReporter(true));

    /* We might need some instructions for the dynamic linker that aren't part of ROSE yet. */
    sim.install_callback(new RSIM_Tools::UnhandledInstruction);

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
