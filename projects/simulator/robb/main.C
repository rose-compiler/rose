/* Emulates an executable. */
#include "rose.h"
#include "VirtualMachineSemantics.h"

/* These are necessary for the system call emulation */
#include <errno.h>
#include <fcntl.h>
#include <sys/user.h>
#include <unistd.h>


/* We use the VirtualMachineSemantics policy. That policy is able to handle a certain level of symbolic computation, but we
 * use it because it also does constant folding, which means that it's symbolic aspects are never actually used here. We only
 * have a few methods to specialize this way.   The VirtualMachineSemantics::Memory is not used -- we use a MemoryMap instead
 * since we're only operating on known addresses and values, and thus override all superclass methods dealing with memory. */
class EmulationPolicy: public VirtualMachineSemantics::Policy {
public:
    MemoryMap map;                              /* Describes how specimen's memory is mapped to simulator memory */
    Disassembler *disassembler;                 /* Disassembler to use for obtaining instructions */
    Disassembler::InstructionMap icache;        /* Cache of disassembled instructions */
    uint32_t brk_va;                            /* Current value for brk() syscall; initialized by load() */
    uint32_t phdr_va;                           /* Virtual address for PT_PHDR ELF segment, or zero; initialized by load() */
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
        for (size_t i=0; i<VirtualMachineSemantics::State::n_segregs; i++)
            writeSegreg((X86SegmentRegister)i, 0);
        for (size_t i=0; i<VirtualMachineSemantics::State::n_flags; i++)
            writeFlag((X86Flag)i, 0);
        writeIP(0);
        writeFlag(x86_flag_1, true_());
        writeGPR(x86_gpr_sp, 0xc0000000ul);
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

    /* Reads a NUL-terminated string from memory */
    std::string read_string(uint32_t va);

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

    /* Reads memory from the memory map rather than the super class. */
    template <size_t Len> VirtualMachineSemantics::ValueType<Len>
    readMemory(X86SegmentRegister segreg, const VirtualMachineSemantics::ValueType<32> &addr,
               const VirtualMachineSemantics::ValueType<1> cond) const {
        ROSE_ASSERT(0==Len % 8 && Len<=32);
        ROSE_ASSERT(addr.is_known());
        ROSE_ASSERT(cond.is_known());
        if (cond.known_value()) {
            uint8_t buf[Len/8];
            size_t nread = map.read(buf, addr.known_value(), Len/8);
            ROSE_ASSERT(nread==Len/8);
            uint64_t result = 0;
            for (size_t i=0, j=0; i<Len; i+=8, j++)
                result |= buf[j] << i;
#if 1
            fprintf(stderr, "  readMemory<%zu>(0x%08"PRIx64") -> 0x%08"PRIx64"\n",
                    Len, addr.known_value(), VirtualMachineSemantics::ValueType<Len>(result).known_value());
#endif
            return result;
        } else {
            return 0;
        }
    }

    /* Writes memory to the memory map rather than the super class. */
    template <size_t Len> void
    writeMemory(X86SegmentRegister segreg, const VirtualMachineSemantics::ValueType<32> &addr, 
                const VirtualMachineSemantics::ValueType<Len> &data,  VirtualMachineSemantics::ValueType<1> cond) {
        ROSE_ASSERT(0==Len % 8 && Len<=32);
        ROSE_ASSERT(addr.is_known());
        ROSE_ASSERT(data.is_known());
        ROSE_ASSERT(cond.is_known());
        if (cond.known_value()) {
#if 1
            fprintf(stderr, "  writeMemory<%zu>(0x%08"PRIx64", 0x%08"PRIx64")\n", Len, addr.known_value(), data.known_value());
#endif
            uint8_t buf[Len/8];
            for (size_t i=0, j=0; i<Len; i+=8, j++)
                buf[j] = (data.known_value() >> i) & 0xff;
            size_t nwritten = map.write(buf, addr.known_value(), Len/8);
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
    SgAsmx86Instruction *insn = isSgAsmx86Instruction(disassembler->disassembleOne(&map, ip));
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

        case 45: { /*brk*/
            uint32_t newbrk = ALIGN_DN(readGPR(x86_gpr_bx).known_value(), PAGE_SIZE);
            if (newbrk >= 0xb0000000ul) {
                writeGPR(x86_gpr_ax, -ENOMEM);
            } else {
                if (newbrk > brk_va) {
                    map.insert(MemoryMap::MapElement(brk_va, newbrk-brk_va, MemoryMap::MM_PROT_READ|MemoryMap::MM_PROT_WRITE));
                    brk_va = newbrk;
                } else if (newbrk < brk_va) {
                    map.erase(MemoryMap::MapElement(newbrk, brk_va-newbrk));
                    brk_va = newbrk;
                }
                writeGPR(x86_gpr_ax, brk_va);
            }
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

    ROSE_ASSERT(argc==2); /*usage: only arg must be the executable name*/
    SgAsmGenericHeader *fhdr = policy.load(argv[1]); /*header for main executable, not libraries*/

    /* Set up specimen's argc and argv. FIXME: for now we just set them to 0 and (NULL) */
    int specimen_argc = 0;
    char **specimen_argv = new char*[specimen_argc+1];
    specimen_argv[specimen_argc] = NULL;
    policy.initialize_stack(fhdr, specimen_argc, specimen_argv);

    /* Debugging */
    fprintf(stdout, "Memory map:\n");
    policy.map.dump(stdout, "  ");

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
