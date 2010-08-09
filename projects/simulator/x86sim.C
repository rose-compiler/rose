/* Emulates an executable. */
#include "rose.h"

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
#include <stdarg.h>

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

#ifndef HAVE_USER_DESC
typedef modify_ldt_ldt_s user_desc;
#endif


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

    /* Must be the same size and layout as struct sigaction on 32-bit linux */
    struct SignalAction {
        uint32_t        handler;
        sigset_t        mask;                   /* same size on 32- and 64-bit linux */
        uint32_t        flags;
    };

    /* Thrown by exit system calls. */
    struct Exit {
        explicit Exit(int status): status(status) {}
        int status;                             /* same value as returned by waitpid() */
    };

public:
    MemoryMap map;                              /* Describes how specimen's memory is mapped to simulator memory */
    Disassembler *disassembler;                 /* Disassembler to use for obtaining instructions */
    Disassembler::InstructionMap icache;        /* Cache of disassembled instructions */
    uint32_t brk_va;                            /* Current value for brk() syscall; initialized by load() */
    uint32_t phdr_va;                           /* Virtual address for PT_PHDR ELF segment, or zero; initialized by load() */
    static const size_t n_gdt=8192;             /* Number of global descriptor table entries */
    user_desc gdt[n_gdt];                       /* Global descriptor table */
    SegmentInfo segreg_shadow[6];               /* Shadow values of segment registers from GDT */
    uint32_t mmap_start;                        /* Minimum address to use when looking for mmap free space */
    bool mmap_recycle;                          /* If false, then never reuse mmap addresses */
    SignalAction signal_action[_NSIG+1];        /* Simulated actions for signal handling */
    uint64_t signal_mask;                       /* Set by sigsetmask() */

#if 0
    uint32_t gsOffset;
    void (*eipShadow)();
    uint32_t signalStack;
    std::vector<user_desc> thread_areas;
