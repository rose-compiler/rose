#include "rose.h"
#include "BinaryLoaderElf.h"
#include "RSIM_Private.h"

#ifdef ROSE_ENABLE_SIMULATOR

#include <boost/regex.hpp>
#include <errno.h>
#include <sys/mman.h>
#include <sys/user.h>
#include <sys/types.h>

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
RSIM_Process::create_thread()
{
    RSIM_Thread *thread = NULL;
    pid_t tid = syscall(SYS_gettid);
    ROSE_ASSERT(tid>=0);
    RTS_WRITE(rwlock()) {
        ROSE_ASSERT(threads.find(tid)==threads.end());
        thread = new RSIM_Thread(this);
        threads.insert(std::make_pair(tid, thread));
    } RTS_WRITE_END;
    return thread;
}

void
RSIM_Process::remove_thread(RSIM_Thread *thread)
{
    RTS_WRITE(rwlock()) {
        std::map<pid_t, RSIM_Thread*>::iterator ti = threads.find(thread->get_tid());
        assert(ti!=threads.end());
        threads.erase(ti);
    } RTS_WRITE_END;
}

RSIM_Thread *
RSIM_Process::get_thread(pid_t tid) const
{
    std::map<pid_t, RSIM_Thread*>::const_iterator ti=threads.find(tid);
    return ti==threads.end() ? NULL : ti->second;
}

size_t
RSIM_Process::mem_write(const void *buf, rose_addr_t va, size_t size)
{
    size_t retval;
    RTS_WRITE(rwlock()) {
        retval = map->write(buf, va, size);
    } RTS_WRITE_END;
    return retval;
}

size_t
RSIM_Process::mem_read(void *buf, rose_addr_t va, size_t size)
{
    size_t retval;
    RTS_READ(rwlock()) {
        retval = map->read(buf, va, size);
    } RTS_READ_END;
    return retval;
}

bool
RSIM_Process::mem_is_mapped(rose_addr_t va) const
{
    bool retval;
    RTS_READ(rwlock()) {
        retval = (map->find(va) != NULL);
    } RTS_READ_END;
    return retval;
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
    if (trace)
        fprintf(trace, "loading %s...\n", exeargs[0].c_str());
    char *frontend_args[4];
    frontend_args[0] = strdup("-");
    frontend_args[1] = strdup("-rose:read_executable_file_format_only"); /*delay disassembly until later*/
    frontend_args[2] = strdup(exeargs[0].c_str());
    frontend_args[3] = NULL;
    SgProject *project = frontend(3, frontend_args);

    /* Create the main thread */
    RSIM_Thread *thread = create_thread();

    /* Find the best interpretation and file header.  Windows PE programs have two where the first is DOS and the second is PE
     * (we'll use the PE interpretation). */
    SgAsmInterpretation *interpretation = SageInterface::querySubTree<SgAsmInterpretation>(project, V_SgAsmInterpretation).back();
    SgAsmGenericHeader *fhdr = interpretation->get_headers()->get_headers().front();
    thread->policy.writeIP(fhdr->get_entry_rva() + fhdr->get_base_va());

    /* Link the interpreter into the AST */
    SimLoader *loader = new SimLoader(interpretation, trace, interpname);

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
        if (trace)
            fprintf(trace, "warning: static executable; no vdso necessary\n");
    } else {
        bool vdso_loaded = false;
        for (size_t i=0; i<vdso_paths.size() && !vdso_loaded; i++) {
            for (int j=0; j<2 && !vdso_loaded; j++) {
                std::string vdso_name = vdso_paths[i] + (j ? "" : "/" + this->vdso_name);
                if (trace)
                    fprintf(trace, "looking for vdso: %s\n", vdso_name.c_str());
                if ((vdso_loaded = loader->map_vdso(vdso_name, interpretation, map))) {
                    vdso_mapped_va = loader->vdso_mapped_va;
                    vdso_entry_va = loader->vdso_entry_va;
                    if (trace) {
                        fprintf(trace, "mapped %s at 0x%08"PRIx64" with entry va 0x%08"PRIx64"\n",
                                vdso_name.c_str(), vdso_mapped_va, vdso_entry_va);
                    }
                }
            }
        }
        if (!vdso_loaded && trace)
            fprintf(trace, "warning: cannot find a virtual dynamic shared object\n");
    }

    /* Find a disassembler. */
    if (!disassembler) {
        disassembler = Disassembler::lookup(interpretation)->clone();
        disassembler->set_progress_reporting(NULL, 0); /* turn off progress reporting */
    }

    /* Initialize the brk value to be the lowest page-aligned address that is above the end of the highest mapped address but
     * below 0x40000000 (the stack, and where ld-linux.so.2 might be loaded when loaded high). */
    rose_addr_t free_area = std::max(map->find_last_free(std::max(ld_linux_base_va, (rose_addr_t)0x40000000)),
                                     (rose_addr_t)brk_base);
    brk_va = ALIGN_UP(free_area, PAGE_SIZE);

    delete loader;

    pid_t tid = syscall(SYS_gettid);
    thread->tracing(TRACE_THREAD)->mesg("new thread with tid %d", tid);

    return fhdr;
}

