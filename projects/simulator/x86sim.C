/* Emulates an executable. */
#include "rose.h"

// DQ (10/14/2010):  This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
// Interestingly it must be at the top of the list of include files.
#include "rose_config.h"

/* Define one CPP symbol to determine whether this simulator can be compiled.  The definition of this one symbol depends on
 * all the header file prerequisites. */
#if defined(HAVE_ASM_LDT_H) && defined(HAVE_ELF_H) && \
    defined(HAVE_LINUX_TYPES_H) && defined(HAVE_LINUX_DIRENT_H) && defined(HAVE_LINUX_UNISTD_H)
#  define ROSE_ENABLE_SIMULATOR
#else
#  undef ROSE_ENABLE_SIMULATOR
#endif

#ifdef ROSE_ENABLE_SIMULATOR /* protects this whole file */


#include "x86print.h"
#include "VirtualMachineSemantics.h"
#include "BinaryLoaderElf.h"
#include <stdarg.h>
#include <boost/regex.hpp>

/* These are necessary for the system call emulation */
#include <asm/ldt.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/futex.h>
#include <syscall.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>
#include <utime.h>


/* AS extra required headrs for system call simulation */
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <sys/wait.h>
#include <sys/utsname.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <linux/types.h>
#include <linux/dirent.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/mman.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <asm/ldt.h>
#include <linux/unistd.h>
#include <sys/sysinfo.h> 
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/ipc.h>


enum CoreStyle { CORE_ELF=0x0001, CORE_ROSE=0x0002 }; /*bit vector*/

#define CONV_FIELD(var1, var2, field) var1.field = var2.field
#ifndef HAVE_USER_DESC
typedef modify_ldt_ldt_s user_desc;
#endif

static const int PROGRESS_INTERVAL = 10; /* seconds */
static int had_alarm = 0;
static void alarm_handler(int) {
    had_alarm = 1;
}

static int
print_user_desc(FILE *f, const uint8_t *_ud, size_t sz)
{
    const user_desc *ud = (const user_desc*)_ud;
    assert(sizeof(*ud)==sz);

    const char *content_type = "unknown";
    switch (ud->contents) {
        case 0: content_type = "data"; break;
        case 1: content_type = "stack"; break;
        case 2: content_type = "code"; break;
    }

    return fprintf(f, "entry=%d, base=0x%08lx, limit=0x%08lx, %s, %s, %s, %s, %s, %s",
                   (int)ud->entry_number, (unsigned long)ud->base_addr, (unsigned long)ud->limit,
                   ud->seg_32bit ? "32bit" : "16bit",
                   content_type, ud->read_exec_only ? "read_exec" : "writable",
                   ud->limit_in_pages ? "page_gran" : "byte_gran",
                   ud->seg_not_present ? "not_present" : "present",
                   ud->useable ? "usable" : "not_usable");
}

static int
print_int_32(FILE *f, const uint8_t *ptr, size_t sz)
{
    assert(4==sz);
    return fprintf(f, "%"PRId32, *(const int32_t*)ptr);
}

/* Kernel stat data structure on 32-bit platforms; the data written back to the specimen's memory */
struct kernel_stat_32 {
    uint64_t        dev;                    /* see 64.dev */
    uint32_t        pad_1;                  /* all bits set */
    uint32_t        ino_lo;                 /* low-order bits only */
    uint32_t        mode;
    uint32_t        nlink;
    uint32_t        user;
    uint32_t        group;
    uint64_t        rdev;
    uint32_t        pad_2;                  /* all bits set */
    uint64_t        size;                   /* 32-bit alignment */
    uint32_t        blksize;
    uint64_t        nblocks;
    uint32_t        atim_sec;
    uint32_t        atim_nsec;              /* always zero */
    uint32_t        mtim_sec;
    uint32_t        mtim_nsec;              /* always zero */
    uint32_t        ctim_sec;
    uint32_t        ctim_nsec;              /* always zero */
    uint64_t        ino;
} __attribute__((packed));

/* Kernel stat data structure on 64-bit platforms; */
struct kernel_stat_64 {
    uint64_t        dev;                   /* probably not 8 bytes, but MSBs seem to be zero anyway */
    uint64_t        ino;
    uint64_t        nlink;
    uint32_t        mode;
    uint32_t        user;
    uint32_t        group;
    uint32_t        pad_1;
    uint64_t        rdev;
    uint64_t        size;
    uint64_t        blksize;
    uint64_t        nblocks;
    uint64_t        atim_sec;
    uint64_t        atim_nsec;              /* always zero */
    uint64_t        mtim_sec;
    uint64_t        mtim_nsec;              /* always zero */
    uint64_t        ctim_sec;
    uint64_t        ctim_nsec;              /* always zero */
    uint64_t        pad_2;
    uint64_t        pad_3;
    uint64_t        pad_4;
};

static int
print_kernel_stat_32(FILE *f, const uint8_t *_sb, size_t sz)
{
    assert(sz==sizeof(kernel_stat_32));
    const kernel_stat_32 *sb = (const kernel_stat_32*)_sb;
    return fprintf(f, "dev=%"PRIu64", ino=%"PRIu64", mode=0%03"PRIo32", nlink=%"PRIu32", uid=%"PRIu32", gid=%"PRIu32
                   ", rdev=%"PRIu64", size=%"PRIu64", blksz=%"PRIu32", blocks=%"PRIu64", ...",
                   sb->dev, sb->ino, sb->mode, sb->nlink, sb->user, sb->group,
                   sb->rdev, sb->size, sb->blksize, sb->nblocks);
}

struct timespec_32 {
    uint32_t sec;
    uint32_t nsec;
} __attribute__((packed));

static int
print_timespec_32(FILE *f, const uint8_t *_ts, size_t sz)
{
    assert(sz==sizeof(timespec_32));
    const timespec_32 *ts = (const timespec_32*)_ts;
    return fprintf(f, "sec=%"PRIu32", nsec=%"PRIu32, ts->sec, ts->nsec);
}

static const Translate signal_names[] = {
    TE(SIGHUP), TE(SIGINT), TE(SIGQUIT), TE(SIGILL), TE(SIGTRAP), TE(SIGABRT), TE(SIGBUS), TE(SIGFPE), TE(SIGKILL),
    TE(SIGUSR1), TE(SIGSEGV), TE(SIGUSR2), TE(SIGPIPE), TE(SIGALRM), TE(SIGTERM), TE(SIGSTKFLT), TE(SIGCHLD), TE(SIGCONT),
    TE(SIGSTOP), TE(SIGTSTP), TE(SIGTTIN), TE(SIGTTOU), TE(SIGURG), TE(SIGXCPU), TE(SIGXFSZ), TE(SIGVTALRM), TE(SIGPROF),
    TE(SIGWINCH), TE(SIGIO), TE(SIGPWR), TE(SIGSYS), TE2(32, SIGRT32), TE2(33, SIGRT33), TE2(34, SIGRT34), TE2(35, SIGRT35),
    TE2(36, SIGRT36), TE2(37, SIGRT37), TE2(38, SIGRT38), TE2(39, SIGRT39), TE2(40, SIGRT40), TE2(41, SIGRT41),
    TE2(42, SIGRT42), TE2(43, SIGRT43), TE2(44, SIGRT44), TE2(45, SIGRT45), TE2(46, SIGRT46), TE2(47, SIGRT47),
    TE2(48, SIGRT48), TE2(49, SIGRT49), TE2(50, SIGRT50), TE2(51, SIGRT51), TE2(52, SIGRT52), TE2(53, SIGRT53),
    TE2(54, SIGRT54), TE2(55, SIGRT55), TE2(56, SIGRT56), TE2(57, SIGRT57), TE2(58, SIGRT58), TE2(59, SIGRT59),
    TE2(60, SIGRT60), TE2(61, SIGRT61), TE2(62, SIGRT62), TE2(63, SIGRT63),
    T_END};

static const Translate signal_flags[] = {
    TF(SA_NOCLDSTOP), TF(SA_NOCLDWAIT), TF(SA_NODEFER), TF(SA_ONSTACK), TF(SA_RESETHAND), TF(SA_RESTART),
    TF(SA_SIGINFO), T_END};

struct sigaction_32 {
    uint32_t handler_va;
    uint32_t flags;
    uint32_t restorer_va;
    uint64_t mask;
} __attribute__((packed));

static int
print_sigaction_32(FILE *f, const uint8_t *_sa, size_t sz)
{
    assert(sz==sizeof(sigaction_32));
    const sigaction_32 *sa = (const sigaction_32*)_sa;
    return (fprintf(f, "handler=0x%08"PRIx32", flags=", sa->handler_va) +
            print_flags(f, signal_flags, sa->flags) +
            fprintf(f, ", restorer=0x%08"PRIx32", mask=0x%016"PRIx64, sa->restorer_va, sa->mask));
}

/* We use the VirtualMachineSemantics policy. That policy is able to handle a certain level of symbolic computation, but we
 * use it because it also does constant folding, which means that it's symbolic aspects are never actually used here. We only
 * have a few methods to specialize this way.   The VirtualMachineSemantics::Memory is not used -- we use a MemoryMap instead
 * since we're only operating on known addresses and values, and thus override all superclass methods dealing with memory. */
class EmulationPolicy: public VirtualMachineSemantics::Policy {
public:
    struct SegmentInfo {
        uint32_t base, limit;
        bool present;
        SegmentInfo(): base(0), limit(0), present(false) {}
        SegmentInfo(const user_desc &ud) {
            base = ud.base_addr;
            limit = ud.limit_in_pages ? (ud.limit << 12) | 0xfff : ud.limit;
            present = !ud.seg_not_present && ud.useable;
        }
    };

    /* Thrown by exit system calls. */
    struct Exit {
        explicit Exit(int status): status(status) {}
        int status;                             /* same value as returned by waitpid() */
    };

public:
    std::string exename;                        /* Name of executable without any path components */
    std::string interpname;                     /* Name of interpreter from ".interp" section or "--interp=" switch */
    std::vector<std::string> exeargs;           /* Specimen argv with PATH-resolved argv[0] */
    MemoryMap *map;                             /* Describes how specimen's memory is mapped to simulator memory */
    Disassembler *disassembler;                 /* Disassembler to use for obtaining instructions */
    Disassembler::InstructionMap icache;        /* Cache of disassembled instructions */
    uint32_t brk_va;                            /* Current value for brk() syscall; initialized by load() */
    static const size_t n_gdt=8192;             /* Number of global descriptor table entries */
    user_desc gdt[n_gdt];                       /* Global descriptor table */
    SegmentInfo segreg_shadow[6];               /* Shadow values of segment registers from GDT */
    uint32_t mmap_start;                        /* Minimum address to use when looking for mmap free space */
    bool mmap_recycle;                          /* If false, then never reuse mmap addresses */
    sigaction_32 signal_action[_NSIG+1];        /* Simulated actions for signal handling */
    uint64_t signal_mask;                       /* Set by sigsetmask() */
    std::vector<uint32_t> auxv;                 /* Auxv vector pushed onto initial stack; also used when dumping core */
    static const uint32_t brk_base=0x08000000;  /* Lowest possible brk() value */
    std::string vdso_name;                      /* Optional base name of virtual dynamic shared object from kernel */
    std::vector<std::string> vdso_paths;        /* Directories and/or filenames to search for vdso */
    rose_addr_t vdso_mapped_va;                 /* Address where vdso is mapped into specimen, or zero */
    rose_addr_t vdso_entry_va;                  /* Entry address for vdso, or zero */
    unsigned core_styles;                       /* What kind of core dump(s) to make for dump_core() */
    std::string core_base_name;                 /* Name to use for core files ("core") */
    rose_addr_t ld_linux_base_va;               /* Base address for ld-linux.so; see c'tor */


#if 0
    uint32_t gsOffset;
    void (*eipShadow)();
    uint32_t signalStack;
    std::vector<user_desc> thread_areas;
#endif
    

    /* Debugging, tracing, etc. */
    FILE *debug;                                /* Stream to which debugging output is sent (or NULL to suppress it) */
    bool trace_insn;                            /* Show each instruction that's executed */
    bool trace_state;                           /* Show machine state after each instruction */
    bool trace_mem;                             /* Show memory read/write operations */
    bool trace_mmap;                            /* Show changes in the memory map */
    bool trace_syscall;                         /* Show each system call */
    bool trace_loader;                          /* Show diagnostics for the program loading */
    bool trace_progress;			/* Show progress now and then */

    EmulationPolicy()
        : map(NULL), disassembler(NULL), brk_va(0), mmap_start(0x40000000ul), mmap_recycle(false), signal_mask(0),
          vdso_mapped_va(0), vdso_entry_va(0), core_styles(CORE_ELF), core_base_name("x-core.rose"), ld_linux_base_va(0),
          debug(NULL), trace_insn(false), trace_state(false), trace_mem(false), trace_mmap(false), trace_syscall(false),
          trace_loader(false), trace_progress(false) {

#if 0
        /* When run under "setarch i386 -LRB3", the ld-linux.so.2 object is mapped at base address 0x40000000. We emulate that
         * behavior here. */
        ld_linux_base_va = 0x40000000;
#else
        /* apparently not true on hudson-rose-07.llnl.gov [RPM 2010-11-04] */
        ld_linux_base_va = 0;
#endif

        vdso_name = "x86vdso";
        vdso_paths.push_back(".");
#ifdef X86_VDSO_PATH_1
        vdso_paths.push_back(X86_VDSO_PATH_1);
#endif
#ifdef X86_VDSO_PATH_2
        vdso_paths.push_back(X86_VDSO_PATH_2);
#endif
        

        for (size_t i=0; i<VirtualMachineSemantics::State::n_gprs; i++)
            writeGPR((X86GeneralPurposeRegister)i, 0);
        for (size_t i=0; i<VirtualMachineSemantics::State::n_flags; i++)
            writeFlag((X86Flag)i, 0);
        writeIP(0);
        writeFlag((X86Flag)1, true_());
        writeGPR(x86_gpr_sp, 0xbffff000ul);     /* high end of stack, exclusive */

        memset(gdt, 0, sizeof gdt);
        gdt[0x23>>3].entry_number = 0x23>>3;
        gdt[0x23>>3].limit = 0x000fffff;
        gdt[0x23>>3].seg_32bit = 1;
        gdt[0x23>>3].read_exec_only = 1;
        gdt[0x23>>3].limit_in_pages = 1;
        gdt[0x23>>3].useable = 1;
        gdt[0x2b>>3].entry_number = 0x2b>>3;
        gdt[0x2b>>3].limit = 0x000fffff;
        gdt[0x2b>>3].seg_32bit = 1;
        gdt[0x2b>>3].limit_in_pages = 1;
        gdt[0x2b>>3].useable = 1;

        writeSegreg(x86_segreg_cs, 0x23);
        writeSegreg(x86_segreg_ds, 0x2b);
        writeSegreg(x86_segreg_es, 0x2b);
        writeSegreg(x86_segreg_ss, 0x2b);
        writeSegreg(x86_segreg_fs, 0x2b);
        writeSegreg(x86_segreg_gs, 0x2b);

        memset(signal_action, 0, sizeof signal_action);
    }

    /* Print machine register state for debugging */
    void dump_registers(FILE *f) const {
        fprintf(f, "  Machine state:\n");
        fprintf(f, "    eax=0x%08"PRIx64" ebx=0x%08"PRIx64" ecx=0x%08"PRIx64" edx=0x%08"PRIx64"\n",
                readGPR(x86_gpr_ax).known_value(), readGPR(x86_gpr_bx).known_value(),
                readGPR(x86_gpr_cx).known_value(), readGPR(x86_gpr_dx).known_value());
        fprintf(f, "    esi=0x%08"PRIx64" edi=0x%08"PRIx64" ebp=0x%08"PRIx64" esp=0x%08"PRIx64" eip=0x%08"PRIx64"\n",
                readGPR(x86_gpr_si).known_value(), readGPR(x86_gpr_di).known_value(),
                readGPR(x86_gpr_bp).known_value(), readGPR(x86_gpr_sp).known_value(),
                get_ip().known_value());
        for (int i=0; i<6; i++) {
            X86SegmentRegister sr = (X86SegmentRegister)i;
            fprintf(f, "    %s=0x%04"PRIx64" base=0x%08"PRIx32" limit=0x%08"PRIx32" present=%s\n",
                    segregToString(sr), readSegreg(sr).known_value(), segreg_shadow[sr].base, segreg_shadow[sr].limit,
                    segreg_shadow[sr].present?"yes":"no");
        }

        uint32_t eflags = get_eflags();
        fprintf(f, "    flags: 0x%08"PRIx32":", eflags);
        static const char *flag_name[] = {"cf",  "#1",  "pf",   "#3",    "af",    "#5",  "zf",  "sf",
                                          "tf",  "if",  "df",   "of", "iopl0", "iopl1",  "nt", "#15",
                                          "rf",  "vm",  "ac",  "vif",   "vip",    "id", "#22", "#23",
                                          "#24", "#25", "#26", "#27",   "#28",   "#29", "#30", "#31"};
        for (uint32_t i=0; i<32; i++) {
            if (eflags & (1u<<i))
                fprintf(f, " %s", flag_name[i]);
        }
        fprintf(f, "\n");
    }

    uint32_t get_eflags() const {
        uint32_t eflags = 0;
        for (size_t i=0; i<VirtualMachineSemantics::State::n_flags; i++) {
            if (readFlag((X86Flag)i).is_known())
                eflags |= readFlag((X86Flag)i).known_value() ? 1u<<i : 0u;
        }
        return eflags;
    }

    /* Generate an ELF Core Dump on behalf of the specimen.  This is a real core dump that can be used with GDB and contains
     * the same information as if the specimen had been running natively and dumped its own core. In other words, the core
     * dump we generate here does not have references to the simulator even though it is being dumped by the simulator. */
    void dump_core(int signo, std::string base_name="");

    /* Recursively load an executable and its libraries libraries into memory, creating the MemoryMap object that describes
     * the mapping from the specimen's address space to the simulator's address space.
     *
     * There are two ways to load dynamic libraries:
     *   1. Load the dynamic linker (ld-linux.so) and simulate it in order to load the libraries.  This is the most accurate
     *      since it delegates the dynamic linking to the actual dynamic linker.  It thus allows different linkers to be
     *      used.
     *   2. Use Matt Brown's work to have ROSE itself resolve the dynamic linking issues.  This approach gives us better
     *      control over the finer details such as which directories are searched, etc. since we have total control over the
     *      linker.  However, Matt's work is not complete at this time [2010-07-20].
     *
     * We use the first approach. */
    SgAsmGenericHeader* load(const char *name);

    /* Initialize the stack for the specimen.  The argc and argv are the command-line of the specimen, not ROSE or the
     * simulator. */
    void initialize_stack(SgAsmGenericHeader*, int argc, char *argv[]);

    /* Returns instruction at current IP, disassembling it if necessary, and caching it. */
    SgAsmx86Instruction *current_insn();

    /* Returns an argument of a system call */
    uint32_t arg(int idx);

    /* Emulates a Linux system call from an INT 0x80 instruction. */
    void emulate_syscall();

    /* Print the name and arguments of a system call in a manner like strace */
    void syscall_enter(const char *name, const char *fmt, ...);

    /* Print the return value of a system call in a manner like strace */
    void syscall_leave(const char *format, ...);

    /* Print the contents of a struct filled in by a system call. */
    void syscall_result(uint32_t ptr, size_t sz, ArgInfo::StructPrinter);

    /* Initializes an ArgInfo object to pass to syscall printing functions. */
    void syscall_arginfo(char fmt, uint32_t val, ArgInfo *info, va_list *ap);

