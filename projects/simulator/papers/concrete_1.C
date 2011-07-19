/* Demonstrates how to perform concrete analysis on the "updcrc" function.  We allow the specimen (compiled from
 * gzip_sample_1.c) to run until main() is entered, then allocate some memory, initialize it with a string, and analyze the
 * updcrc() function to obtain an output value.
 *
 * Since this is a concrete analysis, most of it could be done in GDB as well.  difficult to adjust memory
 * mapping of the specimen in GDB.
 *
 * Documentation can be found by running:
 *    $ doxygen RSIM.dxy
 *    $ $WEB_BROWSER docs/html/index.html
 */

#include "rose.h"
#include "RSIM_Private.h"

#ifdef ROSE_ENABLE_SIMULATOR /* protects this whole file */

#include "RSIM_Linux32.h"

// Monitors the CPU instruction pointer.  When it reaches the specified value (beginning of main()), perform some analysis as
// described at the top of this file.
class Analysis: public RSIM_Callbacks::InsnCallback {
public:
    rose_addr_t trigger_va, analysis_va;
    bool triggered;
    uint8_t buf[0x1000];                // backing store for specimen memory used during this analysis
    rose_addr_t buf_va;                 // specimen address for "buf"
    pt_regs_32 regs;                    // registers we save during the analysis, and restore at the end

    static const uint32_t END_OF_ANALYSIS = 0x99887766;

    Analysis(rose_addr_t trigger_va, rose_addr_t analysis_va)
        : trigger_va(trigger_va), analysis_va(analysis_va), triggered(false), buf_va(0) {}

    // No-op since we're single threaded for this demo.
    virtual Analysis *clone() { return this; }

