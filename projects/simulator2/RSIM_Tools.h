#ifndef ROSE_RSIM_Tools_H
#define ROSE_RSIM_Tools_H

#include "stringify.h"          // Needed by the MemoryAccessWatcher tool
#include <Disassembler.h>

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

/** A collection of useful tools.
 *
 *  Most of these tools are implemented as callbacks.  There's nothing special about any of the tools -- you could look at the
 *  source code and write similar tools as classes within your own source files, which are then registered as RSIM callbacks. */
namespace RSIM_Tools {

/** Pauses at process fork.
 *
 *  When a process forks, this callback is invoked first for the process performing the fork, and then for the new process
 *  created by the fork.  Whether the callback is on the BEFORE or AFTER list determines whether it's called by the parent
 *  and/or child process.  The affected process creates a named fifo and blocks until some other process writes something to
 *  that fifo.  The fifo is then closed, removed from the file system, and the specimen calling thread is allowed to continue.
 *
 *  For instance, to cause the specimen to stop in all newly forked processes, the callback should be registered on the AFTER
 *  list only, like this:
 *
 *  @code
 *  RSIM_Linux32 *sim = ...;
 *  RSIM_Tools::ForkPauser pause;
 *  sim.get_callbacks().add_process_callback(RSIM_Callbacks::AFTER, &pause);
 *  @endcode
 */
class ForkPauser: public RSIM_Callbacks::ProcessCallback {
public:
    bool pause_on_fork;

    ForkPauser(): pause_on_fork(true) {}

    virtual ForkPauser *clone() { return this; }

    virtual bool operator()(bool enabled, const Args &args) {
        if (enabled && args.reason==FORK) {
            std::cerr <<"ForkPauser: pid=" <<getpid();
            if (pause_on_fork) {
                std::string filename("/tmp/paused_"); filename += Rose::StringUtility::numberToString(getpid());
                int status __attribute__((unused)) = mkfifo(filename.c_str(), 0666);
                assert(status>=0);
                std::cerr <<"; paused -- say \"echo >" <<filename <<"\" to resume...";
                int fd = open(filename.c_str(), O_RDONLY);
                assert(fd>=0);
                char buf;
                read(fd, &buf, 1);
                std::cerr <<" resumed";
                close(fd);
                unlink(filename.c_str());
            }
            std::cerr <<std::endl;
        }
        return enabled;
    }
};

/** Traverses the AST to find a symbol for a global function with the specified name.
 *
 *  This class operates over an AST and is not a callback like most of the other classes in this collection of tools.  The
 *  simulator doesn't have an AST available by default, so one must be created by parsing the executable file.  Once the AST is
 *  obtained, FunctionFinder traverses the symbol table (not debugging information) to find a global, defined function with the
 *  specified name and returns its starting address.
 *
 *  Example:
 *  @code
 *    int main(int argc, char *argv[], char *envp[]) {
 *       // Create the simulator and parse arguments
 *       RSIM_Linux32 sim;
 *       int n = sim.configure(argc, argv, envp);
 *
 *       // Parse the ELF container (no disassembly)
 *       char *rose_argv[4];
 *       rose_argv[0] = argv[0];
 *       rose_argv[1] = strdup("-rose:read_executable_file_format_only");
 *       rose_argv[2] = argv[n];
 *       rose_argv[3] = NULL;
 *       SgProject *project = frontend(3, rose_argv);
 *
 *       // Find the addresses of "main" and "printf"
 *       rose_addr_t f1_va = FunctionFinder().address(project, "main");
 *       rose_addr_t f2_va = FunctionFinder().address(project, "printf");
 *  @endcode
 */
class FunctionFinder {
private:
    struct T1: public AstSimpleProcessing {
        std::string fname;          /* Holds address() function for use during traversal. */
        T1(const std::string &fname): fname(fname) {}
        void visit(SgNode *node) {
            SgAsmElfSymbol *sym = isSgAsmElfSymbol(node);
            if (sym &&
                sym->get_def_state() == SgAsmGenericSymbol::SYM_DEFINED &&
                sym->get_binding()   == SgAsmGenericSymbol::SYM_GLOBAL &&
                sym->get_type()      == SgAsmGenericSymbol::SYM_FUNC &&
                sym->get_name()->get_string() == fname)
                throw sym->get_value();
        }
    };

public:
    /** Search for a function.  Searches for a function named @p fname in the specified @p ast and returns its entry
     *  address. The function must be a global, defined symbol.  If the function cannot be found, then the null address is
     *  returned. */
    rose_addr_t address(SgNode *ast, const std::string &fname) {
        try {
            T1(fname).traverse(ast, preorder);
        } catch (rose_addr_t addr) {
            return addr;
        }
        return 0;
    }

