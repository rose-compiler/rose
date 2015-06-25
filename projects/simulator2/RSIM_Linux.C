#include "rose.h"
#include "RSIM_Private.h"

#ifdef ROSE_ENABLE_SIMULATOR

#include "RSIM_Linux.h"
#include "BinaryLoaderElf.h"

#include <sys/mman.h>
#include <sys/user.h>
#include <sys/types.h>

using namespace rose;
using namespace rose::BinaryAnalysis;

void
RSIM_Linux::init() {}

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

    SimLoader(SgAsmInterpretation *interpretation, std::string default_interpname)
        : interpreter(NULL), vdso(NULL), vdso_mapped_va(0), vdso_entry_va(0) {
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

        SgBinaryComposite *composite = SageInterface::getEnclosingNode<SgBinaryComposite>(interpretation);
        ROSE_ASSERT(composite!=NULL);
        SgAsmGenericFile *file = createAsmAST(composite, vdso_name);
        ROSE_ASSERT(file!=NULL);
        SgAsmGenericHeader *fhdr = file->get_headers()->get_headers()[0];
        ROSE_ASSERT(isSgAsmElfFileHeader(fhdr)!=NULL);
        rose_addr_t entry_rva = fhdr->get_entry_rva();

        AddressInterval freeArea = map->unmapped(AddressInterval::whole().greatest(), Sawyer::Container::MATCH_BACKWARD);
        assert(!freeArea.isEmpty());
        vdso_mapped_va = alignUp(freeArea.least(), (rose_addr_t)PAGE_SIZE);
        vdso_mapped_va = std::max(vdso_mapped_va, (rose_addr_t)0x40000000); /* value used on hudson-rose-07 */

        unsigned vdso_access = MemoryMap::READABLE | MemoryMap::EXECUTABLE;
        MemoryMap::Segment vdso_segment = MemoryMap::Segment::fileInstance(vdso_name, vdso_access, "[vdso]");
        assert((ssize_t)vdso_segment.buffer()->size()==sb.st_size);
        map->insert(AddressInterval::baseSize(vdso_mapped_va, vdso_segment.buffer()->size()), vdso_segment);

        if (vdso_segment.buffer()->size()!=alignUp(vdso_segment.buffer()->size(), (rose_addr_t)PAGE_SIZE)) {
            rose_addr_t anon_va = vdso_mapped_va + vdso_segment.buffer()->size();
            rose_addr_t anon_size = alignUp(vdso_segment.buffer()->size(),
                                            (rose_addr_t)PAGE_SIZE) - vdso_segment.buffer()->size();
            map->insert(AddressInterval::baseSize(anon_va, anon_size),
                        MemoryMap::Segment::anonymousInstance(anon_size, vdso_access, vdso_segment.name()));
        }

        vdso_entry_va = vdso_mapped_va + entry_rva;
        vdso = fhdr;
        return true;
    }
};

