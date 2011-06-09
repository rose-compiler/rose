#ifndef ROSE_RSIM_Tools_H
#define ROSE_RSIM_Tools_H

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

/** Prints the name of the currently executing function.
 *
 *  In order for this to work, instructions must be assigned to functions.  This can be done by the MemoryDisassembler
 *  tool. The function name is printed to the TRACE_MISC facility whenever the current function changes. */
class FunctionReporter: public RSIM_Callbacks::InsnCallback {
public:
    virtual FunctionReporter *clone() { return this; }

    virtual bool operator()(bool enabled, const Args &args) {
        SgAsmBlock *basic_block = isSgAsmBlock(args.insn->get_parent());
        SgAsmFunctionDeclaration *func = basic_block ? basic_block->get_enclosing_function() : NULL;
        std::string new_name = func ? func->get_name() : "";
        if (new_name!=name) {
            name = new_name;
            if (name.empty()) {
                args.thread->tracing(TRACE_MISC)->mesg("in unknown function");
            } else {
                args.thread->tracing(TRACE_MISC)->mesg("in function \"%s\"", name.c_str());
            }
        }
        return enabled;
    }

private:
    std::string name;
};


/** Checks whether specimen memory matches a known value.
 *
 *  This instruction callback reads from the specified memory area and verifies that the conetnts of memory at that location
 *  match the expected value.  If not, a message is printed to the TRACE_MISC facility and the callback is disabled.
 *
 *  Here's an example of how to use this tool:
 *  @code
 *  uint8_t valid_mem = {0x00, 0x00, 0x11, 0x00, 0x00, 0x00, 0x00, 0xef,
 *                       0x67, 0x00, 0x00, 0x00, 0x7f, 0x00, 0x00, 0x03};
 *  MemoryChecker mcheck(0x7c402740, sizeof valid_mem, valid_mem);
 *  simulator.install_callback(&mcheck);
 *  @endcode
 */
class MemoryChecker: public RSIM_Callbacks::InsnCallback {
    uint8_t *buffer;
public:
    rose_addr_t va;                             /**< Starting address of memory to check. */
    size_t nbytes;                              /**< Number of bytes to check. */
    const uint8_t *answer;                      /**< Valid memory values to check against. */
    bool report_short;                          /**< Treat short read as a difference. */
    bool triggered;                             /**< Set once the checker has been triggered. */

    MemoryChecker(rose_addr_t va, size_t nbytes, const uint8_t *answer)
        : va(va), nbytes(nbytes), answer(answer), report_short(false), triggered(false) {
        buffer = new uint8_t[nbytes];
    }

    ~MemoryChecker() {
        delete[] buffer;
    }

    virtual MemoryChecker *clone() { return this; }

    virtual bool operator()(bool enabled, const Args &args) {
        if (!triggered) {
            size_t nread = args.thread->get_process()->mem_read(buffer, va, nbytes);
            if (nread<nbytes && report_short) {
                args.thread->tracing(TRACE_MISC)->mesg("MemoryChecker: read failed at 0x%08"PRIx64, va+nread);
                triggered = true;
            } else {
                for (size_t i=0; i<nread && i<nbytes; i++) {
                    if (answer[i]!=buffer[i]) {
                        args.thread->tracing(TRACE_MISC)->mesg("MemoryChecker: failed at 0x%08"PRIx64, va+i);
                        triggered = true;
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
 *  Runs the disassembler the first time we hit the specified execution address. */
class MemoryDisassembler: public RSIM_Callbacks::InsnCallback {
public:
    rose_addr_t when;                   /**< IP value when this callback is to be triggered. */
    bool triggered;                     /**< Set once this callback has been triggered. */
    bool show;                          /**< Controls whether result is emitted. If true, then emit the results to stdout. */

    MemoryDisassembler(rose_addr_t when, bool show)
        : when(when), triggered(false), show(show) {}

    virtual MemoryDisassembler *clone() { return this; }

    virtual bool operator()(bool enabled, const Args &args) {
        if (enabled && !triggered && args.insn->get_address()==when) {
            triggered = true;
            RTS_Message *m = args.thread->tracing(TRACE_MISC);
            m->mesg("MemoryDisassembler triggered: disassembling now...");
            SgAsmBlock *block = args.thread->get_process()->disassemble();
            if (show)
                AsmUnparser().unparse(std::cout, block);
        }
        return enabled;
    }
};

/** Initialize memory from file.
 *
 *  When a certain instruction address is hit, a file is read into the specimens address space to initialize some of its
 *  memory.  This callback is triggered only one time.
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
    std::string filename;               /**< Name of file containing memory image. */
    rose_addr_t memaddr;                /**< Address where file contents are loaded. */
    rose_addr_t when;                   /**< IP value when this callback is to be triggered. */
    bool triggered;                     /**< Set once this callback has been triggered. */
    bool need_write_perm;               /**< Is write permission needed on the memory? */

    MemoryInitializer(const std::string &filename, rose_addr_t memaddr, rose_addr_t when)
        : filename(filename), memaddr(memaddr), when(when), triggered(false), need_write_perm(true) {}

    virtual MemoryInitializer *clone() { return this; }

    virtual bool operator()(bool enabled, const Args &args) {
        if (enabled && !triggered && args.insn->get_address()==when) {
            triggered = true;
            int fd = open(filename.c_str(), O_RDONLY);
            if (fd<0) {
                perror(filename.c_str());
                return enabled;
            }

            uint8_t buf[4096];
            ssize_t nread;
            size_t total_written=0;
            rose_addr_t va = memaddr;
            while ((nread=read(fd, buf, sizeof buf))>0) {
                unsigned perms = need_write_perm ? MemoryMap::MM_PROT_WRITE : MemoryMap::MM_PROT_NONE;
                size_t nwrite = args.thread->get_process()->mem_write(buf, va, (size_t)nread, perms);
                total_written += nwrite;
                va += nwrite;
                if (nwrite!=(size_t)nread) {
                    fprintf(stderr, "MemoryInitializer write failed at 0x%08"PRIx64"\n", va);
                    break;
                }
            }
            if (nread<0) {
                close(fd);
                perror(filename.c_str());
                return enabled;
            }
            close(fd);

            RTS_Message *m = args.thread->tracing(TRACE_MISC);
            m->mesg("MemoryInitializer triggered: wrote 0x%zx bytes at 0x%08"PRIx64, total_written, memaddr);
        }
        return enabled;
    }
};

/** Show memory mapping when an address is executed. */
class MemoryMapDumper: public RSIM_Callbacks::InsnCallback {
public:
    rose_addr_t when;                   /**< IP value when this callback is to be triggered. */

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
 *  Every time execution hits a specified address, registers are dumped to the TRACE_MISC facility. */
class RegisterDumper: public RSIM_Callbacks::InsnCallback {
public:
    rose_addr_t when;                   /**< IP value when this callback is to be triggered. */

    RegisterDumper(rose_addr_t when)
        : when(when) {}

    virtual RegisterDumper *clone() { return this; }

    virtual bool operator()(bool enabled, const Args &args) {
        if (enabled && args.insn->get_address()==when)
            args.thread->policy.dump_registers(args.thread->tracing(TRACE_MISC));
        return enabled;
    }
};

/** Generates a stack trace when a signal arrives. */
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



#endif