    /** Search for a function.  Searches for a function named @p fname in all of the specified binary headers and returns the
     *  first one found.  The function must be a global, defined symbol.  If the function cannot be found under any header,
     *  then the null address is returned.  This function is intended to be called with RSIM_Process::headers() as the
     *  argument, which is why the vector uses SgAsmGenericHeader instead of SgNode. */
    rose_addr_t address(const std::vector<SgAsmGenericHeader*> &headers, const std::string &fname) {
        for (std::vector<SgAsmGenericHeader*>::const_iterator hi=headers.begin(); hi!=headers.end(); ++hi) {
            rose_addr_t addr = address(*hi, fname);
            if (addr!=0)
                return addr;
        }
        return 0;
    }
};

/** Shows addresses and names of all known functions.
 *
 *  When a particular execution address is hit for the first time, this callback prints information about all known functions.
 *  Note that functions that were loaded by simulating the dynamic linker will probably not have meaningful names because ROSE
 *  will not know about the ELF files of the shared objects -- they would have been mapped into specimen memory by the dynamic
 *  linker rather than the simulator itself.
 *
 *  This callback only does something if the disassembler has been run and the triggering instruction is one of the
 *  instructions that was disassembled.  It is permissible to run this callback on the same instruction that triggered the
 *  disassembler as long as the disassembler happens before this callback.
 *
 *  All output is to the TRACE_MISC facility.
 *
 *  Example usage:
 *  @code
 *    RSIM_Linux32 sim;
 *    rose_addr_t main_va = ....; // see FunctionFinder
 *    sim.install_callback(new MemoryDisassembler(main_va);
 *    sim.install_callback(new FunctionIndex(main_va));
 *  @endcode
 *
 *  Example output:
 *  @verbatim
32283:1 1.684 0x08048462[119574]:  FunctionIndex: triggered. Showing all functions in the AST rooted at (SgAsmBlock*)0x310fbb0
32283:1 1.684 0x08048462[119574]:      Key for reason(s) address is a suspected function:
32283:1 1.684 0x08048462[119574]:        E = entry address         C = function call(*)      X = exception frame
32283:1 1.684 0x08048462[119574]:        S = function symbol       P = instruction pattern   G = interblock branch graph
32283:1 1.684 0x08048462[119574]:        U = user-def detection    N = NOP/Zero padding      D = discontiguous blocks
32283:1 1.684 0x08048462[119574]:        H = insn sequence head    I = imported/dyn-linked   L = leftover blocks
32283:1 1.684 0x08048462[119574]:      Note: "c" means this is the target of a call-like instruction or instruction sequence but
32283:1 1.684 0x08048462[119574]:            the sequence is not present in the set of nodes of the control flow graph.
32283:1 1.684 0x08048462[119574]:      Note: Functions detected in memory that's not part of the executable loaded by the simulator
32283:1 1.684 0x08048462[119574]:            will probably not have names because the simulator never parsed those ELF containers.
32283:1 1.684 0x08048462[119574]:  
32283:1 1.684 0x08048462[119574]:      Num  Low-Addr   End-Addr  Insns/Bytes  Reason      Name or memory region
32283:1 1.684 0x08048462[119574]:      --- ---------- ---------- ------------ ----------- ------------------------
32283:1 1.685 0x08048462[119574]:        1 0x080482e4 0x08048314    17/48     .C..S...... _init
32283:1 2.024 0x08048462[119574]:        3 0x08048324 0x0804832a     1/6      .C......... __gmon_start__@plt
32283:1 2.024 0x08048462[119574]:        4 0x08048334 0x0804833a     1/6      .C......... write@plt
32283:1 2.024 0x08048462[119574]:        5 0x08048344 0x0804834a     1/6      .C......... __libc_start_main@plt
32283:1 2.024 0x08048462[119574]:        6 0x08048354 0x0804835a     1/6      .C......... mprotect@plt
32283:1 2.025 0x08048462[119574]:        7 0x08048380 0x080483a1    13/33     E...S...... _start
32283:1 2.025 0x08048462[119574]:        8 0x080483b0 0x08048405    25/85     .C..S...... __do_global_dtors_aux
32283:1 2.025 0x08048462[119574]:        9 0x08048410 0x08048433    11/26     .C..S....D. frame_dummy
32283:1 2.026 0x08048462[119574]:       10 0x08048433 0x08048434     1/1      ........N.. in demo5input(LOAD#2)
32283:1 2.026 0x08048462[119574]:       11 0x08048434 0x08048456     7/34     ....S...... payload (0x8048440)
32283:1 2.026 0x08048462[119574]:       12 0x08048462 0x0804848c    10/42     ....S...... main
32283:1 2.026 0x08048462[119574]:       13 0x080484c0 0x080484c5     4/5      ....S...... __libc_csu_fini
32283:1 2.026 0x08048462[119574]:       14 0x080484d0 0x0804852a    21/54     ....S....D. __libc_csu_init
32283:1 2.027 0x08048462[119574]:       15 0x0804852a 0x0804852e     2/4      .C..S...... __i686.get_pc_thunk.bx
32283:1 2.027 0x08048462[119574]:       16 0x0804852e 0x08048530     2/2      ........N.. in demo5input(LOAD#2)
32283:1 2.027 0x08048462[119574]:       17 0x08048530 0x0804855a    18/42     .C..S...... __do_global_ctors_aux
32283:1 2.027 0x08048462[119574]:       18 0x0804855a 0x0804855c     2/2      ........N.. in demo5input(LOAD#2)
32283:1 2.027 0x08048462[119574]:       19 0x0804855c 0x08048578    12/28     ....S...... _fini
32283:1 2.027 0x08048462[119574]:       20 0x40000840 0x40000844     2/4      .C......... in ld-linux.so.2(LOAD#0)
32283:1 2.028 0x08048462[119574]:       21 0x40000850 0x4000089a    24/74     E.......... in ld-linux.so.2(LOAD#0)
32283:1 2.028 0x08048462[119574]:       22 0x400009c0 0x40000a0b    19/75     .C......... in ld-linux.so.2(LOAD#0)
32283:1 2.037 0x08048462[119574]:       23 0x40000fa0 0x40001522   351/1396   .C.......D. in ld-linux.so.2(LOAD#0)
32283:1 2.039 0x08048462[119574]:       24 0x400090b0 0x4000917e    57/206    .C......... in ld-linux.so.2(LOAD#0)
32283:1 2.041 0x08048462[119574]:       25 0x4000e670 0x4000e790    89/281    .C.......D. in ld-linux.so.2(LOAD#0)
32283:1 2.044 0x08048462[119574]:       26 0x4000e790 0x4000e8c3    91/300    .C.......D. in ld-linux.so.2(LOAD#0)
32283:1 2.069 0x08048462[119574]:      --- ---------- ---------- ------------ ----------- ------------------------
@endverbatim
 */
class FunctionIndex: public RSIM_Callbacks::InsnCallback {
protected:
    rose_addr_t when;                   /**< Address at which to trigger this callback. */
    bool triggered;                     /**< Set to true once this callback has been triggered. */

public:
    /** Constructor that takes an instruction address.  The callback is triggered the first time any thread executes an
     * instruction at the specified address. */
    explicit FunctionIndex(rose_addr_t when): when(when), triggered(false) {}