void
RSIM_Linux::loadSpecimenArch(RSIM_Process *process, SgAsmInterpretation *interpretation, const std::string &interpreterName) {
    ASSERT_forbid(settings().nativeLoad);

    FILE *trace = (process->tracingFlags() & tracingFacilityBit(TRACE_LOADER)) ? process->tracingFile() : NULL;
    SimLoader *loader = new SimLoader(interpretation, interpreterName);
    ASSERT_require(process->headers().size() == 1);

    // Load the interpreter (dynamic linker).
    // For i386 it's usually ld-linux.so and gets loaded at 0x40000000 (setarch i386 -LRB3)
    // For x86-64 it's usually ld-linux-x86-64.so and gets loaded at 0x00007ffff7fe1000 (setarch x86_64 -R)
    // These values are initialized by the subclass constructors.
    if (loader->interpreter) {
        process->headers().push_back(loader->interpreter);
        SgAsmGenericSection *load0 = loader->interpreter->get_section_by_name("LOAD#0");
        if (load0 && load0->is_mapped() && load0->get_mapped_preferred_rva()==0 && load0->get_mapped_size()>0)
            loader->interpreter->set_base_va(interpreterBaseVa_);
        process->entryPointStartVa(loader->interpreter->get_entry_rva() + loader->interpreter->get_base_va());
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
    ASSERT_require(process->mem_ntransactions() == 0);
    process->mem_transaction_start("specimen main memory");
    process->get_memory() = *interpretation->get_map(); // shallow copy, new segments point to same old data

    /* Load and map the virtual dynamic shared library. */
    bool vdso_loaded = false;
    for (size_t i=0; i<settings().vdsoPaths.size() && !vdso_loaded; i++) {
        for (int j=0; j<2 && !vdso_loaded; j++) {
            std::string vdsoName = settings().vdsoPaths[i] + (j ? "" : "/" + vdsoName_);
            if (trace)
                fprintf(trace, "looking for vdso: %s\n", vdsoName.c_str());
            if ((vdso_loaded = loader->map_vdso(vdsoName, interpretation, &process->get_memory()))) {
                vdsoMappedVa_ = loader->vdso_mapped_va;
                vdsoEntryVa_ = loader->vdso_entry_va;
                headers.push_back(loader->vdso);
                if (trace) {
                    fprintf(trace, "mapped %s at 0x%08"PRIx64" with entry va 0x%08"PRIx64"\n",
                            vdsoName.c_str(), vdsoMappedVa_, vdsoEntryVa_);
                }
            }
        }
    }
    if (!vdso_loaded && trace && !settings().vdsoPaths.empty())
        fprintf(trace, "warning: cannot find a virtual dynamic shared object\n");

    // Cleanup
    delete loader;
}

void
RSIM_Linux::initializeSimulatedOs(RSIM_Process *process, SgAsmGenericHeader *mainHeader) {
    // Initialize the brk value. This is the first free area after the main executable.
    struct FindInitialBrk: public SgSimpleProcessing {
        rose_addr_t max_mapped_va;
        bool usePreferredMapping;

        FindInitialBrk(bool usePreferredMapping): max_mapped_va(0), usePreferredMapping(usePreferredMapping) {}

        void visit(SgNode *node) {
            SgAsmGenericSection *section = isSgAsmGenericSection(node);
            if (section && section->is_mapped()) {
                rose_addr_t begin = (section->get_mapped_actual_va() == 0 && usePreferredMapping) ?
                                    section->get_mapped_preferred_va() :
                                    section->get_mapped_actual_va();
                max_mapped_va = std::max(begin + section->get_mapped_size(), max_mapped_va);
            }
        }
    } t1(process->get_simulator()->settings().nativeLoad);
    t1.traverse(mainHeader, preorder);

    AddressInterval restriction = AddressInterval::hull(t1.max_mapped_va, AddressInterval::whole().greatest());
    process->brkVa(process->get_memory().findFreeSpace(PAGE_SIZE, PAGE_SIZE, restriction).orElse(0));

    // File descriptors. For now we just re-use ROSE's standard I/O, but in the future we could open new host descriptors to
    // serve as standard I/O for the guest.
    process->allocateFileDescriptors(0, 0);
    process->allocateFileDescriptors(1, 1);
    process->allocateFileDescriptors(2, 2);
}

template<typename Word>
static std::vector<Word>
pushEnvironmentStrings(RSIM_Process *process, rose_addr_t sp, FILE *trace) {
    // Create new environment variables by stripping "X86SIM_" off the front of any environment variable and using that value
    // to override the non-X86SIM_ value, if any.  We try to make sure the variables are in the same order as if the X86SIM_
    // overrides were not present. In other words, if X86SIM_FOO and FOO are both present, then X86SIM_FOO is deleted from the
    // list and its value used for FOO; but if X86SIM_FOO is present without FOO, then we just change the name to FOO and leave
    // it at that location. We do all this so that variables are in the same order whether run natively or under the simulator.

    // Get all the X86SIM_*=VALUE pairs and store them without the X86SIM_ prefix.
    typedef Sawyer::Container::Map<std::string, std::string> VarVal;
    VarVal varVal;
    for (int i=0; environ[i]; ++i) {
        if (0 == strncmp(environ[i], "X86SIM_", 7) && environ[i][7]!='=') {
            char *eq = strchr(environ[i], '=');
            ASSERT_not_null(eq);
            std::string var(environ[i]+7, eq);
            std::string val(eq+1);
            varVal.insert(var, val);
        }
    }

    // Build the string for all the non-X86SIM_ vars, using the overrides we found above.
    std::vector<Word> offsets;
    std::string envVarBuffer;
    for (int i=0; environ[i]; ++i) {
        if (0 != strncmp(environ[i], "X86SIM_", 7)) {
            char *eq = strchr(environ[i], '=');
            ASSERT_not_null(eq);
            std::string var(environ[i], eq);
            std::string val = varVal.getOptional(var).orElse(std::string(eq+1));
            offsets.push_back(envVarBuffer.size());
            envVarBuffer += var + "=" + val + '\0';
        }
    }

    // Write the var=val strings to the stack
    sp -= envVarBuffer.size();
    process->mem_write(envVarBuffer.c_str(), sp, envVarBuffer.size());

    // Adjust the offsets so they become addresses instead.
    BOOST_FOREACH (Word &va, offsets)
        va += sp;
    offsets.push_back(0);
    return offsets;
}

template<typename Word>
static std::vector<Word>
pushArgumentStrings(RSIM_Process *process, rose_addr_t sp, FILE *trace) {
    // Copy argv strings to the stack
    const std::vector<std::string> &argv = process->get_simulator()->exeArgs();
    std::vector<Word> pointers(argv.size()+1, 0);
    for (size_t i=argv.size(); i>0; --i) {
        size_t len = argv[i-1].size() + 1;         // including NUL terminator
        sp -= len;
        process->mem_write(argv[i-1].c_str(), sp, len);
        pointers[i-1] = sp;
    }
    pointers[argv.size()] = 0;
    if (trace) {
        for (size_t i=0; i<argv.size(); i++) {
            std::ostringstream ss;
            ss <<"argv[" <<i <<" ] " <<StringUtility::plural(argv[i].size()+1, "bytes")
               <<" at " <<StringUtility::addrToString(pointers[i])
               <<" = \"" <<StringUtility::cEscape(argv[i]) <<"\"\n";
            fputs(ss.str().c_str(), trace);
        }
    }

    return pointers;
}

template<typename Word>
rose_addr_t
RSIM_Linux::pushArgcArgvEnvAuxv(RSIM_Process *process, FILE* trace, SgAsmElfFileHeader *fhdr, rose_addr_t sp,
                                rose_addr_t execfn_va) {
    std::vector<Word> envPointers = pushEnvironmentStrings<Word>(process, sp, trace);
    sp = envPointers[0];
    std::vector<Word> argPointers = pushArgumentStrings<Word>(process, sp, trace);
    sp = argPointers[0];
    sp &= ~0xf;
    sp = pushAuxVector(process, sp, execfn_va, fhdr, trace);

    sp -= envPointers.size() * sizeof(Word);
    process->mem_write(&envPointers[0], sp, envPointers.size()*sizeof(Word));
    sp -= argPointers.size() * sizeof(Word);
    process->mem_write(&argPointers[0], sp, argPointers.size()*sizeof(Word));

    Word argc = exeArgs().size();
    sp -= sizeof argc;
    process->mem_write(&argc, sp, sizeof argc);

    return sp;
}

rose_addr_t
RSIM_Linux::segmentTableVa(SgAsmElfFileHeader *fhdr) const {
    /* Find the virtual address of the ELF Segment Table.  We actually only know its file offset directly, but the segment
     * table is also always included in one of the PT_LOAD segments, so we can compute its virtual address by finding the
     * PT_LOAD segment tha contains the table, and then looking at the table file offset relative to the segment offset. */
    struct T1: public SgSimpleProcessing {
        rose_addr_t segtab_offset;
        size_t segtab_size;
        T1(): segtab_offset(0), segtab_size(0) {}
        void visit(SgNode *node) {
            SgAsmElfSegmentTable *segtab = isSgAsmElfSegmentTable(node);
            if (0==segtab_offset && segtab!=NULL) {
                segtab_offset = segtab->get_offset();
                segtab_size = segtab->get_size();
            }
        }
    } t1;
    t1.traverse(fhdr, preorder);
    assert(t1.segtab_offset>0 && t1.segtab_size>0); /* all ELF executables have a segment table */

    struct T2: public SgSimpleProcessing {
        rose_addr_t segtab_offset, segtab_va;
        size_t segtab_size;
        T2(rose_addr_t segtab_offset, size_t segtab_size)
            : segtab_offset(segtab_offset), segtab_va(0), segtab_size(segtab_size)
            {}
        void visit(SgNode *node) {
            SgAsmElfSection *section = isSgAsmElfSection(node);
            SgAsmElfSegmentTableEntry *entry = section ? section->get_segment_entry() : NULL;
            if (entry && section->get_offset()<=segtab_offset &&
                section->get_offset()+section->get_size()>=segtab_offset+segtab_size)
                segtab_va = section->get_mapped_actual_va() + segtab_offset - section->get_offset();
        }
    } t2(t1.segtab_offset, t1.segtab_size);
    t2.traverse(fhdr, preorder);
    assert(t2.segtab_va>0); /* all ELF executables include the segment table in one of the segments */
    return t2.segtab_va;
}

void
RSIM_Linux::initializeStackArch(RSIM_Thread *thread, SgAsmGenericHeader *_fhdr) {
    if (settings().nativeLoad)
        return;                                         // the stack is already initialized
    
    RSIM_Process *process = thread->get_process();
    FILE *trace = (process->tracingFlags() & tracingFacilityBit(TRACE_LOADER)) ? process->tracingFile() : NULL;

    /* We only handle ELF for now */
    SgAsmElfFileHeader *fhdr = isSgAsmElfFileHeader(_fhdr);
    ASSERT_not_null(fhdr);

    /* Allocate the stack */
    static const size_t stack_size = 0x00016000;
    rose_addr_t origSp = thread->operators()->readRegister(thread->dispatcher()->REG_anySP)->get_number();
    rose_addr_t sp = origSp;
    rose_addr_t stack_addr = sp - stack_size;
    process->get_memory().insert(AddressInterval::baseSize(stack_addr, stack_size),
                                 MemoryMap::Segment::anonymousInstance(stack_size, MemoryMap::READABLE|MemoryMap::WRITABLE,
                                                                       "[stack]"));

    // Top eight bytes on the stack seem to be always zero.
    static const uint8_t unknown_top[] = {0, 0, 0, 0, 0, 0, 0, 0};
    sp -= sizeof unknown_top;
    process->mem_write(unknown_top, sp, sizeof unknown_top);

    // Copy the executable name to the top of the stack. It will be pointed to by the AT_EXECFN auxv.
    sp -= exeArgs()[0].size() + 1;
    rose_addr_t execfn_va = sp;
    process->mem_write(exeArgs()[0].c_str(), sp, exeArgs()[0].size()+1);

    // Argument count, argument pointer array, argument strings, environment pointer array, environment strings, the aux vector
    // used by the linker, and any data needed by the auxv.  This varies by architecture and even the environment in which
    // Linux is executing the program, not only in the sizes of the pointers, but also in what values are pushed.
    if (32 == process->wordSize()) {
        sp = pushArgcArgvEnvAuxv<uint32_t>(process, trace, fhdr, sp, execfn_va);
    } else {
        sp = pushArgcArgvEnvAuxv<uint64_t>(process, trace, fhdr, sp, execfn_va);
    }

#if 0 // DEBUGGING [Robb P. Matzke 2015-05-29]
    // Dump the stack
    {
        std::cerr <<"Initial stack contents:\n";
        rose_addr_t tmpSp = sp & ~0xf;
        size_t stackUsed = origSp - tmpSp;
        uint8_t *buf = new uint8_t[stackUsed];
        process->mem_read(buf, tmpSp, stackUsed);
        SgAsmExecutableFileFormat::hexdump(std::cerr, tmpSp, buf, stackUsed, HexdumpFormat());
        std::cerr <<"\n";
        delete[] buf;
    }
#endif

    // Initialize the stack pointer register
    const RegisterDescriptor &REG_SP = thread->dispatcher()->stackPointerRegister();
    thread->operators()->writeRegister(REG_SP, thread->operators()->number_(REG_SP.get_nbits(), sp));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      System calls
//
// The system calls defined here are identical for Linux 32- and 64-bit, although they're almost certainly at different
// locations in the syscall table.
// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Some miscellaneous stuff

void
RSIM_Linux::syscall_default_leave(RSIM_Thread *t, int callno) {
    t->syscall_leave().ret().str("\n");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux::syscall_access_enter(RSIM_Thread *t, int callno) {
    static const Translate flags[] = { TF(R_OK), TF(W_OK), TF(X_OK), TF(F_OK), T_END };
    t->syscall_enter("access").s().f(flags);
}

void
RSIM_Linux::syscall_access_body(RSIM_Thread *t, int callno) {
    rose_addr_t nameVa = t->syscall_arg(0);
    bool error;
    std::string name = t->get_process()->read_string(nameVa, 0, &error);
    if (error) {
        t->syscall_return(-EFAULT);
        return;
    }
    int mode = t->syscall_arg(1);
    int result = access(name.c_str(), mode);
    if (-1 == result)
        result = -errno;
    t->syscall_return(result);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux::syscall_brk_enter(RSIM_Thread *t, int callno) {
    t->syscall_enter("brk").p();
}

void
RSIM_Linux::syscall_brk_body(RSIM_Thread *t, int callno) {
    rose_addr_t newbrk = t->syscall_arg(0);
    t->syscall_return(t->get_process()->mem_setbrk(newbrk, t->tracing(TRACE_MMAP)));
}

void
RSIM_Linux::syscall_brk_leave(RSIM_Thread *t, int callno) {
    t->syscall_leave().eret().p().str("\n");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux::syscall_close_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("close").d();
}

void
RSIM_Linux::syscall_close_body(RSIM_Thread *t, int callno)
{
    int guestFd = t->syscall_arg(0);
    int hostFd = t->get_process()->hostFileDescriptor(guestFd);
    if (-1 == hostFd) {
        t->syscall_return(-EBADF);
    } else if (0==hostFd || 1==hostFd || 2==hostFd) {
        // Simulator is using these, so fake it.
        t->syscall_return(0);
        t->get_process()->eraseGuestFileDescriptor(guestFd);
    } else if (-1 == close(hostFd)) {
        t->syscall_return(-errno);
    } else {
        t->syscall_return(0);
        t->get_process()->eraseGuestFileDescriptor(guestFd);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux::syscall_creat_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("creat").s().d();
}

void
RSIM_Linux::syscall_creat_body(RSIM_Thread *t, int callno)
{
    rose_addr_t fileNameVa = t->syscall_arg(0);
    bool error;
    std::string fileName = t->get_process()->read_string(fileNameVa, 0, &error);
    if (error) {
        t->syscall_return(-EFAULT);
        return;
    }
    mode_t mode = t->syscall_arg(1);

    int hostFd = creat(fileName.c_str(), mode);
    if (hostFd == -1) {
        t->syscall_return(-errno);
        return;
    }
    int guestFd = t->get_process()->allocateGuestFileDescriptor(hostFd);
    if (-1 == guestFd) {
        t->syscall_return(-ENOMEM);
        return;
    }

    t->syscall_return(guestFd);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux::syscall_dup_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("dup").d();
}

void
RSIM_Linux::syscall_dup_body(RSIM_Thread *t, int callno)
{
    int guestFd = t->syscall_arg(0);
    int hostFd = t->get_process()->hostFileDescriptor(guestFd);
    int hostResult = dup(hostFd);
    int guestResult = t->get_process()->allocateGuestFileDescriptor(hostResult);
    if (-1==guestResult) guestResult = -errno;
    t->syscall_return(guestResult);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux::syscall_dup2_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("dup2").d().d();
}

void
RSIM_Linux::syscall_dup2_body(RSIM_Thread *t, int callno)
{
    int guestSource = t->syscall_arg(0);
    int hostSource = t->get_process()->hostFileDescriptor(guestSource);

    int guestTarget = t->syscall_arg(1);
    int hostTarget = t->get_process()->hostFileDescriptor(guestTarget);   // -1 if guestTarget is not opened yet

    if (-1 == hostTarget) {
        hostTarget = dup(guestSource);
    } else {
        hostTarget = dup2(hostSource, hostTarget);
    }
    if (-1 == hostTarget) {
        t->syscall_return(-errno);
        return;
    }
    t->get_process()->allocateFileDescriptors(guestTarget, hostTarget);
    t->syscall_return(guestTarget);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux::syscall_exit_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("exit").d();
}

void
RSIM_Linux::syscall_exit_body(RSIM_Thread *t, int callno)
{
    if (t->clear_child_tid) {
        uint32_t zero = 0;                              // FIXME[Robb P. Matzke 2015-06-24]: is this right for 64-bit?
        size_t n = t->get_process()->mem_write(&zero, t->clear_child_tid, sizeof zero);
        ROSE_ASSERT(n==sizeof zero);
        int nwoke = t->futex_wake(t->clear_child_tid, INT_MAX);
        ROSE_ASSERT(nwoke>=0);
    }

    // Throwing an Exit will cause the thread main loop to terminate (and perhaps the real thread terminates as well). The
    // simulated thread is effectively dead at this point.
    t->tracing(TRACE_SYSCALL) <<" = <throwing Exit>\n";
    throw RSIM_Process::Exit(__W_EXITCODE(t->syscall_arg(0), 0), false); // false=>exit only this thread
}

void
RSIM_Linux::syscall_exit_leave(RSIM_Thread *t, int callno)
{
    // This should not be reached, but might be reached if the exit system call body was skipped over.
    t->tracing(TRACE_SYSCALL) <<" = <should not have returned>\n";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux::syscall_exit_group_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("exit_group").d();
}

void
RSIM_Linux::syscall_exit_group_body(RSIM_Thread *t, int callno)
{
    if (t->clear_child_tid) {
        // From the set_tid_address(2) man page:
        //   When clear_child_tid is set, and the process exits, and the process was sharing memory with other processes or
        //   threads, then 0 is written at this address, and a futex(child_tidptr, FUTEX_WAKE, 1, NULL, NULL, 0) call is
        //   done. (That is, wake a single process waiting on this futex.) Errors are ignored.
        uint32_t zero = 0;                              // FIXME[Robb P. Matzke 2015-06-24]: is this right for 64-bit?
        size_t n = t->get_process()->mem_write(&zero, t->clear_child_tid, sizeof zero);
        ROSE_ASSERT(n==sizeof zero);
        int nwoke = t->futex_wake(t->clear_child_tid, INT_MAX);
        ROSE_ASSERT(nwoke>=0);
    }

    t->tracing(TRACE_SYSCALL) <<" = <throwing Exit>\n";
    throw RSIM_Process::Exit(__W_EXITCODE(t->syscall_arg(0), 0), true); // true=>exit entire process
}

void
RSIM_Linux::syscall_exit_group_leave(RSIM_Thread *t, int callno)
{
    // This should not be reached, but might be reached if the exit_group system call body was skipped over.
    t->tracing(TRACE_SYSCALL) <<" = <should not have returned>\n";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux::syscall_mprotect_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("mprotect").p().d().f(mmap_pflags);
}

void
RSIM_Linux::syscall_mprotect_body(RSIM_Thread *t, int callno)
{
    rose_addr_t va = t->syscall_arg(0);
    size_t size = t->syscall_arg(1);
    unsigned real_perms = t->syscall_arg(2);
    unsigned rose_perms = ((real_perms & PROT_READ) ? MemoryMap::READABLE : 0) |
                          ((real_perms & PROT_WRITE) ? MemoryMap::WRITABLE : 0) |
                          ((real_perms & PROT_EXEC) ? MemoryMap::EXECUTABLE : 0);
    if (va % PAGE_SIZE) {
        t->syscall_return(-EINVAL);
    } else {
        size_t aligned_sz = alignUp(size, (size_t)PAGE_SIZE);
        t->syscall_return(t->get_process()->mem_protect(va, aligned_sz, rose_perms, real_perms));
    }
}

void
RSIM_Linux::syscall_mprotect_leave(RSIM_Thread *t, int callno)
{
    t->syscall_leave().ret().str("\n");
    t->get_process()->mem_showmap(t->tracing(TRACE_MMAP), "  memory map after mprotect syscall:\n");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux::syscall_open_enter(RSIM_Thread *t, int callno)
{
    if (t->syscall_arg(1) & O_CREAT) {
        t->syscall_enter("open").s().f(open_flags).f(file_mode_flags);
    } else {
        t->syscall_enter("open").s().f(open_flags).unused();
    }
}

void
RSIM_Linux::syscall_open_body(RSIM_Thread *t, int callno)
{
    rose_addr_t filename_va = t->syscall_arg(0);
    bool error;
    std::string filename = t->get_process()->read_string(filename_va, 0, &error);
    if (error) {
        t->syscall_return(-EFAULT);
        return;
    }
    ASSERT_forbid(boost::starts_with(filename, "/proc/self/"));
    ASSERT_forbid(boost::starts_with(filename, "/proc/"));

    // Open the host file
    unsigned flags = t->syscall_arg(1);
    unsigned mode = (flags & O_CREAT) ? t->syscall_arg(2) : 0;
    int hostFd = open(filename.c_str(), flags, mode);
    if (-1==hostFd) {
        t->syscall_return(-errno);
        return;
    }

    // Find a free guest file descriptor to return.
    int guestFd = t->get_process()->allocateGuestFileDescriptor(hostFd);
    t->syscall_return(guestFd);

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux::syscall_pipe_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("pipe").p();
}

void
RSIM_Linux::syscall_pipe_body(RSIM_Thread *t, int callno)
{
    int32_t guest[2];
    int host[2];
    int result = pipe(host);
    if (-1==result) {
        t->syscall_return(-errno);
        return;
    }

    guest[0] = t->get_process()->allocateGuestFileDescriptor(host[0]);
    guest[1] = t->get_process()->allocateGuestFileDescriptor(host[1]);
    if (sizeof(guest)!=t->get_process()->mem_write(guest, t->syscall_arg(0), sizeof guest)) {
        close(host[0]);
        close(host[1]);
        t->get_process()->eraseGuestFileDescriptor(guest[0]);
        t->get_process()->eraseGuestFileDescriptor(guest[1]);
        t->syscall_return(-EFAULT);
        return;
    }

    t->syscall_return(result);
}

void
RSIM_Linux::syscall_pipe_leave(RSIM_Thread *t, int callno)
{
    t->syscall_leave().ret().P(8, print_int_32).str("\n");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux::syscall_pipe2_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("pipe").p().f(open_flags);
}

void
RSIM_Linux::syscall_pipe2_body(RSIM_Thread *t, int callno)
{
#ifdef HAVE_PIPE2
    int flags = t->syscall_arg(1);
    int host[2];
    int result = pipe2(host, flags);
    if (-1==result) {
        t->syscall_return(-errno);
        return;
    }

    int32_t guest[2];
    guest[0] = t->get_process()->allocateGuestFileDescriptor(host[0]);
    guest[1] = t->get_process()->allocateGuestFileDescriptor(host[1]);
    if (sizeof(guest)!=t->get_process()->mem_write(guest, t->syscall_arg(0), sizeof guest)) {
        close(host[0]);
        close(host[1]);
        t->get_process()->eraseGuestFileDescriptor(guest[0]);
        t->get_process()->eraseGuestFileDescriptor(guest[1]);
        t->syscall_return(-EFAULT);
        return;
    }

    t->syscall_return(result);
#else
    t->syscall_return(-ENOSYS);
#endif
}

void
RSIM_Linux::syscall_pipe2_leave(RSIM_Thread *t, int callno)
{
    t->syscall_leave().ret().P(8, print_int_32).str("\n");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux::syscall_read_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("read").d().p().d();
}

void
RSIM_Linux::syscall_read_body(RSIM_Thread *t, int callno)
{
    int guestFd = t->syscall_arg(0);
    int hostFd = t->get_process()->hostFileDescriptor(guestFd);
    if (-1 == hostFd) {
        t->syscall_return(-EBADF);
    } else {
        rose_addr_t buf_va = t->syscall_arg(1);
        size_t size = t->syscall_arg(2);
        char *buf = new char[size];
        ssize_t nread = read(hostFd, buf, size);
        if (-1==nread) {
            t->syscall_return(-errno);
        } else if (t->get_process()->mem_write(buf, buf_va, (size_t)nread)!=(size_t)nread) {
            t->syscall_return(-EFAULT);
        } else {
            t->syscall_return(nread);
        }
        delete[] buf;
    }
}

void
RSIM_Linux::syscall_read_leave(RSIM_Thread *t, int callno)
{
    ssize_t nread = t->syscall_arg(-1);
    t->syscall_leave().ret().arg(1).b(nread>0?nread:0).str("\n");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux::syscall_munmap_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("munmap").p().d();
}

void
RSIM_Linux::syscall_munmap_body(RSIM_Thread *t, int callno)
{
    rose_addr_t va=t->syscall_arg(0);
    size_t sz=t->syscall_arg(1);
    rose_addr_t aligned_va = alignDown(va, (rose_addr_t)PAGE_SIZE);
    size_t aligned_sz = alignUp(sz + va - aligned_va, (rose_addr_t)PAGE_SIZE);

    // Check ranges
    if (aligned_va + aligned_sz <= aligned_va) { // FIXME: not sure if sz==0 is an error
        t->syscall_return(-EINVAL);
        return;
    }

    int status = t->get_process()->mem_unmap(aligned_va, aligned_sz, t->tracing(TRACE_MMAP));
    t->syscall_return(status);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux::syscall_write_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("write").d().b(t->syscall_arg(2)).d();
}

void
RSIM_Linux::syscall_write_body(RSIM_Thread *t, int callno)
{
    int guestFd = t->syscall_arg(0);
    int hostFd = t->get_process()->hostFileDescriptor(guestFd);
    if (-1 == hostFd) {
        t->syscall_return(-EBADF);
    } else {
        rose_addr_t buf_va = t->syscall_arg(1);
        size_t size = t->syscall_arg(2);
        uint8_t *buf = new uint8_t[size];
        size_t nread = t->get_process()->mem_read(buf, buf_va, size);
        if (nread!=size) {
            t->syscall_return(-EFAULT);
        } else {
            ssize_t nwritten = write(hostFd, buf, size);
            if (-1==nwritten) {
                t->syscall_return(-errno);
            } else {
                t->syscall_return(nwritten);
            }
        }
        delete[] buf;
    }
}


#endif
