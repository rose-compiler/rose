#include "rose.h"
#include "RSIM_Private.h"

#ifdef ROSE_ENABLE_SIMULATOR

#include "RSIM_Linux.h"
#include "BinaryLoaderElf.h"

#include <sys/mman.h>
#include <sys/user.h>
#include <sys/types.h>

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
    FILE *trace = (process->tracingFlags() & tracingFacilityBit(TRACE_LOADER)) ? process->tracingFile() : NULL;
    SimLoader *loader = new SimLoader(interpretation, interpreterName);

    /* If we found an interpreter then use its entry address as the start of simulation.  When running the specimen directly
     * in Linux with "setarch i386 -LRB3", the ld-linux.so.2 gets mapped to 0x40000000 if it has no preferred address.  We can
     * accomplish the same thing simply by rebasing the library. */
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
    for (size_t i=0; i<vdsoPaths_.size() && !vdso_loaded; i++) {
        for (int j=0; j<2 && !vdso_loaded; j++) {
            std::string vdsoName = vdsoPaths_[i] + (j ? "" : "/" + vdsoName_);
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
    if (!vdso_loaded && trace && !vdsoPaths_.empty())
        fprintf(trace, "warning: cannot find a virtual dynamic shared object\n");

    // Initialize the brk value
    struct FindInitialBrk: public SgSimpleProcessing {
        FindInitialBrk(): max_mapped_va(0) {}
        rose_addr_t max_mapped_va;
        void visit(SgNode *node) {
            SgAsmGenericSection *section = isSgAsmGenericSection(node);
            if (section && section->is_mapped())
                max_mapped_va = std::max(section->get_mapped_actual_va() + section->get_mapped_size(), max_mapped_va);
        }
    } t1;
    BOOST_FOREACH (SgAsmGenericHeader *fhdr, process->headers())
        t1.traverse(fhdr, preorder);
    AddressInterval restriction = AddressInterval::hull(t1.max_mapped_va, AddressInterval::whole().greatest());
    process->brkVa(process->get_memory().findFreeSpace(PAGE_SIZE, PAGE_SIZE, restriction).orElse(0));

    // Cleanup
    delete loader;
}

template<typename Word>
static std::vector<Word>
pushEnvironmentStrings(RSIM_Process *process, rose_addr_t sp, FILE *trace) {
    // Create new environment variables by stripping "X86SIM_" off the front of any environment variable and using that value
    // to override the non-X86SIM_ value, if any.  We try to make sure the variables are in the same order as if the X86SIM_
    // overrides were not present. In other words, if X86SIM_FOO and FOO are both present, then X86SIM_FOO is deleted from the
    // list and its value used for FOO; but if X86SIM_FOO is present without FOO, then we just change the name to FOO and leave
    // it at that location. We do all this so that variables are in the same order whether run natively or under the simulator.
    std::vector<size_t> env_offsets;
    std::string env_buffer;
    std::map<std::string, std::string> envvars;
    std::map<std::string, std::string>::iterator found;
    for (int i=0; environ[i]; i++) {
        char *eq = strchr(environ[i], '=');
        ROSE_ASSERT(eq!=NULL);
        std::string var(environ[i], eq-environ[i]);
        std::string val(eq+1);
        envvars.insert(std::make_pair(var, val));
    }
    for (int i=0; environ[i]; i++) {
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
        env_offsets.push_back(env_buffer.size());
        env_buffer += env + (char)0;
    }

    // Push the environment strings onto the stack
    sp -= env_buffer.size();
    rose_addr_t env_va = sp;
    process->mem_write(env_buffer.c_str(), env_va, env_buffer.size());

    // Create the envp vector.
    std::vector<Word> pointers;
    for (size_t i=0; i<env_offsets.size(); i++) {
        pointers.push_back(env_va+env_offsets[i]);
        if (trace) {
            fprintf(trace, "environ[%zu] %zu bytes at 0x%08zx = \"\%s\"\n",
                    i, strlen(&(env_buffer[env_offsets[i]]))+1, env_va+env_offsets[i], &(env_buffer[env_offsets[i]]));
        }
    }
    pointers.push_back(0); /*environment NULL terminator*/
    return pointers;
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
            fprintf(trace, "argv[%zu] %zu bytes at 0x%08"PRIx32" = \"%s\"\n", i,
                    argv[i].size()+1, pointers[i], argv[i].c_str());
        }
    }

    return pointers;
}

void
RSIM_Linux::initializeStackArch(RSIM_Thread *thread, SgAsmGenericHeader *_fhdr, int argc, char *argv[])
{
    RSIM_Process *process = thread->get_process();
    FILE *trace = (process->tracingFlags() & tracingFacilityBit(TRACE_LOADER)) ? process->tracingFile() : NULL;

    /* We only handle ELF for now */
    SgAsmElfFileHeader *fhdr = isSgAsmElfFileHeader(_fhdr);
    ASSERT_not_null(fhdr);

    /* Allocate the stack */
    static const size_t stack_size = 0x00015000;
    rose_addr_t sp = thread->operators()->readRegister(thread->dispatcher()->REG_anySP)->get_number();
    rose_addr_t stack_addr = sp - stack_size;
    process->get_memory().insert(AddressInterval::baseSize(stack_addr, stack_size),
                                 MemoryMap::Segment::anonymousInstance(stack_size, MemoryMap::READABLE|MemoryMap::WRITABLE,
                                                                       "[stack]"));

    /* Not sure what the first eight bytes are */
    static const uint8_t unknown_top[] = {0, 0, 0, 0, 0, 0, 0, 0};
    sp -= sizeof unknown_top;
    process->mem_write(unknown_top, sp, sizeof unknown_top);

    /* Copy the executable name to the top of the stack. It will be pointed to by the AT_EXECFN auxv. */
    sp -= exeArgs()[0].size() + 1;
    rose_addr_t execfn_va = sp;
    process->mem_write(exeArgs()[0].c_str(), sp, exeArgs()[0].size()+1);

    // Environment variables, argv
    if (32 == process->wordSize()) {
        typedef uint32_t Word;
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
        sp -= sizeof(argc);
        process->mem_write(&argc, sp, sizeof(argc));

    } else {
        ASSERT_require(process->wordSize() == 64);
        TODO("[Robb P. Matzke 2015-05-29]");
    }

    // Initialize the stack pointer register
    const RegisterDescriptor &REG_SP = thread->dispatcher()->stackPointerRegister();
    thread->operators()->writeRegister(REG_SP, thread->operators()->number_(REG_SP.get_nbits(), sp));
}


#endif