#endif
    

    /* Debugging, tracing, etc. */
    FILE *debug;                                /* Stream to which debugging output is sent (or NULL to suppress it) */
    bool tty;                                   /* True if 'debug' stream is a tty; affects output of trace_insn */
    bool trace_insn;                            /* Show each instruction that's executed */
    bool trace_state;                           /* Show machine state after each instruction */
    bool trace_mem;                             /* Show memory read/write operations */
    bool trace_mmap;                            /* Show changes in the memory map */
    bool trace_syscall;                         /* Show each system call */


    EmulationPolicy()
        : disassembler(NULL), brk_va(0), phdr_va(0), mmap_start(0x40000000ul), mmap_recycle(false), signal_mask(0),
          debug(NULL), tty(true),
          trace_insn(true), trace_state(false), trace_mem(false), trace_mmap(false), trace_syscall(false) {

        for (size_t i=0; i<VirtualMachineSemantics::State::n_gprs; i++)
            writeGPR((X86GeneralPurposeRegister)i, 0);
        for (size_t i=0; i<VirtualMachineSemantics::State::n_flags; i++)
            writeFlag((X86Flag)i, 0);
        writeIP(0);
        writeFlag(x86_flag_1, true_());
        writeGPR(x86_gpr_sp, 0xc0000000ul);

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
        fprintf(f, "    flags: %s %s %s %s %s %s %s\n", 
                readFlag(x86_flag_of).known_value()?"ov":"nv", readFlag(x86_flag_df).known_value()?"dn":"up",
                readFlag(x86_flag_sf).known_value()?"ng":"pl", readFlag(x86_flag_zf).known_value()?"zr":"nz",
                readFlag(x86_flag_af).known_value()?"ac":"na", readFlag(x86_flag_pf).known_value()?"pe":"po", 
                readFlag(x86_flag_cf).known_value()?"cy":"nc");
    }

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

    /* Initializes an ArgInfo object to pass to syscall printing functions. */
    void syscall_arginfo(char fmt, uint32_t val, ArgInfo *info, va_list ap);

    /* Returns the memory address in ROSE where the specified specimen address is located. */
    void *my_addr(uint32_t va);

    /* Reads a NUL-terminated string from specimen memory. The NUL is not included in the string. */
    std::string read_string(uint32_t va);

    /* Copies a stat buffer into specimen memory. */
    void copy_stat64(struct stat64 *sb, uint32_t va);

    /* Called by X86InstructionSemantics. Used by x86_and instruction to set AF flag */
    VirtualMachineSemantics::ValueType<1> undefined_() {
        return 1;
    }

    /* Called by X86InstructionSemantics */
    void hlt() {
        fprintf(stderr, "hlt\n");
        abort();
    }

    /* Called by X86InstructionSemantics */
    void interrupt(uint8_t num) {
        if (num != 0x80) {
            fprintf(stderr, "Bad interrupt\n");
            abort();
        }
        emulate_syscall();
    }

    /* Called by X86InstructionSemantics */
    void startInstruction(SgAsmInstruction* insn) {
        if (debug && trace_insn) {
            if (tty) {
                fprintf(debug, "\033[K\n[%07zu] %s\033[K\r\033[1A", get_ninsns(), unparseInstructionWithAddress(insn).c_str());
            } else {
                fprintf(debug, "[%07zu] %s\n", get_ninsns(), unparseInstructionWithAddress(insn).c_str());
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
               const VirtualMachineSemantics::ValueType<1> cond) const {
        ROSE_ASSERT(0==Len % 8 && Len<=64);
        uint32_t base = segreg_shadow[sr].base;
        uint32_t offset = addr.known_value();
        ROSE_ASSERT(offset <= segreg_shadow[sr].limit);
        ROSE_ASSERT(offset + (Len/8) - 1 <= segreg_shadow[sr].limit);

        ROSE_ASSERT(cond.is_known());
        if (cond.known_value()) {
            uint8_t buf[Len/8];
            size_t nread = map.read(buf, base+offset, Len/8);
            if (nread!=Len/8) {
                fprintf(stderr, "read %zu byte%s failed at 0x%08"PRIx32"\n\n", Len/8, 1==Len/8?"":"s", base+offset);
                fprintf(stderr, "memory map at failure:\n");
                map.dump(stderr, "  ");
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
            size_t nwritten = map.write(buf, base+offset, Len/8);
            if (nwritten!=Len/8) {
                fprintf(stderr, "write %zu byte%s failed at 0x%08"PRIx32"\n\n", Len/8, 1==Len/8?"":"s", base+offset);
                fprintf(stderr, "memory map at failure:\n");
                map.dump(stderr, "  ");
                abort();
            }
        }
    }
};

SgAsmGenericHeader*
EmulationPolicy::load(const char *name)
{
    if (debug)
        fprintf(debug, "loading %s...\n", name);
    char *frontend_args[4];
    frontend_args[0] = strdup("-");
    frontend_args[1] = strdup("-rose:read_executable_file_format_only"); /*delay disassembly until later*/
    frontend_args[2] = strdup(name);
    frontend_args[3] = NULL;
    SgProject *project = frontend(3, frontend_args);

    /* Find the best file header. For Windows programs, skip the DOS header if there's another later header. */
    SgAsmGenericHeader *fhdr = SageInterface::querySubTree<SgAsmGenericHeader>(project, V_SgAsmGenericHeader).back();
    writeIP(fhdr->get_entry_rva() + fhdr->get_base_va());

    /* Find a suitable disassembler and clone it in case we want to set properties locally. We only do this for the first
     * (non-recursive) call of load() and assume that all dynamically linked libraries would use the same disassembler. */
    if (!disassembler) {
        disassembler = Disassembler::lookup(fhdr)->clone();
        ROSE_ASSERT(disassembler!=NULL);
    }

    /* Determine which mappable sections should be loaded into the specimen's address space.  We load LOAD and INTERP
     * segments. INTERP segments will additionally cause EmulationPolicy::load() to run recursively.  Note that
     * LoaderELF::order_sections() cause SgAsmGenericSections which are both ELF Sections and ELF Segments to appear twice in
     * the list that's ultimately processed by Loader::create_map(). Therefore we need to keep track of what sections we've
     * actually seen. */
    struct T2: public Loader::Selector {
        EmulationPolicy *policy;
        std::set<SgAsmGenericSection*> seen;
        T2(EmulationPolicy *policy): policy(policy) {}
        virtual Loader::Contribution contributes(SgAsmGenericSection *_section) {
            SgAsmElfSection *section = isSgAsmElfSection(_section);
            SgAsmElfSegmentTableEntry *segment = section ? section->get_segment_entry() : NULL;
            if (segment && seen.find(section)==seen.end()) {
                seen.insert(section);
                switch (segment->get_type()) {
                    case SgAsmElfSegmentTableEntry::PT_LOAD:
                        return Loader::CONTRIBUTE_ADD;
                    case SgAsmElfSegmentTableEntry::PT_INTERP:
                        char interp_name[section->get_size()+1];
                        section->read_content_local(0, interp_name, section->get_size());
                        interp_name[section->get_size()] = '\0';
                        policy->load(interp_name);
                        return Loader::CONTRIBUTE_ADD;
                    case SgAsmElfSegmentTableEntry::PT_PHDR:
                        policy->phdr_va = section->get_mapped_preferred_rva();
                        return Loader::CONTRIBUTE_NONE;
                    default:
                        return Loader::CONTRIBUTE_NONE;
                }
            }
            return Loader::CONTRIBUTE_NONE;
        }
    } selector(this);

    /* Load applicable sections into specimen's memory recursively, defining a MemoryMap that describes how the specimen
     * address spaces maps to our own (the simulator's) address space. */
    Loader *loader = Loader::find_loader(fhdr);
    ROSE_ASSERT(loader!=NULL);
    loader->create_map(&map, fhdr->get_mapped_sections(), &selector);

    /* Initialize the brk value to be the lowest page-aligned address that is above the end of the highest mapped address.
     * Note that we haven't mapped the stack yet, which is typically above all the segments loaded from the file. */
    brk_va = ALIGN_UP(map.highest_va()+1, PAGE_SIZE);
    return fhdr;
}

void EmulationPolicy::initialize_stack(SgAsmGenericHeader *_fhdr, int argc, char *argv[])
{
    /* We only handle ELF for now */
    SgAsmElfFileHeader *fhdr = isSgAsmElfFileHeader(_fhdr);
    ROSE_ASSERT(fhdr!=NULL);

    /* Allocate the stack */
    static const size_t stack_size = 0x01000000;
    size_t sp = readGPR(x86_gpr_sp).known_value();
    size_t stack_addr = sp - stack_size;
    MemoryMap::MapElement melmt(stack_addr, stack_size, MemoryMap::MM_PROT_READ|MemoryMap::MM_PROT_WRITE);
    melmt.set_name("stack");
    map.insert(melmt);

    /* Initialize the stack with specimen's argc and argv */
    std::vector<uint32_t> pointers;                     /* pointers pushed onto stack at the end of initialization */
    pointers.push_back(argc);
    for (int i=0; i<argc; i++) {
        size_t len = strlen(argv[i]) + 1; /*inc. NUL termination*/
        sp -= len;
        map.write(argv[i], sp, len);
        pointers.push_back(sp);
    }
    pointers.push_back(0); /*the argv NULL terminator*/

    /* Initialize the stack with specimen's environment. For now we'll use the same environment as this simulator. */
    for (int i=0; true; i++) {
        if (!environ[i]) break;
        size_t len = strlen(environ[i]) + 1;
        sp -= len;
        map.write(environ[i], sp, len);
        pointers.push_back(sp);
    }
    pointers.push_back(0); /*environment NULL terminator*/

    /* Initialize stack with auxv, where each entry is two words in the pointers vector. This information is only present for
     * dynamically linked executables. */
    if (fhdr->get_section_by_name(".interp")) {
        pointers.push_back(3); /*AT_PHDR*/              pointers.push_back(phdr_va);
        pointers.push_back(4); /*AT_PHENT*/
          pointers.push_back(fhdr->get_phextrasz() + sizeof(SgAsmElfSegmentTableEntry::Elf32SegmentTableEntry_disk));
        pointers.push_back(5); /*AT_PHNUM*/             pointers.push_back(fhdr->get_e_phnum());
        pointers.push_back(6); /*AT_PAGESZ*/            pointers.push_back(PAGE_SIZE);
        pointers.push_back(9); /*AT_ENTRY*/             pointers.push_back(fhdr->get_entry_rva() + fhdr->get_base_va());
        pointers.push_back(11); /*AT_UID*/              pointers.push_back(getuid());
        pointers.push_back(12); /*AT_EUID*/             pointers.push_back(geteuid());
        pointers.push_back(13); /*AT_GID*/              pointers.push_back(getgid());
        pointers.push_back(14); /*AT_EGID*/             pointers.push_back(getegid());
        pointers.push_back(23); /*AT_SECURE*/           pointers.push_back(false);
    }
    pointers.push_back(0); /*AT_NULL*/          pointers.push_back(0);

    /* Finalize stack initialization by writing all the pointers to data we've pushed:
     *    argc
     *    argv with NULL terminator
     *    environment with NULL terminator
     *    auxv pairs terminated with (AT_NULL,0)
     */
    sp &= ~3U; /*align to four-bytes*/
    sp -= 4 * pointers.size();
    map.write(&(pointers[0]), sp, 4*pointers.size());

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
        size_t nread = map.read(&curmem[0], ip, insn_sz);
        if (nread==insn_sz && curmem==insn->get_raw_bytes())
            return insn;
        icache.erase(found);
    }

    /* Disassemble (and cache) a new instruction */
    SgAsmx86Instruction *insn = NULL;
    try {
        insn = isSgAsmx86Instruction(disassembler->disassembleOne(&map, ip));
    } catch (Disassembler::Exception &e) {
        std::cerr <<e <<"\n";
        throw;
    }
    ROSE_ASSERT(insn!=NULL); /*only happens if our disassembler is not an x86 disassembler!*/
    icache.insert(std::make_pair(ip, insn));
    return insn;
}

void *
EmulationPolicy::my_addr(uint32_t va)
{
    /* Read from specimen in order to make sure that the memory is allocated and mapped into ROSE. */
    uint32_t word;
    size_t nread = map.read(&word, va, sizeof word);
    ROSE_ASSERT(nread==sizeof word);

    /* Obtain mapping information */
    const MemoryMap::MapElement *me = map.find(va);
    ROSE_ASSERT(me!=NULL); /*because the map.read() was successful above*/
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
        size_t nread = map.read(&byte, va++, 1);
        ROSE_ASSERT(1==nread); /*or we've read past the end of the mapped memory*/
        if (!byte)
            return retval;
        retval += byte;
    }
}