    /* Returns the memory address in ROSE where the specified specimen address is located. */
    void *my_addr(uint32_t va);

    /* Reads a NUL-terminated string from specimen memory. The NUL is not included in the string. */
    std::string read_string(uint32_t va);

    /* Reads a vector of NUL-terminated strings from specimen memory. */
    std::vector<std::string> read_string_vector(uint32_t va);

    /* Called by X86InstructionSemantics. Used by x86_and instruction to set AF flag */
    VirtualMachineSemantics::ValueType<1> undefined_() {
        return 1;
    }

    /* Called by X86InstructionSemantics for the HLT instruction */
    void hlt() {
        fprintf(stderr, "hlt\n");
        abort();
    }

    /* Called by X86InstructionSemantics for the INT instruction */
    void interrupt(uint8_t num) {
        if (num != 0x80) {
            fprintf(stderr, "Bad interrupt\n");
            abort();
        }
        emulate_syscall();
    }

    /* Called by X86InstructionSemantics for the SYSENTER instruction */
    void sysenter() {
        emulate_syscall();

        /* On linux, SYSENTER is followed by zero or more NOPs, followed by a JMP back to just before the SYSENTER in order to
         * restart interrupted system calls, followed by POPs for the callee-saved registers. A non-interrupted system call
         * should return to the first POP instruction, which happens to be 9 bytes after the end of the SYSENTER. */
        writeIP(add(readIP(), number<32>(9)));
    }

    /* Called by X86InstructionSemantics */
    void startInstruction(SgAsmInstruction* insn) {
        if (debug && trace_insn) {
            if (isatty(fileno(debug))) {
                fprintf(debug, "\033[K\n[%07zu] %s\033[K\r\033[1A", get_ninsns(), unparseInstructionWithAddress(insn).c_str());
            } else {
                fprintf(debug, "[%07zu] 0x%08"PRIx64": %s\n", get_ninsns(), insn->get_address(), unparseInstruction(insn).c_str());
            }
        }
        VirtualMachineSemantics::Policy::startInstruction(insn);
#if 0
        if (ms.signalQueue.anySignalsWaiting()) {
            simulate_signal_check(ms, insn->get_address());
        }
#endif
    }

    /* Write value to a segment register and its shadow. */
    void writeSegreg(X86SegmentRegister sr, const VirtualMachineSemantics::ValueType<16> &val) {
        ROSE_ASSERT(3 == (val.known_value() & 7)); /*GDT and privilege level 3*/
        VirtualMachineSemantics::Policy::writeSegreg(sr, val);
        segreg_shadow[sr] = gdt[val.known_value()>>3];
        ROSE_ASSERT(segreg_shadow[sr].present);
    }

    /* Reads memory from the memory map rather than the super class. */
    template <size_t Len> VirtualMachineSemantics::ValueType<Len>
    readMemory(X86SegmentRegister sr, const VirtualMachineSemantics::ValueType<32> &addr,
               const VirtualMachineSemantics::ValueType<1> cond) {
        ROSE_ASSERT(0==Len % 8 && Len<=64);
        uint32_t base = segreg_shadow[sr].base;
        uint32_t offset = addr.known_value();
        ROSE_ASSERT(offset <= segreg_shadow[sr].limit);
        ROSE_ASSERT(offset + (Len/8) - 1 <= segreg_shadow[sr].limit);

        ROSE_ASSERT(cond.is_known());
        if (cond.known_value()) {
            uint8_t buf[Len/8];
            size_t nread = map->read(buf, base+offset, Len/8);
            if (nread!=Len/8) {
                fprintf(stderr, "read %zu byte%s failed at 0x%08"PRIx32"\n\n", Len/8, 1==Len/8?"":"s", base+offset);
                dump_core(SIGSEGV);
                abort();
            }
            uint64_t result = 0;
            for (size_t i=0, j=0; i<Len; i+=8, j++)
                result |= buf[j] << i;
            if (debug && trace_mem) {
                fprintf(debug, "  readMemory<%zu>(0x%08"PRIx32"+0x%08"PRIx32"=0x%08"PRIx32") -> 0x%08"PRIx64"\n",
                        Len, base, offset, base+offset, VirtualMachineSemantics::ValueType<Len>(result).known_value());
            }
            return result;
        } else {
            return 0;
        }
    }

    /* Writes memory to the memory map rather than the super class. */
    template <size_t Len> void
    writeMemory(X86SegmentRegister sr, const VirtualMachineSemantics::ValueType<32> &addr,
                const VirtualMachineSemantics::ValueType<Len> &data,  VirtualMachineSemantics::ValueType<1> cond) {
        ROSE_ASSERT(0==Len % 8 && Len<=64);
        uint32_t base = segreg_shadow[sr].base;
        uint32_t offset = addr.known_value();
        ROSE_ASSERT(offset <= segreg_shadow[sr].limit);
        ROSE_ASSERT(offset + (Len/8) - 1 <= segreg_shadow[sr].limit);
        ROSE_ASSERT(data.is_known());
        ROSE_ASSERT(cond.is_known());
        if (cond.known_value()) {
            if (debug && trace_mem) {
                fprintf(debug, "  writeMemory<%zu>(0x%08"PRIx32"+0x%08"PRIx32"=0x%08"PRIx32", 0x%08"PRIx64")\n",
                        Len, base, offset, base+offset, data.known_value());
            }
            uint8_t buf[Len/8];
            for (size_t i=0, j=0; i<Len; i+=8, j++)
                buf[j] = (data.known_value() >> i) & 0xff;
            size_t nwritten = map->write(buf, base+offset, Len/8);
            if (nwritten!=Len/8) {
                fprintf(stderr, "write %zu byte%s failed at 0x%08"PRIx32"\n\n", Len/8, 1==Len/8?"":"s", base+offset);
                dump_core(SIGSEGV);
                abort();
            }
        }
    }

    int 
    ipc_kernel(uint32_t call, int32_t first, int32_t second, int32_t third, uint32_t ptr, uint8_t ptr_addr, uint32_t fifth);

};

/* Using the new interface is still about as complicated as the old interface because we need to perform only a partial link.
 * We want ROSE to link the interpreter (usually /lib/ld-linux.so) into the AST but not link in any other shared objects.
 * Then we want ROSE to map the interpreter (if present) and all main ELF Segments into the specimen address space but not
 * make any of the usual adjustments for ELF Sections that also specify a mapping. */

struct SimLoader: public BinaryLoaderElf {
public:
    SgAsmGenericHeader *interpreter;                    /* header linked into AST for .interp section */
    SgAsmGenericHeader *vdso;                           /* header for the vdso file, if any */
    rose_addr_t vdso_mapped_va;                         /* base address where vdso is mapped, or zero */
    rose_addr_t vdso_entry_va;                          /* entry address for the vdso, or zero */

    SimLoader(SgAsmInterpretation *interpretation, FILE *debug, std::string default_interpname)
        : interpreter(NULL), vdso(NULL), vdso_mapped_va(0), vdso_entry_va(0) {
        set_debug(debug);
        set_perform_dynamic_linking(false);             /* we explicitly link in the interpreter and nothing else */
        set_perform_remap(true);                        /* map interpreter and main binary into specimen memory */
        set_perform_relocations(false);                 /* allow simulated interpreter to perform relocation fixups */

        /* Link the interpreter into the AST */
        SgAsmGenericHeader *header = interpretation->get_headers()->get_headers().front();
        std::string interpreter_name = find_interpreter(header, default_interpname);
        if (!interpreter_name.empty()) {
            SgBinaryComposite *composite = SageInterface::getEnclosingNode<SgBinaryComposite>(interpretation);
            ROSE_ASSERT(composite!=NULL);
            SgAsmGenericFile *ifile = createAsmAST(composite, interpreter_name);
            interpreter = ifile->get_headers()->get_headers().front();
        }
    }

    /* Finds the name of the interpreter (usually "/lib/ld-linux.so") if any. The name comes from the PT_INTERP section,
     * usually named ".interp".  If an interpreter name is supplied as an argument, then it will be used instead, but only
     * if a PT_INTERP section is present. */
    std::string find_interpreter(SgAsmGenericHeader *header, std::string default_interpname="") {
        struct: public SgSimpleProcessing {
            std::string interp_name;
            void visit(SgNode *node) {
                SgAsmElfSection *section = isSgAsmElfSection(node);
                SgAsmElfSegmentTableEntry *segment = section ? section->get_segment_entry() : NULL;
                if (segment && SgAsmElfSegmentTableEntry::PT_INTERP==segment->get_type()) {
                    char buf[section->get_size()];
                    section->read_content_local(0, buf, section->get_size());
                    interp_name = std::string(buf, section->get_size());
                }
            }
        } t1;
        t1.traverse(header, preorder);
        return (t1.interp_name.empty() || default_interpname.empty()) ? t1.interp_name : default_interpname;
    }

    /* Returns ELF PT_LOAD Segments in order by virtual address. */
    virtual SgAsmGenericSectionPtrList get_remap_sections(SgAsmGenericHeader *header) {
        SgAsmGenericSectionPtrList retval;
        SgAsmGenericSectionPtrList sections = BinaryLoaderElf::get_remap_sections(header);
        for (SgAsmGenericSectionPtrList::iterator si=sections.begin(); si!=sections.end(); si++) {
            SgAsmElfSection *section = isSgAsmElfSection(*si);
            SgAsmElfSegmentTableEntry *entry = section ? section->get_segment_entry() : NULL;
            if (entry && entry->get_type()==SgAsmElfSegmentTableEntry::PT_LOAD)
                retval.push_back(section);
        }
        return retval;
    }

    /* Load the specified file as a virtual dynamic shared object. Returns true if the vdso was found and mapped. The side
     * effect is that the "vdso", "vdso_mapped_va", and "vdso_entry_va" data members are initialized when the vdso is found and
     * mapped into memory. */
    bool map_vdso(const std::string &vdso_name, SgAsmInterpretation *interpretation, MemoryMap *map) {
        ROSE_ASSERT(vdso==NULL);
        ROSE_ASSERT(vdso_mapped_va==0);
        ROSE_ASSERT(vdso_entry_va==0);

        struct stat sb;
        if (stat(vdso_name.c_str(), &sb)<0 || !S_ISREG(sb.st_mode))
            return false;
        int fd = open(vdso_name.c_str(), O_RDONLY);
        if (fd<0)
            return false;
        
        SgBinaryComposite *composite = SageInterface::getEnclosingNode<SgBinaryComposite>(interpretation);
        ROSE_ASSERT(composite!=NULL);
        SgAsmGenericFile *file = createAsmAST(composite, vdso_name);
        ROSE_ASSERT(file!=NULL);
        SgAsmGenericHeader *fhdr = file->get_headers()->get_headers()[0];
        ROSE_ASSERT(isSgAsmElfFileHeader(fhdr)!=NULL);
        rose_addr_t entry_rva = fhdr->get_entry_rva();

        uint8_t *buf = new uint8_t[sb.st_size];
        ssize_t nread = read(fd, buf, sb.st_size);
        ROSE_ASSERT(nread==sb.st_size);
        close(fd); fd=-1;

        vdso_mapped_va = ALIGN_UP(map->find_last_free(), PAGE_SIZE);
        vdso_mapped_va = std::max(vdso_mapped_va, (rose_addr_t)0x40000000); /* value used on hudson-rose-07 */
        MemoryMap::MapElement me(vdso_mapped_va, sb.st_size, buf, 0, MemoryMap::MM_PROT_READ|MemoryMap::MM_PROT_EXEC);
        me.set_name("[vdso]");
        map->insert(me);

        if (sb.st_size!=ALIGN_UP(sb.st_size, PAGE_SIZE)) {
            MemoryMap::MapElement me2(vdso_mapped_va+sb.st_size, ALIGN_UP(sb.st_size, PAGE_SIZE)-sb.st_size,
                                      MemoryMap::MM_PROT_READ|MemoryMap::MM_PROT_EXEC);
            me2.set_name(me.get_name());
            map->insert(me2);
        }

        vdso_entry_va = vdso_mapped_va + entry_rva;
        return true;
    }
};

SgAsmGenericHeader*
EmulationPolicy::load(const char *name)
{
    /* Find the executable by searching the PATH environment variable. The executable name and full path name are both saved
     * in the class (exename and exeargs[0]). */ 
    ROSE_ASSERT(exename.empty() && exeargs.empty());
    if (strchr(name, '/')) {
        if (access(name, R_OK)<0) {
            fprintf(stderr, "%s: %s\n", name, strerror(errno));
            exit(1);
        }
        exename = strrchr(name, '/')+1;
        exeargs.push_back(std::string(name));
    } else {
        const char *path_env = getenv("PATH");
        if (path_env) {
            std::string s = path_env;
            boost::regex re;
            re.assign("[:;]");
            boost::sregex_token_iterator iter(s.begin(), s.end(), re, -1);
            boost::sregex_token_iterator iterEnd;
            for (; iter!=iterEnd; ++iter) {
                std::string fullname = *iter + "/" + name;
                if (access(fullname.c_str(), R_OK)>=0) {
                    exename = name;
                    exeargs.push_back(fullname);
                    break;
                }
            }
        }
    }
    if (exeargs.empty()) {
        fprintf(stderr, "%s: not found\n", name);
        exit(1);
    }
       
    /* Link the main binary into the AST without further linking, mapping, or relocating. */
    if (debug && trace_loader)
        fprintf(debug, "loading %s...\n", exeargs[0].c_str());
    char *frontend_args[4];
    frontend_args[0] = strdup("-");
    frontend_args[1] = strdup("-rose:read_executable_file_format_only"); /*delay disassembly until later*/
    frontend_args[2] = strdup(exeargs[0].c_str());
    frontend_args[3] = NULL;
    SgProject *project = frontend(3, frontend_args);

    /* Find the best interpretation and file header.  Windows PE programs have two where the first is DOS and the second is PE
     * (we'll use the PE interpretation). */
    SgAsmInterpretation *interpretation = SageInterface::querySubTree<SgAsmInterpretation>(project, V_SgAsmInterpretation).back();
    SgAsmGenericHeader *fhdr = interpretation->get_headers()->get_headers().front();
    writeIP(fhdr->get_entry_rva() + fhdr->get_base_va());

    /* Link the interpreter into the AST */
    SimLoader *loader = new SimLoader(interpretation, trace_loader ? debug : NULL, interpname);

    /* If we found an interpreter then use its entry address as the start of simulation.  When running the specimen directly
     * in Linux with "setarch i386 -LRB3", the ld-linux.so.2 gets mapped to 0x40000000 even though the libs preferred
     * addresses start at zero.  We can accomplish the same thing simply by rebasing the library. */
    if (loader->interpreter) {
        loader->interpreter->set_base_va(ld_linux_base_va);
        writeIP(loader->interpreter->get_entry_rva() + loader->interpreter->get_base_va());
    }

    /* Sort the headers so they're in order by entry address. In other words, if the interpreter's entry address is below the
     * entry address of the main executable, then make sure the interpretter gets mapped first. */
    SgAsmGenericHeaderPtrList &headers = interpretation->get_headers()->get_headers();
    if (2==headers.size()) {
        if (headers[0]->get_base_va() + headers[0]->get_entry_rva() >
            headers[1]->get_base_va() + headers[1]->get_entry_rva())
            std::swap(headers[0], headers[1]);
    } else {
        ROSE_ASSERT(1==headers.size());
    }

    /* Map all segments into simulated memory */
    loader->load(interpretation);
    map = interpretation->get_map();

    /* Load and map the virtual dynamic shared library. */
    if (!loader->interpreter) {
        if (debug && trace_loader)
            fprintf(stderr, "warning: static executable; no vdso necessary\n");
    } else {
        bool vdso_loaded = false;
        for (size_t i=0; i<vdso_paths.size() && !vdso_loaded; i++) {
            for (int j=0; j<2 && !vdso_loaded; j++) {
                std::string vdso_name = vdso_paths[i] + (j ? "" : "/" + this->vdso_name);
                if (debug && trace_loader)
                    fprintf(debug, "looking for vdso: %s\n", vdso_name.c_str());
                if ((vdso_loaded = loader->map_vdso(vdso_name, interpretation, map))) {
                    vdso_mapped_va = loader->vdso_mapped_va;
                    vdso_entry_va = loader->vdso_entry_va;
                    if (debug && trace_loader) {
                        fprintf(debug, "mapped %s at 0x%08"PRIx64" with entry va 0x%08"PRIx64"\n",
                                vdso_name.c_str(), vdso_mapped_va, vdso_entry_va);
                    }
                }
            }
        }
        if (!vdso_loaded && debug && trace_loader)
            fprintf(stderr, "warning: cannot find a virtual dynamic shared object\n");
    }

    /* Find a disassembler. */
    if (!disassembler)
        disassembler = Disassembler::lookup(interpretation)->clone();

    /* Initialize the brk value to be the lowest page-aligned address that is above the end of the highest mapped address but
     * below 0x40000000 (the stack, and where ld-linux.so.2 might be loaded when loaded high). */
    rose_addr_t free_area = std::max(map->find_last_free(std::max(ld_linux_base_va, (rose_addr_t)0x40000000)),
                                     (rose_addr_t)brk_base);
    brk_va = ALIGN_UP(free_area, PAGE_SIZE);

    delete loader;
    return fhdr;
}