    virtual FunctionIndex *clone() { return this; }

    virtual bool operator()(bool enabled, const Args &args) {
        using namespace Rose::Diagnostics;
        if (enabled && !triggered && args.insn->get_address()==when) {
            triggered = true;
            Sawyer::Message::Stream &m = args.thread->tracing(TRACE_MISC);

            /* Traversal just prints information about each individual function. */
            struct T1: public AstSimpleProcessing {
                RSIM_Process *process;
                Sawyer::Message::Stream &m;
                size_t nfuncs;
                explicit T1(RSIM_Process *process, Sawyer::Message::Stream &m): process(process), m(m), nfuncs(0) {}
                void visit(SgNode *node) {
                    SgAsmFunction *defn = isSgAsmFunction(node);
                    if (defn!=NULL) {
                        /* Scan through the function's instructions to find the range of addresses for the function. */
                        rose_addr_t func_start=~(rose_addr_t)0, func_end=0;
                        size_t nbytes=0;
                        std::vector<SgAsmInstruction*> insns = SageInterface::querySubTree<SgAsmInstruction>(defn);
                        for (std::vector<SgAsmInstruction*>::iterator ii=insns.begin(); ii!=insns.end(); ++ii) {
                            SgAsmInstruction *insn = *ii;
                            func_start = std::min(func_start, insn->get_address());
                            func_end = std::max(func_end, insn->get_address()+insn->get_size());
                            nbytes += insn->get_size();
                        }

                        /* Compute name string */
                        std::string name = defn->get_name();
                        if (name.empty()) {
                            SAWYER_THREAD_TRAITS::RecursiveLockGuard lock(process->rwlock());
                            if (process->get_memory()->at(defn->get_entry_va()).exists()) {
                                const Rose::BinaryAnalysis::MemoryMap::Segment &sgmt =
                                    process->get_memory()->find(defn->get_entry_va())->value();
                                if (!sgmt.name().empty())
                                    name = "in " + sgmt.name();
                            }
                        }
                        if (defn->get_entry_va()!=func_start)
                            name += " (" + Rose::StringUtility::addrToString(defn->get_entry_va()) + ")";

                        /* Print the whole line at once */
                        mfprintf(m)("    %3zu 0x%08" PRIx64" 0x%08" PRIx64" %5zu/%-6zu %s %s\n",
                                    ++nfuncs, func_start, func_end, insns.size(), nbytes,
                                    defn->reason_str(true).c_str(), name.c_str());

                    }
                }
            } t1(args.thread->get_process(), m);

            /* If we just ran the disassembler on at this same instruction address, then args.insn is still pointing to the
             * originally fetched instruction and probably has no parent (or it doesn't belong to the same AST as the recently
             * disassembled instructions).  We need to get the fresh instruction, so we fetch it again, then follow the parent
             * points until we reach the parent of all the recently disassembled instructions (probably an SgAsmBlock). We then
             * traverse the AST from the top looking for and printing names of all functions. */
            SgNode *top = args.thread->get_process()->get_instruction(when);
            assert(top!=NULL);
            while (top->get_parent()) top = top->get_parent();
            mfprintf(m)("FunctionIndex triggered: showing all functions in the AST rooted at (%s*)%p\n",
                        top?Rose::stringifyVariantT(top->variantT(), "V_").c_str() : "void", top);
            m <<"    Key for reason(s) address is a suspected function:\n";
            m <<"      E = entry address         C = function call(*)      X = exception frame\n";
            m <<"      S = function symbol       P = instruction pattern   G = interblock branch graph\n";
            m <<"      U = user-def detection    N = NOP/Zero padding      D = discontiguous blocks\n";
            m <<"      H = insn sequence head    I = imported/dyn-linked   L = leftover blocks\n";
            m <<"    Note: \"c\" means this is the target of a call-like instruction or instruction sequence but\n";
            m <<"          the sequence is not present in the set of nodes of the control flow graph.\n";
            m <<"    Note: Functions detected in memory that's not part of the executable loaded by the simulator\n";
            m <<"          will probably not have names because the simulator never parsed those ELF containers.\n";
            m <<"\n";
            m <<"    Num  Low-Addr   End-Addr  Insns/Bytes  Reason      Name or memory region\n";
            m <<"    --- ---------- ---------- ------------ ----------- ------------------------\n";
            t1.traverse(top, preorder);
            m <<"    --- ---------- ---------- ------------ ----------- ------------------------\n";
        }
        return enabled;
    }
};


/** Prints the name of the currently executing function.
 *
 *  This instruction callback looks at the current instruction's AST ancestors to find an enclosing SgAsmFunction
 *  node. If the current instruction's function is different than the previous instruction's, then we print either the current
 *  function name or a full stack trace to the TRACE_MISC facility.
 *
 *  Two things must exist for this callback to work:  First the instructions must have been linked into the AST.  Normally the
 *  simulator's instruction fetching returns raw instructions and makes no attempt to discover how they're organized into basic
 *  blocks and functions.  The easiest way to get AST-linked instructions is to use the MemoryDisassembler class defined in
 *  this same header file.  The second requirement is that the TRACE_MISC facility is enabled, which can be done by configuring
 *  the simulator with the "--debug" switch.
 *
 *  The best time to disassemble is when the dynamic linker has finished running.  One can use the FunctionFinder class to find
 *  the address of main() and use that as the disassembly trigger.  Alternatively, one could write a callback that looks at the
 *  current instruction and if that instruction has no parent (see SgNode::get_parent()) then invoke
 *  RSIM_Process::disassemble().
 *
 *  Example:  Assuming that we wish to disassemble when we hit main(), and the address of main() has been determined to be
 *  0x401eb7, here's how one would use the FunctionReporter callback:
 *  @code
 *    RSIM_Linux32 sim;
 *    sim.install_callback(new MemoryDisassembler(0x401eb7, false));
 *    sim.install_callback(new FunctionReporter);
 *  @endcode
 */
class FunctionReporter: public RSIM_Callbacks::InsnCallback {
public:
    bool show_call_stack;                       /**< Show a stack trace rather than just a function name. */
    explicit FunctionReporter(bool show_call_stack=false)
        : show_call_stack(show_call_stack) {}

