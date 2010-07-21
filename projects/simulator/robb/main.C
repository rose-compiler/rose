/* Emulates an executable. */
#include "rose.h"
#include "VirtualMachineSemantics.h"

/* We use the VirtualMachineSemantics policy. That policy is able to handle a certain level of symbolic computation, but we
 * use it because it also does constant folding, which means that it's symbolic aspects are never actually used here. We only
 * have a few methods to specialize this way.   The VirtualMachineSemantics::Memory is not used -- we use a MemoryMap instead
 * since we're only operating on known addresses and values, and thus override all superclass methods dealing with memory. */
class EmulationPolicy: public VirtualMachineSemantics::Policy {
public:
    MemoryMap *map;                             /* describes how specimen's memory is mapped to simulator memory */
#if 0
    uint32_t gsOffset;
    void (*eipShadow)();
    uint32_t signalHandlers[_NSIG + 1];
    uint32_t signalStack;
    std::vector<user_desc> thread_areas;
#endif

    EmulationPolicy() {
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

    /* Load an executable plus libraries into memory, creating the MemoryMap object that describes the mapping from the
     * specimen's address space to the simulator's address space. The arguments passed to this function are those that would
     * be applicable to ROSE itself, not the arguments for the specimen.
     *
     * There are two ways to load dynamic libraries:
     *   1. Load the dynamic linker (ld-linux.so) and simulate it in order to load the libraries.  This is the most accurate
     *      since it delegates the dynamic linking to the actual dynamic linker.  It thus allows different linkers to be
     *      used.
     *   2. Use Matt Brown's work to have ROSE itself resolve the dynamic linking issues.  This approach gives us better
     *      control over the finer details such as which directories are searched, etc. since we have total control over the
     *      linker.  However, Matt's work is not complete at this time [2010-07-20].
     *
     * We will use Matt's approach for now even though Jeremiah's original work used the first approach. This gives us an
     * opportunity to test those parts of ROSE. */
    void load_program(int argc, char *argv[]);

    /* Initialize the stack for the specimen.  The argc and argv are the command-line of the specimen, not ROSE or the
     * simulator. */
    void initialize_stack(int argc, char *argv[]);

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
#if 0
        linuxSyscall(ms);
#else
        ROSE_ASSERT(!"syscalls not handled yet");
#endif
    }

#if 0
    /* Called by X86InstructionSemantics */
    void startInstruction(SgAsmInstruction* insn) {
        if (ms.signalQueue.anySignalsWaiting()) {
            simulate_signal_check(ms, insn->get_address());
        }
    }
#endif
};

void EmulationPolicy::load_program(int argc, char *argv[])
{
    int new_argc = argc+1;
    char **new_argv = new char*[argc+1];
    new_argv[0] = strdup("-rose:read_executable_file_format_only"); /*no need to disassemble yet*/
    for (int i=0; i<=argc; i++)
        new_argv[i+1] = argv[i];

    SgProject *project = frontend(new_argc, new_argv);

    /* Find the best file header. If an executable has both a DOS and a PE header then the PE header is preferred. */
    struct T1: public SgSimpleProcessing {
        SgAsmGenericHeader *best;
        T1(SgNode *ast): best(NULL) {
            traverse(ast, preorder);
        }
        void visit(SgNode *node) {
            if (!best || isSgAsmPEFileHeader(node))
                best = isSgAsmGenericHeader(node);
        }
    } fhdr(project);
    ROSE_ASSERT(fhdr.best!=NULL);

    /* Build a memory map for the executable and dynamic libraries. We load all LOAD segments into memory. */
    struct T2: public Loader::Selector {
        virtual Loader::Contribution contributes(SgAsmGenericSection *_section) {
            SgAsmElfSection *section = isSgAsmElfSection(_section);
            SgAsmElfSegmentTableEntry *segment = section ? section->get_segment_entry() : NULL;
            if (segment) {
                switch (segment->get_type()) {
                    case SgAsmElfSegmentTableEntry::PT_LOAD:
                    case SgAsmElfSegmentTableEntry::PT_INTERP:
                        return Loader::CONTRIBUTE_ADD;
                    default:
                        return Loader::CONTRIBUTE_NONE;
                }
            }
            return Loader::CONTRIBUTE_NONE;
        }
    } selector;

    Loader *loader = Loader::find_loader(fhdr.best);
    ROSE_ASSERT(loader!=NULL);
    map = loader->create_map(NULL, fhdr.best->get_mapped_sections(), &selector);
    ROSE_ASSERT(map!=NULL);
}

void EmulationPolicy::initialize_stack(int argc, char *argv[])
{
    /* load_program() must have been called already */
    ROSE_ASSERT(map!=NULL);

    /* Allocate the stack */
    static const size_t stack_size = 0x01000000;
    size_t sp = readGPR(x86_gpr_sp).known_value();
    size_t stack_addr = sp - stack_size;
    void *stack = new uint8_t[stack_size];
    map->insert(MemoryMap::MapElement(stack_addr, stack_size, stack, 0, MemoryMap::MM_PROT_READ|MemoryMap::MM_PROT_WRITE));

    /* Initialize the stack with specimen's argc and argv */
    std::vector<uint32_t> pointers;                     /* pointers pushed onto stack at the end of initialization */
    for (int i=0; i<argc; i++) {
        size_t len = strlen(argv[i]) + 1; /*inc. NUL termination*/
        sp -= len;
        map->write(argv[i], sp, len);
        pointers.push_back(sp);
    }
    pointers.push_back(0); /*the argv NULL terminator*/

    /* Initialize the stack with specimen's environment. For now we'll use the same environment as this simulator. */
    for (int i=0; true; i++) {
        if (!environ[i]) break;
        size_t len = strlen(environ[i]) + 1;
        sp -= len;
        map->write(environ[i], sp, len);
        pointers.push_back(sp);
    }
    pointers.push_back(0); /*environment NULL terminator*/

    /* Initialize stack with auxv, where each entry is two words in the pointers vector. This information is only present for
     * dynamically linked executables. */
#if 0 /*FIXME*/
    if (has_interp) {
        pointers.push_back(AT_PHDR);   pointers.push_back(phdr);
        pointers.push_back(AT_PHENT);  pointers.push_back(phent);
        pointers.push_back(AT_PHNUM);  pointers.push_back(phnum);
        pointers.push_back(AT_PAGESZ); pointers.push_back(PAGE_SIZE);
        pointers.push_back(AT_ENTRY);  pointers.push_back(entry);
        pointers.push_back(AT_UID);    pointers.push_back(getuid());
        pointers.push_back(AT_EUID);   pointers.push_back(geteuid());
        pointers.push_back(AT_GID);    pointers.push_back(getgid());
        pointers.push_back(AT_EGID);   pointers.push_back(getegid());
        pointers.push_back(AT_SECURE); pointers.push_back(false);
    }
#endif
    pointers.push_back(0 /*AT_NULL*/); pointers.push_back(0);

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

int
main(int argc, char *argv[])
{
    EmulationPolicy policy;

    policy.load_program(argc, argv);

    /* Set up specimen's argc and argv. FIXME: for now we just set them to 0 and (NULL) */
    int specimen_argc = 0;
    char **specimen_argv = new char*[specimen_argc+1];
    specimen_argv[specimen_argc] = NULL;
    policy.initialize_stack(specimen_argc, specimen_argv);

    fprintf(stdout, "Memory map:\n");
    policy.map->dump(stdout, "  ");
}
