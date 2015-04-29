#include "rose.h"
#include "BinaryLoaderElf.h"
#include "RSIM_Private.h"

#ifdef ROSE_ENABLE_SIMULATOR

#include "Diagnostics.h"
#include <boost/foreach.hpp>
#include <boost/regex.hpp>
#include <errno.h>
#include <sys/mman.h>
#include <sys/user.h>
#include <sys/types.h>

using namespace rose::BinaryAnalysis;
using namespace rose::Diagnostics;

RSIM_Process::~RSIM_Process() {
    delete futexes;
}

void
RSIM_Process::ctor()
{
    gettimeofday(&time_created, NULL);

    bool do_unlink;
    sem_t *sem = simulator->get_semaphore(&do_unlink);
    futexes = new RSIM_FutexTable(sem, simulator->get_semaphore_name(), do_unlink);
    assert(futexes!=NULL);

    vdso_name = "x86vdso";
    vdso_paths.push_back(".");
#ifdef X86_VDSO_PATH_1
    vdso_paths.push_back(X86_VDSO_PATH_1);
#endif
#ifdef X86_VDSO_PATH_2
    vdso_paths.push_back(X86_VDSO_PATH_2);
#endif

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

    memset(signal_action, 0, sizeof signal_action);
}

FILE *
RSIM_Process::get_tracing_file() const
{
    return tracing_file;
}

unsigned
RSIM_Process::get_tracing_flags() const
{
    return tracing_flags;
}

void
RSIM_Process::set_tracing(FILE *file, unsigned flags)
{
    tracing_file = file;
    tracing_flags = flags;
}

RSIM_Thread *
RSIM_Process::create_thread(boost::thread &hostThread)
{
    SAWYER_THREAD_TRAITS::RecursiveLockGuard lock(rwlock());
    ASSERT_require(hostThread.get_id() == boost::this_thread::get_id());

    pid_t tid = syscall(SYS_gettid);
    ROSE_ASSERT(tid>=0);
    ROSE_ASSERT(threads.find(tid)==threads.end());
    RSIM_Thread *thread = new RSIM_Thread(this, hostThread); // hostThread is moved into the RSIM_Thread
    thread->set_callbacks(callbacks);
    threads.insert(std::make_pair(tid, thread));
    return thread;
}

void
RSIM_Process::set_main_thread(RSIM_Thread *t)
{
    threads.clear();
    threads[t->get_tid()] = t;
}

RSIM_Thread *
RSIM_Process::get_main_thread() const {
    ASSERT_require(threads.size() == 1);
    return threads.begin()->second;
}

void
RSIM_Process::remove_thread(RSIM_Thread *thread)
{
    SAWYER_THREAD_TRAITS::RecursiveLockGuard lock(rwlock());
    std::map<pid_t, RSIM_Thread*>::iterator ti = threads.find(thread->get_tid());
    assert(ti!=threads.end());
    threads.erase(ti);
}

RSIM_Thread *
RSIM_Process::get_thread(pid_t tid) const
{
    SAWYER_THREAD_TRAITS::RecursiveLockGuard lock(rwlock());
    std::map<pid_t, RSIM_Thread*>::const_iterator ti=threads.find(tid);
    return ti==threads.end() ? NULL : ti->second;
}

std::vector<RSIM_Thread*>
RSIM_Process::get_all_threads() const
{
    SAWYER_THREAD_TRAITS::RecursiveLockGuard lock(rwlock());
    std::vector<RSIM_Thread*> retval;
    for (std::map<pid_t, RSIM_Thread*>::const_iterator ti=threads.begin(); ti!=threads.end(); ++ti)
        retval.push_back(ti->second);
    return retval;
}

size_t
RSIM_Process::mem_write(const void *buf, rose_addr_t va, size_t size, unsigned req_perms/*=MM_PROT_WRITE*/)
{
    size_t retval = 0;
    bool cb_status = callbacks.call_memory_callbacks(RSIM_Callbacks::BEFORE, this, MemoryMap::WRITABLE, req_perms,
                                                     va, size, (void*)buf, retval, true);
    {
        SAWYER_THREAD_TRAITS::RecursiveLockGuard lock(rwlock());
        if (cb_status)
            retval = get_memory().at(va).limit(size).require(req_perms).write((uint8_t*)buf).size();
    }
    callbacks.call_memory_callbacks(RSIM_Callbacks::AFTER, this, MemoryMap::WRITABLE, req_perms,
                                    va, size, (void*)buf, retval, cb_status);
    return retval;
}

size_t
RSIM_Process::mem_read(void *buf, rose_addr_t va, size_t size, unsigned req_perms/*=MM_PROT_READ*/)
{
    size_t retval = 0;
    bool cb_status = callbacks.call_memory_callbacks(RSIM_Callbacks::BEFORE, this, MemoryMap::READABLE, req_perms,
                                                     va, size, buf, retval, true);
    {
        SAWYER_THREAD_TRAITS::RecursiveLockGuard lock(rwlock());
        if (cb_status)
            retval = get_memory().at(va).limit(size).require(req_perms).read((uint8_t*)buf).size();
    }
    callbacks.call_memory_callbacks(RSIM_Callbacks::AFTER, this, MemoryMap::READABLE, req_perms, va, size,
                                    buf, retval, cb_status);
    return retval;
}

bool
RSIM_Process::mem_is_mapped(rose_addr_t va) const
{
    SAWYER_THREAD_TRAITS::RecursiveLockGuard lock(rwlock());
    return get_memory().at(va).exists();
}

