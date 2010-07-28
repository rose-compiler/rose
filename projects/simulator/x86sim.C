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





#include "VirtualMachineSemantics.h"

/* These are necessary for the system call emulation */
#include <asm/ldt.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/user.h>
#include <unistd.h>

#ifndef HAVE_USER_DESC
typedef modify_ldt_ldt_s user_desc;
#endif


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

public:
    MemoryMap map;                              /* Describes how specimen's memory is mapped to simulator memory */
    Disassembler *disassembler;                 /* Disassembler to use for obtaining instructions */
    Disassembler::InstructionMap icache;        /* Cache of disassembled instructions */
    uint32_t brk_va;                            /* Current value for brk() syscall; initialized by load() */
    uint32_t phdr_va;                           /* Virtual address for PT_PHDR ELF segment, or zero; initialized by load() */
    static const size_t n_gdt=8192;             /* Number of global descriptor table entries */
    user_desc gdt[n_gdt];                       /* Global descriptor table */
    SegmentInfo segreg_shadow[6];               /* Shadow values of segment registers from GDT */
    
#if 0
    uint32_t gsOffset;
    void (*eipShadow)();
    uint32_t signalHandlers[_NSIG + 1];
    uint32_t signalStack;
    std::vector<user_desc> thread_areas;
#endif

    EmulationPolicy(): disassembler(NULL), brk_va(0), phdr_va(0) {
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
    }

    /* Print machine register state for debugging */
    void dump_registers() const {
        fprintf(stderr, "  Machine state:\n");
        fprintf(stderr, "    eax=0x%08"PRIx64" ebx=0x%08"PRIx64" ecx=0x%08"PRIx64" edx=0x%08"PRIx64"\n",
                readGPR(x86_gpr_ax).known_value(), readGPR(x86_gpr_bx).known_value(),
                readGPR(x86_gpr_cx).known_value(), readGPR(x86_gpr_dx).known_value());
        fprintf(stderr, "    esi=0x%08"PRIx64" edi=0x%08"PRIx64" ebp=0x%08"PRIx64" esp=0x%08"PRIx64" eip=0x%08"PRIx64"\n",
                readGPR(x86_gpr_si).known_value(), readGPR(x86_gpr_di).known_value(),
                readGPR(x86_gpr_bp).known_value(), readGPR(x86_gpr_sp).known_value(),
                get_ip().known_value());
        for (int i=0; i<6; i++) {
            X86SegmentRegister sr = (X86SegmentRegister)i;
            fprintf(stderr, "    %s=0x%04"PRIx64" base=0x%08"PRIx32" limit=0x%08"PRIx32" present=%s\n",
                    segregToString(sr), readSegreg(sr).known_value(), segreg_shadow[sr].base, segreg_shadow[sr].limit,
                    segreg_shadow[sr].present?"yes":"no");
        }
        fprintf(stderr, "    flags: %s %s %s %s %s %s %s\n", 
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

    /* Emulates a Linux system call from an INT 0x80 instruction. */
    void emulate_syscall();

    /* Reads a NUL-terminated string from specimen memory. */
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

#if 0
    /* Called by X86InstructionSemantics */
    void startInstruction(SgAsmInstruction* insn) {
        if (ms.signalQueue.anySignalsWaiting()) {
            simulate_signal_check(ms, insn->get_address());
        }
    }
#endif

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
            ROSE_ASSERT(nread==Len/8);
            uint64_t result = 0;
            for (size_t i=0, j=0; i<Len; i+=8, j++)
                result |= buf[j] << i;
#if 1
            fprintf(stderr, "  readMemory<%zu>(0x%08"PRIx32"+0x%08"PRIx32"=0x%08"PRIx32") -> 0x%08"PRIx64"\n",
                    Len, base, offset, base+offset, VirtualMachineSemantics::ValueType<Len>(result).known_value());
#endif
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
#if 1
            fprintf(stderr, "  writeMemory<%zu>(0x%08"PRIx32"+0x%08"PRIx32"=0x%08"PRIx32", 0x%08"PRIx64")\n",
                    Len, base, offset, base+offset, data.known_value());
#endif
            uint8_t buf[Len/8];
            for (size_t i=0, j=0; i<Len; i+=8, j++)
                buf[j] = (data.known_value() >> i) & 0xff;
            size_t nwritten = map.write(buf, base+offset, Len/8);
            ROSE_ASSERT(nwritten==Len/8);
        }
    }
};