    virtual FunctionReporter *clone() { return this; }

    virtual bool operator()(bool enabled, const Args &args) {
        using namespace Rose::Diagnostics;
        RSIM_Process *process = args.thread->get_process();
        SgAsmBlock *basic_block = isSgAsmBlock(args.insn->get_parent());
        SgAsmFunction *func = basic_block ? basic_block->get_enclosing_function() : NULL;
        std::string new_name = func ? func->get_name() : "";
        if (new_name!=name) {
            name = new_name;
            if (show_call_stack) {
                /* Check for the case when we recently executed a CALL instruction, but the called function has not yet pushed
                 * the old EBP register onto the stack.  In this case, the word at ss:[esp] will be two or five bytes past the
                 * address of a CALL instruction in executable memory.  This only handles CALLs encoded in two or five
                 * bytes. */
                bool bp_not_pushed = false;
                Rose::BinaryAnalysis::RegisterDescriptor REG_SP = args.thread->get_process()->disassembler()->stackPointerRegister();
                uint64_t sp = args.thread->operators()->readRegister(REG_SP)->get_number();
                uint64_t top = 0;
                bool isTopValid = false;
                SgAsmX86Instruction *call_insn = NULL;
                try {
                    if (REG_SP.nBits() == 32) {
                        uint32_t top32 = 0;
                        isTopValid = 4 == process->mem_read(&top32, sp, 4);
                        top = top32;
                    } else {
                        ASSERT_require(REG_SP.nBits() == 64);
                        isTopValid = 8 == process->mem_read(&top, sp, 8);
                    }
                    if (isTopValid) {
                        if (NULL!=(call_insn=isSgAsmX86Instruction(process->get_instruction(top-5))) &&
                            (Rose::BinaryAnalysis::x86_call==call_insn->get_kind() ||
                             Rose::BinaryAnalysis::x86_farcall==call_insn->get_kind())) {
                            bp_not_pushed = true;
                        } else if (NULL!=(call_insn=isSgAsmX86Instruction(process->get_instruction(top-2))) &&
                                   (Rose::BinaryAnalysis::x86_call==call_insn->get_kind() ||
                                    Rose::BinaryAnalysis::x86_farcall==call_insn->get_kind())) {
                            bp_not_pushed = true;
                        } else if (NULL!=(call_insn=isSgAsmX86Instruction(process->get_instruction(top-6))) &&
                                   (Rose::BinaryAnalysis::x86_call==call_insn->get_kind() ||
                                    Rose::BinaryAnalysis::x86_farcall==call_insn->get_kind())) {
                            bp_not_pushed = true;
                        }
                    }
                } catch (const Rose::BinaryAnalysis::Disassembler::Exception&) {
                    /* ignored -- it just means the top of stack probably doesn't even point to executable memory */
                }
                args.thread->report_stack_frames(args.thread->tracing(TRACE_MISC), "FunctionReporter: stack frames",
                                                 bp_not_pushed);
            } else if (name.empty()) {
                args.thread->tracing(TRACE_MISC) <<"FunctionReporter: in unknown function\n";
            } else {
                mfprintf(args.thread->tracing(TRACE_MISC))("FunctionReporter: in function \"%s\"\n", name.c_str());
            }
        }
        return enabled;
    }

private:
    std::string name;
};

/** Watches for memory access.
 *
 *  This memory callback watches for access to certain memory locations and prints a message to the specified facility when
 *  such an access occurs.  An access need not change the value of the memory location in order to be reported.  This callback
 *  is triggered if the operation matches one of the @p how bits and the required memory region protection bits match at least
 *  one of the specified @p req_perms bits.
 *
 *  Memory locations can be arbitrarily large since this class, unlike MemoryChecker, doesn't actually allocate any backing
 *  store, but rather just looks at the memory addresses.  Also, the addresses to watch do not need to be mapped to the process
 *  yet--the memory access watcher can detect access to addresses that would cause a segmentation fault.
 *
 *  A message facility must be specified (or defaults to standard error) because memory callbacks are not associated with any
 *  particular thread.  Likewise, at this time there's no way to obtain information about the thead that triggered the memory
 *  operation.  We hope to remedy this in the future.  You might be able to use MemoryChecker if you need thread information,
 *  although that callback is more expensive and will only detect writes operations.
 *
 *  See also, MemoryChecker, which checks the contents of memory after every instruction; and the "--debug=mem" switch which
 *  prints all memory access and the values which are copied to/from memory.
 *
 *  Example:  Here's how to use a memory watcher to monitor reads and writes to the first page of memory, which are indicative
 *  of dereferencing a null pointer.
 *  @code
 *    RSIM_Linux32 sim;
 *    unsigned operations = MemoryMap::MM_PROT_READ | MemoryMap::MM_PROT_WRITE;
 *    unsigned req_perms = MemoryMap::MM_PROT_ANY; //read, write, or execute
 *    sim.install_callback(new MemoryAccessWatcher(0, 4096, operations, req_perms, mlog[INFO]);
 *  @endcode
 *
 *  Example: Here's how to use a memory watch to detect when instructions are executed on the stack, which might indicate a
 *  buffer overflow attack.
 *  @code
 *    RSIM_Linux32 sim;
 *    unsigned operations = MemoryMap::MM_PROT_READ;
 *    unsigned req_perms = MemoryMap::MM_PROT_EXEC;
 *    rose_addr_t stack_base = 0xbffeb000;
 *    rose_addr_t stack_size = 0x00015000;
 *    sim.install_callback(new MemoryAccessWatcher(stack_base, stack_size, operations, req_perms);
 *  @endcode
 */
class MemoryAccessWatcher: public RSIM_Callbacks::MemoryCallback {
public:
    Sawyer::Message::Stream &mesg;              /**< Tracing facility, since no thread is available. */
    rose_addr_t va;                             /**< Starting address for watched memory region. */
    size_t nbytes;                              /**< Size of watched memory region. */
    unsigned how;                               /**< What kind of operations we are watching. This should be the bits
                                                 *   MM_PROT_READ and/or MM_PROT_WRITE from MemoryMap::Protection. */
    unsigned req_perms;                         /**< What requested protection bits trigger the MemoryAccessWatcher.  This bit
                                                 *   vector is compared with the req_perms argument to RSIM_Process::mem_read()
                                                 *   or RSIM_Process::mem_write().  If the intersection is empty then this
                                                 *   callback will not be triggered. */

