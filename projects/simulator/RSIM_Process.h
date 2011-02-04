#ifndef ROSE_RSIM_Process_H
#define ROSE_RSIM_Process_H

#include "sage3basic.h"
#include "RSIM_Common.h"

class RSIM_Thread;

/** Represents a single simulated process. The process object holds resources that are shared among its threads. Some of the
 *  properties of a simulated process (such as PID) are shared with the real process (the process running the simulator). */
class RSIM_Process {
public:
    struct SegmentInfo {
        uint32_t base, limit;
        bool present;
        SegmentInfo(): base(0), limit(0), present(false) {}
        SegmentInfo(const user_desc_32 &ud) {
            base = ud.base_addr;
            limit = ud.limit_in_pages ? (ud.limit << 12) | 0xfff : ud.limit;
            present = !ud.seg_not_present && ud.useable;
        }
    };

    RSIM_Process()
        : map(NULL),
          debug(NULL), trace(0), binary_trace(NULL),
          brk_va(0), mmap_start(0x40000000ul), mmap_recycle(false), vdso_mapped_va(0), vdso_entry_va(0),
          core_styles(CORE_ELF), core_base_name("x-core.rose"), ld_linux_base_va(0x40000000) {
        ctor();
    }

    /* Accessors */
    std::string get_exename() const {           /**< Returns the name of the executable without any path components. */
        return exename;
    }
    std::string get_interpname() const {        /**< Returns the interpreter name for dynamically linked executables. */
        return interpname;
    }
    void set_interpname(const std::string &s) {
        interpname = s;
    }
    MemoryMap *get_memory() const {             /**< Returns the memory map for the simulated process. */
        return map;
    }
    FILE *get_debug() const {                   /**< Return file for debugging; null if debugging is disabled. */
        return debug;
    }

    /** Initialize tracing by (re)opening the log file with the specified name pattern.  The pattern should be a printf-style
     *  format with an optional integer specifier for the process ID. */
    void open_log_file(const char *pattern);

    /** Returns a file for tracing, or NULL if tracing is disabled.  The WHAT argument should be a bit vector describing the
     *  tracing facilities in which we're interested (all facilities use the same file).  If tracing is enabled for any of the
     *  specified facilities, then a file pointer is returned; otherwise a null pointer is returned. */
    FILE *tracing(unsigned what) const;

    /** Sets tracing file and facilities. */
    void set_tracing(FILE*, unsigned what);

    /** Loads a new executable image into an existing process. */
    SgAsmGenericHeader *load(const char *name);

    /** Create the main thread. */
    RSIM_Thread *create_thread(pid_t tid);

    /** Generate an ELF Core Dump on behalf of the specimen.  This is a real core dump that can be used with GDB and contains
     *  the same information as if the specimen had been running natively and dumped its own core. In other words, the core
     *  dump we generate here does not have references to the simulator even though it is being dumped by the simulator. */
    void dump_core(int signo, std::string base_name="");

    /** Start an instruction trace file. No-op if "binary_trace" is null. */
    void binary_trace_start();

    /** Add an instruction to the binary trace file. No-op if "binary_trace" is null. */
    void binary_trace_add(RSIM_Thread*, const SgAsmInstruction*);

    /** Returns the thread having the specified thread ID. */
    RSIM_Thread *get_thread(pid_t tid) const;

    /** Returns the total number of instructions processed across all threads. */
    size_t get_ninsns() const;

    void load_segreg_shadow(X86SegmentRegister sr, unsigned gtd_num);
    


private:
    void ctor();                                /**< Constructor */
    std::map<pid_t, RSIM_Thread*> threads;      /**< All threads associated with this process. */

private:
    std::string exename;                        /**< Name of executable without any path components */
    std::string interpname;                     /**< Name of interpreter from ".interp" section or "--interp=" switch */
    MemoryMap *map;                             /**< Describes how specimen's memory is mapped to simulator memory */
    FILE *debug;                                /**< Stream to which debugging output is sent (or NULL to suppress it) */
    unsigned trace;                             /**< Bit vector of what to trace. See TraceFlags. */

public: /* FIXME */
    FILE *binary_trace;                         /**< Stream for binary trace. See projects/traceAnalysis/trace.C for details */
    std::vector<std::string> exeargs;           /**< Specimen argv with PATH-resolved argv[0] */
    Disassembler::InstructionMap icache;        /**< Cache of disassembled instructions */

    uint32_t brk_va;                            /* Current value for brk() syscall; initialized by load() */
    static const size_t n_gdt=8192;             /* Number of global descriptor table entries */
    user_desc_32 gdt[n_gdt];                    /* Global descriptor table */
    SegmentInfo segreg_shadow[6];               /* Shadow values of segment registers from GDT */
    uint32_t mmap_start;                        /* Minimum address to use when looking for mmap free space */
    bool mmap_recycle;                          /* If false, then never reuse mmap addresses */
    std::vector<uint32_t> auxv;                 /* Auxv vector pushed onto initial stack; also used when dumping core */
    static const uint32_t brk_base=0x08000000;  /* Lowest possible brk() value */
    std::string vdso_name;                      /* Optional base name of virtual dynamic shared object from kernel */
    std::vector<std::string> vdso_paths;        /* Directories and/or filenames to search for vdso */
    rose_addr_t vdso_mapped_va;                 /* Address where vdso is mapped into specimen, or zero */
    rose_addr_t vdso_entry_va;                  /* Entry address for vdso, or zero */
    unsigned core_styles;                       /* What kind of core dump(s) to make for dump_core() */
    std::string core_base_name;                 /* Name to use for core files ("core") */
    rose_addr_t ld_linux_base_va;               /* Base address for ld-linux.so if no preferred addresss for "LOAD#0" */


};

#endif /* ROSE_RSIM_Process_H */
