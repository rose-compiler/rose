#ifndef ROSE_RSIM_Tools_H
#define ROSE_RSIM_Tools_H

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

/** Show memory mapping when an address is executed. */
class MemoryMapDumper: public RSIM_Callbacks::InsnCallback {
public:
    rose_addr_t when;

    MemoryMapDumper(rose_addr_t when)
        : when(when) {}

    virtual MemoryMapDumper *clone() { return this; }

    virtual bool operator()(bool enabled, const Args &args) {
        if (enabled && args.insn->get_address()==when)
            args.thread->get_process()->mem_showmap(args.thread->tracing(TRACE_MISC));
        return enabled;
    }
};

/** Disassemble memory when an address is executed.
 *
 *  Runs the disassembler the first time we hit the specified execution address. */
class MemoryDisassembler: public RSIM_Callbacks::InsnCallback {
public:
    rose_addr_t when;                   // IP value when this callback is triggered
    bool triggered;                     // set once this callback has been triggered
    bool show;                          // if true, then emit the results to stdout

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
    std::string filename;               // name of file containing memory image
    rose_addr_t memaddr;                // address where file is loaded
    rose_addr_t when;                   // IP value when this callback is triggered
    bool triggered;                     // set once this callback has been triggered
    bool need_write_perm;               // write permission needed?

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

/** Prints register contents.
 *
 *  Every time execution hits a specified address, registers are dumped to the TRACE_MISC facility. */
class RegisterDumper: public RSIM_Callbacks::InsnCallback {
public:
    rose_addr_t when;                   // IP value when this callback is triggered

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
    bool disassembled;

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