    MemoryAccessWatcher(rose_addr_t va, size_t nbytes, unsigned how, unsigned req_perms, Sawyer::Message::Stream &mesg)
        : mesg(mesg), va(va), nbytes(nbytes), how(how), req_perms(req_perms) {}

    virtual MemoryAccessWatcher *clone() { return this; }

    virtual bool operator()(bool enabled, const Args &args) {
        using namespace Rose::Diagnostics;
        if (enabled && 0!=(args.how & how) && 0!=(args.req_perms & req_perms) && args.va<va+nbytes && args.va+args.nbytes>=va) {
            std::string operation = args.how == Rose::BinaryAnalysis::MemoryMap::READABLE ? "READ" : "WRITE";
            for (size_t i=0; i<operation.size(); i++)
                operation[i] = tolower(operation[i]);
            mfprintf(mesg)("MemoryAccessWatcher: triggered for %s access at 0x%08" PRIx64" for %zu byte%s\n",
                           operation.c_str(), args.va, args.nbytes, 1==args.nbytes?"":"s");
        }
        return enabled;
    }
};

/** Checks whether specimen memory matches a known value.
 *
 *  This instruction callback reads from the specified memory area and verifies that the conetnts of memory at that location
 *  match the expected value.  If not, a message is printed to the TRACE_MISC facility and the callback is disabled.
 *
 *  See also, MemoryAccessWatcher, which is a memory callback to look for memory access; and the "--debug=mem" switch which
 *  prints all memory access and the values which are copied to/from memory.
 *
 *  Here's an example of how to use this tool:
 *  @code
 *  uint8_t valid_mem = {0x00, 0x00, 0x11, 0x00, 0x00, 0x00, 0x00, 0xef,
 *                       0x67, 0x00, 0x00, 0x00, 0x7f, 0x00, 0x00, 0x03};
 *  MemoryChecker mcheck(0x7c402740, sizeof valid_mem, valid_mem, true);
 *  simulator.install_callback(&mcheck);
 *  @endcode
 */
class MemoryChecker: public RSIM_Callbacks::InsnCallback {
    uint8_t *buffer;
public:
    rose_addr_t va;                             /**< Starting address of memory to check. */
    size_t nbytes;                              /**< Number of bytes to check. */
    uint8_t *answer;                            /**< Valid memory values to check against. User-supplied buffer. */
    bool report_short;                          /**< Treat short read as a difference. */
    bool update_answer;                         /**< If true, update answer when memory changes, and re-arm. */
    bool show_stack_frames;                     /**< If true, show stack frames when memory changes. */
    bool armed;                                 /**< Is this callback armed? */

