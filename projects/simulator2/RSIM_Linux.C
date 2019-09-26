#include "rose.h"
#include "RSIM_Private.h"

#ifdef ROSE_ENABLE_SIMULATOR

#include "RSIM_Linux.h"
#include "BinaryLoaderElf.h"

#include <sys/mman.h>
#include <sys/syscall.h>                                // SYS_xxx definitions
#include <sys/user.h>
#include <sys/types.h>

using namespace Rose;
using namespace Rose::BinaryAnalysis;

void
RSIM_Linux::init() {}

void
RSIM_Linux::updateExecutablePath() {
    ASSERT_require(!exeName().empty() && !exeArgs().empty());
    if (!boost::contains(exeName(), "/")) {
        ASSERT_not_null(getenv("PATH"));
        std::string path_env = getenv("PATH");
        size_t len;
        for (size_t pos=0; pos!=std::string::npos && pos<path_env.size(); pos+=len+1) {
            size_t colon = path_env.find_first_of(":;", pos);
            len = colon==std::string::npos ? path_env.size()-pos : colon-pos;
            std::string path = path_env.substr(pos, len);
            std::string fullname = path + "/" + exeName();
            if (access(fullname.c_str(), R_OK)>=0) {
                exeArgs()[0] = fullname;
                break;
            }
        }
    }

    if (access(exeArgs()[0].c_str(), R_OK)<0) {
        std::cerr <<exeArgs()[0] <<": " <<strerror(errno) <<"\n";
        exit(1);
    }
}

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
        SgAsmGenericSectionPtrList sections = BinaryLoaderElf::getRemapSections(header);
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
    bool map_vdso(const std::string &vdso_name, SgAsmInterpretation *interpretation, const MemoryMap::Ptr &map) {
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
        MemoryMap::Segment vdso_segment = MemoryMap::Segment::fileInstance(vdso_name, vdso_access, "[vdso] "+vdso_name);
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
    *process->get_memory() = *interpretation->get_map(); // shallow copy, new segments point to same old data

    /* Load and map the virtual dynamic shared library. */
    bool vdso_loaded = false;
    std::vector<std::string> paths = settings().vdsoPaths;
    if (paths.empty()) {
        paths.push_back(".");
        paths.push_back(ROSE_AUTOMAKE_TOP_BUILDDIR + "/projects/simulator2");
        paths.push_back(ROSE_AUTOMAKE_TOP_SRCDIR + "/projects/simulator2");
        paths.push_back(ROSE_AUTOMAKE_DATADIR + "/projects/simulator2");
    }
    for (size_t i=0; i<paths.size() && !vdso_loaded; ++i) {
        FileSystem::Path path = paths[i];
        for (int j=0; j<2 && !vdso_loaded; ++j) {
            FileSystem::Path name = j ? path / vdsoName_ : path;
            if (FileSystem::isFile(name) &&
                (vdso_loaded = loader->map_vdso(FileSystem::toString(name), interpretation, process->get_memory()))) {
                vdsoMappedVa_ = loader->vdso_mapped_va;
                vdsoEntryVa_ = loader->vdso_entry_va;
                headers.push_back(loader->vdso);
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
    process->brkVa(process->get_memory()->findFreeSpace(PAGE_SIZE, PAGE_SIZE, restriction).orElse(0));

    // File descriptors. For now we just re-use ROSE's standard I/O, but in the future we could open new host descriptors to
    // serve as standard I/O for the guest.
    process->allocateFileDescriptors(0, 0);
    process->allocateFileDescriptors(1, 1);
    process->allocateFileDescriptors(2, 2);

    // Load the virtual system call page
    loadVsyscalls(process);
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
    RegisterDescriptor SP = thread->get_process()->disassembler()->stackPointerRegister();
    rose_addr_t origSp = thread->operators()->readRegister(SP)->get_number();
    rose_addr_t sp = origSp;
    rose_addr_t stack_addr = sp - stack_size;
    process->get_memory()->insert(AddressInterval::baseSize(stack_addr, stack_size),
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
    RegisterDescriptor REG_SP = thread->dispatcher()->stackPointerRegister();
    thread->operators()->writeRegister(REG_SP, thread->operators()->number_(REG_SP.nBits(), sp));
}

template<class src_dirent_t, class dst_dirent_t>
void
copy_dirent_type(const src_dirent_t *src, dst_dirent_t *dst) {
    ((uint8_t*)dst)[dst->d_reclen-1] = ((const uint8_t*)src)[src->d_reclen-1];
}

template<>
void
copy_dirent_type<dirent_64, dirent64_32>(const dirent_64 *src, dirent64_32 *dst) {
    dst->d_type = ((const uint8_t*)src)[src->d_reclen-1];
}

template<class src_dirent_t, class dst_dirent_t>
struct ExtraDirentPadding {
    enum { value = 0 };
};

template<>
struct ExtraDirentPadding<dirent_32, dirent64_32> {
    enum { value = 2 };
};

template<class guest_dirent_t,                          // dirent_32, dirent64_32, or dirent_64
         class host_dirent_t>                           // dirent64_32 or dirent_64
int
RSIM_Linux::getdents_syscall(RSIM_Thread *thread, int syscallNumber, int fd, rose_addr_t dirent_va, size_t sz)
{
    size_t at = 0; /* position when filling specimen's buffer */
    uint8_t guest_buf[sz];
    uint8_t host_buf[sz];

    /* Read dentries from host kernel and copy to specimen's buffer. We must do this one dentry at a time because we don't want
     * to over read (there's no easy way to back up).  In other words, we read a dentry (but not more than what would fit in
     * the specimen) and if successful we copy to the specimen, translating from 64- to 32-bit.  The one-at-a-time requirement
     * is due to the return buffer value being run-length encoded. */
    long status = -EINVAL; /* buffer too small */
    while (at+sizeof(guest_dirent_t)<sz) {

        /* Read one dentry from host if possible */
        host_dirent_t *host_dirent = (host_dirent_t*)host_buf;
        size_t limit = sizeof(*host_dirent);
        status = -EINVAL; /* buffer too small */
        while (limit<=sz-at && -EINVAL==status) {
            status = syscall(syscallNumber, fd, host_buf, limit++);
            if (-1==status)
                status = -errno;
        }

        /* Convert and copy the host dentry into the specimen memory. */
        if (status>0) {
            ROSE_ASSERT(status>(long)sizeof(*host_dirent));
            guest_dirent_t *guest_dirent = (guest_dirent_t*)(guest_buf+at);

            /* name */
            ROSE_ASSERT(host_dirent->d_reclen > sizeof(*host_dirent));
            char *name_src = (char*)host_dirent + sizeof(*host_dirent);
            char *name_dst = (char*)guest_dirent + sizeof(*guest_dirent);
            size_t name_sz = host_dirent->d_reclen - sizeof(*host_dirent);
            memcpy(name_dst, name_src, name_sz);

            /* inode */
            guest_dirent->d_ino = host_dirent->d_ino;

            /* record length */
            guest_dirent->d_reclen = host_dirent->d_reclen - sizeof(*host_dirent) + sizeof(*guest_dirent);
            guest_dirent->d_reclen += ExtraDirentPadding<host_dirent_t, guest_dirent_t>::value;

            /* type */
            copy_dirent_type(host_dirent, guest_dirent);

            /* offset to next dentry */
            at += guest_dirent->d_reclen;
            guest_dirent->d_off = at;
        }

        /* Termination conditions */
        if (status<=0) break;
    }

    if ((size_t)at != thread->get_process()->mem_write(guest_buf, dirent_va, at))
        return -EFAULT;

    return at>0 ? at : status;
}

template int RSIM_Linux::getdents_syscall<dirent_64, dirent_64>(RSIM_Thread*, int, int, rose_addr_t, size_t);
template int RSIM_Linux::getdents_syscall<dirent_32, dirent_64>(RSIM_Thread*, int, int, rose_addr_t, size_t);
template int RSIM_Linux::getdents_syscall<dirent64_32, dirent_64>(RSIM_Thread*, int, int, rose_addr_t, size_t);


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
    t->syscall_leave().ret();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux::syscall_accept_helper(RSIM_Thread *t, int guestSrcFd, rose_addr_t addr_va, rose_addr_t addrlen_va, unsigned flags)
{
    int hostSrcFd = t->get_process()->hostFileDescriptor(guestSrcFd);
    uint8_t addr[4096];
    socklen_t addrlen = 0;
    if (addr_va != 0 && addrlen_va != 0) {
        ASSERT_require(4 == sizeof(socklen_t));
        if (4!=t->get_process()->mem_read(&addrlen, addrlen_va, 4)) {
            t->syscall_return(-EFAULT);
            return;
        }
        if (addrlen > sizeof addr) {
            t->syscall_return(-EINVAL);
            return;
        }
    }

#ifdef SYS_socketcall /*i686*/
    ROSE_ASSERT(4==sizeof(int));
    int a[4];
    a[0] = hostSrcFd;
    a[1] = addr_va ? (uint32_t)addr : 0;
    a[2] = addrlen_va ? (uint32_t)&addrlen : 0;
    a[3] = flags;
    int hostNewFd = syscall(SYS_socketcall, 18 /*SYS_ACCEPT4*/, a);
#else /*amd64*/
    int hostNewFd = syscall(SYS_accept4, hostSrcFd, addr_va?addr:NULL, addrlen_va?&addrlen:NULL, flags);
#endif
    int guestNewFd = t->get_process()->allocateGuestFileDescriptor(hostNewFd);
    if (-1 == guestNewFd) {
        t->syscall_return(-errno);
        return;
    }

    if (addr_va != 0) {
        if (addrlen != t->get_process()->mem_write(addr, addr_va, addrlen)) {
            close(hostNewFd);
            t->get_process()->eraseGuestFileDescriptor(guestNewFd);
            t->syscall_return(-EFAULT);
            return;
        }
    }
    if (addrlen_va != 0) {
        ASSERT_require(4 == sizeof(socklen_t));
        if (4 != t->get_process()->mem_write(&addrlen, addrlen_va, 4)) {
            close(hostNewFd);
            t->get_process()->eraseGuestFileDescriptor(guestNewFd);
            t->syscall_return(-EFAULT);
            return;
        }
    }

    t->syscall_return(guestNewFd);
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
RSIM_Linux::syscall_alarm_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("alarm").d();
}

void
RSIM_Linux::syscall_alarm_body(RSIM_Thread *t, int callno)
{
    int result = alarm(t->syscall_arg(0));
    t->syscall_return(result);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux::syscall_bind_helper(RSIM_Thread *t, int guestFd, rose_addr_t addr_va, size_t addrlen)
{
    int hostFd = t->get_process()->hostFileDescriptor(guestFd);
    if (addrlen<1 || addrlen>4096) {
        t->syscall_return(-EINVAL);
        return;
    }
    uint8_t *addrbuf = new uint8_t[addrlen];
    if (addrlen!=t->get_process()->mem_read(addrbuf, addr_va, addrlen)) {
        t->syscall_return(-EFAULT);
        delete[] addrbuf;
        return;
    }

#ifdef SYS_socketcall /* i686 */
    ROSE_ASSERT(4==sizeof(int));
    ROSE_ASSERT(4==sizeof(void*));
    int a[3];
    a[0] = hostFd;
    a[1] = (int)addrbuf;
    a[2] = addrlen;
    int result = syscall(SYS_socketcall, 2/*SYS_BIND*/, a);
#else /* amd64 */
    int result = syscall(SYS_bind, hostFd, addrbuf, addrlen);
#endif
    t->syscall_return(-1==result?-errno:result);
    delete[] addrbuf;
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
    t->syscall_leave().eret().p();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux::syscall_chdir_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("chdir").s();
}

void
RSIM_Linux::syscall_chdir_body(RSIM_Thread *t, int callno)
{
    rose_addr_t pathVa = t->syscall_arg(0);
    bool error;
    std::string path = t->get_process()->read_string(pathVa, 0, &error);
    if (error) {
        t->syscall_return(-EFAULT);
        return;
    }

    int result = chdir(path.c_str());
    if (result == -1) {
        t->syscall_return(-errno);
        return;
    }

    t->syscall_return(result);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux::syscall_chmod_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("chmod").s().d();
}

void
RSIM_Linux::syscall_chmod_body(RSIM_Thread *t, int callno)
{
    rose_addr_t fileNameVa = t->syscall_arg(0);
    bool error;
    std::string fileName = t->get_process()->read_string(fileNameVa, 0, &error);
    if (error) {
        t->syscall_return(-EFAULT);
        return;
    }
    mode_t mode = t->syscall_arg(1);
    int result = chmod(fileName.c_str(), mode);
    if (result == -1) result = -errno;
    t->syscall_return(result);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux::syscall_chown_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("chown").s().d().d();
}

void
RSIM_Linux::syscall_chown_body(RSIM_Thread *t, int callno)
{
    bool error;
    std::string filename = t->get_process()->read_string(t->syscall_arg(0), 0, &error);
    if (error) {
        t->syscall_return(-EFAULT);
        return;
    }
    uid_t user = t->syscall_arg(1);
    gid_t group = t->syscall_arg(2);
    int result = chown(filename.c_str(),user,group);
    t->syscall_return(result);
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
RSIM_Linux::syscall_connect_helper(RSIM_Thread *t, int guestFd, rose_addr_t addr_va, size_t addrlen)
{
    int hostFd = t->get_process()->hostFileDescriptor(guestFd);
    uint8_t addr[4096];
    if (addrlen==0 || addrlen>sizeof addr) {
        t->syscall_return(-EINVAL);
        return;
    }
    if (addrlen != t->get_process()->mem_read(addr, addr_va, addrlen)) {
        t->syscall_return(-EFAULT);
        return;
    }
    
#ifdef SYS_socketcall /*i686*/
    ROSE_ASSERT(4==sizeof(int));
    int a[3];
    a[0] = hostFd;
    a[1] = (uint32_t)addr;
    a[2] = addrlen;
    int result = syscall(SYS_socketcall, 3 /*SYS_CONNECT*/, a);
#else /*amd64*/
    int result = syscall(SYS_connect, hostFd, addr, addrlen);
#endif

    if (-1 == result) {
        t->syscall_return(-errno); 
        return;
    }
    
    t->syscall_return(result);
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
RSIM_Linux::syscall_execve_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("execve").s().p().p();
}

void
RSIM_Linux::syscall_execve_body(RSIM_Thread *t, int callno)
{
    bool error;

    /* Name of executable */
    std::string filename = t->get_process()->read_string(t->syscall_arg(0), 0, &error);
    if (error) {
        t->syscall_return(-EFAULT);
        return;
    }

    /* Argument vector */
    const size_t ptrSize = t->get_process()->wordSize() / 8;
    std::vector<std::string> argv = t->get_process()->read_string_vector(t->syscall_arg(1), ptrSize, &error);
    if (!argv.empty()) {
        for (size_t i=0; i<argv.size(); i++) {
            t->tracing(TRACE_SYSCALL) <<"    argv[" <<i <<"] = ";
            Printer::print_string(t->tracing(TRACE_SYSCALL), argv[i]);
            t->tracing(TRACE_SYSCALL) <<"\n";
        }
    }
    if (error) {
        t->syscall_return(-EFAULT);
        return;
    }
    std::vector<char*> sys_argv;
    for (size_t i = 0; i < argv.size(); ++i)
        sys_argv.push_back(strdup(argv[i].c_str()));
    sys_argv.push_back(NULL);

    /* Environment vector */
    std::vector<std::string> envp = t->get_process()->read_string_vector(t->syscall_arg(2), ptrSize, &error);
    if (!envp.empty()) {
        for (size_t i=0; i<envp.size(); i++) {
            t->tracing(TRACE_SYSCALL) <<"    envp[" <<i <<"] = ";
            Printer::print_string(t->tracing(TRACE_SYSCALL), envp[i], false, false);
            t->tracing(TRACE_SYSCALL) <<"\n";
        }
    }
    if (error) {
        t->syscall_return(-EFAULT);
        return;
    }
    std::vector<char*> sys_envp;
    for (unsigned int i = 0; i < envp.size(); ++i)
        sys_envp.push_back(strdup(envp[i].c_str()));
    sys_envp.push_back(NULL);

    /* Signal the clear_tid address if necessary, since this is sort of like a child exit. */
    t->do_clear_child_tid();

    /* The real system call */
    int result = execve(&filename[0], &sys_argv[0], &sys_envp[0]);
    ROSE_ASSERT(-1==result);
    t->syscall_return(-errno);
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
    if (t->clearChildTidVa()) {
        uint32_t zero = 0;                              // FIXME[Robb P. Matzke 2015-06-24]: is this right for 64-bit?
        size_t n = t->get_process()->mem_write(&zero, t->clearChildTidVa(), sizeof zero);
        ROSE_ASSERT(n==sizeof zero);
        int nwoke = t->futex_wake(t->clearChildTidVa(), INT_MAX);
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
    if (t->clearChildTidVa()) {
        // From the set_tid_address(2) man page:
        //   When clear_child_tid is set, and the process exits, and the process was sharing memory with other processes or
        //   threads, then 0 is written at this address, and a futex(child_tidptr, FUTEX_WAKE, 1, NULL, NULL, 0) call is
        //   done. (That is, wake a single process waiting on this futex.) Errors are ignored.
        uint32_t zero = 0;                              // FIXME[Robb P. Matzke 2015-06-24]: is this right for 64-bit?
        size_t n = t->get_process()->mem_write(&zero, t->clearChildTidVa(), sizeof zero);
        ROSE_ASSERT(n==sizeof zero);
        int nwoke = t->futex_wake(t->clearChildTidVa(), INT_MAX);
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
RSIM_Linux::syscall_fchdir_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("fchdir").d();
}

void
RSIM_Linux::syscall_fchdir_body(RSIM_Thread *t, int callno)
{
    int guestFd = t->syscall_arg(0);
    int hostFd = t->get_process()->hostFileDescriptor(guestFd);
    int result = fchdir(hostFd);
    if (result == -1)
        result = -errno;
    t->syscall_return(result);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux::syscall_fchmod_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("fchmod").d().d();
}

void
RSIM_Linux::syscall_fchmod_body(RSIM_Thread *t, int callno)
{
    int guestFd = t->syscall_arg(0);
    int hostFd = t->get_process()->hostFileDescriptor(guestFd);
    mode_t mode = t->syscall_arg(1);

    int result = fchmod(hostFd, mode);
    if (result == -1) result = -errno;
    t->syscall_return(result);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux::syscall_fchmodat_enter(RSIM_Thread *t, int callno)
{
    // Note that the library fchmodat() takes a fourth flags argument with the only defined bit being AT_SYMLINK_NOFOLLOW, but
    // the Linux 2.6.32 man page notes that "this flag is not currently implemented."
    t->syscall_enter("fchmodat").d().s().f(file_mode_flags).unused();
}

void
RSIM_Linux::syscall_fchmodat_body(RSIM_Thread *t, int callno)
{
    int guestFd = t->syscall_arg(0);
    int hostFd = t->get_process()->hostFileDescriptor(guestFd);
    rose_addr_t pathVa = t->syscall_arg(1);
    bool error;
    std::string path = t->get_process()->read_string(pathVa, 0, &error);
    if (error) {
        t->syscall_return(-EFAULT);
        return;
    }
    mode_t mode = t->syscall_arg(2);
    int flags = 0;  // t->syscall_arg(3);

    int result = fchmodat(hostFd, path.c_str(), mode, flags);
    t->syscall_return(-1==result ? -errno : result);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux::syscall_fchown_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("fchown").d().d().d();
}

void
RSIM_Linux::syscall_fchown_body(RSIM_Thread *t, int callno)
{
    int guestFd = t->syscall_arg(0);
    int hostFd = t->get_process()->hostFileDescriptor(guestFd);
    int user = t->syscall_arg(1);
    int group = t->syscall_arg(2);
    int result = syscall(SYS_fchown, hostFd, user, group);
    t->syscall_return(-1==result?-errno:result);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux::syscall_fcntl_enter(RSIM_Thread *t, int callno)
{
    static const Translate fcntl_cmds[] = { TE(F_DUPFD),
                                            TE(F_GETFD), TE(F_SETFD),
                                            TE(F_GETFL), TE(F_SETFL),
                                            TE(F_GETLK), TE(F_GETLK64),
                                            TE(F_SETLK), TE(F_SETLK64),
                                            TE(F_SETLKW), TE(F_SETLKW64),
                                            TE(F_SETOWN), TE(F_GETOWN),
                                            TE(F_SETSIG), TE(F_GETSIG),
                                            TE(F_SETLEASE), TE(F_GETLEASE),
                                            TE(F_NOTIFY),
#ifdef F_DUPFD_CLOEXEC
                                            TE(F_DUPFD_CLOEXEC),
#endif
                                            T_END};

    const char *name = t->get_process()->wordSize()==64 ? "fcntl" : "fcntl64";

    int cmd = t->syscall_arg(1);
    switch (cmd) {
        case F_DUPFD:
#ifdef F_DUPFD_CLOEXEC
        case F_DUPFD_CLOEXEC:
#endif
        case F_GETFD:
        case F_GETFL:
        case F_GETOWN:
        case F_GETSIG:
            t->syscall_enter(name).d().f(fcntl_cmds);
            break;
        case F_SETFD:
        case F_SETOWN:
            t->syscall_enter(name).d().f(fcntl_cmds).d();
            break;
        case F_SETFL:
            t->syscall_enter(name).d().f(fcntl_cmds).f(open_flags);
            break;
        case F_SETSIG:
            t->syscall_enter(name).d().f(fcntl_cmds).f(signal_names);
            break;
        case F_GETLK:
        case F_SETLK:
        case F_SETLKW:
            if (t->get_process()->wordSize()==32) {
                t->syscall_enter(name).d().f(fcntl_cmds).P(sizeof(flock_32), print_flock_32);
            } else {
                t->syscall_enter(name).d().f(fcntl_cmds).P(sizeof(flock_64), print_flock_64);
            }
            break;
        default:
            t->syscall_enter(name).d().f(fcntl_cmds).d();
            break;
    }
}

void
RSIM_Linux::syscall_fcntl_body(RSIM_Thread *t, int callno)
{
    int guestFd = t->syscall_arg(0);
    int cmd = t->syscall_arg(1);
    unsigned long other = t->syscall_arg(2);
    int result = -EINVAL;
    int hostFd = t->get_process()->hostFileDescriptor(guestFd);

    switch (cmd) {
        case F_DUPFD:
#ifdef F_DUPFD_CLOEXEC
        case F_DUPFD_CLOEXEC:
#endif
        case F_GETFD:
        case F_GETFL:
        case F_GETOWN:
        case F_GETSIG:
        case F_SETFD:
        case F_SETOWN:
        case F_SETFL:
        case F_SETSIG: {
            if (-1 == (result = fcntl(hostFd, cmd, other))) {
                t->syscall_return(-errno);
                return;
            }
            t->syscall_return(result);
            break;
        }
        case F_GETLK:
        case F_SETLK:
        case F_SETLKW: {
            static flock_native host_fl;
            if (t->get_process()->wordSize() == 32) {
                flock_32 guest_fl;
                if (sizeof(guest_fl)!=t->get_process()->mem_read(&guest_fl, t->syscall_arg(2), sizeof guest_fl)) {
                    t->syscall_return(-EFAULT);
                    break;
                }
                host_fl.l_type = guest_fl.l_type;
                host_fl.l_whence = guest_fl.l_whence;
                host_fl.l_start = guest_fl.l_start;
                host_fl.l_len = guest_fl.l_len;
                host_fl.l_pid = guest_fl.l_pid;
            } else {
                ASSERT_require(t->get_process()->wordSize() == 64);
                flock_64 guest_fl;
                if (sizeof(guest_fl)!=t->get_process()->mem_read(&guest_fl, t->syscall_arg(2), sizeof guest_fl)) {
                    t->syscall_return(-EFAULT);
                    break;
                }
                host_fl.l_type = guest_fl.l_type;
                host_fl.l_whence = guest_fl.l_whence;
                host_fl.l_start = guest_fl.l_start;
                host_fl.l_len = guest_fl.l_len;
                host_fl.l_pid = guest_fl.l_pid;
            }

#ifdef SYS_fcntl64      /* host is 32-bit */
            result = syscall(SYS_fcntl64, hostFd, cmd, &host_fl);
#else                   /* host is 64-bit */
            result = syscall(SYS_fcntl, hostFd, cmd, &host_fl);
#endif
            if (-1==result) {
                t->syscall_return(-errno);
                break;
            }
            if (F_GETLK==cmd) {
                if (t->get_process()->wordSize() == 32) {
                    flock_32 guest_fl;
                    guest_fl.l_type = host_fl.l_type;
                    guest_fl.l_whence = host_fl.l_whence;
                    guest_fl.l_start = host_fl.l_start;
                    guest_fl.l_len = host_fl.l_len;
                    guest_fl.l_pid = host_fl.l_pid;
                    if (sizeof(guest_fl)!=t->get_process()->mem_write(&guest_fl, t->syscall_arg(2), sizeof guest_fl)) {
                        t->syscall_return(-EFAULT);
                        break;
                    }
                } else {
                    ASSERT_require(t->get_process()->wordSize() == 64);
                    flock_64 guest_fl;
                    guest_fl.l_type = host_fl.l_type;
                    guest_fl.l_whence = host_fl.l_whence;
                    guest_fl.l_start = host_fl.l_start;
                    guest_fl.l_len = host_fl.l_len;
                    guest_fl.l_pid = host_fl.l_pid;
                    if (sizeof(guest_fl)!=t->get_process()->mem_write(&guest_fl, t->syscall_arg(2), sizeof guest_fl)) {
                        t->syscall_return(-EFAULT);
                        break;
                    }
                }
            }

            t->syscall_return(result);
            break;
        }
        default:
            t->syscall_return(-EINVAL);
            break;
    }
}

void
RSIM_Linux::syscall_fcntl_leave(RSIM_Thread *t, int callno)
{
    int cmd=t->syscall_arg(1);
    switch (cmd) {
        case F_GETFL:
            t->syscall_leave().eret().f(open_flags);
            break;
        case F_GETLK:
            if (t->get_process()->wordSize() == 32) {
                t->syscall_leave().ret().arg(2).P(sizeof(flock_32), print_flock_32);
            } else {
                t->syscall_leave().ret().arg(2).P(sizeof(flock_64), print_flock_64);
            }
            break;
        default:
            t->syscall_leave().ret();
            break;
    }
}
    
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux::syscall_fsync_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("fsync").d();
}

void
RSIM_Linux::syscall_fsync_body(RSIM_Thread *t, int callno)
{
    int guestFd = t->syscall_arg(0);
    int hostFd = t->get_process()->hostFileDescriptor(guestFd);
    int result = fsync(hostFd);
    t->syscall_return(-1==result?-errno:result);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux::syscall_ftruncate_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("ftruncate").d().d();
}

void
RSIM_Linux::syscall_ftruncate_body(RSIM_Thread *t, int callno)
{
    int guestFd = t->syscall_arg(0);
    int hostFd = t->get_process()->hostFileDescriptor(guestFd);
    off_t len = t->syscall_arg(1);
    int result = ftruncate(hostFd, len);
    t->syscall_return(-1==result ? -errno : result);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux::syscall_getcwd_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("getcwd").p().d();
}

void
RSIM_Linux::syscall_getcwd_body(RSIM_Thread *t, int callno)
{
    static char buf[4096]; /* page size in kernel */
    int result = syscall(SYS_getcwd, buf, sizeof buf);
    if (-1==result) {
        t->syscall_return(-errno);
        return;
    }

    size_t len = strlen(buf) + 1;
    if (len > t->syscall_arg(1)) {
        t->syscall_return(-ERANGE);
        return;
    }

    if (len!=t->get_process()->mem_write(buf, t->syscall_arg(0), len)) {
        t->syscall_return(-EFAULT);
        return;
    }

    t->syscall_return(result);
}

void
RSIM_Linux::syscall_getcwd_leave(RSIM_Thread *t, int callno)
{
    t->syscall_leave().ret().s();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux::syscall_getegid_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("getegid");
}

void
RSIM_Linux::syscall_getegid_body(RSIM_Thread *t, int callno)
{
    t->syscall_return(getegid());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux::syscall_geteuid_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("geteuid");
}

void
RSIM_Linux::syscall_geteuid_body(RSIM_Thread *t, int callno)
{
    t->syscall_return(geteuid());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux::syscall_getgid_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("getgid");
}

void
RSIM_Linux::syscall_getgid_body(RSIM_Thread *t, int callno)
{
    t->syscall_return(getgid());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux::syscall_getpgrp_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("getpgrp");
}

void
RSIM_Linux::syscall_getpgrp_body(RSIM_Thread *t, int callno)
{
    t->syscall_return(getpgrp());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux::syscall_getpid_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("getpid");
}

void
RSIM_Linux::syscall_getpid_body(RSIM_Thread *t, int callno)
{
    t->syscall_return(getpid());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux::syscall_getppid_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("getppid");
}

void
RSIM_Linux::syscall_getppid_body(RSIM_Thread *t, int callno)
{
    t->syscall_return(getppid());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux::syscall_gettid_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("gettid");
}

void
RSIM_Linux::syscall_gettid_body(RSIM_Thread *t, int callno)
{
    t->syscall_return(t->get_tid());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux::syscall_getuid_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("getuid");
}

void
RSIM_Linux::syscall_getuid_body(RSIM_Thread *t, int callno)
{
    t->syscall_return(getuid());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux::syscall_kill_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("kill").d().f(signal_names);
}

void
RSIM_Linux::syscall_kill_body(RSIM_Thread *t, int callno)
{
    pid_t pid=t->syscall_arg(0);
    int signo=t->syscall_arg(1);
    int result = t->sys_kill(pid, RSIM_SignalHandling::mk_kill(signo, SI_USER));
    t->syscall_return(result);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux::syscall_link_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("link").s().s();
}

void
RSIM_Linux::syscall_link_body(RSIM_Thread *t, int callno)
{
    bool error;

    std::string oldpath = t->get_process()->read_string(t->syscall_arg(0), 0, &error);
    if (error) {
        t->syscall_return(-EFAULT);
	return;
    }
    std::string newpath = t->get_process()->read_string(t->syscall_arg(1), 0, &error);
    if (error) {
        t->syscall_return(-EFAULT);
        return;
    }

    int result = syscall(SYS_link, oldpath.c_str(), newpath.c_str());
    t->syscall_return(-1==result?-errno:result);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux::syscall_listen_helper(RSIM_Thread *t, int guestFd, int backlog)
{
    int hostFd = t->get_process()->hostFileDescriptor(guestFd);
#ifdef SYS_socketcall /* i686 */
    ROSE_ASSERT(4==sizeof(int));
    int a[2];
    a[0] = hostFd;
    a[1] = backlog;
    int result = syscall(SYS_socketcall, 4/*SYS_LISTEN*/, a);
#else /* amd64 */
    int result = syscall(SYS_listen, hostFd, backlog);
#endif
    t->syscall_return(-1==result?-errno:result);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux::syscall_lseek_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("lseek").d().d().f(seek_whence);
}

void
RSIM_Linux::syscall_lseek_body(RSIM_Thread *t, int callno)
{
    int guestFd = t->syscall_arg(0);
    int hostFd = t->get_process()->hostFileDescriptor(guestFd);
    if (-1 == hostFd) {
        t->syscall_return(-EBADF);
        return;
    }

    off_t offset = t->syscall_arg(1);
    int whence = t->syscall_arg(2);
    off_t result = lseek(hostFd, offset, whence);
    t->syscall_return(-1==result?-errno:result);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux::syscall_madvise_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("madvise").x().d().e(madvise_behaviors);
}

void
RSIM_Linux::syscall_madvise_body(RSIM_Thread *t, int callno)
{
    rose_addr_t start = t->syscall_arg(0);
    rose_addr_t size = t->syscall_arg(1);
    int behavior = t->syscall_arg(2);

    // See linux kernel madvise_behavior_valid()
    switch (behavior) {
        case MADV_DOFORK:
        case MADV_DONTFORK:
        case MADV_NORMAL:
        case MADV_SEQUENTIAL:
        case MADV_RANDOM:
        case MADV_REMOVE:
        case MADV_WILLNEED:
        case MADV_DONTNEED:
            break;
        default:
            t->syscall_return(-EINVAL);
            return;
    }

    // Start must be page aligned
    if (start % PAGE_SIZE) {
        t->syscall_return(-EINVAL);
        return;
    }
    if (start + size < start) {
        t->syscall_return(-EINVAL);
        return;
    }

    // If pages are unmapped, return -ENOMEM
    ExtentMap mapped_mem;
    {
        SAWYER_THREAD_TRAITS::RecursiveLockGuard lock(t->get_process()->rwlock());
        AddressIntervalSet addresses(*t->get_process()->get_memory());
        mapped_mem = toExtentMap(addresses);
    }
    ExtentMap unmapped = mapped_mem.subtract_from(Extent(start, size));
    if (unmapped.size()>0) {
        t->syscall_return(-ENOMEM);
        return;
    }

    // From the madvise manpage: "This call does not influence the semantics of the application (except in the case of
    // MADV_DONTNEED), but may influence its performance.  The kernel is free to ignore the advice."  So we ignore the advise
    // in the simulator.
    if (behavior==MADV_DONTNEED) {
        t->syscall_return(-ENOSYS);
        return;
    }

    t->syscall_return(0);
    return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux::syscall_mkdir_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("mkdir").s().d();
}

void
RSIM_Linux::syscall_mkdir_body(RSIM_Thread *t, int callno)
{
    rose_addr_t pathNameVa = t->syscall_arg(0);
    bool error;
    std::string pathName = t->get_process()->read_string(pathNameVa, 0, &error);
    if (error) {
        t->syscall_return(-EFAULT);
        return;
    }
    mode_t mode = t->syscall_arg(1);

    int result = mkdir(pathName.c_str(), mode);
    if (result == -1) result = -errno;
    t->syscall_return(result);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux::syscall_mknod_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("mknod").s().f(file_mode_flags).d();
}

void
RSIM_Linux::syscall_mknod_body(RSIM_Thread *t, int callno)
{
    rose_addr_t pathVa = t->syscall_arg(0);
    int mode = t->syscall_arg(1);
    unsigned dev = t->syscall_arg(2);
    bool error;
    std::string path = t->get_process()->read_string(pathVa, 0, &error);
    if (error) {
        t->syscall_return(-EFAULT);
        return;
    }
    int result = mknod(path.c_str(), mode, dev);
    t->syscall_return(-1==result ? -errno : result);
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
    t->syscall_leave().ret();
    t->get_process()->mem_showmap(t->tracing(TRACE_MMAP), "  memory map after mprotect syscall:\n");
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
RSIM_Linux::syscall_nanosleep_enter(RSIM_Thread *t, int callno)
{
    if (t->get_process()->wordSize() == 32) {
        t->syscall_enter("nanosleep").P(sizeof(timespec_32), print_timespec_32).p();
    } else {
        t->syscall_enter("nanosleep").P(sizeof(timespec_64), print_timespec_64).p();
    }
}

void
RSIM_Linux::syscall_nanosleep_body(RSIM_Thread *t, int callno)
{
    timespec host_ts_in, host_ts_out;
    if (t->get_process()->wordSize() == 32) {
        timespec_32 guest_ts;
        if (sizeof(guest_ts)!=t->get_process()->mem_read(&guest_ts, t->syscall_arg(0), sizeof guest_ts)) {
            t->syscall_return(-EFAULT);
            return;
        }
        host_ts_in.tv_sec = guest_ts.tv_sec;
        host_ts_in.tv_nsec = guest_ts.tv_nsec;
    } else {
        ASSERT_require(t->get_process()->wordSize() == 64);
        timespec_64 guest_ts;
        if (sizeof(guest_ts)!=t->get_process()->mem_read(&guest_ts, t->syscall_arg(0), sizeof guest_ts)) {
            t->syscall_return(-EFAULT);
            return;
        }
        host_ts_in.tv_sec = guest_ts.tv_sec;
        host_ts_in.tv_nsec = guest_ts.tv_nsec;
    }
    if (host_ts_in.tv_sec<0 || (unsigned long)host_ts_in.tv_nsec >= 1000000000L) {
        t->syscall_return(-EINVAL);
        return;
    }

    int result = nanosleep(&host_ts_in, &host_ts_out);

    if (t->syscall_arg(1) && -1==result && EINTR==errno) {
        if (t->get_process()->wordSize() == 32) {
            timespec_32 guest_ts;
            guest_ts.tv_sec = host_ts_out.tv_sec;
            guest_ts.tv_nsec = host_ts_out.tv_nsec;
            if (sizeof(guest_ts)!=t->get_process()->mem_write(&guest_ts, t->syscall_arg(1), sizeof guest_ts)) {
                t->syscall_return(-EFAULT);
                return;
            }
        } else {
            ASSERT_require(t->get_process()->wordSize() == 64);
            timespec_64 guest_ts;
            guest_ts.tv_sec = host_ts_out.tv_sec;
            guest_ts.tv_nsec = host_ts_out.tv_nsec;
            if (sizeof(guest_ts)!=t->get_process()->mem_write(&guest_ts, t->syscall_arg(1), sizeof guest_ts)) {
                t->syscall_return(-EFAULT);
                return;
            }
        }
    }
    t->syscall_return(-1==result?-errno:result);
}

void
RSIM_Linux::syscall_nanosleep_leave(RSIM_Thread *t, int callno)
{
    if (t->get_process()->wordSize() == 32) {
        t->syscall_leave().ret().arg(1).P(sizeof(timespec_32), print_timespec_32);
    } else {
        t->syscall_leave().ret().arg(1).P(sizeof(timespec_64), print_timespec_64);
    }
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
RSIM_Linux::syscall_pause_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("pause");
}

void
RSIM_Linux::syscall_pause_body(RSIM_Thread *t, int callno)
{
    t->syscall_info.signo = t->sys_sigsuspend(NULL);
    t->syscall_return(-EINTR);
}

void
RSIM_Linux::syscall_pause_leave(RSIM_Thread *t, int callno)
{
    t->syscall_leave().ret();
    if (t->syscall_info.signo>0) {
        t->tracing(TRACE_SYSCALL) <<"    retured due to ";
        Printer::print_enum(t->tracing(TRACE_SYSCALL), signal_names, t->syscall_info.signo);
        t->tracing(TRACE_SYSCALL) <<"(" <<t->syscall_info.signo <<")\n";
    }
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
    t->syscall_leave().ret().P(8, print_int_32);
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
    t->syscall_leave().ret().P(8, print_int_32);
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
    t->syscall_leave().ret().arg(1).b(nread>0?nread:0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux::syscall_readlink_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("readlink").s().p().d();
}

void
RSIM_Linux::syscall_readlink_body(RSIM_Thread *t, int callno)
{
    rose_addr_t pathVa = t->syscall_arg(0);
    rose_addr_t bufVa = t->syscall_arg(1);
    size_t bufSize = t->syscall_arg(2);
    if (bufSize > 32768) {
        t->syscall_return(-ENOMEM);
        return;
    }
    bool error;
    std::string path = t->get_process()->read_string(pathVa, 0, &error);
    if (error) {
        t->syscall_return(-EFAULT);
        return;
    }

    char *buf = new char[bufSize];
    int result = readlink(path.c_str(), buf, bufSize);
    if (result == -1) {
        result = -errno;
    } else {
        size_t nWritten = t->get_process()->mem_write(buf, bufVa, result);
        if (nWritten < (size_t)result)
            result = -EFAULT;
    }
    t->syscall_return(result);
    delete[] buf;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux::syscall_rename_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("rename").s().s();
}

void
RSIM_Linux::syscall_rename_body(RSIM_Thread *t, int callno)
{
    bool error;

    std::string oldpath = t->get_process()->read_string(t->syscall_arg(0), 0, &error);
    if (error) {
        t->syscall_return(-EFAULT);
	return;
    }
    std::string newpath = t->get_process()->read_string(t->syscall_arg(1), 0, &error);
    if (error) {
        t->syscall_return(-EFAULT);
	return;
    }

    int result = syscall(SYS_rename,oldpath.c_str(), newpath.c_str());
    t->syscall_return(-1==result?-errno:result);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux::syscall_rmdir_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("rmdir").s();
}

void
RSIM_Linux::syscall_rmdir_body(RSIM_Thread *t, int callno)
{
    rose_addr_t pathNameVa = t->syscall_arg(0);
    bool error;
    std::string pathName = t->get_process()->read_string(pathNameVa, 0, &error);
    if (error) {
        t->syscall_return(-EFAULT);
        return;
    }

    int result = rmdir(pathName.c_str());
    if (result == -1) result = -errno;
    t->syscall_return(result);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux::syscall_rt_sigaction_enter(RSIM_Thread *t, int callno)
{
    if (t->get_process()->wordSize() == 32) {
        t->syscall_enter("rt_sigaction").f(signal_names).P(sizeof(sigaction_32), print_sigaction_32).p().d();
    } else {
        t->syscall_enter("rt_sigaction").f(signal_names).P(sizeof(sigaction_64), print_sigaction_64).p().d();
    }
}

void
RSIM_Linux::syscall_rt_sigaction_body(RSIM_Thread *t, int callno)
{
    int sigNum = t->syscall_arg(0);
    rose_addr_t newActionVa = t->syscall_arg(1);
    rose_addr_t oldActionVa=t->syscall_arg(2);
    size_t sigSetSize=t->syscall_arg(3);

    if (sigSetSize!=8 || sigNum<1 || sigNum>_NSIG) {
        t->syscall_return(-EINVAL);
        return;
    }

    SigAction newActionHost, oldActionHost;
    SigAction *newActionHostPtr = NULL, *oldActionHostPtr = NULL;
    if (newActionVa) {
        if (t->get_process()->wordSize() == 32) {
            sigaction_32 newActionGuest;
            if (sizeof(newActionGuest) != t->get_process()->mem_read(&newActionGuest, newActionVa, sizeof newActionGuest)) {
                t->syscall_return(-EFAULT);
                return;
            }
            newActionHost = SigAction(newActionGuest);
        } else {
            ASSERT_require(t->get_process()->wordSize() == 64);
            sigaction_64 newActionGuest;
            if (sizeof(newActionGuest) != t->get_process()->mem_read(&newActionGuest, newActionVa, sizeof newActionGuest)) {
                t->syscall_return(-EFAULT);
                return;
            }
            newActionHost = SigAction(newActionGuest);
        }
        newActionHostPtr = &newActionHost;
    }
    if (oldActionVa)
        oldActionHostPtr = &oldActionHost;
                
    int status = t->get_process()->sys_sigaction(sigNum, newActionHostPtr, oldActionHostPtr);

    if (status>=0 && oldActionVa) {
        if (t->get_process()->wordSize() == 32) {
            sigaction_32 oldActionGuest = oldActionHost.get_sigaction_32();
            if (sizeof(oldActionGuest) != t->get_process()->mem_write(&oldActionGuest, oldActionVa, sizeof oldActionGuest)) {
                t->syscall_return(-EFAULT);
                return;
            }
        } else {
            ASSERT_require(t->get_process()->wordSize() == 64);
            sigaction_64 oldActionGuest = oldActionHost.get_sigaction_64();
            if (sizeof(oldActionGuest) != t->get_process()->mem_write(&oldActionGuest, oldActionVa, sizeof oldActionGuest)) {
                t->syscall_return(-EFAULT);
                return;
            }
        }
    }

    t->syscall_return(status);
}

void
RSIM_Linux::syscall_rt_sigaction_leave(RSIM_Thread *t, int callno)
{
    if (t->get_process()->wordSize() == 32) {
        t->syscall_leave().ret().arg(2).P(sizeof(sigaction_32), print_sigaction_32);
    } else {
        t->syscall_leave().ret().arg(2).P(sizeof(sigaction_64), print_sigaction_64);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux::syscall_rt_sigprocmask_enter(RSIM_Thread *t, int callno)
{
    static const Translate flags[] = { TE(SIG_BLOCK), TE(SIG_UNBLOCK), TE(SIG_SETMASK), T_END };
    t->syscall_enter("rt_sigprocmask").e(flags).P(sizeof(RSIM_SignalHandling::SigSet), print_SigSet).p();
}

void
RSIM_Linux::syscall_rt_sigprocmask_body(RSIM_Thread *t, int callno)
{
    int how=t->syscall_arg(0);
    rose_addr_t inVa = t->syscall_arg(1), outVa = t->syscall_arg(2);
    size_t sigsetsize __attribute__((unused)) = t->syscall_arg(3);
    assert(sigsetsize==sizeof(RSIM_SignalHandling::SigSet));

    RSIM_SignalHandling::SigSet inSet, outSet;
    RSIM_SignalHandling::SigSet *inSetPtr  = inVa ? &inSet  : NULL;
    RSIM_SignalHandling::SigSet *outSetPtr = outVa? &outSet : NULL;

    if (inSetPtr && sizeof(inSet)!=t->get_process()->mem_read(inSetPtr, inVa, sizeof inSet)) {
        t->syscall_return(-EFAULT);
        return;
    }

    int result = t->sys_sigprocmask(how, inSetPtr, outSetPtr);
    t->syscall_return(result);
    if (result<0)
        return;

    if (outSetPtr && sizeof(outSet)!=t->get_process()->mem_write(outSetPtr, outVa, sizeof outSet)) {
        t->syscall_return(-EFAULT);
        return;
    }
}

void
RSIM_Linux::syscall_rt_sigprocmask_leave(RSIM_Thread *t, int callno)
{
    t->syscall_leave().ret().arg(2).P(sizeof(RSIM_SignalHandling::SigSet), print_SigSet);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux::syscall_sched_get_priority_max_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("sched_get_priority_max").f(scheduler_policies);
}

void
RSIM_Linux::syscall_sched_get_priority_max_body(RSIM_Thread *t, int callno)
{
    int policy = t->syscall_arg(0);
    int result = sched_get_priority_max(policy);
    t->syscall_return(result);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux::syscall_sched_get_priority_min_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("sched_get_priority_min").f(scheduler_policies);
}

void
RSIM_Linux::syscall_sched_get_priority_min_body(RSIM_Thread *t, int callno)
{
    int policy = t->syscall_arg(0);
    int result = sched_get_priority_min(policy);
    t->syscall_return(result);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux::syscall_sched_getscheduler_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("sched_getscheduler").d();
}

void
RSIM_Linux::syscall_sched_getscheduler_body(RSIM_Thread *t, int callno)
{
    pid_t pid = t->syscall_arg(0);
    int result = sched_getscheduler(pid);
    t->syscall_return(-1==result ? -errno : result);
}

void
RSIM_Linux::syscall_sched_getscheduler_leave(RSIM_Thread *t, int callno)
{
    t->syscall_leave().eret().f(scheduler_policies);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux::syscall_sched_yield_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("sched_yield");
}

void
RSIM_Linux::syscall_sched_yield_body(RSIM_Thread *t, int callno)
{
    t->syscall_return(sched_yield());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux::syscall_setpgid_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("setpgid").d().d();
}

void
RSIM_Linux::syscall_setpgid_body(RSIM_Thread *t, int callno)
{
    pid_t pid=t->syscall_arg(0), pgid=t->syscall_arg(1);
    int result = setpgid(pid, pgid);
    if (-1==result) { result = -errno; }
    t->syscall_return(result);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux::syscall_setsockopt_helper(RSIM_Thread *t, int guestFd, int level, int optname, rose_addr_t optval_va, size_t optsz) {
    int hostFd = t->get_process()->hostFileDescriptor(guestFd);
    uint8_t optval[4096];
    if (optsz<0 || optsz>sizeof optval) {
        t->syscall_return(-EINVAL);
        return;
    }
    if (optval_va && optsz!=t->get_process()->mem_read(optval, optval_va, optsz)) {
        t->syscall_return(-EFAULT);
        return;
    }
    
#ifdef SYS_socketcall /*i686*/
    // FIXME[Robb P. Matzke 2015-01-27]: some of these option values might need to be converted from the 32-bit guest to the
    // 64-bit host format.
    ROSE_ASSERT(4==sizeof(int));
    int a[5];
    a[0] = hostFd;
    a[1] = level;
    a[2] = optname;
    a[3] = optval_va ? (uint32_t)optval : 0;
    a[4] = optsz;
    int result = syscall(SYS_socketcall, 14 /*SYS_SETSOCKOPT*/, a);
#else /*amd64*/
    int result = syscall(SYS_setsockopt, hostFd, level, optname, optval_va?optval:NULL, optsz);
#endif
    if (-1 == result) {
        t->syscall_return(-errno);
        return;
    }
    
    t->syscall_return(result);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux::syscall_set_robust_list_enter(RSIM_Thread *t, int callno)
{
    if (t->get_process()->wordSize() == 32) {
        t->syscall_enter("set_robust_list").P(sizeof(robust_list_head_32), print_robust_list_head_32).d();
    } else {
        t->syscall_enter("set_robust_list").P(sizeof(robust_list_head_64), print_robust_list_head_64).d();
    }
}

void
RSIM_Linux::syscall_set_robust_list_body(RSIM_Thread *t, int callno)
{
    rose_addr_t head_va = t->syscall_arg(0);
    size_t len = t->syscall_arg(1);

    if (t->get_process()->wordSize() == 32) {
        if (len!=sizeof(robust_list_head_32)) {
            t->syscall_return(-EINVAL);
            return;
        }
        robust_list_head_32 guest_head;
        if (sizeof(guest_head)!=t->get_process()->mem_read(&guest_head, head_va, sizeof(guest_head))) {
            t->syscall_return(-EFAULT);
            return;
        }
    } else {
        ASSERT_require(t->get_process()->wordSize() == 64);
        if (len!=sizeof(robust_list_head_64)) {
            t->syscall_return(-EINVAL);
            return;
        }
        robust_list_head_64 guest_head;
        if (sizeof(guest_head)!=t->get_process()->mem_read(&guest_head, head_va, sizeof(guest_head))) {
            t->syscall_return(-EFAULT);
            return;
        }
    }

    /* The robust list is maintained in user space and accessed by the kernel only when we a thread dies. Since the
     * simulator handles thread death, we don't need to tell the kernel about the specimen's list until later. In
     * fact, we can't tell the kernel because that would cause our own list (set by libc) to be removed from the
     * kernel. */
    t->robustListHeadVa(head_va);
    t->syscall_return(0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux::syscall_set_tid_address_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("set_tid_address").p();
}

void
RSIM_Linux::syscall_set_tid_address_body(RSIM_Thread *t, int callno)
{
    t->clearChildTidVa(t->syscall_arg(0));
    t->syscall_return(getpid());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux::syscall_socket_helper(RSIM_Thread *t, int family, int type, int protocol)
{
#ifdef SYS_socketcall /* i686 */
    ROSE_ASSERT(4==sizeof(int));
    int a[3];
    a[0] = family;
    a[1] = type;
    a[2] = protocol;
    int guestFd = syscall(SYS_socketcall, 1/*SYS_SOCKET*/, a);
#else /* amd64 */
    int hostFd = syscall(SYS_socket, family, type, protocol);
    if (-1 == hostFd) {
        t->syscall_return(-errno);
        return;
    }
    int guestFd = t->get_process()->allocateGuestFileDescriptor(hostFd);
#endif
    t->syscall_return(guestFd);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux::syscall_symlink_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("symlink").s().s();
}

void
RSIM_Linux::syscall_symlink_body(RSIM_Thread *t, int callno)
{
    rose_addr_t oldPathVa = t->syscall_arg(0);
    rose_addr_t newPathVa = t->syscall_arg(1);
    bool error;
    std::string oldPath = t->get_process()->read_string(oldPathVa, 0, &error);
    if (error) {
        t->syscall_return(-EFAULT);
        return;
    }
    std::string newPath = t->get_process()->read_string(newPathVa, 0, &error);
    if (error) {
        t->syscall_return(-EFAULT);
        return;
    }
    int result = symlink(oldPath.c_str(), newPath.c_str());
    if (result == -1) result = -errno;
    t->syscall_return(result);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux::syscall_sync_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("sync");
}

void
RSIM_Linux::syscall_sync_body(RSIM_Thread *t, int callno)
{
    sync();
    t->syscall_return(0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux::syscall_tgkill_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("tgkill").d().d().f(signal_names);
}

void
RSIM_Linux::syscall_tgkill_body(RSIM_Thread *t, int callno)
{
    int tgid=t->syscall_arg(0), tid=t->syscall_arg(1), sig=t->syscall_arg(2);
    int result = t->sys_tgkill(tgid, tid, sig);
    t->syscall_return(result);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux::syscall_umask_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("umask").d();
}

void
RSIM_Linux::syscall_umask_body(RSIM_Thread *t, int callno)
{
    mode_t mode = t->syscall_arg(0);
    int result = syscall(SYS_umask, mode); 
    if (result == -1) result = -errno;
    t->syscall_return(result);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RSIM_Linux::syscall_unlink_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("unlink").s();
}

void
RSIM_Linux::syscall_unlink_body(RSIM_Thread *t, int callno)
{
    rose_addr_t filename_va = t->syscall_arg(0);
    bool error;
    std::string filename = t->get_process()->read_string(filename_va, 0, &error);
    if (error) {
        t->syscall_return(-EFAULT);
        return;
    }

    int result = unlink(filename.c_str());
    if (result == -1) {
        t->syscall_return(-errno);
        return;
    }

    t->syscall_return(result);
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