void EmulationPolicy::initialize_stack(SgAsmGenericHeader *_fhdr, int argc, char *argv[])
{
    /* We only handle ELF for now */
    SgAsmElfFileHeader *fhdr = isSgAsmElfFileHeader(_fhdr);
    ROSE_ASSERT(fhdr!=NULL);

    /* Allocate the stack */
    static const size_t stack_size = 0x00015000;
    size_t sp = readGPR(x86_gpr_sp).known_value();
    size_t stack_addr = sp - stack_size;
    MemoryMap::MapElement melmt(stack_addr, stack_size, MemoryMap::MM_PROT_READ|MemoryMap::MM_PROT_WRITE);
    melmt.set_name("[stack]");
    map->insert(melmt);

    /* Initialize the stack with specimen's argc and argv. Also save the arguments in the class. */
    ROSE_ASSERT(exeargs.size()==1);                     /* contains only the executable path */
    std::vector<uint32_t> pointers;                     /* pointers pushed onto stack at the end of initialization */
    pointers.push_back(argc);
    for (int i=0; i<argc; i++) {
        std::string arg;
        if (0==i) {
            arg = exeargs[0];
        } else {
            arg = argv[i];
            exeargs.push_back(arg);
        }
        size_t len = arg.size() + 1; /*inc. NUL termination*/
        sp -= len;
        map->write(arg.c_str(), sp, len);
        pointers.push_back(sp);
        if (debug && trace_loader)
            fprintf(debug, "argv[%d] %zu bytes at 0x%08zu = \"%s\"\n", i, len, sp, arg.c_str());
    }
    pointers.push_back(0); /*the argv NULL terminator*/

    /* Create new environment variables by stripping "X86SIM_" off the front of any environment variable and using that
     * value to override the non-X86SIM_ value, if any.  We try to make sure the variables are in the same order as if the
     * X86SIM_ overrides were not present. In other words, if X86SIM_FOO and FOO are both present, then X86SIM_FOO is deleted
     * from the list and its value used for FOO; but if X86SIM_FOO is present without FOO, then we just change the name to FOO
     * and leave it at that location. We do all this so that variables are in the same order whether run natively or under the
     * simulator. */
    std::map<std::string, std::string> envvars;
    std::map<std::string, std::string>::iterator found;
    for (int i=0; environ[i]; i++) {
        char *eq = strchr(environ[i], '=');
        ROSE_ASSERT(eq!=NULL);
        std::string var(environ[i], eq-environ[i]);
        std::string val(eq+1);
        envvars.insert(std::make_pair(var, val));
    }
    for (int i=0, j=0; environ[i]; i++) {
        char *eq = strchr(environ[i], '=');
        ROSE_ASSERT(eq!=NULL);
        std::string var(environ[i], eq-environ[i]);
        std::string val(eq+1);
        if (!strncmp(var.c_str(), "X86SIM_", 7) && environ[i]+7!=eq) {
            std::string var_short = var.substr(7);
            if ((found=envvars.find(var_short))==envvars.end()) {
                var = var_short;
                val = eq+1;
            } else {
                continue;
            }
        } else {
            std::string var_long = "X86SIM_" + var;
            if ((found=envvars.find(var_long))!=envvars.end()) {
                val = found->second;
            }
        }
        std::string env = var + "=" + val;
        sp -= env.size() + 1;
        map->write(env.c_str(), sp, env.size()+1);
        pointers.push_back(sp);
        if (debug && trace_loader)
            fprintf(debug, "environ[%d] %zu bytes at 0x%08zu = \"%s\"\n", j++, env.size(), sp, env.c_str());
    }
    pointers.push_back(0); /*environment NULL terminator*/

    /* Initialize stack with auxv, where each entry is two words in the pointers vector. This information is only present for
     * dynamically linked executables. */
    if (fhdr->get_section_by_name(".interp")) {
        struct T1: public SgSimpleProcessing {
            rose_addr_t phdr_rva;
            T1(): phdr_rva(0) {}
            void visit(SgNode *node) {
                SgAsmElfSection *section = isSgAsmElfSection(node);
                SgAsmElfSegmentTableEntry *entry = section ? section->get_segment_entry() : NULL;
                if (0==phdr_rva && entry && entry->get_type()==SgAsmElfSegmentTableEntry::PT_PHDR)
                    phdr_rva = section->get_mapped_preferred_rva();
            }
        } t1;
        t1.traverse(fhdr, preorder);
        auxv.clear();

        if (vdso_mapped_va!=0) {
            /* AT_SYSINFO */
            auxv.push_back(32);
            auxv.push_back(vdso_entry_va);
            if (debug && trace_loader)
                fprintf(debug, "AT_SYSINFO:       0x%08"PRIx32"\n", auxv.back());

            /* AT_SYSINFO_PHDR */
            auxv.push_back(33);
            auxv.push_back(vdso_mapped_va);
            if (debug && trace_loader)
                fprintf(debug, "AT_SYSINFO_PHDR:  0x%08"PRIx32"\n", auxv.back());
        }

#if 0 /*Disabled because it causes ld.so to execute MXX instructions [RPM 2010-09-21]*/
        /* AT_HWCAP (see linux <include/asm/cpufeature.h>) */
        auxv.push_back(16);
        auxv.push_back(
                       //(1u<<3)  |       /*pse           page size extensions*/
                       //(1u<<4)  |       /*tsc           time stamp counter*/
                       //(1u<<5)  |       /*msr           model-specific registers*/
                       //(1u<<6)  |       /*pae           physical address extensions*/
                       //(1u<<7)  |       /*mce           machine check exception*/
                       //(1u<<8)  |       /*cx8           CMPXCHG8 instruction*/
                       //(1u<<9)  |       /*apic          onboard APIC*/
                       (1u<<11) |       /*sep           SYSENTER/SYSEXIT instructions*/
                       //(1u<<12) |       /*mtrr          memory type range registers*/
                       //(1u<<13) |       /*pge           page global enable*/
                       //(1u<<14) |       /*mca           machine check architecture */
                       (1u<<15) |       /*cmov          CMOV instructions (and floating point varieties with FPU)*/
                       //(1u<<16) |       /*pat           page attribute table*/
                       //(1u<<17) |       /*pse36         36-bit PSEs*/
                       //(1u<<18) |       /*clflush       CLFLUSH instruction*/
                       //(1u<<22) |       /*acpi          ACPI via MSR*/
                       //(1u<<29) |       /*acc           "tm" automatic clock control*/
                       //(1u<<31) |       /*pbe           pending break enable*/
                       0);
        if (debug && trace_loader)
            fprintf(debug, "AT_HWCAP:         0x%08"PRIx32"\n", auxv.back());
#endif

        /* AT_PAGESZ */
        auxv.push_back(6);
        auxv.push_back(PAGE_SIZE);
        if (debug && trace_loader)
            fprintf(debug, "AT_PAGESZ:        %"PRId32"\n", auxv.back());

        /* AT_CLKTCK */
        auxv.push_back(17);
        auxv.push_back(100);
        if (debug && trace_loader)
            fprintf(debug, "AT_CLKTCK:        %"PRId32"\n", auxv.back());

        /* AT_PHDR */
        auxv.push_back(3); /*AT_PHDR*/
        auxv.push_back(t1.phdr_rva + fhdr->get_base_va());
        if (debug && trace_loader)
            fprintf(debug, "AT_PHDR:          0x%08"PRIx32"\n", auxv.back());

        /*AT_PHENT*/
        auxv.push_back(4);
        auxv.push_back(fhdr->get_phextrasz() + sizeof(SgAsmElfSegmentTableEntry::Elf32SegmentTableEntry_disk));
        if (debug && trace_loader)
            fprintf(debug, "AT_PHENT:         %"PRId32"\n", auxv.back());

        /* AT_PHNUM */
        auxv.push_back(5);
        auxv.push_back(fhdr->get_e_phnum());
        if (debug && trace_loader)
            fprintf(debug, "AT_PHNUM:         %"PRId32"\n", auxv.back());

        /* AT_BASE */
        auxv.push_back(7);
        auxv.push_back(ld_linux_base_va);
        if (debug && trace_loader)
            fprintf(debug, "AT_BASE:          0x%08"PRIx32"\n", auxv.back());

        /* AT_FLAGS */
        auxv.push_back(8);
        auxv.push_back(0);
        if (debug && trace_loader)
            fprintf(debug, "AT_FLAGS:         0x%08"PRIx32"\n", auxv.back());

        /* AT_ENTRY */
        auxv.push_back(9);
        auxv.push_back(fhdr->get_entry_rva() + fhdr->get_base_va());
        if (debug && trace_loader)
            fprintf(debug, "AT_ENTRY:         0x%08"PRIx32"\n", auxv.back());

        /* AT_UID */
        auxv.push_back(11);
        auxv.push_back(getuid());
        if (debug && trace_loader)
            fprintf(debug, "AT_UID:           %"PRId32"\n", auxv.back());

        /* AT_EUID */
        auxv.push_back(12);
        auxv.push_back(geteuid());
        if (debug && trace_loader)
            fprintf(debug, "AT_EUID:          %"PRId32"\n", auxv.back());

        /* AT_GID */
        auxv.push_back(13);
        auxv.push_back(getgid());
        if (debug && trace_loader)
            fprintf(debug, "AT_GID:           %"PRId32"\n", auxv.back());

        /* AT_EGID */
        auxv.push_back(14);
        auxv.push_back(getegid());
        if (debug && trace_loader)
            fprintf(debug, "AT_EGID:          %"PRId32"\n", auxv.back());

        /* AT_SECURE */
        auxv.push_back(23);
        auxv.push_back(false);
        if (debug && trace_loader)
            fprintf(debug, "AT_SECURE:        %"PRId32"\n", auxv.back());

        /* AT_PLATFORM */
        {
            const char *platform = "i386";
            size_t len = strlen(platform)+1;
            sp -= len;
            map->write(platform, sp, len);
            auxv.push_back(15);
            auxv.push_back(sp);
            if (debug && trace_loader)
                fprintf(debug, "AT_PLATFORM:      0x%08"PRIx32" (%s)\n", auxv.back(), platform);
        }
    }

    /* AT_NULL */
    auxv.push_back(0);
    auxv.push_back(0);
    pointers.insert(pointers.end(), auxv.begin(), auxv.end());

    /* Finalize stack initialization by writing all the pointers to data we've pushed:
     *    argc
     *    argv with NULL terminator
     *    environment with NULL terminator
     *    auxv pairs terminated with (AT_NULL,0)
     */
    sp &= ~3U; /*align to four-bytes*/
    sp -= 4 * pointers.size();
    map->write(&(pointers[0]), sp, 4*pointers.size());

    writeGPR(x86_gpr_sp, sp);
}

SgAsmx86Instruction *
EmulationPolicy::current_insn()
{
    rose_addr_t ip = readIP().known_value();

    /* Use the cached instruction if possible. */
    Disassembler::InstructionMap::iterator found = icache.find(ip);
    if (found!=icache.end()) {
        SgAsmx86Instruction *insn = isSgAsmx86Instruction(found->second);
        ROSE_ASSERT(insn!=NULL); /*shouldn't be possible due to check below*/
        size_t insn_sz = insn->get_raw_bytes().size();
        SgUnsignedCharList curmem(insn_sz);
        size_t nread = map->read(&curmem[0], ip, insn_sz);
        if (nread==insn_sz && curmem==insn->get_raw_bytes())
            return insn;
        icache.erase(found);
    }

    /* Disassemble (and cache) a new instruction */
    SgAsmx86Instruction *insn = NULL;
    try {
        insn = isSgAsmx86Instruction(disassembler->disassembleOne(map, ip));
    } catch (Disassembler::Exception &e) {
        std::cerr <<e <<"\n";
        dump_core(SIGSEGV);
        throw;
    }
    ROSE_ASSERT(insn!=NULL); /*only happens if our disassembler is not an x86 disassembler!*/
    icache.insert(std::make_pair(ip, insn));
    return insn;
}

