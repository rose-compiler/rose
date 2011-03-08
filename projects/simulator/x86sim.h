#ifndef ROSE_X86SIM_H
#define ROSE_X86SIM_H

#include "RSIM_Common.h"
#include "RSIM_Process.h"
#include "RSIM_Thread.h"
#include "RSIM_Templates.h"



#if 0
    /** The Kernel Interface is a virtual base class for describing operating system services emulated by the simulator.  These
     *  are things such as system calls, process creation, etc. */
    class Kernel {
    public:
        uint32_t brk_va;                        /**< Current value for brk() syscall */
        static const uint32_t brk_base=0x08000000; /**< Lowest possible brk() value */
        uint32_t mmap_start;                    /**< Minimum address to use when looking for mmap free space */
        bool mmap_recycle;                      /**< If false, then never reuse mmap addresses */
        static const size_t n_gdt=8192;         /**< Number of global descriptor table entries */
        user_desc gdt[n_gdt];                   /**< Global descriptor table. */
        SegmentInfo segreg_shadow[6];           /**< Shadow values for segment registers from GDT */
        std::vector<uint32_t> auxv;             /**< Auxv vector pushed onto initial stack; also used when dumping core */
        std::string vdso_name;                  /**< Optional base name of virtual dynamic shared object */
        std::vector<std::string> vdso_paths;    /**< Directories and/or filenames to search for vdso */
        rose_addr_t vdso_mapped_va;             /**< Address where vdso is mapped into the process, or zero */
        rose_addr_t vdso_entry_va;              /**< Entry address for vdso, or zero */
        unsigned core_styles;                   /**< Flags for dumping specimen core */
        std::string core_base_name;             /**< Name to use for core files (e.g., "core") */
        rose_addr_t ld_linux_base_va;           /**< Base address for ld-linux.so if no preferred addresses for "LOAD#0" */
    };
#endif



#if 0
    /** The Simulator is the top-level object for binary simulations.  Each Simulator object represents the complete state of
     *  one simulated process with any number of threads.  The Simulator object is multi-threaded, creating and joining threads
     *  as necessary in order to manage the threads of the simulated specimen. */
    class Simulator {
    public:
    };
#endif
    

#endif /* ROSE_X86SIM_H */