SgAsmGenericHeader*
EmulationPolicy::load(const char *name)
{
    fprintf(stderr, "loading %s...\n", name);
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
    map.insert(MemoryMap::MapElement(stack_addr, stack_size, MemoryMap::MM_PROT_READ|MemoryMap::MM_PROT_WRITE));

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
    fprintf(stderr, "esp after argc/argv = 0x%08zx, pointers=%zu\n", sp, pointers.size());

    /* Initialize the stack with specimen's environment. For now we'll use the same environment as this simulator. */
    for (int i=0; true; i++) {
        if (!environ[i]) break;
        size_t len = strlen(environ[i]) + 1;
        sp -= len;
        map.write(environ[i], sp, len);
        pointers.push_back(sp);
    }
    pointers.push_back(0); /*environment NULL terminator*/
    fprintf(stderr, "esp after environ = 0x%08zx, pointers=%zu\n", sp, pointers.size());

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
    fprintf(stderr, "esp after auxv = 0x%08zx, pointers=%zu\n", sp, pointers.size());

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
    fprintf(stderr, "esp = 0x%08zx\n", sp);
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
        fprintf(stderr, "disassembly failed at eip=0x%08"PRIx64": %s\n", e.ip, e.mesg.c_str());
        throw;
    }
    ROSE_ASSERT(insn!=NULL); /*only happens if our disassembler is not an x86 disassembler!*/
    icache.insert(std::make_pair(ip, insn));
    return insn;
}