    MemoryChecker(rose_addr_t va, size_t nbytes, uint8_t *answer, bool update_answer)
        : va(va), nbytes(nbytes), answer(answer), report_short(false), update_answer(update_answer),
          show_stack_frames(true), armed(true) {
        buffer = new uint8_t[nbytes];
    }

    ~MemoryChecker() {
        delete[] buffer;
    }

    virtual MemoryChecker *clone() { return this; }

    virtual bool operator()(bool enabled, const Args &args) {
        using namespace Rose::Diagnostics;
        if (armed) {
            size_t nread = args.thread->get_process()->mem_read(buffer, va, nbytes);
            if (nread<nbytes && report_short) {
                mfprintf(args.thread->tracing(TRACE_MISC))("MemoryChecker: read failed at 0x%08" PRIx64"\n", va+nread);
                armed = false;
            } else {
                for (size_t i=0; i<nread && i<nbytes; i++) {
                    if (answer[i]!=buffer[i]) {
                        mfprintf(args.thread->tracing(TRACE_MISC))("MemoryChecker: memory changed at 0x%08" PRIx64
                                                                   " from 0x%02x to 0x%02x", va+i, answer[i], buffer[i]);
                        if (show_stack_frames)
                            args.thread->report_stack_frames(args.thread->tracing(TRACE_MISC));
                        if (update_answer) {
                            memcpy(answer, buffer, std::min(nread, nbytes));
                        } else {
                            armed = false;
                        }
                        break;
                    }
                }
            }
        }
        return enabled;
    }
};

/** Disassemble memory when an address is executed.
 *
 *  Runs the disassembler the first time we hit the specified execution address.  A good place to run the disassembler is at
 *  the start of main(), after the dynamic linker has had a chance to load and link all shared libraries.  See FunctionFinder
 *  for an example of how to obtain the address of a function.
 *
 *  Example:
 *  @code
 *    RSIM_Linux32 sim;
 *    rose_addr_t main_va = ....; // See FunctionFinder
 *    sim.install_callback(new MemoryDisassembler(main_va, false);
 *  @endcode
 */
class MemoryDisassembler: public RSIM_Callbacks::InsnCallback {
public:
    rose_addr_t when;                   /**< IP value when this callback is to be triggered. */
    bool triggered;                     /**< Set once this callback has been triggered. */
    bool show;                          /**< Controls whether result is emitted. If true, then emit the results to stdout. */

    /** Constructor.  The disassembler is triggered the first time that an instruction at address @p when is simulated.  If @p
     *  show is true then the assembly listing is produced on standard output.  All disassembled instructions are added to the
     *  instruction cache used by the simulator's instruction fetching methods. */
    explicit MemoryDisassembler(rose_addr_t when, bool show=false)
        : when(when), triggered(false), show(show) {}

    virtual MemoryDisassembler *clone() { return this; }

    virtual bool operator()(bool enabled, const Args &args) {
        if (enabled && !triggered && args.insn->get_address()==when) {
            triggered = true;
            Sawyer::Message::Stream &m = args.thread->tracing(TRACE_MISC);
            m <<"MemoryDisassembler triggered: disassembling now...\n";
            SgAsmBlock *block = args.thread->get_process()->disassemble();
            if (show)
                Rose::BinaryAnalysis::AsmUnparser().unparse(std::cout, block);
        }
        return enabled;
    }
};

/** Show memory contents.
 *
 *  This instruction callback prints the contents of a specified memory region every time a certain instruction address is
 *  reached. The output is a hexdump-like format controlled by the public HexdumpFormat fmt data member.  Output is sent to the
 *  TRACE_MISC facility.
 *
 *  Note: SgAsmExecutableFileFormat isn't able to write directly to an std::ostream such as Sawyer::Message::Stream. Therefore
 *  we buffer up the output from hexdump before sending it to the tracing facility.  In other words, don't try to print huge
 *  memory traces.
 *
 *  See also MemoryChecker and the "--debug=mem" switch.
 *
 *  Example:  Here's how one would set a breakpoint at address 0x0804660d to print the contents of an 80-byte variable at
 *  address @p var_va:
 *  @code
 *    RSIM_Linux32 sim;
 *    rose_addr_t var_va = ...;
 *    sim.install_callback(new MemoryDumper(0x0804660d, var_va, 80);
 *  @endcode
 */
class MemoryDumper: public RSIM_Callbacks::InsnCallback {
public:
    rose_addr_t when;                   /**< IP value when this callback is to be triggered. */
    rose_addr_t va;                     /**< Starting address to dump. */
    size_t nbytes;                      /**< Number of bytes to dump. */
    bool do_binary;                     /**< If true, then dump binary data, otherwise format with 'fmt'. */
    HexdumpFormat fmt;                  /**< Format to use for hexdump. */
    std::string filename;               /**< Optional output file.  If empty, output is sent to the TRACE_MISC facility. */