size_t
RSIM_Process::get_ninsns() const
{
    SAWYER_THREAD_TRAITS::RecursiveLockGuard lock(rwlock());
    size_t retval = 0;
    for (std::map<pid_t, RSIM_Thread*>::const_iterator ti=threads.begin(); ti!=threads.end(); ++ti)
        retval += ti->second->get_ninsns();
    return retval;
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

SgAsmGenericHeader*
RSIM_Process::load(const char *name)
{
    FILE *trace = (tracing_flags & tracingFacilityBit(TRACE_LOADER)) ? tracing_file : NULL;

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
        assert(getenv("PATH")!=NULL);
        std::string path_env = getenv("PATH");
        size_t len;
        for (size_t pos=0; pos!=std::string::npos && pos<path_env.size(); pos+=len+1) {
            size_t colon = path_env.find_first_of(":;", pos);
            len = colon==std::string::npos ? path_env.size()-pos : colon-pos;
            std::string path = path_env.substr(pos, len);
            std::string fullname = path + "/" + name;
            if (access(fullname.c_str(), R_OK)>=0) {
                exename = name;
                exeargs.push_back(fullname);
                break;
            }
        }
    }
    if (exeargs.empty()) {
        fprintf(stderr, "%s: not found\n", name);
        exit(1);
    }

    /* Link the main binary into the AST without further linking, mapping, or relocating. */
    if (trace)
        fprintf(trace, "loading %s...\n", exeargs[0].c_str());
    char *frontend_args[4];
    frontend_args[0] = strdup("-");
    frontend_args[1] = strdup("-rose:read_executable_file_format_only"); /*delay disassembly until later*/
    frontend_args[2] = strdup(exeargs[0].c_str());
    frontend_args[3] = NULL;
    project = frontend(3, frontend_args);

    /* Find the best interpretation and file header.  Windows PE programs have two where the first is DOS and the second is PE
     * (we'll use the PE interpretation). */
    interpretation = SageInterface::querySubTree<SgAsmInterpretation>(project, V_SgAsmInterpretation).back();
    SgAsmGenericHeader *fhdr = interpretation->get_headers()->get_headers().front();
    headers.push_back(fhdr);
    ep_orig_va = ep_start_va = fhdr->get_entry_rva() + fhdr->get_base_va();

    /* Link the interpreter into the AST */
    SimLoader *loader = new SimLoader(interpretation, interpname);

    /* If we found an interpreter then use its entry address as the start of simulation.  When running the specimen directly
     * in Linux with "setarch i386 -LRB3", the ld-linux.so.2 gets mapped to 0x40000000 if it has no preferred address.  We can
     * accomplish the same thing simply by rebasing the library. */
    if (loader->interpreter) {
        headers.push_back(loader->interpreter);
        SgAsmGenericSection *load0 = loader->interpreter->get_section_by_name("LOAD#0");
        if (load0 && load0->is_mapped() && load0->get_mapped_preferred_rva()==0 && load0->get_mapped_size()>0)
            loader->interpreter->set_base_va(ld_linux_base_va);
        ep_start_va = loader->interpreter->get_entry_rva() + loader->interpreter->get_base_va();
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
    assert(map_stack.empty());
    mem_transaction_start("specimen main memory");
    get_memory() = *interpretation->get_map();          // shallow copy, new segments point to same old data

    /* Load and map the virtual dynamic shared library. */
    bool vdso_loaded = false;
    for (size_t i=0; i<vdso_paths.size() && !vdso_loaded; i++) {
        for (int j=0; j<2 && !vdso_loaded; j++) {
            std::string vdso_name = vdso_paths[i] + (j ? "" : "/" + this->vdso_name);
            if (trace)
                fprintf(trace, "looking for vdso: %s\n", vdso_name.c_str());
            if ((vdso_loaded = loader->map_vdso(vdso_name, interpretation, &get_memory()))) {
                vdso_mapped_va = loader->vdso_mapped_va;
                vdso_entry_va = loader->vdso_entry_va;
                headers.push_back(loader->vdso);
                if (trace) {
                    fprintf(trace, "mapped %s at 0x%08"PRIx64" with entry va 0x%08"PRIx64"\n",
                            vdso_name.c_str(), vdso_mapped_va, vdso_entry_va);
                }
            }
        }
    }
    if (!vdso_loaded && trace && !vdso_paths.empty())
        fprintf(trace, "warning: cannot find a virtual dynamic shared object\n");

    /* Find a disassembler. */
    if (!disassembler) {
        disassembler = Disassembler::lookup(interpretation)->clone();
        disassembler->set_progress_reporting(-1); /* turn off progress reporting */
    }

#if 0
    /* Initialize the brk value to be the lowest page-aligned address that is above the end of the highest mapped address but
     * below 0x40000000 (the stack, and where ld-linux.so.2 might be loaded when loaded high). */
    rose_addr_t free_area = std::max(map->find_last_free(std::max(ld_linux_base_va, (rose_addr_t)0x40000000)),
                                     (rose_addr_t)brk_base);
    brk_va = alignUp(free_area, PAGE_SIZE);
#else
    struct FindInitialBrk: public SgSimpleProcessing {
        FindInitialBrk(): max_mapped_va(0) {}
        rose_addr_t max_mapped_va;
        void visit(SgNode *node) {
            SgAsmGenericSection *section = isSgAsmGenericSection(node);
            if (section && section->is_mapped())
                max_mapped_va = std::max(section->get_mapped_actual_va() + section->get_mapped_size(), max_mapped_va);
        }
    } t1;
    t1.traverse(fhdr, preorder);
    AddressInterval restriction = AddressInterval::hull(t1.max_mapped_va, AddressInterval::whole().greatest());
    brk_va = get_memory().findFreeSpace(PAGE_SIZE, PAGE_SIZE, restriction).orElse(0);
#endif

    delete loader;

    // Create the main thread, but don't allow it to start running yet.  Once a process is up and running there's nothing
    // special about the main thread other than its ID is the thread group for the process.
    pt_regs_32 initialRegisters;
    memset(&initialRegisters, 0, sizeof initialRegisters);
    initialRegisters.sp = 0xc0000000ul;                 // high end of stack, exclusive
    initialRegisters.flags = 2;                         // flag bit 1 is set, although this is a reserved bit
    initialRegisters.cs = 0x23;
    initialRegisters.ds = 0x2b;
    initialRegisters.es = 0x2b;
    initialRegisters.ss = 0x2b;
    initialRegisters.ip = ep_start_va;
    pid_t mainTid = clone_thread(0, 0, 0, initialRegisters, false/*don't start*/);
    RSIM_Thread *thread = get_thread(mainTid);

    mfprintf(thread->tracing(TRACE_THREAD))("new thread with tid %d", thread->get_tid());

    return fhdr;
}

void
RSIM_Process::dump_core(int signo, std::string base_name)
{
    if (!get_callbacks().call_process_callbacks(RSIM_Callbacks::BEFORE, this, RSIM_Callbacks::ProcessCallback::COREDUMP, true))
        return;

    if (base_name.empty())
        base_name = core_base_name;

#if 0 /* FIXME: we need to make core dumping thread-aware. [RPM 2011-02-03] */
    if (core_styles & CORE_ROSE)
        map->dump(base_name);
    if (0==(core_styles & CORE_ELF))
        return;

    /* Get current instruction pointer. We subtract the size of the current instruction if we're in the middle of processing
     * an instruction because it would have already been incremented by the semantics. */ 
    uint32_t eip = readIP().known_value();
    if (get_insn())
        eip -= get_insn()->get_size();

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
            set_offset(alignUp(get_offset(), 4096));
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
                size_t nread = process->get_memory()->read(buf, cur_va, to_write);
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
#endif
    
    get_callbacks().call_process_callbacks(RSIM_Callbacks::AFTER, this,
                                           RSIM_Callbacks::ProcessCallback::COREDUMP, true);
}

void
RSIM_Process::open_tracing_file()
{
    char name[4096];

    if (!tracing_file_name.empty()) {
        size_t nprinted = snprintf(name, sizeof name, tracing_file_name.c_str(), getpid());
        if (nprinted > sizeof name) {
            fprintf(stderr, "name pattern overflow: %s\n", tracing_file_name.c_str());
            tracing_file = stderr;
            return;
        }
    } else {
        name[0] = '\0';
    }

    if (tracing_file && tracing_file!=stderr && tracing_file!=stdout) {
        fclose(tracing_file);
        tracing_file = NULL;
    }

    if (name[0]) {
        if (NULL==(tracing_file = fopen(name, "w"))) {
            fprintf(stderr, "%s: %s\n", strerror(errno), name);
            return;
        }
#ifdef X86SIM_LOG_UNBUFFERED
        setbuf(tracing_file, NULL);
#endif
    }
}

void
RSIM_Process::binary_trace_start()
{
    if (!btrace_file)
        return;
    
    static const uint16_t magic = 0x5445;
    size_t n = fwrite(&magic, 2, 1, btrace_file);
    assert(1==n);

    static const uint16_t version = 0x0033;
    n = fwrite(&version, 2, 1, btrace_file);
    assert(1==n);

    static const uint32_t nprocs = 1;
    n = fwrite(&nprocs, 4, 1, btrace_file);
    assert(1==n);

    char exename_buf[32];
    strncpy(exename_buf, exename.c_str(), 32);
    exename_buf[31] = '\0';
    n = fwrite(exename_buf, 32, 1, btrace_file);
    assert(1==n);

    uint32_t pid = getpid();
    n = fwrite(&pid, 4, 1, btrace_file);
    assert(1==n);

    static const uint32_t nmodules = 0;
    n = fwrite(&nmodules, 4, 1, btrace_file);
    assert(1==n);
}

void
RSIM_Process::binary_trace_add(RSIM_Thread *thread, const SgAsmInstruction *insn)
{
    if (!btrace_file)
        return;

    uint32_t addr = insn->get_address();
    size_t n = fwrite(&addr, 4, 1, btrace_file);
    assert(1==n);

    static const uint32_t tid = thread->get_tid();
    n = fwrite(&tid, 4, 1, btrace_file);
    assert(1==n);

    size_t insn_size = insn->get_size();
    assert(insn_size<=255);
    uint8_t insn_size_byte = insn_size;
    n = fwrite(&insn_size_byte, 1, 1, btrace_file);
    assert(1==n);

    n = fwrite(&insn->get_raw_bytes()[0], insn_size, 1, btrace_file);
    assert(1==n);

    pt_regs_32 regs = thread->get_regs();

    n = fwrite(&regs.flags, 4, 1, btrace_file);
    assert(1==n);

    n = fwrite(&regs.ax, 4, 1, btrace_file);
    assert(1==n);
    
    n = fwrite(&regs.bx, 4, 1, btrace_file);
    assert(1==n);
    
    n = fwrite(&regs.cx, 4, 1, btrace_file);
    assert(1==n);
    
    n = fwrite(&regs.dx, 4, 1, btrace_file);
    assert(1==n);
    
    n = fwrite(&regs.si, 4, 1, btrace_file);
    assert(1==n);
    
    n = fwrite(&regs.di, 4, 1, btrace_file);
    assert(1==n);
    
    n = fwrite(&regs.bp, 4, 1, btrace_file);
    assert(1==n);
    
    n = fwrite(&regs.sp, 4, 1, btrace_file);
    assert(1==n);

    n = fwrite(&regs.cs, 2, 1, btrace_file);
    assert(1==n);

    n = fwrite(&regs.ss, 2, 1, btrace_file);
    assert(1==n);

    n = fwrite(&regs.es, 2, 1, btrace_file);
    assert(1==n);

    n = fwrite(&regs.ds, 2, 1, btrace_file);
    assert(1==n);

    n = fwrite(&regs.fs, 2, 1, btrace_file);
    assert(1==n);

    n = fwrite(&regs.gs, 2, 1, btrace_file);
    assert(1==n);
}

SgAsmInstruction *
RSIM_Process::get_instruction(rose_addr_t va)
{
    SgAsmInstruction *insn = NULL;

    /* Use a cached instruction if possible. */
    {
        SAWYER_THREAD_TRAITS::RecursiveLockGuard lock(rwlock());
        Disassembler::InstructionMap::iterator found = icache.find(va);
        insn = found!=icache.end() ? found->second : NULL;
    }

    /* If we found a cached instruction, make sure memory still contains that value. If we didn't find an instruction, read one
     * word from the address anyway (and discard it) so that memory access callbacks will see the memory access.  We'll read
     * the rest of the instruction words after we know the instruction size.   Note that since we discard the memory that was
     * read, the callbacks will not have an opportunity to change the instruction that's fetched.  If you need to do that, use
     * an instruction callback instead. */
    if (insn) {
        size_t insn_sz = insn->get_size();
        SgUnsignedCharList curmem(insn_sz);
        size_t nread = mem_read(&curmem[0], va, insn_sz, MemoryMap::EXECUTABLE);
        if (nread==insn_sz && curmem==insn->get_raw_bytes())
            return insn;
    } else {
        uint32_t word;
        (void)mem_read(&word, va, 4, MemoryMap::EXECUTABLE);
    }

    /* Disassemble (and cache) a new instruction. At this time it is not safe to be multi-threaded inside a single Disassemble
     * object, so we'll protect the whole call with a write lock.  We need one anyway in order to update the icache.
     * [RPM 2011-02-09] */
    {
        SAWYER_THREAD_TRAITS::RecursiveLockGuard lock(rwlock());
        insn = isSgAsmX86Instruction(disassembler->disassembleOne(&get_memory(), va)); /* might throw Disassembler::Exception */
        ROSE_ASSERT(insn!=NULL); /*only happens if our disassembler is not an x86 disassembler!*/
        icache[va] = insn;
    }

    /* Read the rest of the instruction if necessary so that memory access callbacks have a chance to see the access. */
    for (uint32_t i=4; i<insn->get_size(); i+=4) {
        uint32_t word;
        (void)mem_read(&word, va+i, 4, MemoryMap::EXECUTABLE);
    }

    return insn;
}
        
void *
RSIM_Process::my_addr(uint32_t va, size_t nbytes)
{
    SAWYER_THREAD_TRAITS::RecursiveLockGuard lock(rwlock());

    /* Obtain mapping information and check that the specified number of bytes are mapped. */
    if (!get_memory().at(va).exists())
        return NULL;
    const MemoryMap::Node &me = *get_memory().find(va);
    size_t offset = me.value().offset() + va - me.key().least();
    uint8_t *base = const_cast<uint8_t*>(me.value().buffer()->data());
    if (!base)
        return NULL;
    return base + offset;
}

uint32_t
RSIM_Process::guest_va(void *addr, size_t nbytes)
{
    SAWYER_THREAD_TRAITS::RecursiveLockGuard lock(rwlock());
    uint32_t retval = 0;
    BOOST_FOREACH (const MemoryMap::Node &node, get_memory().nodes()) {
        const AddressInterval &range = node.key();
        const MemoryMap::Segment &segment = node.value();
        const uint8_t *base = segment.buffer()->data();
        rose_addr_t offset = segment.offset();
        size_t size = range.size();
        if (base && addr>=base+offset && (uint8_t*)addr+nbytes<=base+offset+size) {
            retval = range.least() + ((uint8_t*)addr - (base+offset));
            break;
        }
    }
    return retval;
}

std::string
RSIM_Process::read_string(uint32_t va, size_t limit/*=0*/, bool *error/*=NULL*/)
{
    std::string retval;
    if (error)
        *error = false;

    do {
        SAWYER_THREAD_TRAITS::RecursiveLockGuard lock(rwlock());
        while (1) {
            uint8_t byte;
            size_t nread = get_memory().at(va++).limit(1).read(&byte).size();
            if (1!=nread) {
                if (error)
                    *error = true;
                break;
            }
            if (!byte)
                break;
            retval += byte;

            if (limit>0 && retval.size()>=limit)
                break;
        }
    } while (0);
    return retval;
}

std::vector<std::string>
RSIM_Process::read_string_vector(uint32_t va, bool *_error/*=NULL*/)
{
    bool had_error;
    bool *error = _error ? _error : &had_error;
    *error = false;

    std::vector<std::string> retval;
    for (/*void*/; 1; va+=4) {
        /* Read the pointer to the string */
        uint32_t ptr;
        if (sizeof(ptr) != mem_read(&ptr, va, sizeof ptr)) {
            *error = true;
            return retval;
        }

        /* Pointer list is null-terminated */
        if (!ptr)
            break;

        /* Read the NUL-terminated string */
        std::string str = read_string(ptr, 0, error);
        if (*error)
            return retval;

        retval.push_back(str);
    }
    return retval;
}

size_t
RSIM_Process::mem_transaction_start(const std::string &name)
{
    MemoryMap new_map;
    if (!map_stack.empty()) {
        new_map = map_stack.back().first;
        BOOST_FOREACH (MemoryMap::Segment &segment, new_map.segments())
            segment.buffer()->copyOnWrite(true);
    }
    map_stack.push_back(std::make_pair(new_map, name));
    return map_stack.size();
}

std::string
RSIM_Process::mem_transaction_name() const
{
    assert(!map_stack.empty());
    return map_stack.back().second;
}

size_t
RSIM_Process::mem_ntransactions() const
{
    return map_stack.size();
}

size_t
RSIM_Process::mem_transaction_rollback(const std::string &name)
{
    for (size_t i=map_stack.size(); i>0; --i) {
        if (0==map_stack[i-1].second.compare(name)) {
            size_t lo = i-1; // index of oldest item to remove
            std::string lo_name = map_stack[lo].second;
            size_t nremoved = map_stack.size() - lo;
            map_stack.erase(map_stack.begin()+lo, map_stack.end());
            if (map_stack.empty())
                mem_transaction_start(lo_name);
            return nremoved;
        }
    }
    return 0;
}

void
RSIM_Process::mem_transaction_commit(const std::string &name)
{
    assert(!"not implemented yet");
    abort();
}

int
RSIM_Process::mem_setbrk(rose_addr_t newbrk, Sawyer::Message::Stream &mesg)
{
    if (newbrk >= 0xb0000000ul)
        return -ENOMEM;

    SAWYER_THREAD_TRAITS::RecursiveLockGuard lock(rwlock());
    if (newbrk > brk_va) {
        size_t size = newbrk - brk_va;
        get_memory().insert(AddressInterval::baseSize(brk_va, size),
                            MemoryMap::Segment::anonymousInstance(size, MemoryMap::READABLE|MemoryMap::WRITABLE, "[heap]"));
        brk_va = newbrk;
    } else if (newbrk>0 && newbrk<brk_va) {
        get_memory().erase(AddressInterval::baseSize(newbrk, brk_va-newbrk));
        brk_va = newbrk;
    }
    int retval= brk_va;

    if (mesg)
        mem_showmap(mesg, "memory map after brk syscall:\n");

    return retval;
}

int
RSIM_Process::mem_unmap(rose_addr_t va, size_t sz, Sawyer::Message::Stream &mesg)
{
    SAWYER_THREAD_TRAITS::RecursiveLockGuard lock(rwlock());

    /* Make sure that the specified memory range is actually mapped, or return -ENOMEM. */
    if (!get_memory().contains(AddressInterval::baseSize(va, sz)))
        return -ENOMEM;

    /* Unmap for real, because if we don't, and the mapping was not anonymous, and the file that was mapped is
     * unlinked, and we're on NFS, an NFS temp file is created in place of the unlinked file. */
    const uint8_t *ptr = NULL;
    try {
        const MemoryMap::Node &me = *get_memory().find(va);// existence checked above
        size_t offset = me.value().offset() + va - me.key().least();
        ptr = me.value().buffer()->data() + offset;
        if (0==(uint64_t)ptr % (uint64_t)PAGE_SIZE && 0==(uint64_t)sz % (uint64_t)PAGE_SIZE)
            (void)munmap((void*)ptr, sz);
    } catch (const MemoryMap::NotMapped) {
    }

    /* Erase the mapping from the simulation */
    get_memory().erase(AddressInterval::baseSize(va, sz));

    /* Tracing */
    if (mesg)
        mem_showmap(mesg, "memory map after munmap syscall:\n");

    return 0;
}

void
RSIM_Process::mem_showmap(Sawyer::Message::Stream &mesg, const char *intro, const char *prefix)
{
    if (!intro || !*intro) intro = "memory map\n";
    if (!prefix) prefix = "    ";

    if (mesg) {
        SAWYER_THREAD_TRAITS::RecursiveLockGuard lock(rwlock());
        std::ostringstream ss;
        get_memory().dump(ss, prefix);
        mfprintf(mesg)("%s%susing memory transaction %zu \"%s\"\n%s\n",
                       intro, prefix, mem_ntransactions(), mem_transaction_name().c_str(), ss.str().c_str());
    }
}

void
RSIM_Process::btrace_close()
{
    SAWYER_THREAD_TRAITS::RecursiveLockGuard lock(rwlock());
    if (btrace_file) {
        fclose(btrace_file);
        btrace_file = NULL;
    }
}

int
RSIM_Process::mem_protect(rose_addr_t va, size_t sz, unsigned rose_perms, unsigned real_perms)
{
    SAWYER_THREAD_TRAITS::RecursiveLockGuard lock(rwlock());
    size_t aligned_sz = alignUp(sz, (size_t)PAGE_SIZE);

    /* Set protection in the underlying real memory (to catch things like trying to add write permission to memory that's
     * mapped from a read-only file), then also set the protection in the simulated memory map so the simulator can make
     * queries about memory access.  Some of the underlying memory points to parts of an ELF file that was read into ROSE's
     * memory in such a way that segments are not aligned on page boundaries. We cannot change protections on these
     * non-aligned sections. */
    if (-1==mprotect(my_addr(va, sz), sz, real_perms) && EINVAL!=errno)
        return -errno;

    try {
        get_memory().at(va).limit(aligned_sz).changeAccess(rose_perms, ~rose_perms);
        return 0;
    } catch (const MemoryMap::NotMapped &e) {
        return -ENOMEM;
    }
}

rose_addr_t
RSIM_Process::mem_map(rose_addr_t start, size_t size, unsigned rose_perms, unsigned flags, size_t offset, int fd)
{
    size_t aligned_size = alignUp(size, (size_t)PAGE_SIZE);
    uint8_t *buf = NULL;
    unsigned prot = ((rose_perms & MemoryMap::READABLE  ? PROT_READ  : 0) |
                     (rose_perms & MemoryMap::WRITABLE ? PROT_WRITE : 0) |
                     (rose_perms & MemoryMap::EXECUTABLE  ? PROT_EXEC  : 0));

    do {
        SAWYER_THREAD_TRAITS::RecursiveLockGuard lock(rwlock());
        if (0==start) {
            if (0!=(flags & MAP_FIXED)) {
                start = (rose_addr_t)(int64_t)-EPERM; /* Linux does not allow addr 0 to be mapped */
                break;
            } else {
                AddressInterval restriction = AddressInterval::hull(mmap_start, AddressInterval::whole().greatest());
                if (!get_memory().findFreeSpace(aligned_size, PAGE_SIZE, restriction).assignTo(start)) {
                    start = (rose_addr_t)(int64_t)-ENOMEM;
                    break;
                }
                if (!mmap_recycle)
                    mmap_start = std::max(mmap_start, start);
            }
        }

        if (flags & MAP_ANONYMOUS) {
            buf = (uint8_t*)mmap(NULL, size, prot, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
        } else {
            buf = (uint8_t*)mmap(NULL, size, prot, flags & ~MAP_FIXED, fd, offset);
        }
        if (MAP_FAILED==buf) {
            start = (rose_addr_t)(int64_t)-errno;
        } else {
            /* Try to figure out a reasonable name for the map element. If we're mapping a file, we can get the file name
             * from the proc filesystem. The name is only used to aid debugging. */
            std::string melmt_name = "anonymous";
            if (fd>=0 && 0==(flags & MAP_ANONYMOUS)) {
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
            
            get_memory().insert(AddressInterval::baseSize(start, aligned_size),
                                MemoryMap::Segment::staticInstance(buf, aligned_size, rose_perms, "mmap("+melmt_name+")"));
        }
    } while (0);
    return start;
}

void
RSIM_Process::set_gdt(const user_desc_32 *ud)
{
    SAWYER_THREAD_TRAITS::RecursiveLockGuard lock(rwlock());
    *(gdt_entry(ud->entry_number)) = *ud;
}

user_desc_32 *
RSIM_Process::gdt_entry(int idx)
{
    SAWYER_THREAD_TRAITS::RecursiveLockGuard lock(rwlock());
    ROSE_ASSERT(idx>=0 && idx<GDT_ENTRIES);
    ROSE_ASSERT(idx<GDT_ENTRY_TLS_MIN || idx>GDT_ENTRY_TLS_MAX); /* call only from RSIM_Thread::set_gdt */
    return gdt + idx;
}



/* Initialize the stack of the main thread. */
void
RSIM_Process::initialize_stack(SgAsmGenericHeader *_fhdr, int argc, char *argv[])
{
    FILE *trace = (tracing_flags & tracingFacilityBit(TRACE_LOADER)) ? tracing_file : NULL;
    SAWYER_THREAD_TRAITS::RecursiveLockGuard lock(rwlock());

    RSIM_Thread *main_thread = get_main_thread();

    /* We only handle ELF for now */
    SgAsmElfFileHeader *fhdr = isSgAsmElfFileHeader(_fhdr);
    ROSE_ASSERT(fhdr!=NULL);

    /* Allocate the stack */
    static const size_t stack_size = 0x00015000;
    rose_addr_t sp = main_thread->operators()->readRegister(main_thread->dispatcher()->REG_anySP)->get_number();
    rose_addr_t stack_addr = sp - stack_size;
    get_memory().insert(AddressInterval::baseSize(stack_addr, stack_size),
                        MemoryMap::Segment::anonymousInstance(stack_size, MemoryMap::READABLE|MemoryMap::WRITABLE,
                                                              "[stack]"));

    /* Save specimen arguments in RSIM_Process object. The executable name is already there. */
    assert(exeargs.size()==1);
    for (int i=1; i<argc; i++)
        exeargs.push_back(std::string(argv[i]));

    /* Not sure what the first eight bytes are */
    static const uint8_t unknown_top[] = {0, 0, 0, 0, 0, 0, 0, 0};
    sp -= sizeof unknown_top;
    mem_write(unknown_top, sp, sizeof unknown_top);

    /* Copy the executable name to the top of the stack. It will be pointed to by the AT_EXECFN auxv. */
    sp -= exeargs[0].size() + 1;
    uint32_t execfn_va = sp;
    mem_write(exeargs[0].c_str(), sp, exeargs[0].size()+1);

    /* Create new environment variables by stripping "X86SIM_" off the front of any environment variable and using that
     * value to override the non-X86SIM_ value, if any.  We try to make sure the variables are in the same order as if the
     * X86SIM_ overrides were not present. In other words, if X86SIM_FOO and FOO are both present, then X86SIM_FOO is
     * deleted from the list and its value used for FOO; but if X86SIM_FOO is present without FOO, then we just change the
     * name to FOO and leave it at that location. We do all this so that variables are in the same order whether run
     * natively or under the simulator. */
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
    sp -= env_buffer.size();
    uint32_t env_va = sp;
    mem_write(env_buffer.c_str(), env_va, env_buffer.size());

    /* Initialize the stack with the specimen's argc and argv.  The argv data must be stored contiguously with arg[0] at
     * the low address and arg[argc-1] at the high address. (WINE's preloader.c set_process_name() depends on this). */
    std::vector<uint32_t> pointers(argc+1, 0);              /* pointers pushed onto stack at the end of initialization */
    pointers[0] = exeargs.size();
    for (size_t i=exeargs.size(); i>0; --i) {
        size_t len = exeargs[i-1].size() + 1;               /* inc. NUL terminator */
        sp -= len;
        mem_write(exeargs[i-1].c_str(), sp, len);
        pointers[i] = sp;
    }
    if (trace) {
        for (size_t i=0; i<exeargs.size(); i++) {
            fprintf(trace, "argv[%zu] %zu bytes at 0x%08"PRIx32" = \"%s\"\n", i,
                    exeargs[i].size()+1, pointers[i+1], exeargs[i].c_str());
        }
    }
    pointers.push_back(0); /*the argv NULL terminator*/

    /* Add envp pointers to the stack */
    for (size_t i=0; i<env_offsets.size(); i++) {
        pointers.push_back(env_va+env_offsets[i]);
        if (trace) {
            fprintf(trace, "environ[%zu] %zu bytes at 0x%08zx = \"\%s\"\n",
                    i, strlen(&(env_buffer[env_offsets[i]]))+1, env_va+env_offsets[i], &(env_buffer[env_offsets[i]]));
        }
    }
    pointers.push_back(0); /*environment NULL terminator*/

    /* Push certain auxv data items onto the stack. */
    sp &= ~0xf;

    static const char *platform = "i686";
    sp -= strlen(platform)+1;
    uint32_t platform_va = sp;
    mem_write(platform, platform_va, strlen(platform)+1);

    static const uint8_t random_data[] = {                  /* use hard-coded values for reproducibility */
        0x00, 0x11, 0x22, 0x33,
        0xff, 0xee, 0xdd, 0xcc,
        0x88, 0x99, 0xaa, 0xbb,
        0x77, 0x66, 0x55, 0x44
    };
    sp -= sizeof random_data;
    uint32_t random_data_va = sp;
    mem_write(random_data, random_data_va, sizeof random_data);


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

    /* Initialize stack with auxv, where each entry is two words in the pointers vector.  The order and values were
     * determined by running the simulator with the "--showauxv" switch on hudson-rose-07. */
    auxv.clear();

    if (vdso_mapped_va!=0) {
        /* AT_SYSINFO */
        auxv.push_back(32);
        auxv.push_back(vdso_entry_va);
        if (trace)
            fprintf(trace, "AT_SYSINFO:       0x%08"PRIx32"\n", auxv.back());

        /* AT_SYSINFO_PHDR */
        auxv.push_back(33);
        auxv.push_back(vdso_mapped_va);
        if (trace)
            fprintf(trace, "AT_SYSINFO_EHDR:  0x%08"PRIx32"\n", auxv.back());
    }

    /* AT_HWCAP (see linux <include/asm/cpufeature.h>). */
    auxv.push_back(16);
    uint32_t hwcap = 0xbfebfbfful; /* value used by hudson-rose-07, and wortheni(Xeon X5680) */
    auxv.push_back(hwcap);
    if (trace)
        fprintf(trace, "AT_HWCAP:         0x%08"PRIx32"\n", auxv.back());

    /* AT_PAGESZ */
    auxv.push_back(6);
    auxv.push_back(PAGE_SIZE);
    if (trace)
        fprintf(trace, "AT_PAGESZ:        %"PRId32"\n", auxv.back());

    /* AT_CLKTCK */
    auxv.push_back(17);
    auxv.push_back(100);
    if (trace)
        fprintf(trace, "AT_CLKTCK:        %"PRId32"\n", auxv.back());

    /* AT_PHDR */
    auxv.push_back(3); /*AT_PHDR*/
    auxv.push_back(t2.segtab_va);
    if (trace)
        fprintf(trace, "AT_PHDR:          0x%08"PRIx32"\n", auxv.back());

    /*AT_PHENT*/
    auxv.push_back(4);
    auxv.push_back(fhdr->get_phextrasz() + sizeof(SgAsmElfSegmentTableEntry::Elf32SegmentTableEntry_disk));
    if (trace)
        fprintf(trace, "AT_PHENT:         %"PRId32"\n", auxv.back());

    /* AT_PHNUM */
    auxv.push_back(5);
    auxv.push_back(fhdr->get_e_phnum());
    if (trace)
        fprintf(trace, "AT_PHNUM:         %"PRId32"\n", auxv.back());

    /* AT_BASE */
    auxv.push_back(7);
    auxv.push_back(fhdr->get_section_by_name(".interp") ? ld_linux_base_va : 0);
    if (trace)
        fprintf(trace, "AT_BASE:          0x%08"PRIx32"\n", auxv.back());

    /* AT_FLAGS */
    auxv.push_back(8);
    auxv.push_back(0);
    if (trace)
        fprintf(trace, "AT_FLAGS:         0x%08"PRIx32"\n", auxv.back());

    /* AT_ENTRY */
    auxv.push_back(9);
    auxv.push_back(fhdr->get_entry_rva() + fhdr->get_base_va());
    if (trace)
        fprintf(trace, "AT_ENTRY:         0x%08"PRIx32"\n", auxv.back());

    /* AT_UID */
    auxv.push_back(11);
    auxv.push_back(getuid());
    if (trace)
        fprintf(trace, "AT_UID:           %"PRId32"\n", auxv.back());

    /* AT_EUID */
    auxv.push_back(12);
    auxv.push_back(geteuid());
    if (trace)
        fprintf(trace, "AT_EUID:          %"PRId32"\n", auxv.back());

    /* AT_GID */
    auxv.push_back(13);
    auxv.push_back(getgid());
    if (trace)
        fprintf(trace, "AT_GID:           %"PRId32"\n", auxv.back());

    /* AT_EGID */
    auxv.push_back(14);
    auxv.push_back(getegid());
    if (trace)
        fprintf(trace, "AT_EGID:          %"PRId32"\n", auxv.back());

    /* AT_SECURE */
    auxv.push_back(23); /* 0x17 */
    auxv.push_back(false);
    if (trace)
        fprintf(trace, "AT_SECURE:        %"PRId32"\n", auxv.back());

    /* AT_RANDOM */
    auxv.push_back(25);/* 0x19 */
    auxv.push_back(random_data_va);
    if (trace)
        fprintf(trace, "AT_RANDOM:       0x%08"PRIx32"\n", auxv.back());

    /* AT_EXECFN */
    auxv.push_back(31); /* 0x1f */
    auxv.push_back(execfn_va);
    if (trace)
        fprintf(trace, "AT_EXECFN:       0x%08"PRIx32" (%s)\n", auxv.back(), exeargs[0].c_str());

    /* AT_PLATFORM */
    auxv.push_back(15);
    auxv.push_back(platform_va);
    if (trace)
        fprintf(trace, "AT_PLATFORM:      0x%08"PRIx32" (%s)\n", auxv.back(), platform);

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
    sp -= 4 * pointers.size();
    sp &= ~0xf; /*align to 16 bytes*/
    mem_write(&(pointers[0]), sp, 4*pointers.size());

    const RegisterDescriptor &REG_SP = main_thread->dispatcher()->stackPointerRegister();
    main_thread->operators()->writeRegister(REG_SP, main_thread->operators()->number_(REG_SP.get_nbits(), sp));
}

/* The "thread" arg must be the calling thread */
pid_t
RSIM_Process::clone_thread(unsigned flags, uint32_t parent_tid_va, uint32_t child_tls_va, const pt_regs_32 &regs,
                           bool startRunning)
{
#ifndef ROSE_THREADS_ENABLED
    fprintf(stderr, "ROSE library is not thread safe; multiple threads cannot be simulated.\n");
    abort();
#endif
    Clone clone_info(this, flags, parent_tid_va, child_tls_va, regs);
    SAWYER_THREAD_TRAITS::RecursiveLockGuard lock(clone_info.mutex);
    try {
        clone_info.hostThread = boost::thread(RSIM_Process::clone_thread_helper, &clone_info);
    } catch (const boost::thread_resource_error&) {
        return -1;
    }

    // wait for child thread to finish initializing and write its thread ID into the clone_info and create an RSIM_Thread and
    // register it with the process.
    clone_info.cond.wait(clone_info.mutex);

    RSIM_Thread *child = get_thread(clone_info.newtid);
    if (startRunning)
        child->start();

    return clone_info.newtid;
}

void
RSIM_Process::clone_thread_helper(void *_clone_info)
{
    /* clone_info points to the creating thread's stack (inside clone_thread). Since the creator's clone_thread doesn't return
     * until after we've signaled clone_info.cond and released clone_info.mutex, its safe to access it here in this thread. */
    Clone *clone_info = (Clone*)_clone_info;
    ROSE_ASSERT(clone_info!=NULL);
    RSIM_Process *process = clone_info->process;
    ROSE_ASSERT(process!=NULL);

    // Create the RSIM_Thread abstraction and move the hostThread information into it.
    RSIM_Thread *thread = process->create_thread(clone_info->hostThread);
    pid_t tid = thread->get_tid();
    mfprintf(thread->tracing(TRACE_THREAD))("new thread with tid %d", tid);

    do {
        // Blocks until clone_thread enters the clone_info.cond.wait call.
        SAWYER_THREAD_TRAITS::RecursiveLockGuard lock1(clone_info->mutex);

        /* Make some info available to our parent. */
        clone_info->newtid = thread->get_tid();
        clone_info->seq = thread->get_seq();

        /* Set up thread local storage */
        if (clone_info->flags & CLONE_SETTLS) {
            user_desc_32 ud;
            if (sizeof(ud)!=process->mem_read(&ud, clone_info->child_tls_va, sizeof ud)) {
                tid = -EFAULT;
                break;
            }
            int status = thread->set_thread_area(&ud, false);
            if (status<0) {
                tid = status;
                break;
            }
        }
        
        /* Initialize our registers.  Has to be after we initialize TLS segments in the GDT */
        thread->init_regs(clone_info->regs);

        /* Write child TID into process memory if requested */
        if ((clone_info->flags & CLONE_PARENT_SETTID) &&
            4!=process->mem_write(&tid, clone_info->parent_tid_va, 4)) {
            tid = -EFAULT;
            break;
        }
        if ((clone_info->flags & CLONE_CHILD_SETTID) &&
            4!=process->mem_write(&tid, clone_info->child_tls_va, 4)) {
            tid = -EFAULT;
            break;
        }

        /* Should a memory location be cleared (and futex signaled) when the thread dies? */
        if (clone_info->flags & CLONE_CHILD_CLEARTID)
            thread->clear_child_tid = clone_info->parent_tid_va;

        clone_info->newtid = tid;
    } while (0);

    /* Parent is still blocked on cond.wait() because we haven't signalled it yet.  We must signal after we've released the
     * mutex, because once we signal, the parent could return from clone_thread(), thus removing clone_info from the stack.  We
     * know the parent is already blocked on the cond.wait(), otherwise we couldn't have gotten this far (we couldn't have
     * acquired the mutex above) until the parent is inside cond.wait(). */
    clone_info->cond.notify_one();                      // tell parent we're done initializing
    clone_info = NULL; /* won't be valid after we signal the parent */

    if (tid<0)
        pthread_exit(NULL);

    thread->main();
}

void
RSIM_Process::sys_exit(int status)
{
    SAWYER_THREAD_TRAITS::RecursiveLockGuard lock(rwlock());
    terminated = true;
    termination_status = status;

    /* Tell all threads to exit. We do this by making sure they return from any blocking system call (by sending a
     * signal). Every thread checks its cancelation state before every instruction, and will therefore exit. */
    for (std::map<pid_t, RSIM_Thread*>::iterator ti=threads.begin(); ti!=threads.end(); ti++) {
        RSIM_Thread *thread = ti->second;
        thread->tracing(TRACE_THREAD) <<"process is canceling this thread\n";
        thread->get_real_thread().interrupt();
        //pthread_kill(thread->get_real_thread(), RSIM_SignalHandling::SIG_WAKEUP); /* in case it's blocked */
    }
}

int
RSIM_Process::sys_sigaction(int signo, const sigaction_32 *new_action, sigaction_32 *old_action) 
{
    if (signo<1 || signo>_NSIG)
        return -EINVAL;

    int retval = 0;
    SAWYER_THREAD_TRAITS::RecursiveLockGuard lock(rwlock());
    if (old_action)
        *old_action = signal_action[signo-1];

    if (new_action) {
        if (SIGKILL==signo || SIGSTOP==signo) {
            retval = -EINVAL;
        } else {
            signal_action[signo-1] = *new_action;
        }
    }

    return retval;
}

int
RSIM_Process::sys_kill(pid_t pid, const RSIM_SignalHandling::siginfo_32 &info)
{
    int signo = info.si_signo;
    int retval = 0;

    if (pid<0)
        return -EINVAL;
    if (signo<0 && (size_t)signo>8*sizeof(RSIM_SignalHandling::sigset_32))
        return -EINVAL;

    SAWYER_THREAD_TRAITS::RecursiveLockGuard lock(rwlock());
    if (pid!=getpid()) {
        retval = kill(pid, signo);
    } else {
        /* Send the signal to any one of our threads where it is not masked. */
        for (std::map<pid_t, RSIM_Thread*>::iterator ti=threads.begin(); signo>0 && ti!=threads.end(); ti++) {
            RSIM_Thread *thread = ti->second;
            int status = thread->signal_accept(info);
            if (status>=0) {
                if (boost::this_thread::get_id() != thread->get_real_thread().get_id()) {
                    pthread_t t = thread->get_real_thread().native_handle();
                    status = pthread_kill(t, RSIM_SignalHandling::SIG_WAKEUP);
                    assert(0==status);
                }
                signo = 0;
            }
        }

        /* If signal could not be delivered to any thread... */
        if (signo>0)
            sighand.generate(info, this, mlog[INFO]);
    }

    return retval;
}

/* Must be async signal safe */
void
RSIM_Process::signal_enqueue(const RSIM_SignalHandling::siginfo_32 &info)
{
    /* Push the signal number onto the tail of the process-wide queue.  It is safe to do this without thread synchronization
     * because:
     *   1. This signal handler is the only place where the queue tail is adjusted
     *   2. All signals are blocked during this signal handler
     *   3. The queue head is not modified.
     *   4. The RSIM_Process object is guaranteed to be valid for the duration of this call.
     */
    if ((sq.tail+1) % sq.size == sq.head) {
        static const char *s = "[***PROCESS SIGNAL QUEUE IS FULL***]";
        write(2, s, strlen(s));
    } else {
        sq.info[sq.tail] = info;
        sq.tail = (sq.tail + 1) % sq.size;
    }
}

int
RSIM_Process::signal_dequeue(RSIM_SignalHandling::siginfo_32 *info/*out*/)
{
    assert(info!=NULL);
    int retval = 0;
    SAWYER_THREAD_TRAITS::RecursiveLockGuard lock(rwlock());
    if (sq.head!=sq.tail) {
        *info = sq.info[sq.head];
        retval = info->si_signo;
        sq.head = (sq.head + 1) % sq.size;
    }
    return retval;
}

/* Dispatch process-received signals to threads */
void
RSIM_Process::signal_dispatch()
{
    /* write lock not required for thread safety here since called functions are already thread safe */
    RSIM_SignalHandling::siginfo_32 info;
    for (int signo=signal_dequeue(&info); signo>0; signo=signal_dequeue(&info)) {
        int status __attribute__((unused)) = sys_kill(getpid(), info);
        assert(status>=0);
    }
}

SgAsmBlock *
RSIM_Process::disassemble(bool fast, MemoryMap *map/*=null*/)
{
    SAWYER_THREAD_TRAITS::RecursiveLockGuard lock(rwlock()); // while using the memory map
    SgAsmBlock *block = NULL;
    Disassembler::InstructionMap insns;
    MemoryMap *allocated_map = NULL;
    if (fast) {
        if (!map) {
            map = allocated_map = new MemoryMap;
            *map = get_memory();                    // shallow copy: new segments point to same old data
            map->require(MemoryMap::EXECUTABLE).keep();
        }
        rose_addr_t start_va = 0; // arbitrary since we set the disassembler's SEARCH_UNUSED bit
        unsigned search = disassembler->get_search();
        disassembler->set_search(search | Disassembler::SEARCH_UNUSED);
        Disassembler::AddressSet successors;
        Disassembler::BadMap bad;
        insns = disassembler->disassembleBuffer(map, start_va, &successors, &bad);
        disassembler->set_search(search);
    } else {
        if (!map) {
            map = allocated_map = new MemoryMap;
            *map = get_memory();                    // shallow copy: new segments point to same old data
            map->require(MemoryMap::READABLE).keep(); // keep only readable memory; probably includes all executable too
        }
        Partitioner partitioner;
        block = partitioner.partition(interpretation, disassembler, map);
        insns = partitioner.get_instructions();
    }
    delete allocated_map; allocated_map=NULL;

    /* Add new instructions to cache */
    icache.insert(insns.begin(), insns.end());

    /* Fast disassembly puts all the instructions in a single SgAsmBlock */
    if (!block) {
        block = new SgAsmBlock;
        for (Disassembler::InstructionMap::const_iterator ii=icache.begin(); ii!=icache.end(); ++ii)
            block->get_statementList().push_back(ii->second);
    }
    return block;
}

void
RSIM_Process::set_callbacks(const RSIM_Callbacks &cb)
{
    SAWYER_THREAD_TRAITS::RecursiveLockGuard lock(rwlock());
    callbacks = cb; // overloaded, thread safe
}

/* Install callback in process and optionally in all existing threads. */
#define RSIM_PROCESS_DEFINE_INSTALL_CALLBACK(CALLBACK_CLASS, INSERTION_FUNCTION)                                               \
    void                                                                                                                       \
    RSIM_Process::install_callback(CALLBACK_CLASS *cb, RSIM_Callbacks::When when, bool everywhere)                             \
    {                                                                                                                          \
        if (cb) {                                                                                                              \
            callbacks.INSERTION_FUNCTION(when, cb);                                                                            \
            if (everywhere) {                                                                                                  \
                std::vector<RSIM_Thread*> thds = get_all_threads();                                                            \
                for (std::vector<RSIM_Thread*>::iterator ti=thds.begin(); ti!=thds.end(); ++ti)                                \
                    (*ti)->install_callback(dynamic_cast<CALLBACK_CLASS*>(cb->clone()), when);                                 \
            }                                                                                                                  \
        }                                                                                                                      \
    }
RSIM_PROCESS_DEFINE_INSTALL_CALLBACK(RSIM_Callbacks::InsnCallback,    add_insn_callback);
RSIM_PROCESS_DEFINE_INSTALL_CALLBACK(RSIM_Callbacks::MemoryCallback,  add_memory_callback);
RSIM_PROCESS_DEFINE_INSTALL_CALLBACK(RSIM_Callbacks::SyscallCallback, add_syscall_callback);
RSIM_PROCESS_DEFINE_INSTALL_CALLBACK(RSIM_Callbacks::SignalCallback,  add_signal_callback);
RSIM_PROCESS_DEFINE_INSTALL_CALLBACK(RSIM_Callbacks::ThreadCallback,  add_thread_callback);
RSIM_PROCESS_DEFINE_INSTALL_CALLBACK(RSIM_Callbacks::ProcessCallback, add_process_callback);
#undef RSIM_PROCESS_DEFINE_INSTALL_CALLBACK

/* Remove callback in process and optionally in all existing threads. */
#define RSIM_PROCESS_DEFINE_REMOVE_CALLBACK(CALLBACK_CLASS, REMOVAL_FUNCTION)                                                  \
    void                                                                                                                       \
    RSIM_Process::remove_callback(CALLBACK_CLASS *cb, RSIM_Callbacks::When when, bool everywhere)                              \
    {                                                                                                                          \
        if (cb) {                                                                                                              \
            callbacks.REMOVAL_FUNCTION(when, cb);                                                                              \
            if (everywhere) {                                                                                                  \
                std::vector<RSIM_Thread*> thds = get_all_threads();                                                            \
                for (std::vector<RSIM_Thread*>::iterator ti=thds.begin(); ti!=thds.end(); ++ti)                                \
                    (*ti)->remove_callback(cb, when);                                                                          \
            }                                                                                                                  \
        }                                                                                                                      \
    }
RSIM_PROCESS_DEFINE_REMOVE_CALLBACK(RSIM_Callbacks::InsnCallback,    remove_insn_callback);
RSIM_PROCESS_DEFINE_REMOVE_CALLBACK(RSIM_Callbacks::MemoryCallback,  remove_memory_callback);
RSIM_PROCESS_DEFINE_REMOVE_CALLBACK(RSIM_Callbacks::SyscallCallback, remove_syscall_callback);
RSIM_PROCESS_DEFINE_REMOVE_CALLBACK(RSIM_Callbacks::SignalCallback,  remove_signal_callback);
RSIM_PROCESS_DEFINE_REMOVE_CALLBACK(RSIM_Callbacks::ThreadCallback,  remove_thread_callback);
RSIM_PROCESS_DEFINE_REMOVE_CALLBACK(RSIM_Callbacks::ProcessCallback, remove_process_callback);
#undef RSIM_PROCESS_DEFINE_REMOVE_CALLBACK

#endif /* ROSE_ENABLE_SIMULATOR */