void
EmulationPolicy::copy_stat64(struct stat64 *sb, uint32_t va) {
    writeMemory<16>(x86_segreg_ds, va+0,  sb->st_dev,     true_());
    writeMemory<32>(x86_segreg_ds, va+12, sb->st_ino,     true_());
    writeMemory<32>(x86_segreg_ds, va+16, sb->st_mode,    true_());
    writeMemory<32>(x86_segreg_ds, va+20, sb->st_nlink,   true_());
    writeMemory<32>(x86_segreg_ds, va+24, sb->st_uid,     true_());
    writeMemory<32>(x86_segreg_ds, va+28, sb->st_gid,     true_());
    writeMemory<16>(x86_segreg_ds, va+32, sb->st_rdev,    true_());
    writeMemory<64>(x86_segreg_ds, va+44, sb->st_size,    true_());
    writeMemory<32>(x86_segreg_ds, va+52, sb->st_blksize, true_());
    writeMemory<32>(x86_segreg_ds, va+56, sb->st_blocks,  true_());
#ifdef FIXME
    writeMemory<32>(x86_segreg_ds, va+64, sb->st_atime,   true_());
#else
    writeMemory<32>(x86_segreg_ds, va+64, 1279897465ul,   true_()); /*use same time always for consistency when debugging*/
#endif
    writeMemory<32>(x86_segreg_ds, va+72, sb->st_mtime,   true_());
    writeMemory<32>(x86_segreg_ds, va+80, sb->st_ctime,   true_());
    writeMemory<64>(x86_segreg_ds, va+88, sb->st_ino,     true_());
}

