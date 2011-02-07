#include "sage3basic.h"

#include "BinaryLoaderElf.h"
#include "RSIM_Simulator.h"

#include <boost/regex.hpp>
#include <errno.h>
#include <sys/user.h>

void
RSIM_Process::ctor()
{
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
}

FILE *
RSIM_Process::tracing(unsigned what) const
{
    return (trace_flags & what) ? trace_file : NULL;
}

void
RSIM_Process::set_tracing(FILE *f, unsigned what)
{
    trace_file = f;
    trace_flags = what;
}

RSIM_Thread *
RSIM_Process::create_thread(pid_t tid)
{
    ROSE_ASSERT(threads.find(tid)==threads.end());

    RSIM_Thread *thread = new RSIM_Thread(this);
    threads.insert(std::make_pair(tid, thread));
    return thread;
}

RSIM_Thread *
RSIM_Process::get_thread(pid_t tid) const
{
    std::map<pid_t, RSIM_Thread*>::const_iterator ti=threads.find(tid);
    return ti==threads.end() ? NULL : ti->second;
}

size_t
RSIM_Process::get_ninsns() const
{
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

        if ((size_t)sb.st_size!=ALIGN_UP((size_t)sb.st_size, PAGE_SIZE)) {
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
RSIM_Process::load(const char *name)
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
    if (tracing(TRACE_LOADER))
        fprintf(tracing(TRACE_LOADER), "loading %s...\n", exeargs[0].c_str());
    char *frontend_args[4];
    frontend_args[0] = strdup("-");
    frontend_args[1] = strdup("-rose:read_executable_file_format_only"); /*delay disassembly until later*/
    frontend_args[2] = strdup(exeargs[0].c_str());
    frontend_args[3] = NULL;
    SgProject *project = frontend(3, frontend_args);

    /* Create the main thread */
    RSIM_Thread *thread = create_thread(getpid());

    /* Find the best interpretation and file header.  Windows PE programs have two where the first is DOS and the second is PE
     * (we'll use the PE interpretation). */
    SgAsmInterpretation *interpretation = SageInterface::querySubTree<SgAsmInterpretation>(project, V_SgAsmInterpretation).back();
    SgAsmGenericHeader *fhdr = interpretation->get_headers()->get_headers().front();
    thread->policy.writeIP(fhdr->get_entry_rva() + fhdr->get_base_va());

    /* Link the interpreter into the AST */
    SimLoader *loader = new SimLoader(interpretation, tracing(TRACE_LOADER), interpname);

    /* If we found an interpreter then use its entry address as the start of simulation.  When running the specimen directly
     * in Linux with "setarch i386 -LRB3", the ld-linux.so.2 gets mapped to 0x40000000 if it has no preferred address.  We can
     * accomplish the same thing simply by rebasing the library. */
    if (loader->interpreter) {
        SgAsmGenericSection *load0 = loader->interpreter->get_section_by_name("LOAD#0");
        if (load0 && load0->is_mapped() && load0->get_mapped_preferred_rva()==0 && load0->get_mapped_size()>0)
            loader->interpreter->set_base_va(ld_linux_base_va);
        thread->policy.writeIP(loader->interpreter->get_entry_rva() + loader->interpreter->get_base_va());
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
        if (tracing(TRACE_LOADER))
            fprintf(stderr, "warning: static executable; no vdso necessary\n");
    } else {
        bool vdso_loaded = false;
        for (size_t i=0; i<vdso_paths.size() && !vdso_loaded; i++) {
            for (int j=0; j<2 && !vdso_loaded; j++) {
                std::string vdso_name = vdso_paths[i] + (j ? "" : "/" + this->vdso_name);
                if (tracing(TRACE_LOADER))
                    fprintf(tracing(TRACE_LOADER), "looking for vdso: %s\n", vdso_name.c_str());
                if ((vdso_loaded = loader->map_vdso(vdso_name, interpretation, map))) {
                    vdso_mapped_va = loader->vdso_mapped_va;
                    vdso_entry_va = loader->vdso_entry_va;
                    if (tracing(TRACE_LOADER)) {
                        fprintf(tracing(TRACE_LOADER), "mapped %s at 0x%08"PRIx64" with entry va 0x%08"PRIx64"\n",
                                vdso_name.c_str(), vdso_mapped_va, vdso_entry_va);
                    }
                }
            }
        }
        if (!vdso_loaded && tracing(TRACE_LOADER))
            fprintf(stderr, "warning: cannot find a virtual dynamic shared object\n");
    }

    /* Find a disassembler. */
    if (!thread->disassembler) {
        thread->disassembler = Disassembler::lookup(interpretation)->clone();
        thread->disassembler->set_progress_reporting(NULL, 0); /* turn off progress reporting */
    }

    /* Initialize the brk value to be the lowest page-aligned address that is above the end of the highest mapped address but
     * below 0x40000000 (the stack, and where ld-linux.so.2 might be loaded when loaded high). */
    rose_addr_t free_area = std::max(map->find_last_free(std::max(ld_linux_base_va, (rose_addr_t)0x40000000)),
                                     (rose_addr_t)brk_base);
    brk_va = ALIGN_UP(free_area, PAGE_SIZE);

    delete loader;
    return fhdr;
}

void
RSIM_Process::dump_core(int signo, std::string base_name)
{
    if (base_name.empty())
        base_name = core_base_name;

    fprintf(stderr, "dumping specimen core...\n");
    fprintf(stderr, "memory map at time of core dump:\n");
    map->dump(stderr, "  ");

#if 0 /* FIXME: we need to make core dumping thread-aware. [RPM 2011-02-03] */
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
}

void
RSIM_Process::open_trace_file()
{
    char name[4096];

    if (!trace_file_name.empty()) {
        size_t nprinted = snprintf(name, sizeof name, trace_file_name.c_str(), getpid());
        if (nprinted > sizeof name) {
            fprintf(stderr, "name pattern overflow: %s\n", trace_file_name.c_str());
            trace_file = stderr;
            return;
        }
    } else {
        name[0] = '\0';
    }

    if (trace_file && trace_file!=stderr && trace_file!=stdout) {
        fclose(trace_file);
        trace_file = NULL;
    }

    if (name[0]) {
        if (NULL==(trace_file = fopen(name, "w"))) {
            fprintf(stderr, "%s: %s\n", strerror(errno), name);
            return;
        }
#ifdef X86SIM_LOG_UNBUFFERED
        setbuf(trace_file, NULL);
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

    size_t insn_size = insn->get_raw_bytes().size();
    assert(insn_size<=255);
    uint8_t insn_size_byte = insn_size;
    n = fwrite(&insn_size_byte, 1, 1, btrace_file);
    assert(1==n);

    n = fwrite(&insn->get_raw_bytes()[0], insn_size, 1, btrace_file);
    assert(1==n);

    uint32_t r = 0;
    for (size_t i=0; i<VirtualMachineSemantics::State::n_flags; i++)
        r |= thread->policy.readFlag((X86Flag)i).known_value() << i;
    n = fwrite(&r, 4, 1, btrace_file);
    assert(1==n);

    r = thread->policy.readGPR(x86_gpr_ax).known_value();
    n = fwrite(&r, 4, 1, btrace_file);
    assert(1==n);
    
    r = thread->policy.readGPR(x86_gpr_bx).known_value();
    n = fwrite(&r, 4, 1, btrace_file);
    assert(1==n);
    
    r = thread->policy.readGPR(x86_gpr_cx).known_value();
    n = fwrite(&r, 4, 1, btrace_file);
    assert(1==n);
    
    r = thread->policy.readGPR(x86_gpr_dx).known_value();
    n = fwrite(&r, 4, 1, btrace_file);
    assert(1==n);
    
    r = thread->policy.readGPR(x86_gpr_si).known_value();
    n = fwrite(&r, 4, 1, btrace_file);
    assert(1==n);
    
    r = thread->policy.readGPR(x86_gpr_di).known_value();
    n = fwrite(&r, 4, 1, btrace_file);
    assert(1==n);
    
    r = thread->policy.readGPR(x86_gpr_bp).known_value();
    n = fwrite(&r, 4, 1, btrace_file);
    assert(1==n);
    
    r = thread->policy.readGPR(x86_gpr_sp).known_value();
    n = fwrite(&r, 4, 1, btrace_file);
    assert(1==n);

    r = thread->policy.readSegreg(x86_segreg_cs).known_value();
    n = fwrite(&r, 2, 1, btrace_file);
    assert(1==n);

    r = thread->policy.readSegreg(x86_segreg_ss).known_value();
    n = fwrite(&r, 2, 1, btrace_file);
    assert(1==n);

    r = thread->policy.readSegreg(x86_segreg_es).known_value();
    n = fwrite(&r, 2, 1, btrace_file);
    assert(1==n);

    r = thread->policy.readSegreg(x86_segreg_ds).known_value();
    n = fwrite(&r, 2, 1, btrace_file);
    assert(1==n);

    r = thread->policy.readSegreg(x86_segreg_fs).known_value();
    n = fwrite(&r, 2, 1, btrace_file);
    assert(1==n);

    r = thread->policy.readSegreg(x86_segreg_gs).known_value();
    n = fwrite(&r, 2, 1, btrace_file);
    assert(1==n);
}

void
RSIM_Process::load_segreg_shadow(X86SegmentRegister sr, unsigned gdt_num)
{
    ROSE_ASSERT(gdt_num<(sizeof(gdt)/sizeof(gdt[0])));
    segreg_shadow[sr] = gdt[gdt_num];
    ROSE_ASSERT(segreg_shadow[sr].present);
}
    