    /** Constructor.  Creates an instruction callback that will dump @p nbytes bytes of memory starting at address @p va every
     *  time the simulator is about to execute an instruction at address @p when.  If a file name is specified, then data will
     *  be dumped in binary format to the specified file, otherwise the data is dumped in hexdump format to the TRACE_MISC
     *  facility. */
    MemoryDumper(rose_addr_t when, rose_addr_t va, size_t nbytes, const std::string filename="")
        : when(when), va(va), nbytes(nbytes), do_binary(!filename.empty()), filename(filename) {
        fmt.prefix = "  ";
        fmt.multiline = true;
    }

    virtual MemoryDumper *clone() { return this; }

    virtual bool operator()(bool enabled, const Args &args) {
        using namespace Rose::Diagnostics;
        if (enabled && args.insn->get_address()==when) {
            Sawyer::Message::Stream m(args.thread->tracing(TRACE_MISC));
            m <<"MemoryDumper triggered: dumping " <<Rose::StringUtility::plural(nbytes, "bytse")
              <<" at " <<Rose::StringUtility::addrToString(va) <<"\n";
            if (do_binary) {
                /* Raw output to a file */
                assert(!filename.empty());
                int fd = open(filename.c_str(), O_TRUNC|O_CREAT|O_RDWR, 0666);
                if (fd<=0) {
                    mfprintf(m)("MemoryDumper: %s: %s\n", filename.c_str(), strerror(errno));
                    goto error;
                }
                uint8_t buffer[4096];
                rose_addr_t va = this->va;
                size_t nbytes = this->nbytes;
                while (nbytes>0) {
                    size_t to_read = std::min(nbytes, sizeof buffer);
                    size_t nread = args.thread->get_process()->mem_read(buffer, va, to_read);
                    for (size_t nwrite=0; nwrite<nread; /*void*/) {
                        ssize_t n = write(fd, buffer+nwrite, nread-nwrite);
                        if (n<0) {
                            m <<"MemoryDumper: write failed\n";
                            close(fd);
                            goto error;
                        }
                        nwrite += n;
                    }
                    if (nread<to_read) {
                        mfprintf(m)("MemoryDumper: read failed at 0x%08" PRIx64"\n", va+nread);
                        close(fd);
                        goto error;
                    }
                    nbytes -= nread;
                    va += nread;
                }
                close(fd);
            } else {
                /* Formatted output to a file or trace facility. */
                uint8_t *buffer = new uint8_t[nbytes];
                size_t nread = args.thread->get_process()->mem_read(buffer, va, nbytes);
                int fd = -1;
                if (nread < nbytes)
                    mfprintf(m)("MemoryDumper: read failed at 0x%08" PRIx64"\n", va+nread);
                std::string s = SgAsmExecutableFileFormat::hexdump(va, buffer, nread, fmt);
                if (!filename.empty()) {
                    if (-1==(fd=open(filename.c_str(), O_TRUNC|O_CREAT|O_RDWR, 0666))) {
                        mfprintf(m)("MemoryDumper: %s: %s\n", filename.c_str(), strerror(errno));
                        delete[] buffer;
                        goto error;
                    }
                    for (size_t nwrite=0; nwrite<nread; /*void*/) {
                        ssize_t n = write(fd, buffer+nwrite, nread-nwrite);
                        if (n<0) {
                            m <<"MemoryDumper: write failed\n";
                            close(fd);
                            delete[] buffer;
                            goto error;
                        }
                        nwrite += n;
                    }
                    close(fd);
                } else {
                    m <<s;
                }
                delete[] buffer;
            }
        error:
            m <<"\n";
        }
        return enabled;
    }
};

/** Initialize memory.
 *
 *  When a certain instruction address is hit, memory is initialized by reading from a file or using a user-supplied buffer.
 *  This callback is triggered only one time.
 *
 *  The memory which is being written to should have write permission in order for this callback to succeed.  You can try
 *  changing need_write_perm to false, but if the underlying memory (in the simulator) was mapped without write permission
 *  you'll get a segmentation fault.
 *
 *  Example:
 *  @code
 *  InitializeMemory stack_initializer("x.data", 0xbfffe000, 0x7c4000a);
 *  RSIM_Linux32 simulator;
 *  simulator.get_callbacks().add_insn_callback(RSIM_Callbacks::BEFORE, &stack_initializer);
 *  @endcode
 */
class MemoryInitializer: public RSIM_Callbacks::InsnCallback {
public:
    std::string filename;               /**< Optional name of file containing memory image. */
    const uint8_t *new_value;           /**< Optional pointer to bytes to be written. */
    size_t nbytes;                      /**< Number of bytes pointed to by new_value. */
    rose_addr_t memaddr;                /**< Address where file contents are loaded. */
    rose_addr_t when;                   /**< IP value when this callback is to be triggered. */
    bool triggered;                     /**< Set once this callback has been triggered. */
    bool need_write_perm;               /**< Is write permission needed on the memory? */

    MemoryInitializer(const std::string &filename, rose_addr_t memaddr, rose_addr_t when)
        : filename(filename), new_value(NULL), nbytes(0),
          memaddr(memaddr), when(when), triggered(false), need_write_perm(true) {}
    MemoryInitializer(const uint8_t *new_value, size_t nbytes, rose_addr_t memaddr, rose_addr_t when)
        : new_value(new_value), nbytes(nbytes),
          memaddr(memaddr), when(when), triggered(false), need_write_perm(true) {}