void
EmulationPolicy::dump_core(int signo, std::string base_name)
{
    if (base_name.empty())
        base_name = core_base_name;

    fprintf(stderr, "dumping specimen core...\n");
    fprintf(stderr, "memory map at time of core dump:\n");
    map->dump(stderr, "  ");

    if (core_styles & CORE_ROSE)
        map->dump(base_name);
    if (0==(core_styles & CORE_ELF))
        return;

    /* Get current instruction pointer. We subtract the size of the current instruction if we're in the middle of processing
     * an instruction because it would have already been incremented by the semantics. */ 
    uint32_t eip = readIP().known_value();
    if (get_insn())
        eip -= get_insn()->get_raw_bytes().size();

    SgAsmGenericFile *ef = new SgAsmGenericFile;
    ef->set_truncate_zeros(false);

    SgAsmElfFileHeader *fhdr = new SgAsmElfFileHeader(ef);
    fhdr->get_exec_format()->set_purpose(SgAsmExecutableFileFormat::PURPOSE_CORE_DUMP);
    fhdr->add_entry_rva(rose_rva_t(0, NULL));

    SgAsmElfSegmentTable *segtab = new SgAsmElfSegmentTable(fhdr);
    
    /*========================================================================================================================
     * NOTE section
     *======================================================================================================================== */

    SgAsmElfNoteSection *notes = new SgAsmElfNoteSection(fhdr);
    segtab->add_section(notes);
    notes->get_segment_entry()->set_type(SgAsmElfSegmentTableEntry::PT_NOTE);
    
    /* Note CORE.PRSTATUS(1)              (144 bytes) */
    struct prstatus {
        uint32_t signo;                 /* signal number */
        uint32_t code;                  /* extra signal code */
        uint32_t err_num;
        uint32_t cursig;                /* current signal; 2-bytes followed by 2 bytes of zero padding */
        uint32_t sigpend;               /* pending signals */
        uint32_t sighold;               /* set of held signals */
        uint32_t pid;
        uint32_t ppid;
        uint32_t pgrp;
        uint32_t sid;
        uint32_t user_sec;              /* user time */
        uint32_t user_usec;
        uint32_t sys_sec;               /* system time */
        uint32_t sys_usec;
        uint32_t cuser_sec;             /* cummulative user time */
        uint32_t cuser_usec;
        uint32_t csys_sec;              /* cummulative system time */
        uint32_t csys_usec;
        uint32_t bx;                    /* general purpose registers */
        uint32_t cx;
        uint32_t dx;
        uint32_t si;
        uint32_t di;
        uint32_t bp;
        uint32_t ax;
        uint32_t ds;
        uint32_t es;
        uint32_t fs;
        uint32_t gs;
        uint32_t orig_ax;               /* ax value before syscall? */
        uint32_t ip;
        uint32_t cs;
        uint32_t flags;
        uint32_t sp;
        uint32_t ss;
        uint32_t fpvalid;               /* is math coprocessor being used? */
    } __attribute__((packed));
    struct prstatus prstatus;
    ROSE_ASSERT(144==sizeof prstatus);
    memset(&prstatus, 0, sizeof prstatus);
    prstatus.signo = prstatus.cursig = signo;
    prstatus.pid = getpid();
    prstatus.ppid = getppid();
    prstatus.pgrp = getpgrp();
    prstatus.sid = getsid(0);
    prstatus.bx = readGPR(x86_gpr_bx).known_value();
    prstatus.cx = readGPR(x86_gpr_cx).known_value();
    prstatus.dx = readGPR(x86_gpr_dx).known_value();
    prstatus.si = readGPR(x86_gpr_si).known_value();
    prstatus.di = readGPR(x86_gpr_di).known_value();
    prstatus.bp = readGPR(x86_gpr_bp).known_value();
    prstatus.ax = readGPR(x86_gpr_ax).known_value();
    prstatus.ds = readSegreg(x86_segreg_ds).known_value();
    prstatus.es = readSegreg(x86_segreg_es).known_value();
    prstatus.fs = readSegreg(x86_segreg_fs).known_value();
    prstatus.gs = readSegreg(x86_segreg_gs).known_value();
    prstatus.orig_ax = readGPR(x86_gpr_ax).known_value();
    prstatus.ip = eip;
    prstatus.cs = readSegreg(x86_segreg_cs).known_value();
    prstatus.flags = get_eflags();
    prstatus.sp = readGPR(x86_gpr_sp).known_value();
    prstatus.ss = readSegreg(x86_segreg_ss).known_value();
    prstatus.fpvalid = 0;     /*ROSE doesn't support floating point yet*/

    SgAsmElfNoteEntry *prstatus_note = new SgAsmElfNoteEntry(notes);
    prstatus_note->get_name()->set_string("CORE");
    prstatus_note->set_type(1); /*ET_PRSTATUS*/
    prstatus_note->set_payload(&prstatus, sizeof prstatus);
            
    /* Note CORE.PRPSINFO(3)              (124 bytes) */
    struct {
        uint8_t state;                                          /* numeric process state */
        uint8_t sname;                                          /* char for state (is this what ps(1) prints? */
        uint8_t zombie;
        uint8_t nice;                                           /* nice value */
        uint32_t flags;
        uint16_t uid;
        uint16_t gid;
        uint32_t pid;
        uint32_t ppid;
        uint32_t pgrp;
        uint32_t sid;
        char fname[16];                                         /* filename of executable */
        char psargs[80];                                        /* initial part of arg list */
    } __attribute__((packed)) prpsinfo;
    ROSE_ASSERT(124==sizeof prpsinfo);
    memset(&prpsinfo, 0, sizeof prpsinfo);
    prpsinfo.state = 0;                                         /* runable */
    prpsinfo.sname = 'R';
    prpsinfo.zombie = 0;
    prpsinfo.nice = getpriority(PRIO_PROCESS, getpid());
    prpsinfo.flags = 0x2400; /* see linux/sched.h PF_* bits (0x2000=>dumped core; 0x400=>forked but not exec*/
    prpsinfo.uid = geteuid();
    prpsinfo.gid = getegid();
    prpsinfo.pid = getpid();
    prpsinfo.ppid = getppid();
    prpsinfo.pgrp = getpgrp();
    prpsinfo.sid = getsid(0);
    strncpy(prpsinfo.fname, exename.c_str(), sizeof(prpsinfo.fname));
    std::string all_args;
    for (size_t i=0; i<exeargs.size(); i++)
        all_args += exeargs[i] + " "; /*yes, there's an extra space at the end*/
    strncpy(prpsinfo.psargs, all_args.c_str(), sizeof(prpsinfo.psargs));
    
    SgAsmElfNoteEntry *prpsinfo_note = new SgAsmElfNoteEntry(notes);
    prpsinfo_note->get_name()->set_string("CORE");
    prpsinfo_note->set_type(3); /*ET_PRPSINFO*/
    prpsinfo_note->set_payload(&prpsinfo, sizeof prpsinfo);

    /* Note CORE.AUXV(6) */
    SgAsmElfNoteEntry *auxv_note = new SgAsmElfNoteEntry(notes);
    auxv_note->get_name()->set_string("CORE");
    auxv_note->set_type(6);
    auxv_note->set_payload(&auxv[0], 4*auxv.size());

#if 0
    /* Note CORE.PRFPREG(2)               (108 bytes) */
    /* This was just copied straight from an actual core dump because we shouldn't need it here anyway. It's the
     * user_i387_struct defined in linux source code <include/asm/user_32.h> containing 27 doublewords. */
    uint8_t prfpreg[] = {
        0x7f, 0x03, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
        0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    SgAsmElfNoteEntry *prfpreg_note = new SgAsmElfNoteEntry(notes);
    prfpreg_note->get_name()->set_string("CORE");
    prfpreg_note->set_type(2);
    prfpreg_note->set_payload(prfpreg, sizeof prfpreg);
#endif
 
#if 0
    /* Note LINUX.PRXFPREG(0x46e62b7f)    (512 bytes) */
    /* FIXME: This was just copied straight from a real core dump. It's the user32_fxsr_struct defined in the linux source
     *        code <include/asm/user_32.h>.  I don't think we need it because we're not using floating point registers. */
    uint8_t prxfpreg[] = {
        0x7f, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x1f, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    SgAsmElfNoteEntry *prxfpreg_note = new SgAsmElfNoteEntry(notes);
    prxfpreg_note->get_name()->set_string("LINUX");
    prxfpreg_note->set_type(0x46e62b7f);
    prxfpreg_note->set_payload(prxfpreg, sizeof prxfpreg);
#endif

    /* Note LINUX.386_TLS(0x200)          (48 bytes)  i386 TLS slots (struct user_desc)*/
    uint8_t i386_tls[] = {
        0x0c, 0x00, 0x00, 0x00, 0xb0, 0xd6, 0x18, 0x40, 0xff, 0xff, 0x0f, 0x00, 0x51, 0x00, 0x00, 0x00,
        0x0d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00,
        0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00
    };
    SgAsmElfNoteEntry *i386_tls_note = new SgAsmElfNoteEntry(notes);
    i386_tls_note->get_name()->set_string("LINUX");
    i386_tls_note->set_type(0x200);
    i386_tls_note->set_payload(i386_tls, sizeof i386_tls);

    
    /*========================================================================================================================
     * LOAD sections
     *======================================================================================================================== */

    class SegmentBuilder: public SgAsmElfSection {
        MemoryMap       *map;                                   /* memory map for specimen's process address space */
    public:
        SegmentBuilder(SgAsmElfFileHeader *fhdr, MemoryMap *map, rose_addr_t va, rose_addr_t sz, unsigned perms)
            : SgAsmElfSection(fhdr), map(map) {
            set_purpose(SgAsmGenericSection::SP_PROGRAM);       /* Program-supplied text, data, etc. */
            set_offset(ALIGN_UP(get_offset(), 4096));
            set_size(sz);
            set_file_alignment(4096);
            set_mapped_alignment(4096);
            set_mapped_preferred_rva(va); /*va==rva for ELF*/
            set_mapped_size(sz);
            set_mapped_rperm(0!=(perms & MemoryMap::MM_PROT_READ));
            set_mapped_wperm(0!=(perms & MemoryMap::MM_PROT_WRITE));
            set_mapped_xperm(0!=(perms & MemoryMap::MM_PROT_EXEC));
        }
        virtual void unparse(std::ostream &f) const {
            if (0==get_size()) return;
            uint8_t buf[8192];
            rose_addr_t cur_va = get_mapped_preferred_va();     /* current virtual address */
            rose_addr_t nremain = get_mapped_size();            /* bytes remaining to be written to the file */
            rose_addr_t offset = 0;                             /* byte offset with respect to beginning of section */
            while (nremain>0) {
                rose_addr_t to_write = std::min(nremain, (rose_addr_t)sizeof buf);
                size_t nread = map->read(buf, cur_va, to_write);
#if 1
                memset(buf+nread, 0, to_write-nread);
#else
                ROSE_ASSERT(nread==to_write);
#endif
                offset = write(f, offset, to_write, buf);
                cur_va += to_write;
                nremain -= to_write;
            }
        }
    };

    /* We dump everything to the core file, although linux (by default) skips private and shared non-writable mappings that
     * have backing store. */
    const std::vector<MemoryMap::MapElement> &elmts = map->get_elements();
    std::vector<MemoryMap::MapElement>::const_iterator ei=elmts.begin();
    while (ei!=elmts.end()) {
        rose_addr_t va = ei->get_va();
        rose_addr_t sz = ei->get_size();
        unsigned perms = ei->get_mapperms();

        /* Combine elmts[i] with as many following elements as possible. */
        std::vector<MemoryMap::MapElement>::const_iterator ej=ei+1;
#if 0
        while (ej!=elmts.end() && va+sz==ej->get_va() && perms==ej->get_mapperms())
            sz += (ej++)->get_size();
#endif
        ei = ej;

        /* Create a segment */
        SgAsmElfSection *segment = new SegmentBuilder(fhdr, map, va, sz, perms);
        segtab->add_section(segment);
        segment->get_segment_entry()->set_type(SgAsmElfSegmentTableEntry::PT_LOAD);
    }

    /*========================================================================================================================
     * Generate the core file.
     *======================================================================================================================== */

    SgAsmExecutableFileFormat::unparseBinaryFormat(base_name, ef);
    //deleteAST(ef); /*FIXME [RPM 2010-09-18]*/
}

void *
EmulationPolicy::my_addr(uint32_t va)
{
    /* Read from specimen in order to make sure that the memory is allocated and mapped into ROSE. */
    uint32_t word;
    size_t nread = map->read(&word, va, sizeof word);
    if (nread!=sizeof word)
        return NULL;

    /* Obtain mapping information */
    const MemoryMap::MapElement *me = map->find(va);
    ROSE_ASSERT(me!=NULL); /*because the map->read() was successful above*/
    uint8_t *base = (uint8_t*)me->get_base();
    size_t offset = me->get_va_offset(va);
    return base+offset;
}

std::string
EmulationPolicy::read_string(uint32_t va)
{
    std::string retval;
    while (1) {
        uint8_t byte;
        size_t nread = map->read(&byte, va++, 1);
        ROSE_ASSERT(1==nread); /*or we've read past the end of the mapped memory*/
        if (!byte)
            return retval;
        retval += byte;
    }
}

std::vector<std::string>
EmulationPolicy::read_string_vector(uint32_t va)
{
    std::vector<std::string> vec;
    size_t size = 4;
    while(1) {
      char buf[size];
      size_t nread = map->read(buf, va, size);

      ROSE_ASSERT(nread == size);

      uint64_t result = 0;
      for (size_t i=0, j=0; i<size; i+=8, j++)
                result |= buf[j] << i;

      //FIXME (?) is this the correct test for memory being null?
      if ( result == 0 ) break;

      vec.push_back(read_string( result  ));
      
      va+=4;
    }
    return vec;
}

#define SEMOP		 1
#define SEMGET		 2
#define SEMCTL		 3
#define SEMTIMEDOP	 4
#define MSGSND		11
#define MSGRCV		12
#define MSGGET		13
#define MSGCTL		14
#define SHMAT		21
#define SHMDT		22
#define SHMGET		23
#define SHMCTL		24


int 
EmulationPolicy::ipc_kernel(uint32_t call, int32_t first, int32_t second, int32_t third, uint32_t ptr, uint8_t ptr_addr, uint32_t fifth)
{
	//int version, ret;
    size_t size_ptr;
    int result;

    if(ptr)
    {
        size_t nread = map->read(&ptr_addr, ptr, 1);
        ROSE_ASSERT(nread == 1);
    }


    /* semop system calls takes an array of these. */
    struct sembuf {
      unsigned short  sem_num;    /* semaphore index in array */
      short       sem_op;     /* semaphore operation */
      short       sem_flg;    /* operation flags */
    };

    switch(call)
    {

      case SEMGET: /* semget */
        /* Equivalent to 
             int semget(key_t key, int nsems, int semflg);
           
           The types are described as
              typedef signed int s32;
              typedef s32 compat_key_t;
        */
        return syscall(117, first, second, third, NULL, fifth);
        //return semget(first, second, third);
        break;

      case SEMCTL: {
                     /*

                        This function has three or four arguments, depending on cmd.  When there are four, the fourth has the 
                        type union semun.  The  calling program must define this union as follows:

                        union semun {
                        int              val;    // Value for SETVAL 
                        struct semid_ds *buf;    // Buffer for IPC_STAT, IPC_SET 
                        unsigned short  *array;  // Array for GETALL, SETALL 
                        struct seminfo  *__buf;  // Buffer for IPC_INFO
                        (Linux specific) 
                        };

                        The semid_ds data structure is defined in <sys/sem.h> as follows:

                        struct semid_ds {
                        struct ipc_perm sem_perm;  // Ownership and permissions
                        time_t          sem_otime; // Last semop time 
                        time_t          sem_ctime; // Last change time 
                        unsigned short  sem_nsems; // No. of semaphores in set 
                        };

                        The ipc_perm structure is defined in <sys/ipc.h> as follows (the highlighted fields are settable using IPC_SET):

                        struct ipc_perm {
                        key_t key;            // Key supplied to semget() 
                        uid_t uid;            // Effective UID of owner 
                        gid_t gid;            // Effective GID of owner 
                        uid_t cuid;           // Effective UID of creator 
                        gid_t cgid;           // Effective GID of creator 
                        unsigned short mode;  // Permissions 
                        unsigned short seq;   // Sequence number 
                        };

                        cmd values that ignore semun (include/linux/ipc.h in linux kernel):
                        IPC_STAT 2
                        IPC_SET  1
                        IPC_RMID 0

                        cmd values where semun is used (include/linux/sem.h in linux kernel):
                        IPC_INFO 3
                        SEM_INFO 19
                        SEM_STAT 18
                        GETALL   13
                        GETNCNT  14
                        GETPID   11
                        GETVAL   12
                        GETZCNT7 15
                        SETALL   17
                        SETVAL   16

                      */  
                       /*
                          union semun fourth;
                          if (!ptr)
                          return -EINVAL;
                          if (get_user(fourth.__pad, (void __user * __user *) ptr))
                          return -EFAULT;
                          return sys_semctl(first, second, third, fourth);
                        */

                       //union semun fourth;

                       if (!ptr)
                           return -EINVAL;
                       //if (get_user(fourth.__pad, (void __user * __user *) ptr))
                       //    return -EFAULT;

                       switch(third)
                       {
                           case IPC_STAT:
                           case IPC_SET:
                           case IPC_RMID:


                           std::cout << "Error: semun not required in SEMCTL. Not implemented." << std::endl;
                           exit(1);

                               break;
                           case IPC_INFO:
                               std::cout <<"IPC_INFO" << std::endl;

                               std::cout << "Error: semun required in SEMCTL. Not implemented." << std::endl;
                               exit(1);
                               break;
                           case SEM_INFO:
 
                               std::cout << "Error: semun required in SEMCTL. Not implemented." << std::endl;
                               exit(1);
                               break;                              std::cout <<"SEM_INFO" << std::endl;

                           case SEM_STAT:
                               std::cout <<"SEM_STAT" << std::endl;

                               std::cout << "Error: semun required in SEMCTL. Not implemented." << std::endl;
                               exit(1);
                               break;
                           case GETALL:
                               std::cout <<"GETALL" << std::endl;

                               std::cout << "Error: semun required in SEMCTL. Not implemented." << std::endl;
                               exit(1);
                               break;
                           case GETNCNT:
                               std::cout <<"GETNCNT" << std::endl;

                               std::cout << "Error: semun required in SEMCTL. Not implemented." << std::endl;
                               exit(1);
                               break;
                           case GETPID:
                               std::cout <<"GETPID" << std::endl;

                               std::cout << "Error: semun required in SEMCTL. Not implemented." << std::endl;
                               exit(1);
                               break;
                           case GETVAL:
                               std::cout <<"GETVAL" << std::endl;

                               std::cout << "Error: semun required in SEMCTL. Not implemented." << std::endl;
                               exit(1);
                               break;
                           case GETZCNT:
                               std::cout <<"GETZCNT7" << std::endl;

                               std::cout << "Error: semun required in SEMCTL. Not implemented." << std::endl;
                               exit(1);
                               break;
                           case SETALL:
                               std::cout <<"SETALL" << std::endl;

                               std::cout << "Error: semun required in SEMCTL. Not implemented." << std::endl;
                               exit(1);
                               break;
                           case SETVAL:

                               std::cout << "Error: semun required in SEMCTL. Not implemented." << std::endl;
                               exit(1);
                               break;

                           default:
                           std::cout << "Error: semun required in SEMCTL. Not implemented." << std::endl;
                           ROSE_ASSERT(false == true);

                           exit(1);
                           break;

                       };

                       //return semctl(first, second, third, fourth);

        std::cerr << "Error: system call ipc case SEMCTL not implemented" << std::endl;

        ROSE_ASSERT(false == true);

        exit(1);

                     break;
                   }

#if 1

      case SEMOP:
        //return sys_semtimedop(first, (struct sembuf __user *)ptr,
        //    second, NULL);
 
        //result = syscall( 117, fi, (long) sys_path.c_str(), mode, flags);
        std::cerr << "Error: system call ipc case SEMOP not implemented" << std::endl;
        exit(1);

        size_ptr = sizeof(sembuf);

        break;
      case SEMTIMEDOP:
       /* return sys_semtimedop(first, (struct sembuf __user *)ptr,
            second,
            (const struct timespec __user *)fifth);
      */
        std::cerr << "Error: system call ipc case SEMTIMEDOP not implemented" << std::endl;
        exit(1);

        size_ptr = sizeof(sembuf);

        break;
      case MSGSND:
                   //return sys_msgsnd(first, (struct msgbuf __user *) ptr,
                    //   second, third);

                   std::cerr << "Error: system call ipc case MSGSND not implemented" << std::endl;
                   exit(1);

                   break;
      case MSGRCV:
                   /*
                   switch (version) {
                     case 0: {
                               struct ipc_kludge tmp;
                               if (!ptr)
                                 return -EINVAL;

                               if (copy_from_user(&tmp,
                                     (struct ipc_kludge __user *) ptr,
                                     sizeof(tmp)))
                                 return -EFAULT;
                               return sys_msgrcv(first, tmp.msgp, second,
                                   tmp.msgtyp, third);
                             }
                     default:
                             return sys_msgrcv(first,
                                 (struct msgbuf __user *) ptr,
                                 second, fifth, third);
                   }
                   */
        std::cerr << "Error: system call ipc case MSGRCV not implemented" << std::endl;
        exit(1);

                   break;
      case MSGGET:
                   //return sys_msgget((key_t) first, second);
        std::cerr << "Error: system call ipc case MSGGET not implemented" << std::endl;
        exit(1);

                   break;
      case MSGCTL:
                   //return sys_msgctl(first, second, (struct msqid_ds __user *)ptr);
        std::cerr << "Error: system call ipc case MSGCTL not implemented" << std::endl;
        exit(1);

                   break;

      case SHMAT:
                   /*
                   switch (version) {
                     default: {
                                unsigned long raddr;
                                ret = do_shmat(first, (char __user *)ptr,
                                    second, &raddr);
                                if (ret)
                                  return ret;
                                return put_user(raddr, (unsigned long __user *) third);
                              }
                     case 1:
                              return -EINVAL;
                   }
    */
        std::cerr << "Error: system call ipc case SHMAT not implemented" << std::endl;
        exit(1);

                   break;
      case SHMDT:
                   //return sys_shmdt((char __user *)ptr);
        std::cerr << "Error: system call ipc case SHMT not implemented" << std::endl;
        exit(1);

                   break;
      case SHMGET:
                   //return sys_shmget(first, second, third);
        std::cerr << "Error: system call ipc case SHMGET not implemented" << std::endl;
        exit(1);

                   break;
      case SHMCTL:
                   //return sys_shmctl(first, second,
                   //    (struct shmid_ds __user *) ptr);
        std::cerr << "Error: system call ipc case SGMCTL not implemented" << std::endl;
        exit(1);

                   break;

#endif

      default:
                   std::cout << "Call is: " << call << std::endl;
                   //return -ENOSYS;
   
                   exit(1);
                   break;
    }


    //syscall(117,first, second, third, ptr, fifth);

    return result;
#if 0
	version = call >> 16; /* hack for backward compatibility */
	call &= 0xffff;

	switch (call) {
	case SEMOP:
		return sys_semtimedop(first, (struct sembuf __user *)ptr,
				      second, NULL);
	case SEMTIMEDOP:
		return sys_semtimedop(first, (struct sembuf __user *)ptr,
				      second,
				      (const struct timespec __user *)fifth);

	case SEMGET:
		return sys_semget(first, second, third);
	case SEMCTL: {
		union semun fourth;
		if (!ptr)
			return -EINVAL;
		if (get_user(fourth.__pad, (void __user * __user *) ptr))
			return -EFAULT;
		return sys_semctl(first, second, third, fourth);
	}

	case MSGSND:
		return sys_msgsnd(first, (struct msgbuf __user *) ptr,
				  second, third);
	case MSGRCV:
		switch (version) {
		case 0: {
			struct ipc_kludge tmp;
			if (!ptr)
				return -EINVAL;

			if (copy_from_user(&tmp,
					   (struct ipc_kludge __user *) ptr,
					   sizeof(tmp)))
				return -EFAULT;
			return sys_msgrcv(first, tmp.msgp, second,
					   tmp.msgtyp, third);
		}
		default:
			return sys_msgrcv(first,
					   (struct msgbuf __user *) ptr,
					   second, fifth, third);
		}
	case MSGGET:
		return sys_msgget((key_t) first, second);
	case MSGCTL:
		return sys_msgctl(first, second, (struct msqid_ds __user *)ptr);

	case SHMAT:
		switch (version) {
		default: {
			unsigned long raddr;
			ret = do_shmat(first, (char __user *)ptr,
				       second, &raddr);
			if (ret)
				return ret;
			return put_user(raddr, (unsigned long __user *) third);
		}
		case 1:
			/*
			 * This was the entry point for kernel-originating calls
			 * from iBCS2 in 2.2 days.
			 */
			return -EINVAL;
		}
	case SHMDT:
		return sys_shmdt((char __user *)ptr);
	case SHMGET:
		return sys_shmget(first, second, third);
	case SHMCTL:
		return sys_shmctl(first, second,
				   (struct shmid_ds __user *) ptr);
	default:
		return -ENOSYS;
	}

#endif
}
void
EmulationPolicy::emulate_syscall()
{
    /* Warning: use hard-coded values here rather than the __NR_* constants from <sys/unistd.h> because the latter varies
     *          according to whether ROSE is compiled for 32- or 64-bit.  We always want the 32-bit syscall numbers. */
    unsigned callno = readGPR(x86_gpr_ax).known_value();

    struct kernel_timeval {
        uint32_t tv_sec;        /* seconds */
        uint32_t tv_usec;       /* microseconds */
    };



    switch (callno) {
        case 3: { /*read*/
            syscall_enter("read", "dpd");
            int fd=arg(0);
            uint32_t buf_va=arg(1), size=arg(2);
            char buf[size];
            ssize_t nread = read(fd, buf, size);
            if (nread<0) {
                writeGPR(x86_gpr_ax, -errno);
            } else {
                writeGPR(x86_gpr_ax, nread);
                map->write(buf, buf_va, nread);
            }
            syscall_leave("d");
            break;
        }

        case 4: { /*write*/
            syscall_enter("write", "dpd");
            int fd=arg(0);
            uint32_t buf_va=arg(1);
            size_t size=arg(2);
            uint8_t buf[size];
            size_t nread = map->read(buf, buf_va, size);
            ROSE_ASSERT(nread==size);
            ssize_t nwritten = write(fd, buf, size);
            if (-1==nwritten) {
                writeGPR(x86_gpr_ax, -errno);
            } else {
                writeGPR(x86_gpr_ax, nwritten);
            }
            syscall_leave("d");
            break;
        }

        case 5: { /*open*/
            static const Translate oflags[] = { TF(O_RDWR), TF(O_RDONLY), TF(O_WRONLY),
                                                TF(O_CREAT), TF(O_EXCL), TF(O_NONBLOCK), TF(O_NOCTTY), TF(O_TRUNC),
                                                TF(O_APPEND), TF(O_NDELAY), TF(O_ASYNC), TF(O_FSYNC), TF(O_SYNC), TF(O_NOATIME),
                                                T_END };
            syscall_enter("open", "sf", oflags);
            uint32_t filename_va=arg(0);
            std::string filename = read_string(filename_va);
            uint32_t flags=arg(1), mode=(flags & O_CREAT)?arg(2):0;
            int fd = open(filename.c_str(), flags, mode);

            if( fd <= 256 ) // 256 is getdtablesize() in the simulator
                writeGPR(x86_gpr_ax, fd<0 ? -errno : fd);
            else {
                writeGPR(x86_gpr_ax, -EMFILE);
                close(fd);
            }
            syscall_leave("d");
            break;
        }

        case 6: { /*close*/
            syscall_enter("close", "d");
            int fd=arg(0);
            if (1==fd || 2==fd) {
                /* ROSE is using these */
                writeGPR(x86_gpr_ax, -EPERM);
            } else {
                int status = close(fd);
                writeGPR(x86_gpr_ax, status<0 ? -errno : status);
            }
            syscall_leave("d");
            break;
        }

        case 7: { // waitpid
            static const Translate wflags[] = { TF(WNOHANG), TF(WUNTRACED), T_END };
            syscall_enter("waitpid", "dpf", wflags);
            pid_t pid=arg(0);
            uint32_t status_va=arg(1);
            int options=arg(2);
            int sys_status;
            int result = waitpid(pid, &sys_status, options);
            if (result == -1) {
                result = -errno;
            } else if (status_va) {
                uint32_t status_le;
                SgAsmExecutableFileFormat::host_to_le(sys_status, &status_le);
                size_t nwritten = map->write(&status_le, status_va, 4);
                ROSE_ASSERT(4==nwritten);
            }
            writeGPR(x86_gpr_ax, result);
            syscall_leave("d");
            break;
        }
             
        case 8: { /* 0x8, creat */
            syscall_enter("creat", "sd");
     	    uint32_t filename = arg(0);
            std::string sys_filename = read_string(filename);
	        mode_t mode = arg(1);

	        int result = creat(sys_filename.c_str(), mode);
            if (result == -1) result = -errno;
            writeGPR(x86_gpr_ax, result);

            syscall_leave("d");
            break;
	    }  
   
        case 10: { /*0xa, unlink*/
            syscall_enter("unlink", "s");
            uint32_t filename_va = arg(0);
            std::string filename = read_string(filename_va);
            int result = unlink(filename.c_str());
            if (result == -1) 
                result = -errno;
            writeGPR(x86_gpr_ax, result);
            syscall_leave("d");
            break;
        }

	case 11: { /* 0xb, execve */
            syscall_enter("execve", "spp");
            std::string filename = read_string(arg(0));
            std::vector<std::string > argv = read_string_vector(arg(1));
            std::vector<std::string > envp = read_string_vector(arg(2));
            std::vector<char*> sys_argv;
            for (unsigned int i = 0; i < argv.size(); ++i) sys_argv.push_back(&argv[i][0]);
            sys_argv.push_back(NULL);
            std::vector<char*> sys_envp;
            for (unsigned int i = 0; i < envp.size(); ++i) sys_envp.push_back(&envp[i][0]);
            sys_envp.push_back(NULL);
            int result;
            if (std::string(&filename[0]) == "/usr/bin/man") {
                result = -EPERM;
            } else {
                result = execve(&filename[0], &sys_argv[0], &sys_envp[0]);
                if (result == -1) result = -errno;
            }
            writeGPR(x86_gpr_ax, result);
            syscall_leave("d");
            break;
        }

	case 12: { /* 0xc, chdir */
            syscall_enter("chdir", "s");
	    uint32_t path = arg(0);
            std::string sys_path = read_string(path);

	    int result = chdir(sys_path.c_str());
            if (result == -1) result = -errno;
            writeGPR(x86_gpr_ax, result);

            syscall_leave("d");
            break;
	}

        case 13: { /*0xd, time */
            syscall_enter("time", "p");
            time_t result = time(NULL);
            if (arg(0)) {
                uint32_t t_le;
                SgAsmExecutableFileFormat::host_to_le(result, &t_le);
                size_t nwritten = map->write(&t_le, arg(0), 4);
                ROSE_ASSERT(4==nwritten);
            }
            writeGPR(x86_gpr_ax, result);
            syscall_leave("t");
            break;
        }

        case 14: { /*0xe, mknod*/
            syscall_enter("mknod", "sdd");
            uint32_t path_va = arg(0);
            int mode = arg(1);
            unsigned dev = arg(2);
            std::string path = read_string(path_va);
            int result = mknod(path.c_str(), mode, dev);
            writeGPR(x86_gpr_ax, -1==result ? -errno : result);
            syscall_leave("d");
            break;
        }

	case 15: { /* 0xf, chmod */
            syscall_enter("chmod", "sd");
	    uint32_t filename = arg(0);
            std::string sys_filename = read_string(filename);
	    mode_t mode = arg(1);
	    int result = chmod(sys_filename.c_str(), mode);
            if (result == -1) result = -errno;
            writeGPR(x86_gpr_ax, result);
            syscall_leave("d");
            break;
	}

        case 20: { /*0x14, getpid*/
            syscall_enter("getpid", "");
            writeGPR(x86_gpr_ax, getpid());
            syscall_leave("d");
            break;
        }

        case 24: { /*0x18, getuid*/
            syscall_enter("getuid", "");
            writeGPR(x86_gpr_ax, getuid());
            syscall_leave("d");
            break;
        }

        case 30: { /* 0x1e, utime */

            /*
               int utime(const char *filename, const struct utimbuf *times);

               The utimbuf structure is:

               struct utimbuf {
               time_t actime;       // access time 
                   time_t modtime;  // modification time 
                 };

               The utime() system call changes the access and modification times of the inode
               specified by filename to the actime and modtime fields of times respectively.

               If times is NULL, then the access and modification times of the file are set
               to the current time.
            */
            syscall_enter("utime", "sp");

            std::string filename = read_string(arg(0));

            //Check to see if times is NULL
            uint8_t byte;
            size_t nread = map->read(&byte, arg(1), 1);
            ROSE_ASSERT(1==nread); /*or we've read past the end of the mapped memory*/

            int result;
            if( byte)
            {
              struct kernel_utimebuf {
                uint32_t actime;
                uint32_t modtime;
              };

              kernel_utimebuf ubuf;
              size_t nread = map->read(&ubuf, arg(1), sizeof(kernel_utimebuf));
              ROSE_ASSERT(nread == sizeof(kernel_utimebuf));

              utimbuf ubuf64;
              ubuf64.actime  = ubuf.actime;
              ubuf64.modtime = ubuf.modtime;

              result = utime(filename.c_str(), &ubuf64);

            }else
              result = utime(filename.c_str(), NULL);

            writeGPR(x86_gpr_ax, result);
            syscall_leave("d");

            break;
        };

        case 33: { /*0x21, access*/
            static const Translate flags[] = { TF(R_OK), TF(W_OK), TF(X_OK), TF(F_OK), T_END };
            syscall_enter("access", "sf", flags);
            uint32_t name_va=arg(0);
            std::string name = read_string(name_va);
            int mode=arg(1);
            int result = access(name.c_str(), mode);
            if (-1==result) result = -errno;
            writeGPR(x86_gpr_ax, result);
            syscall_leave("d");
            break;
        }

	case 37: { /* 0x25, kill */
            syscall_enter("kill", "df", signal_names);
            pid_t pid=arg(0);
            int sig=arg(1);
            int result = kill(pid, sig);
            if (result == -1) result = -errno;
            writeGPR(x86_gpr_ax, result);
            syscall_leave("d");
            break;
        }

	case 39: { /* 0x27, mkdir */
            syscall_enter("mkdir", "sd");
	    uint32_t pathname = arg(0);
            std::string sys_pathname = read_string(pathname);
	    mode_t mode = arg(1);

	    int result = mkdir(sys_pathname.c_str(), mode);
            if (result == -1) result = -errno;
            writeGPR(x86_gpr_ax, result);

            syscall_leave("d");
            break;
	}

	case 40: { /* 0x28, rmdir */
            syscall_enter("rmdir", "s");
	    uint32_t pathname = arg(0);
            std::string sys_pathname = read_string(pathname);

	    int result = rmdir(sys_pathname.c_str());
            if (result == -1) result = -errno;
            writeGPR(x86_gpr_ax, result);

            syscall_leave("d");
            break;
	}

        case 41: { /*0x29, dup*/
            syscall_enter("dup", "d");
            uint32_t fd = arg(0);
            int result = dup(fd);
            if (-1==result) result = -errno;
            writeGPR(x86_gpr_ax, result);
            syscall_leave("d");
            break;
        }

        case 42: { /*0x2a, pipe*/
            /*
               int pipe(int filedes[2]); 

               pipe() creates a pair of file descriptors, pointing to a pipe inode, and 
               places them in the array pointed to by filedes. filedes[0] is for reading, 
               filedes[1] is for writing. 

            */
            syscall_enter("pipe", "p");


            int32_t filedes_kernel[2];
            size_t  size_filedes = sizeof(int32_t)*2;


            int filedes[2];
            int result = pipe(filedes);

            filedes_kernel[0] = filedes[0];
            filedes_kernel[1] = filedes[1];

            map->write(filedes_kernel, arg(0), size_filedes);


            if (-1==result) result = -errno;
            writeGPR(x86_gpr_ax, result);
            syscall_leave("d");
            break;
        }

        case 45: { /*0x2d, brk*/
            syscall_enter("brk", "x");
            uint32_t newbrk = arg(0);
            int retval = 0;

            if (newbrk >= 0xb0000000ul) {
                retval = -ENOMEM;
            } else {
                if (newbrk > brk_va) {
                    MemoryMap::MapElement melmt(brk_va, newbrk-brk_va, MemoryMap::MM_PROT_READ|MemoryMap::MM_PROT_WRITE);
                    melmt.set_name("brk syscall");
                    map->insert(melmt);
                    brk_va = newbrk;
                } else if (newbrk>0 && newbrk<brk_va) {
                    map->erase(MemoryMap::MapElement(newbrk, brk_va-newbrk));
                    brk_va = newbrk;
                }
                retval= brk_va;
            }
            if (debug && newbrk!=0 && trace_mmap) {
                fprintf(debug, "  memory map after brk syscall:\n");
                map->dump(debug, "    ");
            }

            writeGPR(x86_gpr_ax, retval);
            syscall_leave("p");
            break;
        }

        case 47: { /*0x2f, getgid*/
            syscall_enter("getgid", "");
            writeGPR(x86_gpr_ax, getgid());
            syscall_leave("d");
            break;
        }

        case 49: { /*0x31, geteuid*/
            syscall_enter("geteuid", "");
            writeGPR(x86_gpr_ax, geteuid());
            syscall_leave("d");
            break;
        }

        case 50: { /*0x32, getegid*/
            syscall_enter("getegid", "");
            writeGPR(x86_gpr_ax, getegid());
            syscall_leave("d");
            break;
        }

        case 54: { /*0x36, ioctl*/
            static const Translate ioctl_cmd[] = {
                TE(TCGETS), TE(TCSETS), TE(TCSETSW), TE(TCGETA), TE(TIOCGPGRP), TE(TIOCSPGRP), TE(TIOCSWINSZ), TE(TIOCGWINSZ),
                T_END};
            syscall_enter("ioctl", "dfd", ioctl_cmd);
            
            int fd=arg(0);
            uint32_t cmd=arg(1), arg2=arg(2);
            int result = -ENOSYS;
            switch (cmd) {
                case TCGETS: { /* 0x00005401, tcgetattr*/
                    struct termios ti;
                    result = tcgetattr(fd, &ti);
                    if (-1==result) {
                        result = -errno;
                    } else {
                        /* The Linux kernel and glibc have different definitions for termios, with very different sizes (39
                         * bytes vs 60) */                  
                        size_t nwritten = map->write(&ti, arg2, 39);
                        ROSE_ASSERT(39==nwritten);
                    }
                    break;
                }

                case TCSETS:  /* 0x,00005402,  tcsetattr */
                    /* TCSETS const struct termios *argp
                       Equivalent to
                          int tcsetattr(int fd, int optional_actions, const struct termios *termios_p);
                          tcsetattr(fd, TCSANOW, argp).
                       Set the current serial port settings. 
                    */
                case TCSETSW: { /* 0x00005403, tcsetattr  */
                    /* Equivalent to 
                         int tcsetattr(int fd, int optional_actions, const struct termios *termios_p);
                         tcsetattr(fd, TCSADRAIN, argp).
                       Allow the output buffer to drain, and set the current serial port settings. 

                       Value of second argument is the only difference between TCSETS and TCSETSW

                       typedef unsigned int     tcflag_t;
                       typedef unsigned char    cc_t;
                       typedef unsigned int     speed_t;

                       struct termios {
                         tcflag_t c_iflag;
                         tcflag_t c_oflag;
                         tcflag_t c_cflag;
                         tcflag_t c_lflag;
                         cc_t c_cc[NCCS];
                         speed_t c_ispeed;
                         speed_t c_ospeed;
                       };


                     */
                    uint32_t optional_actions = arg(2);

                    //Convert between 32 bit termios and whatever is
                    //used on this machine
                    struct termios_kernel {
                         uint32_t c_iflag;
                         uint32_t c_oflag;
                         uint32_t c_cflag;
                         uint32_t c_lflag;
                         unsigned char c_cc[NCCS];
                         uint32_t c_ispeed;
                         uint32_t c_ospeed;
                    };

                    termios_kernel tik;

                    size_t nread = map->read(&tik, arg(3), sizeof(termios_kernel));
                    ROSE_ASSERT(sizeof(termios_kernel) == nread);
 
                    struct termios ti;

                    CONV_FIELD(ti,tik,c_iflag);
                    CONV_FIELD(ti,tik,c_oflag);
                    CONV_FIELD(ti,tik,c_cflag);
                    CONV_FIELD(ti,tik,c_lflag);
                    CONV_FIELD(ti,tik,c_ispeed);

                    for (int i = 0 ; i < NCCS ; i++ )
                      ti.c_cc[i] = tik.c_cc[i];

                    CONV_FIELD(ti,tik,c_ospeed);

                    result = tcsetattr(fd, optional_actions, &ti);

                    if (result==-1) {
                        result = -errno;
                    } 
  
                    break;
                }

                case TCGETA: { /* 0x,00005405 */
                    /* gets a data structure of type 
                           struct termio * 

                       struct termio {
                         unsigned short c_iflag;     // input mode flags 
                         unsigned short c_oflag;     // output mode flags 
                         unsigned short c_cflag;     // control mode flags 
                         unsigned short c_lflag;     // local mode flags 
                         unsigned char c_line;       // line discipline 
                         unsigned char c_cc[NCC];    // control characters 
                       };

                     */

                    termio to;

                    result = ioctl(fd, TCGETA, &to);
                    if (-1==result) {
                        result = -errno;
                    } else {
                        size_t nwritten = map->write(&to, arg2, sizeof to);
                        ROSE_ASSERT(nwritten==sizeof to);
                    }

                    break;
                }

                case TIOCGPGRP: { /* 0x0000540F, tcgetpgrp*/
                    /* equivalent to 
                        pid_t tcgetpgrp(int fd);
                       The  function tcgetpgrp() returns the process group ID of the foreground process group 
                       on the terminal associated to fd, which must be the controlling terminal of the calling 
                       process.
                    */

                    pid_t pgrp = tcgetpgrp(fd);
                    if (-1==pgrp) {
                        result = -errno;
                    } else {
                        uint32_t pgrp_le;
                        SgAsmExecutableFileFormat::host_to_le(pgrp, &pgrp_le);
                        size_t nwritten = map->write(&pgrp_le, arg2, 4);
                        ROSE_ASSERT(4==nwritten);
                        result = 0;
                    }
                    break;
                }
                    
                case TIOCSPGRP: { /* 0x5410, tcsetpgrp*/
                    uint32_t pgid_le;
                    size_t nread = map->read(&pgid_le, arg2, 4);
                    ROSE_ASSERT(4==nread);
                    pid_t pgid = SgAsmExecutableFileFormat::le_to_host(pgid_le);
                    result = tcsetpgrp(fd, pgid);
                    if (-1==result)
                        result = -errno;
                    break;
                }

                case TIOCSWINSZ: { /* 0x5413, the winsize is const */
                    /* Set window size.
                       struct winsize {
                          unsigned short ws_row;
                          unsigned short ws_col;
                          unsigned short ws_xpixel;   // unused 
                          unsigned short ws_ypixel;   // unused 
                       };
                    */
                    winsize ws;
                    size_t nread = map->read(&ws, arg(2), sizeof(winsize));
                    ROSE_ASSERT(sizeof(winsize) == nread);

                    result = ioctl(fd, TIOCSWINSZ, &ws);
                    if (-1==result)
                        result = -errno;
                    break;
                }

                case TIOCGWINSZ: /* 0x5414, */ {
                    struct winsize ws;
                    result = ioctl(fd, TIOCGWINSZ, &ws);
                    if (-1==result) {
                        result = -errno;
                    } else {
                        size_t nwritten = map->write(&ws, arg2, sizeof ws);
                        ROSE_ASSERT(nwritten==sizeof ws);
                    }
                    break;
                }
                default:
                    fprintf(stderr, "  unhandled ioctl: %u\n", cmd);
                    abort();
            }
            writeGPR(x86_gpr_ax, result);
            syscall_leave("d");
            break;
        }

        case 57: { /*0x39, setpgid*/
            syscall_enter("setpgid", "dd");
            pid_t pid=arg(0), pgid=arg(1);
            int result = setpgid(pid, pgid);
            if (-1==result) { result = -errno; }
            writeGPR(x86_gpr_ax, result);
            syscall_leave("d");
            break;
        }

        case 64: { /*0x40, getppid*/
            syscall_enter("getppid", "");
            writeGPR(x86_gpr_ax, getppid());
            syscall_leave("d");
            break;
        }

        case 65: { /*0x41, getpgrp*/
            syscall_enter("getpgrp", "");
            writeGPR(x86_gpr_ax, getpgrp());
            syscall_leave("d");
            break;
        }

        case 78: { /*0x4e, gettimeofday*/       
            syscall_enter("gettimeofday", "p");
            uint32_t tp = arg(0);
            struct timeval sys_t;
            int result = gettimeofday(&sys_t, NULL);
            if (result == -1) {
                result = -errno;
            } else {
                writeMemory<32>(x86_segreg_ds, tp, sys_t.tv_sec, true_() );
                writeMemory<32>(x86_segreg_ds, tp + 4, sys_t.tv_usec, true_() );
            }
            writeGPR(x86_gpr_ax, result);
            syscall_leave("d");
            break;
        }

        case 83: { /*0x53, symlink*/
            syscall_enter("symlink", "ss");
            uint32_t oldpath=arg(0), newpath=arg(1);
            std::string sys_oldpath = read_string(oldpath);
            std::string sys_newpath = read_string(newpath);
            int result = symlink(sys_oldpath.c_str(),sys_newpath.c_str());
            if (result == -1) result = -errno;
            writeGPR(x86_gpr_ax, result);
            syscall_leave("d");
            break;
        }

        case 85: { /*0x55, readlink*/
            syscall_enter("readlink", "spd");
            uint32_t path=arg(0), buf_va=arg(1), bufsize=arg(2);
            char sys_buf[bufsize];
            std::string sys_path = read_string(path);
            int result = readlink(sys_path.c_str(), sys_buf, bufsize);
            if (result == -1) {
                result = -errno;
            } else {
                size_t nwritten = map->write(sys_buf, buf_va, result);
                ROSE_ASSERT(nwritten == (size_t)result);
            }
            writeGPR(x86_gpr_ax, result);
            syscall_leave("d");
            break;
        }
            
        case 91: { /*0x5b, munmap*/
            syscall_enter("munmap", "pd");
            uint32_t va=arg(0);
            uint32_t sz=arg(1);
            uint32_t aligned_va = ALIGN_DN(va, PAGE_SIZE);
            uint32_t aligned_sz = ALIGN_UP(sz+va-aligned_va, PAGE_SIZE);
            map->erase(MemoryMap::MapElement(aligned_va, aligned_sz));
            if (debug && trace_mmap) {
                fprintf(debug, " memory map after munmap syscall:\n");
                map->dump(debug, "    ");
            }
            writeGPR(x86_gpr_ax, 0);
            syscall_leave("d");
            break;
        }

        case 94: { /* 0x5d, fchmod */

            /*
                int fchmod(int fd, mode_t mode);

                fchmod() changes the permissions of the file referred to by the open file
                         descriptor fd.
            */
            syscall_enter("fchmod", "dd");
	        uint32_t fd = arg(0);
	        mode_t mode = arg(1);

	        int result = fchmod(fd, mode);
            if (result == -1) result = -errno;
            writeGPR(x86_gpr_ax, result);

            syscall_leave("d");
            break;
	    }

     	case 95: { /*0x5f, fchown */
            /*
                   int fchown(int fd, uid_t owner, gid_t group);

                   typedef unsigned short  __kernel_old_uid_t;
                   typedef unsigned short  __kernel_old_gid_t;

                   fchown() changes the ownership of the file referred to by the open file
                            descriptor fd.

               */

            syscall_enter("fchown16", "ddd");
	        uint32_t fd = arg(0);
            unsigned short  user = arg(1);
	        unsigned short  group = arg(2);
	        int result = syscall(95,fd,user,group);
            writeGPR(x86_gpr_ax, result);
            syscall_leave("d");
            break;
        }

        case 102: { // socketcall
            syscall_enter("socketcall", "dp");
            //uint32_t call=arg(0), args=arg(1);
            writeGPR(x86_gpr_ax, -ENOSYS);
            syscall_leave("d");
            break;
        }

        case 114: { /*0x72, wait4*/
            static const Translate wflags[] = { TF(WNOHANG), TF(WUNTRACED), T_END };
            syscall_enter("wait4", "dpfp", wflags);
            pid_t pid=arg(0);
            uint32_t status_va=arg(1), rusage_va=arg(3);
            int options=arg(2);
            int status;
            struct rusage rusage;
            int result = wait4(pid, &status, options, &rusage);
            if( result == -1) {
                result = -errno;
            } else {
                if (status_va != 0) {
                    size_t nwritten = map->write(&status, status_va, 4);
                    ROSE_ASSERT(nwritten == 4);
                }
                if (rusage_va != 0) {
                    struct rusage_32 {
                        uint32_t utime_sec;     /* user time used; seconds */
                        uint32_t utime_usec;    /* user time used; microseconds */
                        uint32_t stime_sec;     /* system time used; seconds */
                        uint32_t stime_usec;    /* system time used; microseconds */
                        uint32_t maxrss;        /* maximum resident set size */
                        uint32_t ixrss;         /* integral shared memory size */
                        uint32_t idrss;         /* integral unshared data size */
                        uint32_t isrss;         /* integral unshared stack size */
                        uint32_t minflt;        /* page reclaims */
                        uint32_t majflt;        /* page faults */
                        uint32_t nswap;         /* swaps */
                        uint32_t inblock;       /* block input operations */
                        uint32_t oublock;       /* block output operations */
                        uint32_t msgsnd;        /* messages sent */
                        uint32_t msgrcv;        /* messages received */
                        uint32_t nsignals;      /* signals received */
                        uint32_t nvcsw;         /* voluntary context switches */
                        uint32_t nivcsw;        /* involuntary " */
                    } __attribute__((packed));
                    struct rusage_32 out;
                    ROSE_ASSERT(18*4==sizeof(out));
                    out.utime_sec = rusage.ru_utime.tv_sec;
                    out.utime_usec = rusage.ru_utime.tv_usec;
                    out.stime_sec = rusage.ru_stime.tv_sec;
                    out.stime_usec = rusage.ru_stime.tv_usec;
                    out.maxrss = rusage.ru_maxrss;
                    out.ixrss = rusage.ru_ixrss;
                    out.idrss = rusage.ru_idrss;
                    out.isrss = rusage.ru_isrss;
                    out.minflt = rusage.ru_minflt;
                    out.majflt = rusage.ru_majflt;
                    out.nswap = rusage.ru_nswap;
                    out.inblock = rusage.ru_inblock;
                    out.msgsnd = rusage.ru_msgsnd;
                    out.msgrcv = rusage.ru_msgrcv;
                    out.nsignals = rusage.ru_nsignals;
                    out.nvcsw = rusage.ru_nvcsw;
                    out.nivcsw = rusage.ru_nivcsw;
                    size_t nwritten = map->write(&out, rusage_va, sizeof out);
                    ROSE_ASSERT(nwritten == sizeof out);
                }
            }
            writeGPR(x86_gpr_ax, result);
            syscall_leave("d");
            break;
        }

#if 1
        case 116: { /* 0x74, sysinfo*/

            /*

               int sysinfo(struct sysinfo *info);   

               struct sysinfo {
               long uptime;             // Seconds since boot 
                    unsigned long loads[3];  // 1, 5, and 15 minute load averages 
                    unsigned long totalram;  // Total usable main memory size 
                    unsigned long freeram;   // Available memory size 
                    unsigned long sharedram; // Amount of shared memory 
                    unsigned long bufferram; // Memory used by buffers 
                    unsigned long totalswap; // Total swap space size 
                    unsigned long freeswap;  // swap space still available 
                    unsigned short procs;    // Number of current processes 
                    unsigned long totalhigh; // Total high memory size 
                    unsigned long freehigh;  // Available high memory size 
                    unsigned int mem_unit;   // Memory unit size in bytes 
                    char _f[20-2*sizeof(long)-sizeof(int)]; // Padding for libc5 
                  };

            */
            syscall_enter("sysinfo", "p");


            struct kernel_sysinfo {
              int32_t uptime;             /* Seconds since boot */
              uint32_t loads[3];  /* 1, 5, and 15 minute load averages */
              uint32_t totalram;  /* Total usable main memory size */
              uint32_t freeram;   /* Available memory size */
              uint32_t sharedram; /* Amount of shared memory */
              uint32_t bufferram; /* Memory used by buffers */
              uint32_t totalswap; /* Total swap space size */
              uint32_t freeswap;  /* swap space still available */
              unsigned short procs;    /* Number of current processes */
              uint32_t totalhigh; /* Total high memory size */
              uint32_t freehigh;  /* Available high memory size */
              uint32_t mem_unit;   /* Memory unit size in bytes */
              char _f[20-2*sizeof(uint32_t)-sizeof(int32_t)]; /* Padding for libc5 */
            };

            struct sysinfo {
              long uptime;             // Seconds since boot 
              unsigned long loads[3];  // 1, 5, and 15 minute load averages 
              unsigned long totalram;  // Total usable main memory size 
              unsigned long freeram;   // Available memory size 
              unsigned long sharedram; // Amount of shared memory 
              unsigned long bufferram; // Memory used by buffers 
              unsigned long totalswap; // Total swap space size 
              unsigned long freeswap;  // swap space still available 
              unsigned short procs;    // Number of current processes 
              unsigned long totalhigh; // Total high memory size 
              unsigned long freehigh;  // Available high memory size 
              unsigned int mem_unit;   // Memory unit size in bytes 
              char _f[20-2*sizeof(long)-sizeof(int)]; // Padding for libc5 
            };

            sysinfo sys;
            int result  = syscall( 116, &sys );

            kernel_sysinfo kernel_sys;
            kernel_sys.uptime = sys.uptime;
            for(int i = 0 ; i < 3 ; i++)
               kernel_sys.loads[i] = sys.loads[i];
            kernel_sys.totalram    = sys.totalram;
            kernel_sys.freeram     = sys.freeram;
            kernel_sys.sharedram   = sys.sharedram;
            kernel_sys.bufferram   = sys.bufferram;
            kernel_sys.totalswap   = sys.totalswap;
            kernel_sys.freeswap    = sys.freeswap;
            kernel_sys.procs       = sys.procs;
            kernel_sys.totalhigh   = sys.totalhigh;
            kernel_sys.mem_unit    = sys.mem_unit;
            for (size_t i = 0; i < 20-2*sizeof(long)-sizeof(int) ; i++)
              kernel_sys._f[i] = sys._f[i];

            map->write(&kernel_sys, arg(0), sizeof(kernel_sysinfo));

            writeGPR(x86_gpr_ax, result);
            syscall_leave("d");

            break;
        };

        case 117: { /* 0x75, ipc */
            //int ipc(unsigned int call, int first, int second, int third, void *ptr, long fifth)
            syscall_enter("ipc", "ddddpd");
 
            uint32_t call, fifth, ptr;
            int32_t  first, second, third;
            call   = arg(0);
            first  = arg(1);
            second = arg(2);
            third  = arg(3);
            ptr    = arg(4);
            fifth  = arg(5);

            uint8_t ptr_addr;


            int result = ipc_kernel(call, first, second, third, ptr, ptr_addr, fifth);

            writeGPR(x86_gpr_ax, result);
            syscall_leave("d");

            break;
        }
#endif

        case 122: { /*0x7a, uname*/
            syscall_enter("uname", "p");
            uint32_t dest_va=arg(0);
            char buf[6*65];
            memset(buf, ' ', sizeof buf);
            strcpy(buf+0*65, "Linux");                                  /*sysname*/
            strcpy(buf+1*65, "mymachine.example.com");                  /*nodename*/
            strcpy(buf+2*65, "2.6.9");                                  /*release*/
            strcpy(buf+3*65, "#1 SMP Wed Jun 18 12:35:02 EDT 2008");    /*version*/
            strcpy(buf+4*65, "i386");                                   /*machine*/
            strcpy(buf+5*65, "example.com");                            /*domainname*/
            size_t nwritten = map->write(buf, dest_va, sizeof buf);
            if( nwritten <= 0 ) {
              writeGPR(x86_gpr_ax, -EFAULT);
              break;
            }

            ROSE_ASSERT(nwritten==sizeof buf);
            writeGPR(x86_gpr_ax, 0);
            syscall_leave("d");
            break;
        }

	case 133: { /* 0x85, fchdir */
            syscall_enter("fchdir", "d");
	    uint32_t file_descriptor = arg(0);

	    int result = fchdir(file_descriptor);
            if (result == -1) result = -errno;
            writeGPR(x86_gpr_ax, result);

            syscall_leave("d");
            break;
	}

        case 125: { /*0x7d, mprotect*/
            static const Translate pflags[] = { TF(PROT_READ), TF(PROT_WRITE), TF(PROT_EXEC), TF(PROT_NONE), T_END };
            syscall_enter("mprotect", "pdf", pflags);
            uint32_t va=arg(0), size=arg(1), perms=arg(2);
            unsigned rose_perms = ((perms & PROT_READ) ? MemoryMap::MM_PROT_READ : 0) |
                                  ((perms & PROT_WRITE) ? MemoryMap::MM_PROT_WRITE : 0) |
                                  ((perms & PROT_EXEC) ? MemoryMap::MM_PROT_EXEC : 0);
            if (va % PAGE_SIZE) {
                writeGPR(x86_gpr_ax, -EINVAL);
                break;
            }
            uint32_t aligned_sz = ALIGN_UP(size, PAGE_SIZE);

            try {
                map->mprotect(MemoryMap::MapElement(va, aligned_sz, rose_perms));
            } catch (const MemoryMap::NotMapped &e) {
                writeGPR(x86_gpr_ax, -EFAULT);
                break;
            }

            writeGPR(x86_gpr_ax, 0);

            syscall_leave("d");
            if (debug && trace_mmap) {
                fprintf(debug, "  memory map after mprotect syscall:\n");
                map->dump(debug, "    ");
            }
            break;
        }

    case 140: { /* 0x8c, llseek */
        /*
           int _llseek(unsigned int fd, unsigned long offset_high,            unsigned
           long offset_low, loff_t *result,            unsigned int whence);


        */

        syscall_enter("llseek","dddpd");
        uint32_t fd = arg(0);
        uint32_t offset_high = arg(1);
        uint32_t offset_low  = arg(2);

        long long whence      = arg(4);

        loff_t llseek_result2; 
        int result = syscall(140, fd, offset_high, offset_low, llseek_result2, whence );

        //FIXME: Is this the correct way of changing this pointer?
        //And is it correct that this is a 'long long*'? That is what it seems like to
        //me by typedef long long        __kernel_loff_t; typedef __kernel_loff_t loff_t
        map->write(&llseek_result2, arg(3), 8);


        writeGPR(x86_gpr_ax, result);

        syscall_leave("d");

        break;

    };
	case 141: {     /*0x8d, getdents*/
	    /* 
               int getdents(unsigned int fd, struct linux_dirent *dirp,
                           unsigned int count);

          struct linux_dirent {
              unsigned long  d_ino;     // Inode number 
              unsigned long  d_off;     // Offset to next linux_dirent 
              unsigned short d_reclen;  // Length of this linux_dirent 
              char           d_name[];  // Filename (null-terminated) 
                                 // length is actually (d_reclen - 2 -
              		         //          offsetof(struct linux_dirent, d_name) 
          }

          The system call getdents() reads several linux_dirent structures from the
          directory referred to by the open file descriptor fd into the buffer pointed
          to by dirp.  The argument count specifies the size of that buffer.
        */

        syscall_enter("getdents", "dpd");
	    unsigned int fd = arg(0);

	    // Create a buffer of the same length as the buffer in the specimen
        const size_t dirent_size = arg(2);

        uint8_t dirent[dirent_size];
        memset(dirent, 0xff, sizeof dirent);

	    //Call the system call and write result to the buffer in the specimen
	    int result = 0xdeadbeef;
	    result = syscall(141, fd, dirent, dirent_size);

        map->write(dirent, arg(1), dirent_size);
        writeGPR(x86_gpr_ax, result);

        syscall_leave("d");
	    break;
        }

        case 142: { /*0x8e , select */
            /* The select system call
                  int select(int nfds, fd_set *readfds, fd_set *writefds,
                                    fd_set *exceptfds, struct timeval *timeout);
               #undef __NFDBITS
               #define __NFDBITS       (8 * sizeof(unsigned long))

               #undef __FD_SETSIZE
               #define __FD_SETSIZE    1024

               #undef __FDSET_LONGS
               #define __FDSET_LONGS   (__FD_SETSIZE/__NFDBITS)

               //The size of this datastructure should allow for 
               //1024 file descriptors
               typedef struct {
                   unsigned long fds_bits [__FDSET_LONGS];
               } __kernel_fd_set;

               typedef __kernel_fd_set fd_set;

               select()  and  pselect()  allow  a  program to monitor multiple file 
               descriptors, waiting until one or more of the file descriptors
               become "ready" for some class of I/O operation (e.g., input possible).  
               A file descriptor is considered ready if it is possible  to
               perform the corresponding I/O operation (e.g., read(2)) without blocking.


            */
            syscall_enter("select", "dpppp");

            struct fd_set32{
                unsigned long fds_bits[32];
            };

            uint32_t fd = arg(0);

            //FIXME:
            //System call is currently only tested on 32 bit, the data structure does
            //seem to be of the same size for 32 and 64 bit, but for 64 bit the field 
            //fd_set32.fds_bits is of length 16 instead of 32. I am not quite sure 
            //how to convert between the two.
            ROSE_ASSERT(sizeof( fd_set32) == sizeof(fd_set) );


            fd_set* readfds   = NULL;
            fd_set* writefds  = NULL;
            fd_set* exceptfds = NULL;

            //uint8_t byte;
            size_t nread;
            // = map->read(&byte, arg(1), 1);
            //ROSE_ASSERT(1==nread);

            if( arg(1) ){ 
                readfds = new fd_set;
                nread = map->read(readfds, arg(1), sizeof(fd_set) );
                ROSE_ASSERT( nread == sizeof(fd_set) );
            }

            //nread = map->read(&byte, arg(2), 1);
            //ROSE_ASSERT(1==nread);

            if( arg(2) ){
                writefds = new fd_set;
                nread = map->read(writefds, arg(2), sizeof(fd_set) );
                ROSE_ASSERT( nread == sizeof(fd_set) );
            }

            //nread = map->read(&byte, arg(3), 1);
            //ROSE_ASSERT(1==nread);


            if( arg(3) ){
                exceptfds = new fd_set;
                nread = map->read(exceptfds, arg(3), sizeof(fd_set) );
                ROSE_ASSERT( nread == sizeof(fd_set) );
            }

            kernel_timeval timeout;
            nread = map->read(&timeout, arg(4), sizeof(kernel_timeval) );
            ROSE_ASSERT( nread == sizeof(kernel_timeval) );

            timeval timeout64;
            timeout64.tv_sec  = timeout.tv_sec;
            timeout64.tv_usec = timeout.tv_usec;

            select(fd, readfds, writefds, exceptfds, &timeout64);

            //nread = map->read(&byte, arg(1), 1);
            //ROSE_ASSERT(1==nread);


            if( arg(1) ){
                size_t nwritten = map->write(readfds, arg(1), sizeof(fd_set) );
                ROSE_ASSERT(nwritten==sizeof(fd_set) );
                delete readfds;
            }

            //nread = map->read(&byte, arg(2), 1);
            //ROSE_ASSERT(1==nread);


            if( arg(2) ){
                size_t nwritten = map->write(writefds, arg(2), sizeof(fd_set));
                ROSE_ASSERT( nwritten==sizeof(fd_set) );
                delete writefds;
            }

            // nread = map->read(&byte, arg(3), 1);
            // ROSE_ASSERT(1==nread);


            if( arg(3) ){
                size_t nwritten = map->write(exceptfds, arg(3), sizeof(fd_set) );
                ROSE_ASSERT(nwritten==sizeof(fd_set));
                delete exceptfds;
            }

            break;
        }
        case 146: { /*0x92, writev*/
            syscall_enter("writev", "dpd");
            uint32_t fd=arg(0), iov_va=arg(1);
            int niov=arg(2);
            uint32_t retval = 0;
            for (int i=0; i<niov; i++) {
                uint32_t buf_va_le;
                size_t nread = map->read(&buf_va_le, iov_va+i*8+0, 4);
                ROSE_ASSERT(4==nread);
                uint32_t buf_va = SgAsmExecutableFileFormat::le_to_host(buf_va_le);
                
                uint32_t buf_sz_le;
                nread = map->read(&buf_sz_le, iov_va+i*8+4, 4);
                ROSE_ASSERT(4==nread);
                uint32_t buf_sz = SgAsmExecutableFileFormat::le_to_host(buf_sz_le);

                if (debug)
                    fprintf(debug, "    #%d: va=0x%08"PRIx32", size=0x%08"PRIx32"\n", i, buf_va, buf_sz);
                uint8_t buf[buf_sz];
                nread = map->read(buf, buf_va, buf_sz);
                ROSE_ASSERT(nread==buf_sz);
                ssize_t nwritten = write(fd, buf, buf_sz);
                if (-1==nwritten) {
                    retval = -errno;
                    break;
                } else if (nwritten<buf_sz) {
                    retval += nwritten;
                    break;
                } else {
                    retval += nwritten;
                }
            }
            writeGPR(x86_gpr_ax, retval);
            syscall_leave("d");
            break;
        }

        case 174: { /*0xae, rt_sigaction*/
            syscall_enter("rt_sigaction", "fPpd", signal_names, sizeof(sigaction_32), print_sigaction_32);
            int signum=arg(0);
            uint32_t action_va=arg(1), oldact_va=arg(2);
            size_t sigsetsize=arg(3);
            ROSE_ASSERT(sigsetsize==8);

            if (signum<1 || signum>_NSIG) {
                writeGPR(x86_gpr_ax, -EINVAL);
                break;
            }

            sigaction_32 saved = signal_action[signum];
            if (action_va) {
                size_t nread = map->read(signal_action+signum, action_va, sizeof saved);
                ROSE_ASSERT(nread==sizeof saved);
            }
            if (oldact_va) {
                size_t nwritten = map->write(&saved, oldact_va, sizeof saved);
                ROSE_ASSERT(nwritten==sizeof saved);
            }

            writeGPR(x86_gpr_ax, 0);
            syscall_leave("d");
            break;
        }

        case 175: { /*0xaf, rt_sigprocmask*/
            static const Translate flags[] = { TF(SIG_BLOCK), TF(SIG_UNBLOCK), TF(SIG_SETMASK), T_END };
            syscall_enter("rt_sigprocmask", "fpp", flags);

            int how=arg(0);
            uint32_t set_va=arg(1), get_va=arg(2);
            //size_t sigsetsize=arg(3);

            uint64_t saved=signal_mask, sigset=0;
            if ( set_va != 0 ) {

                size_t nread = map->read(&sigset, set_va, sizeof sigset);
                ROSE_ASSERT(nread==sizeof sigset);

                if (0==how) {
                    /* SIG_BLOCK */
                    signal_mask |= sigset;
                } else if (1==how) {
                    /* SIG_UNBLOCK */
                    signal_mask &= ~sigset;
                } else if (2==how) {
                    /* SIG_SETMASK */
                    signal_mask = sigset;
                } else {
                    writeGPR(x86_gpr_ax, -EINVAL);
                    break;
                }
            }

            if (get_va) {
                size_t nwritten = map->write(&saved, get_va, sizeof saved);
                ROSE_ASSERT(nwritten==sizeof saved);
            }
            writeGPR(x86_gpr_ax, 0);
            syscall_leave("d");
            break;
        }

	case 183: { /* 0xb7, getcwd */
            syscall_enter("getcwd", "pd");
            char buf[arg(1)];
            int result = getcwd(buf, arg(1)) ? 0 : -errno;
            if (result>=0) {
                size_t nwritten = map->write(buf, arg(0), arg(1));
                ROSE_ASSERT(nwritten==arg(1));
            }
            writeGPR(x86_gpr_ax, result);
            syscall_leave("d");
            break;
        }


        case 186: { /* 0xba, sigaltstack*/
          /*
             int sigaltstack(const stack_t *restrict ss, stack_t *restrict oss);

             The sigaltstack() function allows a process to define and examine the state of an alternate stack for signal handlers for the current thread. Signals that have been explicitly declared to execute on the alternate stack shall be delivered on the alternate stack.

             If ss is not a null pointer, it points to a stack_t structure that specifies the alternate signal stack that shall take effect upon return from sigaltstack(). The ss_flags member specifies the new stack state. If it is set to SS_DISABLE, the stack is disabled and ss_sp and ss_size are ignored. Otherwise, the stack shall be enabled, and the ss_sp and ss_size members specify the new address and size of the stack.

             The range of addresses starting at ss_sp up to but not including ss_sp+ ss_size is available to the implementation for use as the stack. This function makes no assumptions regarding which end is the stack base and in which direction the stack grows as items are pushed.

             If oss is not a null pointer, on successful completion it shall point to a stack_t structure that specifies the alternate signal stack that was in effect prior to the call to sigaltstack(). The ss_sp and ss_size members specify the address and size of that stack. The ss_flags member specifies the stack's state, and may contain one of the following values:

             SS_ONSTACK
             The process is currently executing on the alternate signal stack. Attempts to modify the alternate signal stack while the process is executing on it fail. This flag shall not be modified by processes.
             SS_DISABLE
             The alternate signal stack is currently disabled.
          */
              syscall_enter("sigaltstack", "pp");
#if 1

              struct stack_t_kernel{
                uint8_t  ss_sp;
                int32_t  ss_flags;
                uint32_t ss_size;

              };

              stack_t_kernel fakestack_ss;
              size_t nread = map->read(&fakestack_ss, arg(0), sizeof(stack_t_kernel));
              ROSE_ASSERT(nread == sizeof(stack_t_kernel));

              //Read in the contents from the fake stack

              void* ss_sp_ss  = malloc(fakestack_ss.ss_size);
              nread = map->read(ss_sp_ss, fakestack_ss.ss_sp, fakestack_ss.ss_size);
              stack_t fakestack_ss_arg;
              fakestack_ss_arg.ss_flags = fakestack_ss.ss_flags;
              fakestack_ss_arg.ss_size  = fakestack_ss.ss_size;
              fakestack_ss_arg.ss_sp    = ss_sp_ss;

              //SECOND ARGUMENT OSS CAN BE NULL
#endif
              int result;
#if 1
              if( arg(1) != 0 )
              {

                ROSE_ASSERT(false == true);
                stack_t_kernel fakestack_oss;
                nread = map->read(&fakestack_oss, arg(1), sizeof(stack_t_kernel));
                ROSE_ASSERT(nread == sizeof(stack_t_kernel));


                //void* ss_sp_oss = malloc(fakestack_oss.ss_size);

              }else{
                std::cout << "Executing syscall" << std::endl;
                result = sigaltstack(&fakestack_ss_arg,NULL);
              }
#endif
              if (result == -1) result = -errno;

              result = -errno;
              writeGPR(x86_gpr_ax, result);

              syscall_leave("d");


              break;


        }

        case 191: { /*0xbf, ugetrlimit*/
            syscall_enter("ugetrlimit", "dp");
#if 1 /*FIXME: We need to translate between 64-bit host and 32-bit guest. [RPM 2010-09-28] */
            writeGPR(x86_gpr_ax, -ENOSYS);
#else
            int resource=arg(0);
            uint32_t rl_va=arg(1);
            struct rlimit rl;
            int status = getrlimit(resource, &rl);
            if (status<=0) {
                writeGPR(x86_gpr_ax, -errno);
            } else {
                if (rl_va) {
                    size_t nwritten = map->write(&rl, rl_va, sizeof rl);
                    ROSE_ASSERT(nwritten==sizeof rl);
                }
                writeGPR(x86_gpr_ax, 0);
            }
#endif
            syscall_leave("d");
            break;
        }

        case 192: { /*0xc0, mmap2*/
            static const Translate pflags[] = { TF(PROT_READ), TF(PROT_WRITE), TF(PROT_EXEC), TF(PROT_NONE), T_END };
            static const Translate mflags[] = { TF(MAP_SHARED), TF(MAP_PRIVATE), TF(MAP_ANONYMOUS), TF(MAP_DENYWRITE),
                                                TF(MAP_EXECUTABLE), TF(MAP_FILE), TF(MAP_FIXED), TF(MAP_GROWSDOWN),
                                                TF(MAP_LOCKED), TF(MAP_NONBLOCK), TF(MAP_NORESERVE),
#ifdef MAP_32BIT
                                                TF(MAP_32BIT),
#endif
                                                TF(MAP_POPULATE), T_END };
            syscall_enter("mmap2", "pdffdd", pflags, mflags);
            uint32_t start=arg(0), size=arg(1), prot=arg(2), flags=arg(3), offset=arg(5)*PAGE_SIZE;
            int fd=arg(4);
            size_t aligned_size = ALIGN_UP(size, PAGE_SIZE);
            void *buf = NULL;
            unsigned rose_perms = ((prot & PROT_READ) ? MemoryMap::MM_PROT_READ : 0) |
                                  ((prot & PROT_WRITE) ? MemoryMap::MM_PROT_WRITE : 0) |
                                  ((prot & PROT_EXEC) ? MemoryMap::MM_PROT_EXEC : 0);
            prot |= PROT_READ | PROT_WRITE | PROT_EXEC; /* ROSE takes care of permissions checking */

            if (!start) {
                try {
                    start = map->find_free(mmap_start, aligned_size, PAGE_SIZE);
                } catch (const MemoryMap::NoFreeSpace &e) {
                    writeGPR(x86_gpr_ax, -ENOMEM);
                    goto mmap2_done;
                }
            }
            if (!mmap_recycle)
                mmap_start = std::max(mmap_start, start);

            if (flags & MAP_ANONYMOUS) {
                buf = mmap(NULL, size, prot, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
            } else {
                buf = mmap(NULL, size, prot, flags & ~MAP_FIXED, fd, offset);
            }
            if (MAP_FAILED==buf) {
                writeGPR(x86_gpr_ax, -errno);
            } else {
                /* Try to figure out a reasonable name for the map element. If we're mapping a file, we can get the file name
                 * from the proc filesystem. The name is only used to aid debugging. */
                std::string melmt_name = "anonymous";
                if (fd>=0) {
                    char fd_namebuf[4096];
                    ssize_t nread = readlink(("/proc/self/fd/"+StringUtility::numberToString(fd)).c_str(),
                                             fd_namebuf, sizeof(fd_namebuf)-1);
                    if (nread>45) {
                        fd_namebuf[nread] = '\0';
                        char *slash = strrchr(fd_namebuf, '/');
                        melmt_name = slash ? slash+1 : fd_namebuf;
                    } else if (nread>0) {
                        fd_namebuf[nread] = '\0';
                        melmt_name = fd_namebuf;
                    } else {
                        melmt_name = "fd=" + StringUtility::numberToString(fd);
                    }
                }

                MemoryMap::MapElement melmt(start, aligned_size, buf, 0, rose_perms);
                melmt.set_name("mmap2("+melmt_name+")");
                map->erase(melmt); /*clear space space first to avoid MemoryMap::Inconsistent exception*/
                map->insert(melmt);
                writeGPR(x86_gpr_ax, start);
            }

        mmap2_done:
            syscall_leave("p");
            if (debug && trace_mmap) {
                fprintf(debug, "  memory map after mmap2 syscall:\n");
                map->dump(debug, "    ");
            }

            break;
        }

        case 195:       /*0xc3, stat64*/
        case 196:       /*0xc4, lstat64*/
        case 197: {     /*0xc5, fstat64*/
            /* We need to be a bit careful with xstat64 calls. The C library invokes one of the xstat64 system calls, which
             * writes a kernel data structure into a temporary buffer, and which the C library then massages into a struct
             * stat64. When simulating, we don't want the C library to monkey with the data returned from the system call
             * because the simulated C library will do the monkeying (it must only happen once).
             *
             * Therefore, we will invoke the system call directly, bypassing the C library, and then copy the result into
             * specimen memory. If the syscall is made on an amd64 host we need to convert it to an i386 host.
             *
             * For some unknown reason, if we invoke the system call with buf allocated on the stack we'll get -EFAULT (-14)
             * as the result; if we allocate it statically there's no problem.  Also, just in case the size is different than
             * we think, we'll allocate a guard area above the kernel_stat and check that the syscall didn't write into it. */
            if (195==callno || 196==callno) {
                syscall_enter(195==callno?"stat64":"lstat64", "sp");
            } else {
                syscall_enter("fstat64", "dp");
            }

            ROSE_ASSERT(96==sizeof(kernel_stat_32));
            ROSE_ASSERT(144==sizeof(kernel_stat_64));
#ifdef SYS_stat64       /* x86sim must be running on i386 */
            ROSE_ASSERT(4==sizeof(long));
            int host_callno = 195==callno ? SYS_stat64 : (196==callno ? SYS_lstat64 : SYS_fstat64);
            static const size_t kernel_stat_size = sizeof(kernel_stat_32);
#else                   /* x86sim must be running on amd64 */
            ROSE_ASSERT(8==sizeof(long));
            int host_callno = 195==callno ? SYS_stat : (196==callno ? SYS_lstat : SYS_fstat);
            static const size_t kernel_stat_size = sizeof(kernel_stat_64);
#endif

            static uint8_t kernel_stat[kernel_stat_size+100];
            memset(kernel_stat, 0xff, sizeof kernel_stat);
            int result = 0xdeadbeef;

            /* Make the system call without going through the C library. Well, we go through syscall(), but nothing else. */
            if (195==callno || 196==callno) {
                std::string name = read_string(arg(0));
                result = syscall(host_callno, (unsigned long)name.c_str(), (unsigned long)kernel_stat);
            } else {
                result = syscall(host_callno, (unsigned long)arg(0), (unsigned long)kernel_stat);
            }
            if (-1==result)
                result = -errno;

            /* Check for overflow */
            for (size_t i=kernel_stat_size; i<sizeof kernel_stat; i++)
                ROSE_ASSERT(0xff==kernel_stat[i]);


            if (result>=0) {
                /* Check for underflow.  Check that the kernel initialized as much data as we thought it should.  We
                 * initialized the kernel_stat to all 0xff bytes before making the system call.  The last data member of
                 * kernel_stat is either an 8-byte inode (i386) or zero (amd64), which in either case the high order byte is
                 * almost certainly not 0xff. */
                ROSE_ASSERT(0xff!=kernel_stat[kernel_stat_size-1]);

                /* On amd64 we need to translate the 64-bit struct that we got back from the host kernel to the 32-bit struct
                 * that the specimen should get back from the guest kernel. */           
                if (sizeof(kernel_stat_64)==kernel_stat_size) {
                    if (debug && trace_syscall)
                        fprintf(debug, "[64-to-32] ");
                    kernel_stat_64 *in = (kernel_stat_64*)kernel_stat;
                    kernel_stat_32 out;
                    out.dev = in->dev;
                    out.pad_1 = (uint32_t)(-1);
                    out.ino_lo = in->ino;
                    out.mode = in->mode;
                    out.nlink = in->nlink;
                    out.user = in->user;
                    out.group = in->group;
                    out.rdev = in->rdev;
                    out.pad_2 = (uint32_t)(-1);
                    out.size = in->size;
                    out.blksize = in->blksize;
                    out.nblocks = in->nblocks;
                    out.atim_sec = in->atim_sec;
                    out.atim_nsec = in->atim_nsec;
                    out.mtim_sec = in->mtim_sec;
                    out.mtim_nsec = in->mtim_nsec;
                    out.ctim_sec = in->ctim_sec;
                    out.ctim_nsec = in->ctim_nsec;
                    out.ino = in->ino;
                    map->write(&out, arg(1), sizeof out);
                } else {
                    map->write(kernel_stat, arg(1), kernel_stat_size);
                }
            }

            writeGPR(x86_gpr_ax, result);
            syscall_leave("d");
            if (result>=0)
                syscall_result(arg(1), sizeof(kernel_stat_32), print_kernel_stat_32);
            break;
        }

	case 199: { /*0xc7, getuid32 */
            syscall_enter("getuid32", "");
            uid_t id = getuid();
            writeGPR(x86_gpr_ax, id);
            syscall_leave("d");
	    break;
	}

	case 200: { /*0xc8, getgid32 */
            syscall_enter("getgid32", "");
            uid_t id = getgid();
            writeGPR(x86_gpr_ax, id);
            syscall_leave("d");
            break;
        }

	case 201: { /*0xc9, geteuid32 */
            syscall_enter("geteuid32", "");
            uid_t id = geteuid();
            writeGPR(x86_gpr_ax, id);
            syscall_leave("d");
            break;
        }

        case 202: { /*0xca, getegid32 */
            syscall_enter("getegid32", "");
            uid_t id = getegid();
            writeGPR(x86_gpr_ax, id);
            syscall_leave("d");
            break;
        }

        case 207: { /*0xcf, fchown */
                   /*
                      int fchown(int fd, uid_t owner, gid_t group);

                      typedef unsigned short  __kernel_old_uid_t;
                      typedef unsigned short  __kernel_old_gid_t;

                      fchown() changes the ownership of the file referred to by the open file
                      descriptor fd.

                    */

                   syscall_enter("fchown16", "ddd");
                   uint32_t fd = arg(0);
                   uid_t  user = arg(1);
                   gid_t group = arg(2);
                   int result = syscall(207,fd,user,group);
                   writeGPR(x86_gpr_ax, result);
                   syscall_leave("d");
                   break;
                 }
        case 212: { /*0xd4, chown */
            syscall_enter("chown", "sdd");
	    std::string filename = read_string(arg(0));
            uid_t user = arg(1);
	    gid_t group = arg(2);
	    int result = chown(filename.c_str(),user,group);
            writeGPR(x86_gpr_ax, result);
            syscall_leave("d");
            break;
        }

 	case 220: {     /*0xdc, getdents64*/
	    /* 

          long sys_getdents64(unsigned int fd, struct linux_dirent64 __user * dirent, unsigned int count) 

          struct linux_dirent {
              unsigned long  d_ino;     // Inode number 
              unsigned long  d_off;     // Offset to next linux_dirent 
              unsigned short d_reclen;  // Length of this linux_dirent 
              char           d_name[];  // Filename (null-terminated) 
                                 // length is actually (d_reclen - 2 -
              		         //          offsetof(struct linux_dirent, d_name) 
          }

          The system call getdents() reads several linux_dirent structures from the
          directory referred to by the open file descriptor fd into the buffer pointed
          to by dirp.  The argument count specifies the size of that buffer.
        */

        syscall_enter("getdents64", "dpd");
	    unsigned int fd = arg(0);

	    // Create a buffer of the same length as the buffer in the specimen
        const size_t dirent_size = arg(2);

        uint8_t dirent[dirent_size];
        memset(dirent, 0xff, sizeof dirent);

	    //Call the system call and write result to the buffer in the specimen
	    int result = 0xdeadbeef;
	    result = syscall(220, fd, dirent, dirent_size);

        map->write(dirent, arg(1), dirent_size);
        writeGPR(x86_gpr_ax, result);

        syscall_leave("d");
	    break;
        }



        case 221: { // fcntl
            syscall_enter("fcntl64", "ddp");
            uint32_t fd=arg(0), cmd=arg(1), other_arg=arg(2);
            int result = -EINVAL;
            switch (cmd) {
                case F_DUPFD: {
                    result = fcntl(fd, cmd, (long)other_arg);
                    if (result == -1) result = -errno;
                    break;
                }
                case F_SETFD: {
                    result = fcntl(fd, cmd, (long)other_arg);
                    if (result == -1) result = -errno;
                    break;
                }
                default: {
                    result = -EINVAL;
                    break;
                }
            }
            writeGPR(x86_gpr_ax, result);
            syscall_leave("d");
            break;
        }

        case 224: { /*0xe0, gettid*/
            // We have no concept of threads
            syscall_enter("gettid", "");
            writeGPR(x86_gpr_ax, getpid());
            syscall_leave("d");
            break;
       }

        case 240: { /*0xf0, futex*/
            static const Translate opflags[] = {
#ifdef FUTEX_CMD_MASK
                TF(FUTEX_PRIVATE_FLAG),
                TF2(FUTEX_CMD_MASK, FUTEX_WAIT),
                TF2(FUTEX_CMD_MASK, FUTEX_WAKE),
                TF2(FUTEX_CMD_MASK, FUTEX_FD),
                TF2(FUTEX_CMD_MASK, FUTEX_REQUEUE),
                TF2(FUTEX_CMD_MASK, FUTEX_CMP_REQUEUE),
                TF2(FUTEX_CMD_MASK, FUTEX_WAKE_OP),
                TF2(FUTEX_CMD_MASK, FUTEX_LOCK_PI),
                TF2(FUTEX_CMD_MASK, FUTEX_UNLOCK_PI),
                TF2(FUTEX_CMD_MASK, FUTEX_TRYLOCK_PI),
#ifdef FUTEX_WAIT_BITSET
                TF2(FUTEX_CMD_MASK, FUTEX_WAIT_BITSET),
#endif
#ifdef FUTEX_WAKE_BITSET
                TF2(FUTEX_CMD_MASK, FUTEX_WAKE_BITSET),
#endif
#endif
                T_END };

            /* Variable arguments */
            unsigned arg1 = arg(1);
#ifdef FUTEX_CMD_MASK
            arg1 &= FUTEX_CMD_MASK;
#endif
            switch (arg1) {
                case FUTEX_WAIT:
                    syscall_enter("futex", "PfdP--", 4, print_int_32, opflags, sizeof(timespec_32), print_timespec_32);
                    break;
                case FUTEX_WAKE:
                case FUTEX_FD:
                    syscall_enter("futex", "Pfd---", 4, print_int_32, opflags);
                    break;
                case FUTEX_REQUEUE:
                    syscall_enter("futex", "Pfd-P-", 4, print_int_32, opflags, 4, print_int_32);
                    break;
                case FUTEX_CMP_REQUEUE:
                    syscall_enter("futex", "Pfd-Pd", 4, print_int_32, opflags, 4, print_int_32);
                    break;
                default:
                    syscall_enter("futex", "PfdPPd", 4, print_int_32, opflags, sizeof(timespec_32), print_timespec_32, 
                                  4, print_int_32);
                    break;
            }

            uint32_t futex1_va=arg(0), op=arg(1), val1=arg(2), timeout_va=arg(3), futex2_va=arg(4), val2=arg(5);
            int *futex1 = (int*)my_addr(futex1_va);
            int *futex2 = (int*)my_addr(futex2_va);

            struct timespec timespec_buf, *timespec=NULL;
            if (timeout_va) {
                timespec_32 ts;
                size_t nread = map->read(&ts, timeout_va, sizeof ts);
                ROSE_ASSERT(nread==sizeof ts);
                timespec_buf.tv_sec = ts.sec;
                timespec_buf.tv_nsec = ts.nsec;
                timespec = &timespec_buf;
            }

            int result = syscall(SYS_futex, futex1, op, val1, timespec, futex2, val2);
            if (-1==result) result = -errno;
            writeGPR(x86_gpr_ax, result);
            syscall_leave("d");
            break;
        }

        case 243: { /*0xf3, set_thread_area*/
            syscall_enter("set_thread_area", "P", sizeof(user_desc), print_user_desc);
            user_desc ud;
            size_t nread = map->read(&ud, arg(0), sizeof ud);
            ROSE_ASSERT(nread==sizeof ud);
            if (ud.entry_number==(unsigned)-1) {
                for (ud.entry_number=0x33>>3; ud.entry_number<n_gdt; ud.entry_number++) {
                    if (!gdt[ud.entry_number].useable) break;
                }
                ROSE_ASSERT(ud.entry_number<8192);
                if (debug && trace_syscall)
                    fprintf(debug, "[entry #%d] ", (int)ud.entry_number);
            }
            gdt[ud.entry_number] = ud;
            size_t nwritten = map->write(&ud, arg(0), sizeof ud);
            ROSE_ASSERT(nwritten==sizeof ud);
            writeGPR(x86_gpr_ax, 0);
            /* Reload all the segreg shadow values from the (modified) descriptor table */
            for (size_t i=0; i<6; i++)
                writeSegreg((X86SegmentRegister)i, readSegreg((X86SegmentRegister)i));
            syscall_leave("d");
            break;
        }

        case 252: { /*0xfc, exit_group*/
            syscall_enter("exit_group", "d");
            if (debug && trace_syscall) fputs("(throwing...)\n", debug);
            int status=arg(0);
            throw Exit(__W_EXITCODE(status, 0));
        }

        case 258: { /*0x102, set_tid_address*/
            syscall_enter("set_tid_address", "p");
            uint32_t tid_va=arg(0);
            
            /* We want the 32-bit value to be updated by Linux, but if we're running on a 64-bit system then Linux will also
             * update the following 32-bits (probably initializing them to zero).  Therefore we'll create 64 bits memory for
             * Linux to update and map the low-order 32-bits into the specimen. */
            int *tidptr = NULL;
            if (sizeof(int)>4) {
                tidptr = new int;
                *tidptr = 0;
                size_t nread = map->read(tidptr, tid_va, 4); /*only low-order bytes*/
                ROSE_ASSERT(4==nread);
                const MemoryMap::MapElement *orig = map->find(tid_va);
                MemoryMap::MapElement submap(tid_va, 4, tidptr, 0, orig->get_mapperms());
                submap.set_name("set_tid_address");
                map->insert(submap);
            } else {
                tidptr = (int*)my_addr(tid_va);
            }

            syscall(SYS_set_tid_address, tidptr);
            writeGPR(x86_gpr_ax, getpid());

            syscall_leave("d");
            if (debug && trace_mmap) {
                fprintf(debug, "  memory map after set_tid_address syscall:\n");
                map->dump(debug, "    ");
            }
            break;
        }

        case 264:    /* 0x108, clock_settime */
        case 265:    /* 0x109, clock_gettime */
        case 266: {  /* 0x1a, clock_getres */
                /*
                  int clock_getres(clockid_t clk_id, struct timespec *res);
                  int clock_gettime(clockid_t clk_id, struct timespec *tp);
                  int clock_settime(clockid_t clk_id, const struct timespec *tp); 

                  struct timespec {
                      time_t   tv_sec;        // seconds 
                      long     tv_nsec;       // nanoseconds 
                  };

                  The function clock_getres() finds the resolution (precision) of the 
                  specified clock clk_id, and, if res is non-NULL, stores it in the 
                  struct timespec pointed to by res. The resolution of clocks depends 
                  on the implementation and cannot be configured by a particular process. 
                  If the time value pointed to by the argument tp of clock_settime() is
                  not a multiple of res, then it is truncated to a multiple of res. 
            */

            syscall_enter("clock_gettime", "dp");
 
            int32_t which_clock = arg(0);
            
            //Check to see if times is NULL
            uint8_t byte;
            size_t nread = map->read(&byte, arg(1), 1);
            ROSE_ASSERT(1==nread); /*or we've read past the end of the mapped memory*/

            int result;
            if( byte )
            {

              struct kernel_timespec {
                uint32_t   tv_sec;        // seconds 
                uint32_t   tv_nsec;       // nanoseconds 
              };



              size_t size_timespec_sample = sizeof(kernel_timespec);

              kernel_timespec ubuf;

              size_t nread = map->read(&ubuf, arg(1), size_timespec_sample);

              ROSE_ASSERT(nread == size_timespec_sample);

              timespec timespec64;
              timespec64.tv_sec  = ubuf.tv_sec;
              timespec64.tv_nsec = ubuf.tv_nsec;
              result = syscall(callno, which_clock, (unsigned long) &timespec64 );

              ubuf.tv_sec = timespec64.tv_sec;
              ubuf.tv_nsec = timespec64.tv_nsec;
              map->write(&ubuf, arg(1), size_timespec_sample);
    
            }else
              result = syscall(callno, which_clock, (unsigned long) NULL );

            writeGPR(x86_gpr_ax, result);

            syscall_leave("d");
            break;
        }

        case 270: { /*0x10e tgkill*/
            syscall_enter("tgkill", "ddf", signal_names);
            uint32_t /*tgid=arg(0), pid=arg(1),*/ sig=arg(2);
            // TODO: Actually check thread group and kill properly
            if (debug && trace_syscall) fputs("(throwing...)\n", debug);
            throw Exit(__W_EXITCODE(0, sig));
            break;

        }

        case 271: { /* 0x10f, utimes */
            /*
                int utimes(const char *filename, const struct timeval times[2]);

                struct timeval {
                    long tv_sec;        // seconds 
                    long tv_usec;   // microseconds 
                };


                The utimes() system call changes the access and modification times of the inode
                specified by filename to the actime and modtime fields of times respectively.

                times[0] specifies the new access time, and times[1] specifies the new
                modification time.  If times is NULL, then analogously to utime(), the access
                and modification times of the file are set to the current time.


            */
            syscall_enter("utimes", "s");


            std::string filename = read_string(arg(0));

            //Check to see if times is NULL
            uint8_t byte;
            size_t nread = map->read(&byte, arg(1), 1);
            ROSE_ASSERT(1==nread); /*or we've read past the end of the mapped memory*/

            int result;
            if( byte )
            {

              size_t size_timeval_sample = sizeof(kernel_timeval)*2;

              kernel_timeval ubuf[1];

              size_t nread = map->read(&ubuf, arg(1), size_timeval_sample);


              timeval timeval64[1];
              timeval64[0].tv_sec  = ubuf[0].tv_sec;
              timeval64[0].tv_usec = ubuf[0].tv_usec;
              timeval64[1].tv_sec  = ubuf[1].tv_sec;
              timeval64[1].tv_usec = ubuf[1].tv_usec;

              ROSE_ASSERT(nread == size_timeval_sample);

              result = utimes(filename.c_str(), timeval64);

            }else
              result = utimes(filename.c_str(), NULL);

            writeGPR(x86_gpr_ax, result);
            syscall_leave("d");
            break;

        }

	case 306: { /* 0x132, fchmodat */
            syscall_enter("fchmodat", "dsdd");
	    int dirfd = arg(0);
	    uint32_t path = arg(1);
            std::string sys_path = read_string(path);
	    mode_t mode = arg(2);
	    int flags = arg(3);

	    int result = syscall( 306, dirfd, (long) sys_path.c_str(), mode, flags);
            if (result == -1) result = -errno;
            writeGPR(x86_gpr_ax, result);

            syscall_leave("d");
            break;
	}

        case 311: { /*0x137, set_robust_list*/
            syscall_enter("set_robust_list", "pd");
            uint32_t head_va=arg(0), len=arg(1);
            void *head = my_addr(head_va);
            
            /* Allow Linux to update the specimen's memory directly. */
            int status = syscall(SYS_set_robust_list, head, len);
            if (status<0) {
                writeGPR(x86_gpr_ax, -errno);
            } else {
                writeGPR(x86_gpr_ax, 0);
            }
            syscall_leave("d");
            break;
        }

        default: {
            fprintf(stderr, "syscall_%u(", callno);
            for (int i=0; i<6; i++)
                fprintf(stderr, "%s0x%08"PRIx32, i?", ":"", arg(i));
            fprintf(stderr, ") is not implemented yet\n\n");
            dump_core(SIGSYS);
            abort();
        }
    }
    ROSE_ASSERT( this != NULL  );
}

void
EmulationPolicy::syscall_arginfo(char format, uint32_t val, ArgInfo *info, va_list *ap)
{
    ROSE_ASSERT(info!=NULL);
    info->val = val;
    switch (format) {
        case 'f':       /*flags*/
        case 'e':       /*enum*/
            info->xlate = va_arg(*ap, const Translate*);
            break;
        case 's':       /*NUL-terminated string*/
            info->str = read_string(val);
            break;
        case 'P': {       /*ptr to a struct*/
            info->struct_size = va_arg(*ap, size_t);
            info->struct_printer = va_arg(*ap, ArgInfo::StructPrinter);
            info->struct_buf = new uint8_t[info->struct_size];
            info->struct_nread = map->read(info->struct_buf, info->val, info->struct_size);
            break;
        }
    }
}

void
EmulationPolicy::syscall_enter(const char *name, const char *format, ...)
{
    va_list ap;
    va_start(ap, format);

    if (debug && trace_syscall) {
        fprintf(debug, "0x%08"PRIx64": ", readIP().known_value());
        ArgInfo args[6];
        for (size_t i=0; format[i]; i++)
            syscall_arginfo(format[i], arg(i), args+i, &ap);
        print_enter(debug, name, format, args);
    }
    
    va_end(ap);
}

void
EmulationPolicy::syscall_leave(const char *format, ...) 
{
    va_list ap;
    va_start(ap, format);

    ROSE_ASSERT(1==strlen(format));
    if (debug && trace_syscall) {
        ArgInfo info;
        uint32_t value = readGPR(x86_gpr_ax).known_value();
        syscall_arginfo(format[0], value, &info, &ap);
        print_leave(debug, format[0], &info);
    }
}

void
EmulationPolicy::syscall_result(uint32_t va, size_t sz, ArgInfo::StructPrinter printer)
{
    if (debug && trace_syscall) {
        ArgInfo info;
        info.val = va;
        info.struct_printer = printer;
        info.struct_buf = new uint8_t[sz];
        info.struct_size = sz;
        info.struct_nread = map->read(info.struct_buf, va, sz);
        fprintf(debug, "    ");
        print_single(debug, 'P',  &info);
        fprintf(debug, "\n");
    }
}

uint32_t
EmulationPolicy::arg(int idx)
{
    switch (idx) {
        case 0: return readGPR(x86_gpr_bx).known_value();
        case 1: return readGPR(x86_gpr_cx).known_value();
        case 2: return readGPR(x86_gpr_dx).known_value();
        case 3: return readGPR(x86_gpr_si).known_value();
        case 4: return readGPR(x86_gpr_di).known_value();
        case 5: return readGPR(x86_gpr_bp).known_value();
        default: assert(!"invalid argument number"); abort();
    }
}

int
main(int argc, char *argv[])
{
    typedef X86InstructionSemantics<EmulationPolicy, VirtualMachineSemantics::ValueType> Semantics;
    EmulationPolicy policy;
    Semantics t(policy);
    uint32_t dump_at = 0;               /* dump core the first time we hit this address, before the instruction is executed */
    std::string dump_name = "dump";
    FILE *log_file = NULL;

    /* Parse command-line */
    int argno = 1;
    while (argno<argc && '-'==argv[argno][0]) {
        if (!strcmp(argv[argno], "--")) {
            argno++;
            break;
        } else if (!strncmp(argv[argno], "--log=", 6)) {
            if (log_file)
                fclose(log_file);
            if (NULL==(log_file = fopen(argv[argno]+6, "w"))) {
                fprintf(stderr, "%s: %s: %s\n", argv[0], strerror(errno), argv[argno+6]);
                exit(1);
            }
            argno++;
        } else if (!strncmp(argv[argno], "--debug=", 8)) {
            policy.debug = stderr;
            char *s = argv[argno]+8;
            while (s && *s) {
                char *comma = strchr(s, ',');
                std::string word(s, comma?comma-s:strlen(s));
                s = comma ? comma+1 : NULL;
                if (word=="all") {
                    policy.trace_insn = true;
                    policy.trace_state = true;
                    policy.trace_mem = true;
                    policy.trace_mmap = true;
                    policy.trace_syscall = true;
                    policy.trace_loader = true;
                    policy.trace_progress = true;
                } else if (word=="insn") {
                    policy.trace_insn = true;
                } else if (word=="state") {
                    policy.trace_state = true;
                } else if (word=="mem") {
                    policy.trace_mem = true;
                } else if (word=="mmap") {
                    policy.trace_mmap = true;
                } else if (word=="syscall") {
                    policy.trace_syscall = true;
                } else if (word=="loader") {
                    policy.trace_loader = true;
                } else if (word=="progress") {
                    policy.trace_progress = true;
                } else {
                    fprintf(stderr, "%s: debug words must be from the set: "
                            "all, insn, state, mem, mmap, syscall, loader, progress\n",
                            argv[0]);
                    exit(1);
                }
            }
            argno++;
        } else if (!strcmp(argv[argno], "--debug")) {
            policy.debug = stderr;
            policy.trace_insn = true;
            policy.trace_syscall = true;
            argno++;
        } else if (!strncmp(argv[argno], "--core=", 7)) {
            policy.core_styles = 0;
            for (char *s=argv[argno]+7; s && *s; /*void*/) {
                if (!strncmp(s, "elf", 3)) {
                    s += 3;
                    policy.core_styles |= CORE_ELF;
                } else if (!strncmp(s, "rose", 4)) {
                    s += 4;
                    policy.core_styles |= CORE_ROSE;
                } else {
                    fprintf(stderr, "%s: unknown core dump type for %s\n", argv[0], argv[argno]);
                }
                while (','==*s) s++;
            }
            argno++;
        } else if (!strncmp(argv[argno], "--dump=", 7)) {
            char *rest;
            errno = 0;
            dump_at = strtoul(argv[argno]+7, &rest, 0);
            if (rest==argv[argno]+7 || errno!=0) {
                fprintf(stderr, "%s: --dump=N requires an address, N\n", argv[0]);
                exit(1);
            }
            if (','==rest[0] && rest[1])
                dump_name = rest+1;
            argno++;
        } else if (!strncmp(argv[argno], "--interp=", 9)) {
            policy.interpname = argv[argno++]+9;
        } else if (!strncmp(argv[argno], "--vdso=", 7)) {
            policy.vdso_paths.clear();
            for (char *s=argv[argno]+7; s && *s; /*void*/) {
                char *colon = strchr(s, ':');
                policy.vdso_paths.push_back(std::string(s, colon?colon-s:strlen(s)));
                s = colon ? colon+1 : NULL;
            }
            argno++;
        } else {
            fprintf(stderr, "usage: %s [--debug] PROGRAM ARGUMENTS...\n", argv[0]);
            exit(1);
        }
    }
    ROSE_ASSERT(argc-argno>=1); /* usage: executable name followed by executable's arguments */
    if (policy.debug && log_file)
        policy.debug = log_file;
    SgAsmGenericHeader *fhdr = policy.load(argv[argno]); /*header for main executable, not libraries*/
    policy.initialize_stack(fhdr, argc-argno, argv+argno);

    /* Debugging */
    if (policy.debug && policy.trace_mmap) {
        fprintf(policy.debug, "memory map after program load:\n");
        policy.map->dump(policy.debug, "  ");
    }
    if (policy.debug && policy.trace_state) {
        fprintf(policy.debug, "Initial state:\n");
        policy.dump_registers(policy.debug);
    }
    if (policy.debug && policy.trace_progress) {
        struct sigaction sa;
        sa.sa_handler = alarm_handler;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = SA_RESTART;
        sigaction(SIGALRM, &sa, NULL);
        alarm(PROGRESS_INTERVAL);
    }

    /* Execute the program */
    struct timeval sim_start_time;
    gettimeofday(&sim_start_time, NULL);
    bool seen_entry_va = false;
    while (true) {
        if (had_alarm) {
            had_alarm = 0;
            alarm(PROGRESS_INTERVAL);
            if (policy.debug && policy.trace_progress) {
                struct timeval cur_time;
                gettimeofday(&cur_time, NULL);
                double nsec = cur_time.tv_sec + cur_time.tv_usec/1e6 - (sim_start_time.tv_sec + sim_start_time.tv_usec/1e6);
                double insn_rate = nsec>0 ? policy.get_ninsns() / nsec : 0;
                fprintf(policy.debug, "x86sim: processed %zu insns in %d sec (%d insns/sec)\n",
                        policy.get_ninsns(), (int)(nsec+0.5), (int)(insn_rate+0.5));
            }
        }
        try {
            if (dump_at!=0 && dump_at == policy.readIP().known_value()) {
                fprintf(stderr, "Reached dump point.\n");
                policy.dump_core(SIGABRT, dump_name);
                dump_at = 0;
            }
            SgAsmx86Instruction *insn = policy.current_insn();
            if (policy.debug && policy.trace_mmap &&
                !seen_entry_va && insn->get_address()==fhdr->get_base_va()+fhdr->get_entry_rva()) {
                fprintf(policy.debug, "memory map at program entry:\n");
                policy.map->dump(policy.debug, "  ");
                seen_entry_va = true;
            }
            t.processInstruction(insn);
            if (policy.debug && policy.trace_state)
                policy.dump_registers(policy.debug);
        } catch (const Semantics::Exception &e) {
            std::cerr <<e <<"\n\n";
#if 0
            policy.dump_core(SIGILL);
            abort();
#else
            std::cerr <<"Ignored. Continuing with a corrupt state...\n";
#endif
        } catch (const VirtualMachineSemantics::Policy::Exception &e) {
            std::cerr <<e <<"\n\n";
            policy.dump_core(SIGILL);
            abort();
        } catch (const EmulationPolicy::Exit &e) {
            /* specimen has exited */
            if (WIFEXITED(e.status)) {
                fprintf(stderr, "specimen exited with status %d\n", WEXITSTATUS(e.status));
		if( WEXITSTATUS(e.status) )
                   exit( WEXITSTATUS(e.status) );
            } else if (WIFSIGNALED(e.status)) {
                fprintf(stderr, "specimen exited due to signal %d (%s)%s\n",
                        WTERMSIG(e.status), strsignal(WTERMSIG(e.status)), 
                        WCOREDUMP(e.status)?" core dumped":"");
                /* Eventually we'll put this where the signal is thrown. [RPM 2010-09-18] */
                policy.dump_core(WTERMSIG(e.status));
            } else if (WIFSTOPPED(e.status)) {
                fprintf(stderr, "specimen is stopped due to signal %d (%s)\n", 
                        WSTOPSIG(e.status), strsignal(WSTOPSIG(e.status)));
            }
            break;
        }
    }
    return 0;
}

#else
int main(int, char *argv[])
{
    std::cerr <<argv[0] <<": not supported on this platform" <<std::endl;
    return 1;
}

#endif /* ROSE_ENABLE_SIMULATOR */