std::string
EmulationPolicy::read_string(uint32_t va)
{
    std::string retval;
    while (1) {
        uint8_t byte;
        size_t nread = map.read(&byte, va++, 1);
        ROSE_ASSERT(1==nread); /*or we've read past the end of the mapped memory*/
        retval += byte;
        if (!byte)
            return retval;
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
            int fd = readGPR(x86_gpr_bx).known_value();
            uint32_t buf_va = readGPR(x86_gpr_cx).known_value();
            uint32_t count = readGPR(x86_gpr_dx).known_value();
            char buf[count];
            ssize_t nread = read(fd, buf, count);
            if (nread<0) {
                writeGPR(x86_gpr_ax, -errno);
            } else {
                writeGPR(x86_gpr_ax, nread);
                map.write(buf, buf_va, nread);
            }
            break;
        }

        case 5: { /*open*/
            uint32_t filename_va = readGPR(x86_gpr_bx).known_value();
            std::string filename = read_string(filename_va);
            uint32_t flags = readGPR(x86_gpr_cx).known_value();
            uint32_t mode = (flags & O_CREAT) ? readGPR(x86_gpr_dx).known_value() : 0;
            int fd = open(filename.c_str(), flags, mode);
            writeGPR(x86_gpr_ax, fd<0 ? -errno : fd);
            break;
        }

        case 6: { /*close*/
            int fd = readGPR(x86_gpr_bx).known_value();
            if (1==fd || 2==fd) {
                /* ROSE is using these */
                writeGPR(x86_gpr_ax, -EPERM);
            } else {
                int status = close(fd);
                writeGPR(x86_gpr_ax, status<0 ? -errno : status);
            }
            break;
        }

        case 33: { /*0x21, access*/
            uint32_t name_va = readGPR(x86_gpr_bx).known_value();
            std::string name = read_string(name_va);
            int mode = readGPR(x86_gpr_cx).known_value();
            int result = access(name.c_str(), mode);
            if (result<0) result = -errno;
            writeGPR(x86_gpr_ax, result);
            break;
        }

        case 45: { /*0x2d, brk*/
            uint32_t newbrk = ALIGN_DN(readGPR(x86_gpr_bx).known_value(), PAGE_SIZE);
#if 1
            fprintf(stderr, "  brk(0x%08x) -- old brk is 0x%08x\n", newbrk, brk_va);
#endif
            if (newbrk >= 0xb0000000ul) {
                writeGPR(x86_gpr_ax, -ENOMEM);
            } else {
                if (newbrk > brk_va) {
                    map.insert(MemoryMap::MapElement(brk_va, newbrk-brk_va, MemoryMap::MM_PROT_READ|MemoryMap::MM_PROT_WRITE));
                    brk_va = newbrk;
                } else if (newbrk>0 && newbrk<brk_va) {
                    map.erase(MemoryMap::MapElement(newbrk, brk_va-newbrk));
                    brk_va = newbrk;
                }
                writeGPR(x86_gpr_ax, brk_va);
            }
#if 1
            if (newbrk!=0) {
                fprintf(stderr, "  memory map after brk():\n");
                map.dump(stderr, "    ");
            }
#endif
            break;
        }

        case 122: { /*0x7a, uname*/
            char buf[6*65];
            memset(buf, ' ', sizeof buf);
            strcpy(buf+0*65, "Linux");                                  /*sysname*/
            strcpy(buf+1*65, "mymachine.example.com");                  /*nodename*/
            strcpy(buf+2*65, "2.6.9");                                  /*release*/
            strcpy(buf+3*65, "#1 SMP Wed Jun 18 12:35:02 EDT 2008");    /*version*/
            strcpy(buf+4*65, "i386");                                   /*machine*/
            strcpy(buf+5*65, "example.com");                            /*domainname*/
            map.write(buf, readGPR(x86_gpr_bx).known_value(), sizeof buf); /*fixme: possible sigsegv for specimen*/
            writeGPR(x86_gpr_ax, 0);
            break;
        }

        case 192: { /*0xc0, mmap2*/
            uint32_t start = readGPR(x86_gpr_bx).known_value();
            uint32_t size = readGPR(x86_gpr_cx).known_value();
            uint32_t prot = readGPR(x86_gpr_dx).known_value();
            uint32_t flags = readGPR(x86_gpr_si).known_value();
            uint32_t fd = readGPR(x86_gpr_di).known_value();
            uint32_t offset = readGPR(x86_gpr_bp).known_value() * PAGE_SIZE;
#if 1
            fprintf(stderr,
                    "  mmap(start=0x%08"PRIx32", size=0x%08"PRIx32", prot=%04"PRIo32", flags=0x%"PRIx32
                    ", fd=%d, offset=0x%08"PRIx32")\n",
                    start, size, prot, flags, fd, offset);
#endif

            size_t aligned_size = ALIGN_UP(size, PAGE_SIZE);
            if (!start) {
                try {
                    start = map.find_free(0x40000000ul, aligned_size, PAGE_SIZE);
                } catch (const MemoryMap::NoFreeSpace &e) {
                    fprintf(stderr, "  (cannot satisfy request for %zu bytes)\n", e.size);
                    writeGPR(x86_gpr_ax, -ENOMEM);
                    break;
                }
#if 1
                fprintf(stderr, "  start = 0x%08"PRIx32"\n", start);
#endif
            }

            unsigned rose_perms = ((prot & PROT_READ) ? MemoryMap::MM_PROT_READ : 0) |
                                  ((prot & PROT_WRITE) ? MemoryMap::MM_PROT_WRITE : 0) |
                                  ((prot & PROT_EXEC) ? MemoryMap::MM_PROT_EXEC : 0);

            void *buf = NULL;
            if (flags & MAP_ANONYMOUS) {
                buf = mmap(NULL, size, prot, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
            } else {
                buf = mmap(NULL, size, prot, flags & ~MAP_FIXED, fd, offset);
            }
            if (MAP_FAILED==buf) {
                writeGPR(x86_gpr_ax, -errno);
            } else {
                map.erase(MemoryMap::MapElement(start, aligned_size));
                map.insert(MemoryMap::MapElement(start, aligned_size, buf, 0, rose_perms));
                writeGPR(x86_gpr_ax, start);
            }
            break;
        }

        case 195: { /*0xc3, stat64*/
            uint32_t name_va = readGPR(x86_gpr_bx).known_value();
            std::string name = read_string(name_va);
            uint32_t sb_va = readGPR(x86_gpr_cx).known_value();
            struct stat64 sb;
            int result = stat64(name.c_str(), &sb);
            if (result<0) {
                result = -errno;
            } else {
                copy_stat64(&sb, sb_va);
            }
            writeGPR(x86_gpr_ax, result);
            break;
        }

        case 197: { /*0xc5, fstat64*/
            int fd = readGPR(x86_gpr_bx).known_value();
            uint32_t sb_va = readGPR(x86_gpr_cx).known_value();
            struct stat64 sb;
            int result = fstat64(fd, &sb);
            if (result<0) {
                result = -errno;
            } else {
                copy_stat64(&sb, sb_va);
            }
            writeGPR(x86_gpr_ax, result);
            break;
        }

        default: {
            fprintf(stderr, "syscall %u is not implemented yet.\n\n", callno);
            abort();
        }
    }
}

int
main(int argc, char *argv[])
{
    typedef X86InstructionSemantics<EmulationPolicy, VirtualMachineSemantics::ValueType> Semantics;
    EmulationPolicy policy;
    Semantics t(policy);

    ROSE_ASSERT(argc>=2); /* usage: executable name followed by executable's arguments */
    SgAsmGenericHeader *fhdr = policy.load(argv[1]); /*header for main executable, not libraries*/
    policy.initialize_stack(fhdr, argc-1, argv+1);

    /* Debugging */
    fprintf(stderr, "Memory map:\n");
    policy.map.dump(stderr, "  ");

    /* Execute the program */
    size_t ninsns = 0;
    while (true) {
        try {
            SgAsmx86Instruction *insn = policy.current_insn();
#if 0
            fprintf(stderr, "\033[K\n[%07zu] %s\033[K\r\033[1A", ninsns++, unparseInstructionWithAddress(insn).c_str());
#else
            fprintf(stderr, "[%07zu] %s\n", ninsns++, unparseInstructionWithAddress(insn).c_str());
#endif
            t.processInstruction(insn);
        } catch (const Semantics::Exception &e) {
            std::cerr <<e <<"\n\n";
            abort();
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