void
RSIM_Process::dump_core(int signo, std::string base_name)
{
    if (base_name.empty())
        base_name = core_base_name;

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

SgAsmInstruction *
RSIM_Process::get_instruction(rose_addr_t va)
{
    SgAsmInstruction *insn = NULL;

    /* Use a cached instruction if possible. */
    RTS_READ(rwlock()) {
        Disassembler::InstructionMap::iterator found = icache.find(va);
        insn = found!=icache.end() ? found->second : NULL;
    } RTS_READ_END;

    /* If we found a cached instruction, make sure memory still contains that value. */
    if (insn) {
        size_t insn_sz = insn->get_raw_bytes().size();
        SgUnsignedCharList curmem(insn_sz);
        size_t nread = mem_read(&curmem[0], va, insn_sz);
        if (nread==insn_sz && curmem==insn->get_raw_bytes())
            return insn;
    }

    /* Disassemble (and cache) a new instruction. At this time it is not safe to be multi-threaded inside a single Disassemble
     * object, so we'll protect the whole call with a write lock.  We need one anyway in order to update the icache.
     * [RPM 2011-02-09] */
    RTS_WRITE(rwlock()) {
        insn = isSgAsmx86Instruction(disassembler->disassembleOne(map, va)); /* might throw Disassembler::Exception */
        ROSE_ASSERT(insn!=NULL); /*only happens if our disassembler is not an x86 disassembler!*/
        icache[va] = insn;
    } RTS_WRITE_END;

    return insn;
}
        
void *
RSIM_Process::my_addr(uint32_t va, size_t nbytes)
{
    void *retval = NULL;

    RTS_READ(rwlock()) {
        /* Obtain mapping information and check that the specified number of bytes are mapped. */
        const MemoryMap::MapElement *me = map->find(va);
        if (!me)
            break;
        size_t offset = 0;
        try {
            offset = me->get_va_offset(va, nbytes);
        } catch (const MemoryMap::NotMapped) {
            break;
        }
        retval = (uint8_t*)me->get_base() + offset;
    } RTS_READ_END;
    return retval;
}

uint32_t
RSIM_Process::guest_va(void *addr, size_t nbytes)
{
    uint32_t retval = 0;
    RTS_READ(rwlock()) {
        const std::vector<MemoryMap::MapElement> elmts = map->get_elements();
        for (std::vector<MemoryMap::MapElement>::const_iterator ei=elmts.begin(); ei!=elmts.end(); ++ei) {
            uint8_t *base = (uint8_t*)ei->get_base(false);
            rose_addr_t offset = ei->get_offset();
            size_t size = ei->get_size();
            if (base && addr>=base+offset && (uint8_t*)addr+nbytes<=base+offset+size) {
                retval = ei->get_va() + ((uint8_t*)addr - (base+offset));
                break;
            }
        }
    } RTS_READ_END;
    return retval;
}

std::string
RSIM_Process::read_string(uint32_t va, size_t limit/*=0*/, bool *error/*=NULL*/)
{
    std::string retval;
    if (error)
        *error = false;

    RTS_READ(rwlock()) {
        while (1) {
            uint8_t byte;
            size_t nread = map->read(&byte, va++, 1);
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
    } RTS_READ_END;
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

int
RSIM_Process::mem_setbrk(rose_addr_t newbrk, RTS_Message *mesg)
{
    int retval = -ENOSYS;

    if (newbrk >= 0xb0000000ul)
        return -ENOMEM;

    RTS_WRITE(rwlock()) {
        if (newbrk > brk_va) {
            MemoryMap::MapElement melmt(brk_va, newbrk-brk_va, MemoryMap::MM_PROT_READ|MemoryMap::MM_PROT_WRITE);
            melmt.set_name("[heap]");
            map->insert(melmt);
            brk_va = newbrk;
        } else if (newbrk>0 && newbrk<brk_va) {
            map->erase(MemoryMap::MapElement(newbrk, brk_va-newbrk));
            brk_va = newbrk;
        }
        retval= brk_va;

        if (mesg && mesg->get_file()) {
            RTS_MESSAGE(*mesg) {
                mesg->mesg("memory map after brk syscall:\n");
                map->dump(mesg->get_file(), "    ");
            } RTS_MESSAGE_END(true);
        }
    } RTS_WRITE_END;

    return retval;
}

int
RSIM_Process::mem_unmap(rose_addr_t va, size_t sz, RTS_Message *mesg)
{
    int retval = -ENOSYS;
    RTS_WRITE(rwlock()) {
        /* Make sure that the specified memory range is actually mapped, or return -ENOMEM. */
        ExtentMap extents;
        extents.insert(ExtentPair(va, sz));
        extents.erase(map->va_extents());
        if (!extents.empty()) {
            retval = -ENOMEM;
            break;
        }

        /* Unmap for real, because if we don't, and the mapping was not anonymous, and the file that was mapped is
         * unlinked, and we're on NFS, an NFS temp file is created in place of the unlinked file. */
        uint8_t *ptr = NULL;
        try {
            const MemoryMap::MapElement *me = map->find(va);
            size_t offset = me->get_va_offset(va, sz);
            ptr = (uint8_t*)me->get_base() + offset;
            if (0==(uint64_t)ptr % (uint64_t)PAGE_SIZE && 0==(uint64_t)sz % (uint64_t)PAGE_SIZE)
                (void)munmap(ptr, sz);
        } catch (const MemoryMap::NotMapped) {
        }

        /* Erase the mapping from the simulation */
        map->erase(MemoryMap::MapElement(va, sz));

        /* Tracing */
        if (mesg && mesg->get_file()) {
            RTS_MESSAGE(*mesg) {
                mesg->mesg("memory map after munmap syscall:\n");
                map->dump(mesg->get_file(), "    ");
            } RTS_MESSAGE_END(true);
        }

        retval = 0;
    } RTS_WRITE_END;
    return retval;
}

void
RSIM_Process::mem_showmap(RTS_Message *mesg, const char *intro, const char *prefix)
{
    if (!intro || !*intro) intro = "memory map\n";
    if (!prefix) prefix = "    ";

    if (mesg && mesg->get_file()) {
        RTS_MESSAGE(*mesg) {
            mesg->mesg(intro);
            RTS_READ(rwlock()) {
                map->dump(mesg->get_file(), prefix);
            } RTS_READ_END;
        } RTS_MESSAGE_END(true);
    }
}

void
RSIM_Process::btrace_close()
{
    RTS_WRITE(rwlock()) {
        if (btrace_file) {
            fclose(btrace_file);
            btrace_file = NULL;
        }
    } RTS_WRITE_END;
}

int
RSIM_Process::mem_protect(rose_addr_t va, size_t sz, unsigned rose_perms, unsigned real_perms)
{
    int retval = -ENOSYS;
    size_t aligned_sz = ALIGN_UP(sz, PAGE_SIZE);

    RTS_WRITE(rwlock()) {
        /* Set protection in the underlying real memory (to catch things like trying to add write permission to memory that's
         * mapped from a read-only file), then also set the protection in the simulated memory map so the simulator can make
         * queries about memory access.  Some of the underlying memory points to parts of an ELF file that was read into ROSE's
         * memory in such a way that segments are not aligned on page boundaries. We cannot change protections on these
         * non-aligned sections. */
        if (-1==mprotect(my_addr(va, sz), sz, real_perms) && EINVAL!=errno) {
            retval = -errno;
            break;
        } else {
            try {
                map->mprotect(MemoryMap::MapElement(va, aligned_sz, rose_perms));
                retval = 0;
            } catch (const MemoryMap::NotMapped &e) {
                retval = -ENOMEM;
            }
        }
    } RTS_WRITE_END;
    return retval;
}

rose_addr_t
RSIM_Process::mem_map(rose_addr_t start, size_t size, unsigned rose_perms, unsigned flags, size_t offset, int fd)
{
    size_t aligned_size = ALIGN_UP(size, PAGE_SIZE);
    void *buf = NULL;
    unsigned prot = ((rose_perms & MemoryMap::MM_PROT_READ  ? PROT_READ  : 0) |
                     (rose_perms & MemoryMap::MM_PROT_WRITE ? PROT_WRITE : 0) |
                     (rose_perms & MemoryMap::MM_PROT_EXEC  ? PROT_EXEC  : 0));

    RTS_WRITE(rwlock()) {
        if (0==start) {
            try {
                start = map->find_free(mmap_start, aligned_size, PAGE_SIZE);
            } catch (const MemoryMap::NoFreeSpace &e) {
                start = (rose_addr_t)(int64_t)-ENOMEM;
                break;
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

            MemoryMap::MapElement melmt(start, aligned_size, buf, 0, rose_perms);
            melmt.set_name("mmap2("+melmt_name+")");
            map->erase(melmt); /*clear space space first to avoid MemoryMap::Inconsistent exception*/
            map->insert(melmt);
        }
    } RTS_WRITE_END;
    return start;
}

void
RSIM_Process::set_gdt(const user_desc_32 *ud)
{
    RTS_WRITE(rwlock()) {
        *(gdt_entry(ud->entry_number)) = *ud;
    } RTS_WRITE_END;
}

user_desc_32 *
RSIM_Process::gdt_entry(int idx)
{
    user_desc_32 *retval;
    RTS_READ(rwlock()) {
        ROSE_ASSERT(idx>0 && idx<GDT_ENTRIES);
        ROSE_ASSERT(idx<GDT_ENTRY_TLS_MIN || idx>GDT_ENTRY_TLS_MAX); /* call only from RSIM_Thread::set_gdt */
        retval = gdt + idx;
    } RTS_READ_END;
    return retval;
}



/* Initialize the stack of the main thread. */
void
RSIM_Process::initialize_stack(SgAsmGenericHeader *_fhdr, int argc, char *argv[])
{
    FILE *trace = (tracing_flags & tracingFacilityBit(TRACE_LOADER)) ? tracing_file : NULL;

    RTS_WRITE(rwlock()) {
        RSIM_Thread *main_thread = get_thread(getpid());
        ROSE_ASSERT(main_thread!=NULL);

        /* We only handle ELF for now */
        SgAsmElfFileHeader *fhdr = isSgAsmElfFileHeader(_fhdr);
        ROSE_ASSERT(fhdr!=NULL);

        /* Allocate the stack */
        static const size_t stack_size = 0x00015000;
        size_t sp = main_thread->policy.readGPR(x86_gpr_sp).known_value();
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
            mem_write(arg.c_str(), sp, len);
            pointers.push_back(sp);
            if (trace)
                fprintf(trace, "argv[%d] %zu bytes at 0x%08zu = \"%s\"\n", i, len, sp, arg.c_str());
        }
        pointers.push_back(0); /*the argv NULL terminator*/

        /* Create new environment variables by stripping "X86SIM_" off the front of any environment variable and using that
         * value to override the non-X86SIM_ value, if any.  We try to make sure the variables are in the same order as if the
         * X86SIM_ overrides were not present. In other words, if X86SIM_FOO and FOO are both present, then X86SIM_FOO is
         * deleted from the list and its value used for FOO; but if X86SIM_FOO is present without FOO, then we just change the
         * name to FOO and leave it at that location. We do all this so that variables are in the same order whether run
         * natively or under the simulator. */
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
            mem_write(env.c_str(), sp, env.size()+1);
            pointers.push_back(sp);
            if (trace)
                fprintf(trace, "environ[%d] %zu bytes at 0x%08zu = \"%s\"\n", j++, env.size(), sp, env.c_str());
        }
        pointers.push_back(0); /*environment NULL terminator*/

        /* Initialize stack with auxv, where each entry is two words in the pointers vector. This information is only present for
         * dynamically linked executables. The order and values were determined by running the simulator with the "--showauxv"
         * switch on hudson-rose-07. */
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
                if (trace)
                    fprintf(trace, "AT_SYSINFO:       0x%08"PRIx32"\n", auxv.back());

                /* AT_SYSINFO_PHDR */
                auxv.push_back(33);
                auxv.push_back(vdso_mapped_va);
                if (trace)
                    fprintf(trace, "AT_SYSINFO_PHDR:  0x%08"PRIx32"\n", auxv.back());
            }

            /* AT_HWCAP (see linux <include/asm/cpufeature.h>). */
            auxv.push_back(16);
            uint32_t hwcap = 0xbfebfbfful; /* value used by hudson-rose-07 */
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
            auxv.push_back(t1.phdr_rva + fhdr->get_base_va());
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
            auxv.push_back(ld_linux_base_va);
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
            auxv.push_back(23);
            auxv.push_back(false);
            if (trace)
                fprintf(trace, "AT_SECURE:        %"PRId32"\n", auxv.back());

            /* AT_PLATFORM */
            {
                const char *platform = "i686";
                size_t len = strlen(platform)+1;
                sp -= len;
                mem_write(platform, sp, len);
                auxv.push_back(15);
                auxv.push_back(sp);
                if (trace)
                    fprintf(trace, "AT_PLATFORM:      0x%08"PRIx32" (%s)\n", auxv.back(), platform);
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
        mem_write(&(pointers[0]), sp, 4*pointers.size());

        main_thread->policy.writeGPR(x86_gpr_sp, sp);
    } RTS_WRITE_END;
}

void
RSIM_Process::post_fork()
{
    assert(1==threads.size());
    RSIM_Thread *t = threads.begin()->second;
    threads.clear();
    threads[t->get_tid()] = t;
}

/* The "thread" arg must be the calling thread */
pid_t
RSIM_Process::clone_thread(RSIM_Thread *thread, unsigned flags, uint32_t parent_tid_va, uint32_t child_tls_va,
                           const pt_regs_32 &regs)
{
    Clone clone_info(this, flags, parent_tid_va, child_tls_va, regs);
    RTS_MUTEX(clone_info.mutex) {
        pthread_t t;
        int err = -pthread_create(&t, NULL, RSIM_Process::clone_thread_helper, &clone_info);
        if (0==err)
            pthread_cond_wait(&clone_info.cond, &clone_info.mutex); /* wait for child to initialize */
    } RTS_MUTEX_END;
    return clone_info.newtid; /* filled in by clone_thread_helper; negative on error */
}

void *
RSIM_Process::clone_thread_helper(void *_clone_info)
{
    /* clone_info points to the creating thread's stack (inside clone_thread). Since the creator's clone_thread doesn't return
     * until after we've signaled clone_info.cond and released clone_info.mutex, its safe to access it here in this thread. */
    Clone *clone_info = (Clone*)_clone_info;
    ROSE_ASSERT(clone_info!=NULL);
    RSIM_Process *process = clone_info->process;
    ROSE_ASSERT(process!=NULL);
    RSIM_Thread *thread = process->create_thread();

    pid_t tid = syscall(SYS_gettid);
    ROSE_ASSERT(tid>=0);
    thread->tracing(TRACE_THREAD)->mesg("new thread with tid %d", tid);

    RTS_MUTEX(clone_info->mutex) {
        /* Make our TID available to our parent. */
        clone_info->newtid = tid;
        clone_info->seq = thread->get_seq();

        /* Set up thread local storage */
        if (clone_info->flags & CLONE_SETTLS) {
            user_desc_32 ud;
            if (sizeof(ud)!=process->mem_read(&ud, clone_info->child_tls_va, sizeof ud)) {
                tid = -EFAULT;
                goto release_mutex;
            }
            int status = thread->set_thread_area(&ud, false);
            if (status<0) {
                tid = status;
                goto release_mutex;
            }
        }
        
        /* Initialize our registers.  Has to be after we initialize TLS segments in the GDT */
        thread->init_regs(clone_info->regs);

        /* Write child TID into process memory if requested */
        if ((clone_info->flags & CLONE_PARENT_SETTID) &&
            4!=process->mem_write(&tid, clone_info->parent_tid_va, 4)) {
            tid = -EFAULT;
            goto release_mutex;
        }
        if ((clone_info->flags & CLONE_CHILD_SETTID) &&
            4!=process->mem_write(&tid, clone_info->child_tls_va, 4)) {
            tid = -EFAULT;
            goto release_mutex;
        }

        /* Should a memory location be cleared (and futex signaled) when the thread dies? */
        if (clone_info->flags & CLONE_CHILD_CLEARTID)
            thread->clear_child_tid = clone_info->parent_tid_va;


    release_mutex:
        clone_info->newtid = tid;
        pthread_cond_signal(&clone_info->cond);       /* tell parent we're done initializing */
        clone_info = NULL; /* won't be valid after we release mutex */
    } RTS_MUTEX_END;
    if (tid<0)
        pthread_exit(NULL);

    /* Allow the real thread to simulate the specimen thred. */
    return thread->main();
}

void
RSIM_Process::sys_exit(int status)
{
    RTS_WRITE(rwlock()) {
        terminated = true;
        termination_status = status;

        /* Tell all threads to exit. We do this by making sure they return from any blocking system call (by sending a
         * signal). Every thread checks its cancelation state before every instruction, and will therefore exit. */
        for (std::map<pid_t, RSIM_Thread*>::iterator ti=threads.begin(); ti!=threads.end(); ti++) {
            RSIM_Thread *thread = ti->second;
            thread->tracing(TRACE_THREAD)->mesg("process is canceling this thread");
            pthread_cancel(thread->get_real_thread());
            pthread_kill(thread->get_real_thread(), RSIM_SignalHandling::SIG_WAKEUP); /* in case it's blocked */
        }
    } RTS_WRITE_END;
}

int
RSIM_Process::sys_sigaction(int signo, const sigaction_32 *new_action, sigaction_32 *old_action) 
{
    if (signo<1 || signo>_NSIG)
        return -EINVAL;

    int retval = 0;
    RTS_WRITE(rwlock()) {
        if (old_action)
            *old_action = signal_action[signo-1];

        if (new_action) {
            if (SIGKILL==signo || SIGSTOP==signo) {
                retval = -EINVAL;
            } else {
                signal_action[signo-1] = *new_action;
            }
        }
    } RTS_WRITE_END;
    return retval;
}

int
RSIM_Process::sys_kill(pid_t pid, int signo)
{
    int retval = 0;

    if (pid<0)
        return -EINVAL;
    if (signo<=0 && (size_t)signo>8*sizeof(RSIM_SignalHandling::sigset_32))
        return -EINVAL;


    RTS_WRITE(rwlock()) {
        if (pid!=getpid()) {
            retval = kill(pid, signo);
        } else {
            /* Send the signal to any one of our threads where it is not masked. */
            for (std::map<pid_t, RSIM_Thread*>::iterator ti=threads.begin(); signo>0 && ti!=threads.end(); ti++) {
                RSIM_Thread *thread = ti->second;
                int status = thread->signal_accept(signo);
                if (status>=0) {
                    status = pthread_kill(thread->get_real_thread(), RSIM_SignalHandling::SIG_WAKEUP);
                    assert(0==status);
                    signo = 0;
                }
            }

            /* If signal could not be delivered to any thread... */
            if (signo>0)
                sighand.generate(signo, this, NULL);
        }
    } RTS_WRITE_END;

    return retval;
}

/* Must be async signal safe */
void
RSIM_Process::signal_enqueue(int signo)
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
        sq.signals[sq.tail] = signo;
        sq.tail = (sq.tail + 1) % sq.size;
    }
}

int
RSIM_Process::signal_dequeue()
{
    int retval = 0;
    RTS_WRITE(rwlock()) {
        if (sq.head!=sq.tail) {
            retval = sq.signals[sq.head];
            sq.head = (sq.head + 1) % sq.size;
        }
    } RTS_WRITE_END;
    return retval;
}

/* Dispatch process-received signals to threads */
void
RSIM_Process::signal_dispatch()
{
    /* write lock not required for thread safety here since called functions are already thread safe */
    for (int signo=signal_dequeue(); signo>0; signo=signal_dequeue()) {
        int status = sys_kill(getpid(), signo);
        assert(status>=0);
    }
}

#endif /* ROSE_ENABLE_SIMULATOR */