    virtual MemoryInitializer *clone() { return this; }

    virtual bool operator()(bool enabled, const Args &args) {
        if (enabled && !triggered && args.insn->get_address()==when) {
            triggered = true;
            size_t total_written=0;
            unsigned perms = need_write_perm ? Rose::BinaryAnalysis::MemoryMap::WRITABLE : 0;

            if (new_value) {
                total_written = args.thread->get_process()->mem_write(new_value, memaddr, nbytes, perms);
                if (total_written!=nbytes)
                    fprintf(stderr, "MemoryInitializer write failed at 0x%08" PRIx64"\n", memaddr+total_written);
            } else {
                int fd = open(filename.c_str(), O_RDONLY);
                if (fd<0) {
                    perror(filename.c_str());
                    return enabled;
                }

                uint8_t buf[4096];
                ssize_t nread;
                rose_addr_t va = memaddr;
                while ((nread=read(fd, buf, sizeof buf))>0) {
                    size_t nwrite = args.thread->get_process()->mem_write(buf, va, (size_t)nread, perms);
                    total_written += nwrite;
                    va += nwrite;
                    if (nwrite!=(size_t)nread) {
                        fprintf(stderr, "MemoryInitializer write failed at 0x%08" PRIx64"\n", va);
                        break;
                    }
                }
                if (nread<0) {
                    close(fd);
                    perror(filename.c_str());
                    return enabled;
                }
                close(fd);
            }
            Sawyer::Message::Stream m(args.thread->tracing(TRACE_MISC));
            m <<"MemoryInitializer triggered: wrote " <<Rose::StringUtility::plural(total_written, "bytes")
              <<" at " <<Rose::StringUtility::addrToString(memaddr) <<"\n";
        }
        return enabled;
    }
};

/** Show memory mapping when an address is executed.
 *
 *  The memory map is the definition of how the specimen's memory regions map into the simulator's memory.  See MemoryMap for
 *  details.  The output is sent to the TRACE_MISC facility.  See also, the "--debug=mmap" switch causes memory dumps to
 *  occur for certain system calls such as "mmap".
 *
 *  Example:  To produce a memory map dump every time the instruction at @p insn_va is executed:
 *  @code
 *    RSIM_Linux32 sim;
 *    sim.install_callback(new MemoryMapDumper(insn_va));
 *  @endcode
 */
class MemoryMapDumper: public RSIM_Callbacks::InsnCallback {
public:
    rose_addr_t when;                   /**< IP value when this callback is to be triggered. */

    /** Constructor.  Causes a memory dump each time the instruction at address @p when is simulated. */
    MemoryMapDumper(rose_addr_t when)
        : when(when) {}

    virtual MemoryMapDumper *clone() { return this; }

    virtual bool operator()(bool enabled, const Args &args) {
        if (enabled && args.insn->get_address()==when)
            args.thread->get_process()->mem_showmap(args.thread->tracing(TRACE_MISC));
        return enabled;
    }
};

/** Prints register contents.
 *
 *  Every time execution hits a specified address, registers are dumped to the TRACE_MISC facility.
 *
 *  See also, the "--debug=state" configuration switch, which prints the contents of registers after every instruction.
 *
 *  Example:  To produce a register dump every time the instruction at @p insn_va is executed:
 *  @code
 *    RSIM_Linux32 sim;
 *    sim.install_callback(new RegisterDumper(insn_va));
 *  @endcode
 */
class RegisterDumper: public RSIM_Callbacks::InsnCallback {
public:
    rose_addr_t when;                   /**< IP value when this callback is to be triggered. */

    /** Constructor.  Causes a register dump each time the instruction at address @p when is simulated. */
    RegisterDumper(rose_addr_t when)
        : when(when) {}

    virtual RegisterDumper *clone() { return this; }

    virtual bool operator()(bool enabled, const Args &args) {
        if (enabled && args.insn->get_address()==when)
            SAWYER_MESG(args.thread->tracing(TRACE_MISC)) <<*args.thread->operators()->currentState()->registerState();
        return enabled;
    }
};

/** Generates a stack trace when a signal arrives.
 *
 *  Each time a signal arrives, a stack trace is printed.  Since stack traces require instructions to be linked into the AST
 *  (specifically, instructions should each have an SgAsmFunction ancestor), the disassembler is invoked the first
 *  time this callback is triggered.
 *
 *  Example:
 *  @code
 *    RSIM_Linux32 sim;
 *    sim.install_callback(new SignalStackTrace);
 *  @endcode
 */
class SignalStackTrace: public RSIM_Callbacks::SignalCallback {
public:
    bool disassembled;                  /**< Controls disassembly.  If clear, then disassemble memory and set. */

    SignalStackTrace()
        : disassembled(false) {}

    virtual SignalStackTrace *clone() { return this; }

    virtual bool operator()(bool enabled, const Args &args) {
        if (args.reason == ARRIVAL) {
            if (!disassembled) {
                args.thread->get_process()->disassemble(); /* so stack trace has function names */
                disassembled = true;
            }
            args.thread->report_stack_frames(args.thread->tracing(TRACE_MISC));
        }
        return enabled;
    }
};

} // namespace
#endif