void
EmulationPolicy::emulate_syscall()
{
    /* Warning: use hard-coded values here rather than the __NR_* constants from <sys/unistd.h> because the latter varies
     *          according to whether ROSE is compiled for 32- or 64-bit.  We always want the 32-bit syscall numbers. */
    unsigned callno = readGPR(x86_gpr_ax).known_value();
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
                map.write(buf, buf_va, nread);
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
            size_t nread = map.read(buf, buf_va, size);
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
            writeGPR(x86_gpr_ax, fd<0 ? -errno : fd);
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
            uint32_t status=arg(1), options=arg(2);
            int sys_status;
            int result = waitpid(pid, &sys_status, options);
            if (result == -1) {
                result = -errno;
            } else {
                if (status) {
                  uint32_t status_le;
                  SgAsmExecutableFileFormat::host_to_le(status, &status_le);
                  size_t nwritten = map.write(&status_le, sys_status, 4);
                  ROSE_ASSERT(4==nwritten);
                }
            }
            writeGPR(x86_gpr_ax, result);
            syscall_leave("d");
            break;
        }

        case 33: { /*0x21, access*/
            static const Translate flags[] = { TF(R_OK), TF(W_OK), TF(X_OK), TF(F_OK), T_END };
            syscall_enter("access", "sf", flags);
            uint32_t name_va=arg(0);
            std::string name = read_string(name_va);
            int mode=arg(1);
            int result = access(name.c_str(), mode);
            if (result<0) result = -errno;
            writeGPR(x86_gpr_ax, result);
            syscall_leave("d");
            break;
        }

        case 45: { /*0x2d, brk*/
            syscall_enter("brk", "x");
            uint32_t newbrk = ALIGN_DN(arg(0), PAGE_SIZE);
            int retval = 0;

            if (newbrk >= 0xb0000000ul) {
                retval = -ENOMEM;
            } else {
                if (newbrk > brk_va) {
                    MemoryMap::MapElement melmt(brk_va, newbrk-brk_va, MemoryMap::MM_PROT_READ|MemoryMap::MM_PROT_WRITE);
                    melmt.set_name("brk syscall");
                    map.insert(melmt);
                    brk_va = newbrk;
                } else if (newbrk>0 && newbrk<brk_va) {
                    map.erase(MemoryMap::MapElement(newbrk, brk_va-newbrk));
                    brk_va = newbrk;
                }
                retval= brk_va;
            }
            if (debug && newbrk!=0 && trace_mmap) {
                fprintf(debug, "  memory map after brk syscall:\n");
                map.dump(debug, "    ");
            }

            writeGPR(x86_gpr_ax, retval);
            syscall_leave("p");
            break;
        }

        case 54: { /*0x36, ioctl*/
            syscall_enter("ioctl", "ddd");
            int fd=arg(0);
            uint32_t cmd=arg(1), arg2=arg(2);
            int result = -ENOSYS;
            switch (cmd) {
                case TCGETS: { /*tcgetattr*/
                    struct termios ti;
                    result = tcgetattr(fd, &ti);
                    if (-1==result) {
                        result = -errno;
                    } else {
                        /* The Linux kernel and glibc have different definitions for termios, with very different sizes (39
                         * bytes vs 60) */                  
                        size_t nwritten = map.write(&ti, arg2, 39);
                        ROSE_ASSERT(39==nwritten);
                    }
                    break;
                }
                    
                case TIOCGPGRP: { /*tcgetpgrp*/
                    pid_t pgrp = tcgetpgrp(fd);
                    if (-1==pgrp) {
                        result = -errno;
                    } else {
                        uint32_t pgrp_le;
                        SgAsmExecutableFileFormat::host_to_le(pgrp, &pgrp_le);
                        size_t nwritten = map.write(&pgrp_le, arg2, 4);
                        ROSE_ASSERT(4==nwritten);
                        result = 0;
                    }
                    break;
                }
                    
                case TIOCSPGRP: { /*tcsetpgrp*/
                    uint32_t pgid_le;
                    size_t nread = map.read(&pgid_le, arg2, 4);
                    ROSE_ASSERT(4==nread);
                    pid_t pgid = SgAsmExecutableFileFormat::le_to_host(pgid_le);
                    result = tcsetpgrp(fd, pgid);
                    if (-1==result)
                        result = -errno;
                    break;
                }

                case TIOCGWINSZ: {
                    struct winsize ws;
                    result = ioctl(fd, TIOCGWINSZ, &ws);
                    if (-1==result) {
                        result = -errno;
                    } else {
                        size_t nwritten = map.write(&ws, arg2, sizeof ws);
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
            
        case 91: { /*0x5b, munmap*/
            syscall_enter("munmap", "pd");
            uint32_t va=arg(0);
            uint32_t sz=arg(1);
            uint32_t aligned_va = ALIGN_DN(va, PAGE_SIZE);
            uint32_t aligned_sz = ALIGN_UP(sz+va-aligned_va, PAGE_SIZE);
            map.erase(MemoryMap::MapElement(aligned_va, aligned_sz));
            if (debug && trace_mmap) {
                fprintf(debug, " memory map after munmap syscall:\n");
                map.dump(debug, "    ");
            }
            writeGPR(x86_gpr_ax, 0);
            syscall_leave("d");
            break;
        }

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
            size_t nwritten = map.write(buf, dest_va, sizeof buf);
            ROSE_ASSERT(nwritten==sizeof buf);
            writeGPR(x86_gpr_ax, 0);
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
                map.mprotect(MemoryMap::MapElement(va, aligned_sz, rose_perms));
            } catch (const MemoryMap::NotMapped &e) {
                writeGPR(x86_gpr_ax, -EFAULT);
                break;
            }

            writeGPR(x86_gpr_ax, 0);

            syscall_leave("d");
            if (debug && trace_mmap) {
                fprintf(debug, "  memory map after mprotect syscall:\n");
                map.dump(debug, "    ");
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
                size_t nread = map.read(&buf_va_le, iov_va+i*8+0, 4);
                ROSE_ASSERT(4==nread);
                uint32_t buf_va = SgAsmExecutableFileFormat::le_to_host(buf_va_le);
                
                uint32_t buf_sz_le;
                nread = map.read(&buf_sz_le, iov_va+i*8+4, 4);
                ROSE_ASSERT(4==nread);
                uint32_t buf_sz = SgAsmExecutableFileFormat::le_to_host(buf_va_le);

                if (debug)
                    fprintf(debug, "    #%d: va=0x%08"PRIx32", size=0x%08"PRIx32"\n", i, buf_va, buf_sz);
                uint8_t buf[buf_sz];
                nread = map.read(buf, buf_va, buf_sz);
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
            syscall_enter("rt_sigaction", "dppd");
            int signum=arg(0);
            uint32_t action_va=arg(1), oldact_va=arg(2);
            //size_t sigsetsize=arg(3);


            if (signum<1 || signum>_NSIG) {
                writeGPR(x86_gpr_ax, -EINVAL);
                break;
            }

            SignalAction saved = signal_action[signum];
            if (action_va) {
                size_t nread = map.read(signal_action+signum, action_va, sizeof saved);
                ROSE_ASSERT(nread==sizeof(*signal_action));
            }
            if (oldact_va) {
                size_t nwritten = map.write(&saved, oldact_va, sizeof saved);
                ROSE_ASSERT(nwritten==sizeof(*signal_action));
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
            size_t nread = map.read(&sigset, set_va, sizeof sigset);
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

            if (get_va) {
                size_t nwritten = map.write(&saved, get_va, sizeof saved);
                ROSE_ASSERT(nwritten==sizeof saved);
            }
            writeGPR(x86_gpr_ax, 0);
            syscall_leave("d");
            break;
        }

        case 191: { /*0xbf, getrlimit*/
            syscall_enter("getrlimit", "dp");
            int resource=arg(0);
            uint32_t rl_va=arg(1);
            struct rlimit rl;
            int status = getrlimit(resource, &rl);
            if (status<=0) {
                writeGPR(x86_gpr_ax, -errno);
            } else {
                if (rl_va) {
                    size_t nwritten = map.write(&rl, rl_va, sizeof rl);
                    ROSE_ASSERT(nwritten==sizeof rl);
                }
                writeGPR(x86_gpr_ax, 0);
            }
            syscall_leave("d");
            break;
        }

        case 192: { /*0xc0, mmap2*/
            static const Translate pflags[] = { TF(PROT_READ), TF(PROT_WRITE), TF(PROT_EXEC), TF(PROT_NONE), T_END };
            static const Translate mflags[] = { TF(MAP_SHARED), TF(MAP_PRIVATE), TF(MAP_32BIT), TF(MAP_ANONYMOUS), 
                                                TF(MAP_DENYWRITE), TF(MAP_EXECUTABLE), TF(MAP_FILE), TF(MAP_FIXED), 
                                                TF(MAP_GROWSDOWN), TF(MAP_LOCKED), TF(MAP_NONBLOCK), TF(MAP_NORESERVE),
                                                TF(MAP_POPULATE), T_END };
            syscall_enter("mmap2", "pdffdd", pflags, mflags);
            uint32_t start=arg(0), size=arg(1), prot=arg(2), flags=arg(3), fd=arg(4), offset=arg(5)*PAGE_SIZE;
            size_t aligned_size = ALIGN_UP(size, PAGE_SIZE);
            void *buf = NULL;
            unsigned rose_perms = ((prot & PROT_READ) ? MemoryMap::MM_PROT_READ : 0) |
                                  ((prot & PROT_WRITE) ? MemoryMap::MM_PROT_WRITE : 0) |
                                  ((prot & PROT_EXEC) ? MemoryMap::MM_PROT_EXEC : 0);

            if (!start) {
                try {
                    start = map.find_free(mmap_start, aligned_size, PAGE_SIZE);
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
                MemoryMap::MapElement melmt(start, aligned_size, buf, 0, rose_perms);
                melmt.set_name("mmap2 syscall");
                map.erase(melmt); /*clear space space first to avoid MemoryMap::Inconsistent exception*/
                map.insert(melmt);
                writeGPR(x86_gpr_ax, start);
            }

        mmap2_done:
            syscall_leave("p");
            if (debug && trace_mmap) {
                fprintf(debug, "  memory map after mmap2 syscall:\n");
                map.dump(debug, "    ");
            }

            break;
        }

        case 195: { /*0xc3, stat64*/
            syscall_enter("stat64", "sp");
            uint32_t name_va=arg(0), sb_va=arg(1);
            std::string name = read_string(name_va);
            struct stat64 sb;
            int result = stat64(name.c_str(), &sb);
            if (result<0) {
                result = -errno;
            } else {
                copy_stat64(&sb, sb_va);
            }
            writeGPR(x86_gpr_ax, result);
            syscall_leave("d");
            break;
        }

        case 197: { /*0xc5, fstat64*/
            syscall_enter("fstat64", "dp");
            int fd=arg(0);
            uint32_t sb_va=arg(1);
            struct stat64 sb;
            int result = fstat64(fd, &sb);
            if (result<0) {
                result = -errno;
            } else {
                copy_stat64(&sb, sb_va);
            }
            writeGPR(x86_gpr_ax, result);
            syscall_leave("d");
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

        case 240: { /*0xf0, futex*/
            static const Translate opflags[] = { TF(FUTEX_PRIVATE_FLAG),
                                                 TF2(FUTEX_CMD_MASK, FUTEX_WAIT),
                                                 TF2(FUTEX_CMD_MASK, FUTEX_WAKE),
                                                 TF2(FUTEX_CMD_MASK, FUTEX_FD),
                                                 TF2(FUTEX_CMD_MASK, FUTEX_REQUEUE),
                                                 TF2(FUTEX_CMD_MASK, FUTEX_CMP_REQUEUE),
                                                 TF2(FUTEX_CMD_MASK, FUTEX_WAKE_OP),
                                                 TF2(FUTEX_CMD_MASK, FUTEX_LOCK_PI),
                                                 TF2(FUTEX_CMD_MASK, FUTEX_UNLOCK_PI),
                                                 TF2(FUTEX_CMD_MASK, FUTEX_TRYLOCK_PI),
                                                 TF2(FUTEX_CMD_MASK, FUTEX_WAIT_BITSET),
                                                 TF2(FUTEX_CMD_MASK, FUTEX_WAKE_BITSET),
                                                 T_END };
            syscall_enter("futex", "pfdppd", opflags);
            //uint32_t addr1=arg(0), op=arg(1), val1=arg(2), ptimeout=arg(3), addr2=arg(4), val3=arg(5);
            writeGPR(x86_gpr_ax, -ENOSYS);
            syscall_leave("d");
            fprintf(stderr, "futex syscall is returning ENOSYS for now.\n"); /*FIXME*/
            break;
        }
            
        case 243: { /*0xf3, set_thread_area*/
            syscall_enter("set_thread_area", "p");
            uint32_t u_info_va=arg(0);
            user_desc ud;
            size_t nread = map.read(&ud, u_info_va, sizeof ud);
            ROSE_ASSERT(nread==sizeof ud);
#if 1 /*FIXME: should be using syscall_enter*/
            if (debug && trace_syscall) {
                fprintf(debug, "  set_thread_area({%d, 0x%08x, 0x%08x, %s, %u, %s, %s, %s, %s})\n",
                        (int)ud.entry_number, ud.base_addr, ud.limit,
                        ud.seg_32bit ? "32bit" : "16bit",
                        ud.contents, ud.read_exec_only ? "read_exec" : "writable",
                        ud.limit_in_pages ? "page_gran" : "byte_gran",
                        ud.seg_not_present ? "not_present" : "present",
                        ud.useable ? "usable" : "not_usable");
            }
#endif
            if (ud.entry_number==(unsigned)-1) {
                for (ud.entry_number=0x33>>3; ud.entry_number<n_gdt; ud.entry_number++) {
                    if (!gdt[ud.entry_number].useable) break;
                }
                ROSE_ASSERT(ud.entry_number<8192);
                if (debug && trace_syscall)
                    fprintf(debug, "  assigned entry number = %d\n", (int)ud.entry_number);
            }
            gdt[ud.entry_number] = ud;
            size_t nwritten = map.write(&ud, u_info_va, sizeof ud);
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
                size_t nread = map.read(tidptr, tid_va, 4);
                ROSE_ASSERT(4==nread);
                const MemoryMap::MapElement *orig = map.find(tid_va);
                MemoryMap::MapElement submap(tid_va, 4, tidptr, 0, orig->get_mapperms());
                submap.set_name("set_tid_address");
                map.insert(submap);
            } else {
                tidptr = (int*)my_addr(tid_va);
            }

            syscall(SYS_set_tid_address, tidptr);
            writeGPR(x86_gpr_ax, getpid());

            syscall_leave("d");
            if (debug && trace_mmap) {
                fprintf(debug, "  memory map after set_tid_address syscall:\n");
                map.dump(debug, "    ");
            }
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
            abort();
        }
    }
}

void
EmulationPolicy::syscall_arginfo(char format, uint32_t val, ArgInfo *info, va_list ap)
{
    ROSE_ASSERT(info!=NULL);
    info->val = val;
    switch (format) {
        case 'f':       /*flags*/
        case 'e':       /*enum*/
            info->xlate = va_arg(ap, const Translate*);
            break;
        case 's':       /*NUL-terminated string*/
            info->str = read_string(val);
            break;
    }
}

void
EmulationPolicy::syscall_enter(const char *name, const char *format, ...)
{
    va_list ap;
    va_start(ap, format);

    if (debug && trace_syscall) {
        ArgInfo args[6];
        for (size_t i=0; format[i]; i++)
            syscall_arginfo(format[i], arg(i), args+i, ap);
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
        syscall_arginfo(format[0], value, &info, ap);
        print_leave(debug, format[0], &info);
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

#if 1 /*DEBUGGING [RPM 2010-08-06]*/
    policy.trace_insn = false;
    policy.trace_syscall = true;
    policy.trace_mmap = true;
#endif

    /* Parse command-line */
    int argno = 1;
    while (argno<argc && '-'==argv[argno][0]) {
        if (!strcmp(argv[argno], "--")) {
            argno++;
            break;
        } else if (!strcmp(argv[argno], "--debug")) {
            policy.debug = stderr;
            argno++;
        } else {
            fprintf(stderr, "usage: %s [--debug] PROGRAM ARGUMENTS...\n", argv[0]);
            exit(1);
        }
    }
    ROSE_ASSERT(argc-argno>=1); /* usage: executable name followed by executable's arguments */
    SgAsmGenericHeader *fhdr = policy.load(argv[argno]); /*header for main executable, not libraries*/
    policy.initialize_stack(fhdr, argc-argno, argv+argno);

    /* Debugging */
    if (policy.debug && policy.trace_mmap) {
        fprintf(policy.debug, "memory map after program load:\n");
        policy.map.dump(policy.debug, "  ");
    }

    /* Execute the program */
    bool seen_entry_va = false;
    while (true) {
        try {
            SgAsmx86Instruction *insn = policy.current_insn();
            if (policy.debug && policy.trace_mmap &&
                !seen_entry_va && insn->get_address()==fhdr->get_base_va()+fhdr->get_entry_rva()) {
                fprintf(policy.debug, "memory map at program entry:\n");
                policy.map.dump(policy.debug, "  ");
                seen_entry_va = true;
            }
            t.processInstruction(insn);
            if (policy.debug && policy.trace_state)
                policy.dump_registers(policy.debug);
        } catch (const Semantics::Exception &e) {
            std::cerr <<e <<"\n\n";
            abort();
        } catch (const VirtualMachineSemantics::Policy::Exception &e) {
            std::cerr <<e <<"\n\n";
            abort();
        } catch (const EmulationPolicy::Exit &e) {
            /* specimen has exited */
            if (WIFEXITED(e.status)) {
                fprintf(stderr, "specimen exited with status %d\n", WEXITSTATUS(e.status));
            } else if (WIFSIGNALED(e.status)) {
                fprintf(stderr, "specimen exited due to signal %d (%s)%s\n",
                        WTERMSIG(e.status), strsignal(WTERMSIG(e.status)), 
                        WCOREDUMP(e.status)?" core dumped":"");
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