    virtual bool operator()(bool enabled, const Args &args) {
        RSIM_Process *process = args.thread->get_process();
        if (enabled && !triggered && args.insn->get_address()==trigger_va) {
            triggered = true;

            // Output is sent to the TRACE_MISC facility.  All messages will have a prefix that includes such things as the
            // process ID, the current EIP register value, time since start of simulation, etc.
            RTS_Message *trace = args.thread->tracing(TRACE_MISC);
            trace->mesg("Analysis: triggered for virtual address 0x%08"PRIx64, analysis_va);

            // Save registers to restore at the end of the analysis.
            regs = args.thread->get_regs();

            // Turn off delivery of async signals to the specimen since we're not actually simulating the real specimen at this
            // point.  We'll enable the delivery again when the analysis is finished.  Any signals that arrive during the
            // analysis will be delivered to the simulator rather than the specimen. Synchronous signals (SIGSEGV, etc) are
            // still delivered to the specimen.
            process->get_simulator()->deactivate();

            // Allocate some memory in the specimen for the buffer (first argument to updcrc()).  This buffer will be
            // removed from the specimen address space at the end of the analysis.  We could use RSIM_Process::mem_map(),
            // which is what the mmap and mmap2 system calls use, but doing so would modify future values returned by those
            // system calls.  By using a lower-level interface, we're able to map memory for the specimen without affecting
            // its future behavior.  GDB is unable to do this kind of manipulation.  We need a write lock around memory map
            // manipulation if this example were multi-threaded.  The buffer should be at or above 0x40000000, at least one
            // page in length, and aligned on a page boundary.
            RTS_WRITE(process->rwlock()) {
                buf_va = process->get_memory()->find_free(0x40000000, sizeof buf, sizeof buf);
                if (buf_va) {
                    MemoryMap::MapElement me(buf_va, sizeof buf, buf, 0, MemoryMap::MM_PROT_RWX);
                    me.set_name("Debugging page");
                    process->get_memory()->insert(me);
                }
            } RTS_WRITE_END;
            if (!buf_va) {
                trace->mesg("Analysis: couldn't find free memory for buffer.");
                return true;  // proceed as if we were never called
            }
            trace->mesg("Analysis: our debug buffer is mapped at 0x%08"PRIx64" in the specimen", buf_va);
            //process->mem_showmap(trace, "Current memory map is:", "    ");

            // Initialize the buffer with some data.  Writing data into our own buffer causes the data to also be written
            // into the specimen.  We could have alternatively used RSIM_Process::mem_write(), which would have used the
            // specimen's buffer address instead.
            strcpy((char*)buf, "hello world!");

            // We also use the top of the buffer as the stack so that our analysis doesn't interfere with the specimen's
            // normal stack.  If the specimen uses the usual stack semantics (that anything below the current stack pointer
            // is dead) and our analysis restores the original stack pointer, we don't really need to do this.   Doing it
            // this way helps the analysis avoid interfering with the specimen.
            buf[0x1000-1] = 0x90;                                       // x86 NOP instruction at return address
            *(uint32_t*)(buf+0x1000-5)  = strlen((char*)buf);           // updcrc's second argument
            *(uint32_t*)(buf+0x1000-9)  = (uint32_t)buf_va;             // updcrc's first argument
            *(uint32_t*)(buf+0x1000-13) = buf_va+0x1000-1;              // special return address, the NOP above
            args.thread->policy.writeGPR(x86_gpr_sp, args.thread->policy.number<32>(buf_va+0x1000-13));

            // Modify EIP to point to the entry address of the function being analyzed, and set "enabled" to false to
            // indicate that the current instruction (the first instruction of main), should not be simulated at this
            // time.
            args.thread->policy.writeIP(args.thread->policy.number<32>(analysis_va));
            enabled = false;
            trace->mesg("Analysis: simulator will continue to run to analyse the function...");

        } else if (enabled && triggered && args.insn->get_address()==buf_va+0x1000-1) {
            // We've just returned from the analyzed function and we're about to execute the NOP instruction we inserted at the
            // top of our debugging stack.  Obtain the analyzed function's return value from the EAX register, restore all
            // registers to their original values, and unmap our debugging buffer from the specimen.
            RTS_Message *trace = args.thread->tracing(TRACE_MISC);
            uint32_t result = args.thread->policy.readGPR(x86_gpr_ax).known_value();
            trace->mesg("Analysis: function returned 0x%08"PRIx32, result);

            // Unmap our debugging page of memory
            RTS_WRITE(process->rwlock()) {
                process->get_memory()->erase(MemoryMap::MapElement(buf_va, sizeof buf));
            } RTS_WRITE_END;

            // Restore registers
            args.thread->init_regs(regs);

            // Reactivate delivery of async signals to the specimen.
            process->get_simulator()->activate();

            // No need to execute our NOP instruction.
            enabled = false;
            trace->mesg("Analysis: simulator will continue running specimen now...");
        }
        return enabled;
    }
};


int main(int argc, char *argv[], char *envp[])
{
    RSIM_Linux32 sim;
    int n = sim.configure(argc, argv, envp);

    // Parse the ELF container so we can get to the symbol table.  This is standard ROSE static analysis.
    char *rose_argv[4];
    rose_argv[0] = argv[0];
    rose_argv[1] = strdup("-rose:read_executable_file_format_only");
    rose_argv[2] = argv[n];
    rose_argv[3] = NULL;
    SgProject *project = frontend(3, rose_argv);

    // Find the address of "main" and "updcrc" functions from the symbol table of the primary executable (the one we just
    // parsed).
    rose_addr_t main_va = RSIM_Tools::FunctionFinder().address(project, "main");
    assert(main_va!=0);
    rose_addr_t updcrc_va = RSIM_Tools::FunctionFinder().address(project, "updcrc");
    assert(updcrc_va!=0);

    // Register the analysis callback.
    sim.install_callback(new Analysis(main_va, updcrc_va));

    // The rest is the usual stuff needed to run the simulator.
    sim.install_callback(new RSIM_Tools::UnhandledInstruction);
    sim.exec(argc-n, argv+n);
    sim.activate();
    sim.main_loop();
    sim.deactivate();
    sim.describe_termination(stderr);
    sim.terminate_self(); // probably doesn't return
    return 0;
}





#else
int main(int, char *argv[])
{
    std::cerr <<argv[0] <<": not supported on this platform" <<std::endl;
    return 0;
}

#endif /* ROSE_ENABLE_SIMULATOR */
