#include "sage3basic.h"

#include "RSIM_Thread.h"
#include "RSIM_Process.h"

#include <errno.h>
#include <stdarg.h>
#include <syscall.h>

#include <sys/user.h>

/* Constructor */
void
RSIM_Thread::ctor()
{
    for (size_t i=0; i<VirtualMachineSemantics::State::n_gprs; i++)
        writeGPR((X86GeneralPurposeRegister)i, 0);
    for (size_t i=0; i<VirtualMachineSemantics::State::n_flags; i++)
        writeFlag((X86Flag)i, 0);
    writeIP(0);
    writeFlag((X86Flag)1, true_());
    writeGPR(x86_gpr_sp, 0xbffff000ul);     /* high end of stack, exclusive */

    writeSegreg(x86_segreg_cs, 0x23);
    writeSegreg(x86_segreg_ds, 0x2b);
    writeSegreg(x86_segreg_es, 0x2b);
    writeSegreg(x86_segreg_ss, 0x2b);
    writeSegreg(x86_segreg_fs, 0x2b);
    writeSegreg(x86_segreg_gs, 0x2b);

    memset(signal_action, 0, sizeof signal_action);
    signal_stack.ss_sp = 0;
    signal_stack.ss_size = 0;
    signal_stack.ss_flags = SS_DISABLE;
}

FILE *
RSIM_Thread::tracing(unsigned what) const
{
    return process->tracing(what);
}

void
RSIM_Thread::dump_registers(FILE *f) const
{
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
                segregToString(sr), readSegreg(sr).known_value(), process->segreg_shadow[sr].base, process->segreg_shadow[sr].limit,
                process->segreg_shadow[sr].present?"yes":"no");
    }

    uint32_t eflags = get_eflags();
    fprintf(f, "    flags: 0x%08"PRIx32":", eflags);
    static const char *flag_name[] = {"cf",  "#1",  "pf",   "#3",    "af",    "#5",  "zf",  "sf",
                                      "tf",  "if",  "df",   "of", "iopl0", "iopl1",  "nt", "#15",
                                      "rf",  "vm",  "ac",  "vif",   "vip",    "id", "#22", "#23",
                                      "#24", "#25", "#26", "#27",   "#28",   "#29", "#30", "#31"};
    for (uint32_t i=0; i<32; i++) {
        if (eflags & (1u<<i))
            fprintf(f, " %s", flag_name[i]);
    }
    fprintf(f, "\n");
}

uint32_t
RSIM_Thread::get_eflags() const
{
    uint32_t eflags = 0;
    for (size_t i=0; i<VirtualMachineSemantics::State::n_flags; i++) {
        if (readFlag((X86Flag)i).is_known())
            eflags |= readFlag((X86Flag)i).known_value() ? 1u<<i : 0u;
    }
    return eflags;
}

void
RSIM_Thread::push(VirtualMachineSemantics::ValueType<32> n)
{
    VirtualMachineSemantics::ValueType<32> new_sp = add(readGPR(x86_gpr_sp), number<32>(-4));
    writeMemory(x86_segreg_ss, new_sp, n, true_());
    writeGPR(x86_gpr_sp, new_sp);
}

VirtualMachineSemantics::ValueType<32>
RSIM_Thread::pop()
{
    VirtualMachineSemantics::ValueType<32> old_sp = readGPR(x86_gpr_sp);
    VirtualMachineSemantics::ValueType<32> retval = readMemory<32>(x86_segreg_ss, old_sp, true_());
    writeGPR(x86_gpr_sp, add(old_sp, number<32>(4)));
    return retval;
}

SgAsmx86Instruction *
RSIM_Thread::current_insn()
{
    rose_addr_t ip = readIP().known_value();

    /* Use the cached instruction if possible. */
    Disassembler::InstructionMap::iterator found = process->icache.find(ip);
    if (found!=process->icache.end()) {
        SgAsmx86Instruction *insn = isSgAsmx86Instruction(found->second);
        ROSE_ASSERT(insn!=NULL); /*shouldn't be possible due to check below*/
        size_t insn_sz = insn->get_raw_bytes().size();
        SgUnsignedCharList curmem(insn_sz);
        size_t nread = process->get_memory()->read(&curmem[0], ip, insn_sz);
        if (nread==insn_sz && curmem==insn->get_raw_bytes())
            return insn;
        process->icache.erase(found);
    }

    /* Disassemble (and cache) a new instruction */
    SgAsmx86Instruction *insn = NULL;
    try {
        insn = isSgAsmx86Instruction(disassembler->disassembleOne(process->get_memory(), ip));
    } catch (Disassembler::Exception &e) {
        std::cerr <<e <<"\n";
        process->dump_core(SIGSEGV);
        throw;
    }
    ROSE_ASSERT(insn!=NULL); /*only happens if our disassembler is not an x86 disassembler!*/
    process->icache.insert(std::make_pair(ip, insn));
    return insn;
}

void *
RSIM_Thread::my_addr(uint32_t va, size_t nbytes)
{
    /* Obtain mapping information and check that the specified number of bytes are mapped. */
    const MemoryMap::MapElement *me = process->get_memory()->find(va);
    if (!me)
        return NULL;
    size_t offset = 0;
    try {
        offset = me->get_va_offset(va, nbytes);
    } catch (const MemoryMap::NotMapped) {
        return NULL;
    }

    return (uint8_t*)me->get_base() + offset;
}

uint32_t
RSIM_Thread::guest_va(void *addr, size_t nbytes)
{
    const std::vector<MemoryMap::MapElement> elmts = process->get_memory()->get_elements();
    for (std::vector<MemoryMap::MapElement>::const_iterator ei=elmts.begin(); ei!=elmts.end(); ++ei) {
        uint8_t *base = (uint8_t*)ei->get_base(false);
        rose_addr_t offset = ei->get_offset();
        size_t size = ei->get_size();
        if (base && addr>=base+offset && (uint8_t*)addr+nbytes<=base+offset+size)
            return ei->get_va() + ((uint8_t*)addr - (base+offset));
    }
    return 0;
}

std::string
RSIM_Thread::read_string(uint32_t va, size_t limit/*=0*/, bool *error/*=NULL*/)
{
    std::string retval;
    while (1) {
        uint8_t byte;
        size_t nread = process->get_memory()->read(&byte, va++, 1);
        if (1!=nread) {
            if (error)
                *error = true;
            return retval;
        }
        if (!byte)
            break;
        retval += byte;

        if (limit>0 && retval.size()>=limit)
            break;
    }
    if (error)
        *error = false;
    return retval;
}

std::vector<std::string>
RSIM_Thread::read_string_vector(uint32_t va, bool *_error/*=NULL*/)
{
    bool had_error;
    bool *error = _error ? _error : &had_error;
    *error = false;

    std::vector<std::string> retval;
    for (/*void*/; 1; va+=4) {
        /* Read the pointer to the string */
        uint32_t ptr;
        if (sizeof(ptr) != process->get_memory()->read(&ptr, va, sizeof ptr)) {
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

void
RSIM_Thread::sys_semtimedop(uint32_t semid, uint32_t sops_va, uint32_t nsops, uint32_t timeout_va)
{
    static const Translate sem_flags[] = {
        TF(IPC_NOWAIT), TF(SEM_UNDO), T_END
    };

    if (nsops<1) {
        writeGPR(x86_gpr_ax, -EINVAL);
        return;
    }

    /* struct sembuf is the same on both 32- and 64-bit platforms */
    sembuf *sops = (sembuf*)my_addr(sops_va, nsops*sizeof(sembuf));
    if (!sops) {
        writeGPR(x86_gpr_ax, -EFAULT);
        return;
    }
    if (tracing(TRACE_SYSCALL)) {
        fprintf(tracing(TRACE_SYSCALL), " <continued...>\n");
        for (uint32_t i=0; i<nsops; i++) {
            fprintf(tracing(TRACE_SYSCALL), "    sops[%"PRIu32"] = { num=%"PRIu16", op=%"PRId16", flg=",
                    i, sops[i].sem_num, sops[i].sem_op);
            print_flags(tracing(TRACE_SYSCALL), sem_flags, sops[i].sem_flg);
            fprintf(tracing(TRACE_SYSCALL), " }\n");
        }
        fprintf(tracing(TRACE_SYSCALL), "%32s", "= ");
    }

    timespec host_timeout;
    if (timeout_va) {
        timespec_32 guest_timeout;
        if (sizeof(guest_timeout)!=process->get_memory()->read(&guest_timeout, timeout_va, sizeof guest_timeout)) {
            writeGPR(x86_gpr_ax, -EFAULT);
            return;
        }
        host_timeout.tv_sec = guest_timeout.tv_sec;
        host_timeout.tv_nsec = guest_timeout.tv_nsec;
    }

    int result = semtimedop(semid, sops, nsops, timeout_va?&host_timeout:NULL);
    writeGPR(x86_gpr_ax, -1==result?-errno:result);
}

void
RSIM_Thread::sys_semget(uint32_t key, uint32_t nsems, uint32_t semflg)
{
#ifdef SYS_ipc /* i686 */
    int result = syscall(SYS_ipc, 2, key, nsems, semflg);
#else
    int result = syscall(SYS_semget, key, nsems, semflg);
#endif
    writeGPR(x86_gpr_ax, -1==result?-errno:result);
}

void
RSIM_Thread::initialize_stack(SgAsmGenericHeader *_fhdr, int argc, char *argv[])
{
    /* We only handle ELF for now */
    SgAsmElfFileHeader *fhdr = isSgAsmElfFileHeader(_fhdr);
    ROSE_ASSERT(fhdr!=NULL);

    /* Allocate the stack */
    static const size_t stack_size = 0x00015000;
    size_t sp = readGPR(x86_gpr_sp).known_value();
    size_t stack_addr = sp - stack_size;
    MemoryMap::MapElement melmt(stack_addr, stack_size, MemoryMap::MM_PROT_READ|MemoryMap::MM_PROT_WRITE);
    melmt.set_name("[stack]");
    process->get_memory()->insert(melmt);

    /* Initialize the stack with specimen's argc and argv. Also save the arguments in the class. */
    ROSE_ASSERT(process->exeargs.size()==1);                     /* contains only the executable path */
    std::vector<uint32_t> pointers;                     /* pointers pushed onto stack at the end of initialization */
    pointers.push_back(argc);
    for (int i=0; i<argc; i++) {
        std::string arg;
        if (0==i) {
            arg = process->exeargs[0];
        } else {
            arg = argv[i];
            process->exeargs.push_back(arg);
        }
        size_t len = arg.size() + 1; /*inc. NUL termination*/
        sp -= len;
        process->get_memory()->write(arg.c_str(), sp, len);
        pointers.push_back(sp);
        if (tracing(TRACE_LOADER))
            fprintf(tracing(TRACE_LOADER), "argv[%d] %zu bytes at 0x%08zu = \"%s\"\n", i, len, sp, arg.c_str());
    }
    pointers.push_back(0); /*the argv NULL terminator*/

    /* Create new environment variables by stripping "X86SIM_" off the front of any environment variable and using that
     * value to override the non-X86SIM_ value, if any.  We try to make sure the variables are in the same order as if the
     * X86SIM_ overrides were not present. In other words, if X86SIM_FOO and FOO are both present, then X86SIM_FOO is deleted
     * from the list and its value used for FOO; but if X86SIM_FOO is present without FOO, then we just change the name to FOO
     * and leave it at that location. We do all this so that variables are in the same order whether run natively or under the
     * simulator. */
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
        process->get_memory()->write(env.c_str(), sp, env.size()+1);
        pointers.push_back(sp);
        if (tracing(TRACE_LOADER))
            fprintf(tracing(TRACE_LOADER), "environ[%d] %zu bytes at 0x%08zu = \"%s\"\n", j++, env.size(), sp, env.c_str());
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
        process->auxv.clear();

        if (process->vdso_mapped_va!=0) {
            /* AT_SYSINFO */
            process->auxv.push_back(32);
            process->auxv.push_back(process->vdso_entry_va);
            if (tracing(TRACE_LOADER))
                fprintf(tracing(TRACE_LOADER), "AT_SYSINFO:       0x%08"PRIx32"\n", process->auxv.back());

            /* AT_SYSINFO_PHDR */
            process->auxv.push_back(33);
            process->auxv.push_back(process->vdso_mapped_va);
            if (tracing(TRACE_LOADER))
                fprintf(tracing(TRACE_LOADER), "AT_SYSINFO_PHDR:  0x%08"PRIx32"\n", process->auxv.back());
        }

        /* AT_HWCAP (see linux <include/asm/cpufeature.h>). */
        process->auxv.push_back(16);
        uint32_t hwcap = 0xbfebfbfful; /* value used by hudson-rose-07 */
        process->auxv.push_back(hwcap);

        if (tracing(TRACE_LOADER))
            fprintf(tracing(TRACE_LOADER), "AT_HWCAP:         0x%08"PRIx32"\n", process->auxv.back());

        /* AT_PAGESZ */
        process->auxv.push_back(6);
        process->auxv.push_back(PAGE_SIZE);
        if (tracing(TRACE_LOADER))
            fprintf(tracing(TRACE_LOADER), "AT_PAGESZ:        %"PRId32"\n", process->auxv.back());

        /* AT_CLKTCK */
        process->auxv.push_back(17);
        process->auxv.push_back(100);
        if (tracing(TRACE_LOADER))
            fprintf(tracing(TRACE_LOADER), "AT_CLKTCK:        %"PRId32"\n", process->auxv.back());

        /* AT_PHDR */
        process->auxv.push_back(3); /*AT_PHDR*/
        process->auxv.push_back(t1.phdr_rva + fhdr->get_base_va());
        if (tracing(TRACE_LOADER))
            fprintf(tracing(TRACE_LOADER), "AT_PHDR:          0x%08"PRIx32"\n", process->auxv.back());

        /*AT_PHENT*/
        process->auxv.push_back(4);
        process->auxv.push_back(fhdr->get_phextrasz() + sizeof(SgAsmElfSegmentTableEntry::Elf32SegmentTableEntry_disk));
        if (tracing(TRACE_LOADER))
            fprintf(tracing(TRACE_LOADER), "AT_PHENT:         %"PRId32"\n", process->auxv.back());

        /* AT_PHNUM */
        process->auxv.push_back(5);
        process->auxv.push_back(fhdr->get_e_phnum());
        if (tracing(TRACE_LOADER))
            fprintf(tracing(TRACE_LOADER), "AT_PHNUM:         %"PRId32"\n", process->auxv.back());

        /* AT_BASE */
        process->auxv.push_back(7);
        process->auxv.push_back(process->ld_linux_base_va);
        if (tracing(TRACE_LOADER))
            fprintf(tracing(TRACE_LOADER), "AT_BASE:          0x%08"PRIx32"\n", process->auxv.back());

        /* AT_FLAGS */
        process->auxv.push_back(8);
        process->auxv.push_back(0);
        if (tracing(TRACE_LOADER))
            fprintf(tracing(TRACE_LOADER), "AT_FLAGS:         0x%08"PRIx32"\n", process->auxv.back());

        /* AT_ENTRY */
        process->auxv.push_back(9);
        process->auxv.push_back(fhdr->get_entry_rva() + fhdr->get_base_va());
        if (tracing(TRACE_LOADER))
            fprintf(tracing(TRACE_LOADER), "AT_ENTRY:         0x%08"PRIx32"\n", process->auxv.back());

        /* AT_UID */
        process->auxv.push_back(11);
        process->auxv.push_back(getuid());
        if (tracing(TRACE_LOADER))
            fprintf(tracing(TRACE_LOADER), "AT_UID:           %"PRId32"\n", process->auxv.back());

        /* AT_EUID */
        process->auxv.push_back(12);
        process->auxv.push_back(geteuid());
        if (tracing(TRACE_LOADER))
            fprintf(tracing(TRACE_LOADER), "AT_EUID:          %"PRId32"\n", process->auxv.back());

        /* AT_GID */
        process->auxv.push_back(13);
        process->auxv.push_back(getgid());
        if (tracing(TRACE_LOADER))
            fprintf(tracing(TRACE_LOADER), "AT_GID:           %"PRId32"\n", process->auxv.back());

        /* AT_EGID */
        process->auxv.push_back(14);
        process->auxv.push_back(getegid());
        if (tracing(TRACE_LOADER))
            fprintf(tracing(TRACE_LOADER), "AT_EGID:          %"PRId32"\n", process->auxv.back());

        /* AT_SECURE */
        process->auxv.push_back(23);
        process->auxv.push_back(false);
        if (tracing(TRACE_LOADER))
            fprintf(tracing(TRACE_LOADER), "AT_SECURE:        %"PRId32"\n", process->auxv.back());

        /* AT_PLATFORM */
        {
            const char *platform = "i686";
            size_t len = strlen(platform)+1;
            sp -= len;
            process->get_memory()->write(platform, sp, len);
            process->auxv.push_back(15);
            process->auxv.push_back(sp);
            if (tracing(TRACE_LOADER))
                fprintf(tracing(TRACE_LOADER), "AT_PLATFORM:      0x%08"PRIx32" (%s)\n", process->auxv.back(), platform);
        }
    }

    /* AT_NULL */
    process->auxv.push_back(0);
    process->auxv.push_back(0);
    pointers.insert(pointers.end(), process->auxv.begin(), process->auxv.end());

    /* Finalize stack initialization by writing all the pointers to data we've pushed:
     *    argc
     *    argv with NULL terminator
     *    environment with NULL terminator
     *    auxv pairs terminated with (AT_NULL,0)
     */
    sp &= ~3U; /*align to four-bytes*/
    sp -= 4 * pointers.size();
    process->get_memory()->write(&(pointers[0]), sp, 4*pointers.size());

    writeGPR(x86_gpr_sp, sp);
}

void
RSIM_Thread::sys_semctl(uint32_t semid, uint32_t semnum, uint32_t cmd, uint32_t semun_va)
{
    int version = cmd & 0x0100/*IPC_64*/;
    cmd &= ~0x0100;

    ROSE_ASSERT(version!=0);

    union semun_32 {
        uint32_t val;
        uint32_t ptr;
    };

    union semun_native {
        int val;
        void *ptr;
    };

    semun_32 guest_semun;
    if (sizeof(guest_semun)!=process->get_memory()->read(&guest_semun, semun_va, sizeof guest_semun)) {
        writeGPR(x86_gpr_ax, -EFAULT);
        return;
    }
    

    switch (cmd) {
        case 3:         /* IPC_INFO */
        case 19: {      /* SEM_INFO */
            seminfo host_seminfo;
#ifdef SYS_ipc /* i686 */
            ROSE_ASSERT(version!=0);
            semun_native host_semun;
            host_semun.ptr = &host_seminfo;
            int result = syscall(SYS_ipc, 3/*SEMCTL*/, semid, semnum, cmd|version, &host_semun);
#else
            int result = syscall(SYS_semctl, semid, semnum, cmd, &host_seminfo);
#endif
            if (-1==result) {
                writeGPR(x86_gpr_ax, -errno);
                return;
            }

            seminfo_32 guest_seminfo;
            guest_seminfo.semmap = host_seminfo.semmap;
            guest_seminfo.semmni = host_seminfo.semmni;
            guest_seminfo.semmns = host_seminfo.semmns;
            guest_seminfo.semmnu = host_seminfo.semmnu;
            guest_seminfo.semmsl = host_seminfo.semmsl;
            guest_seminfo.semopm = host_seminfo.semopm;
            guest_seminfo.semume = host_seminfo.semume;
            guest_seminfo.semusz = host_seminfo.semusz;
            guest_seminfo.semvmx = host_seminfo.semvmx;
            guest_seminfo.semaem = host_seminfo.semaem;
            if (sizeof(guest_seminfo)!=process->get_memory()->write(&guest_seminfo, guest_semun.ptr, sizeof guest_seminfo)) {
                writeGPR(x86_gpr_ax, -EFAULT);
                return;
            }

            writeGPR(x86_gpr_ax, result);
            break;
        }

        case 2:         /* IPC_STAT */
        case 18: {      /* SEM_STAT */
            semid_ds host_ds;
#ifdef SYS_ipc /* i686 */
            ROSE_ASSERT(version!=0);
            semun_native host_semun;
            host_semun.ptr = &host_ds;
            int result = syscall(SYS_ipc, 3/*SEMCTL*/, semid, semnum, cmd|version, &host_semun);
#else
            int result = syscall(SYS_semctl, semid, semnum, cmd, &host_ds);
#endif
            if (-1==result) {
                writeGPR(x86_gpr_ax, -errno);
                return;
            }

            semid64_ds_32 guest_ds;
            guest_ds.sem_perm.key = host_ds.sem_perm.__key;
            guest_ds.sem_perm.uid = host_ds.sem_perm.uid;
            guest_ds.sem_perm.gid = host_ds.sem_perm.gid;
            guest_ds.sem_perm.cuid = host_ds.sem_perm.cuid;
            guest_ds.sem_perm.cgid = host_ds.sem_perm.cgid;
            guest_ds.sem_perm.mode = host_ds.sem_perm.mode;
            guest_ds.sem_perm.pad1 = host_ds.sem_perm.__pad1;
            guest_ds.sem_perm.seq = host_ds.sem_perm.__seq;
            guest_ds.sem_perm.pad2 = host_ds.sem_perm.__pad2;
            guest_ds.sem_perm.unused1 = host_ds.sem_perm.__unused1;
            guest_ds.sem_perm.unused2 = host_ds.sem_perm.__unused1;
            guest_ds.sem_otime = host_ds.sem_otime;
            guest_ds.unused1 = host_ds.__unused1;
            guest_ds.sem_ctime = host_ds.sem_ctime;
            guest_ds.unused2 = host_ds.__unused2;
            guest_ds.sem_nsems = host_ds.sem_nsems;
            guest_ds.unused3 = host_ds.__unused3;
            guest_ds.unused4 = host_ds.__unused4;
            if (sizeof(guest_ds)!=process->get_memory()->write(&guest_ds, guest_semun.ptr, sizeof guest_ds)) {
                writeGPR(x86_gpr_ax, -EFAULT);
                return;
            }
                        
            writeGPR(x86_gpr_ax, result);
            break;
        };

        case 1: {       /* IPC_SET */
            semid64_ds_32 guest_ds;
            if (sizeof(guest_ds)!=process->get_memory()->read(&guest_ds, guest_semun.ptr, sizeof(guest_ds))) {
                writeGPR(x86_gpr_ax, -EFAULT);
                return;
            }
#ifdef SYS_ipc  /* i686 */
            ROSE_ASSERT(version!=0);
            semun_native semun;
            semun.ptr = &guest_ds;
            int result = syscall(SYS_ipc, 3/*SEMCTL*/, semid, semnum, cmd|version, &semun);
#else           /* amd64 */
            semid_ds host_ds;
            host_ds.sem_perm.__key = guest_ds.sem_perm.key;
            host_ds.sem_perm.uid = guest_ds.sem_perm.uid;
            host_ds.sem_perm.gid = guest_ds.sem_perm.gid;
            host_ds.sem_perm.cuid = guest_ds.sem_perm.cuid;
            host_ds.sem_perm.cgid = guest_ds.sem_perm.cgid;
            host_ds.sem_perm.mode = guest_ds.sem_perm.mode;
            host_ds.sem_perm.__pad1 = guest_ds.sem_perm.pad1;
            host_ds.sem_perm.__seq = guest_ds.sem_perm.seq;
            host_ds.sem_perm.__pad2 = guest_ds.sem_perm.pad2;
            host_ds.sem_perm.__unused1 = guest_ds.sem_perm.unused1;
            host_ds.sem_perm.__unused1 = guest_ds.sem_perm.unused2;
            host_ds.sem_otime = guest_ds.sem_otime;
            host_ds.__unused1 = guest_ds.unused1;
            host_ds.sem_ctime = guest_ds.sem_ctime;
            host_ds.__unused2 = guest_ds.unused2;
            host_ds.sem_nsems = guest_ds.sem_nsems;
            host_ds.__unused3 = guest_ds.unused3;
            host_ds.__unused4 = guest_ds.unused4;
            int result = syscall(SYS_semctl, semid, semnum, cmd, &host_ds);
#endif
            writeGPR(x86_gpr_ax, -1==result?-errno:result);
            break;
        }

        case 13: {      /* GETALL */
            semid_ds host_ds;
            int result = semctl(semid, -1, IPC_STAT, &host_ds);
            if (-1==result) {
                writeGPR(x86_gpr_ax, -errno);
                return;
            }
            if (host_ds.sem_nsems<1) {
                writeGPR(x86_gpr_ax, -EINVAL);
                return;
            }
            size_t nbytes = 2 * host_ds.sem_nsems;
            if (NULL==my_addr(guest_semun.ptr, nbytes)) {
                writeGPR(x86_gpr_ax, -EFAULT);
                return;
            }
            uint16_t *sem_values = new uint16_t[host_ds.sem_nsems];
#ifdef SYS_ipc  /* i686 */
            semun_native semun;
            semun.ptr = sem_values;
            result = syscall(SYS_ipc, 3/*SEMCTL*/, semid, semnum, cmd|version, &semun);
#else
            result = syscall(SYS_semctl, semid, semnum, cmd, sem_values);
#endif
            if (-1==result) {
                delete[] sem_values;
                writeGPR(x86_gpr_ax, -errno);
                return;
            }
            if (nbytes!=process->get_memory()->write(sem_values, guest_semun.ptr, nbytes)) {
                delete[] sem_values;
                writeGPR(x86_gpr_ax, -EFAULT);
                return;
            }
            if (tracing(TRACE_SYSCALL)) {
                fprintf(tracing(TRACE_SYSCALL), "<continued...>\n");
                for (size_t i=0; i<host_ds.sem_nsems; i++) {
                    fprintf(tracing(TRACE_SYSCALL), "    value[%zu] = %"PRId16"\n", i, sem_values[i]);
                }
                fprintf(tracing(TRACE_SYSCALL), "%32s", "= ");
            }
            delete[] sem_values;
            writeGPR(x86_gpr_ax, result);
            break;
        }
            
        case 17: {      /* SETALL */
            semid_ds host_ds;
            int result = semctl(semid, -1, IPC_STAT, &host_ds);
            if (-1==result) {
                writeGPR(x86_gpr_ax, -errno);
                return;
            }
            if (host_ds.sem_nsems<1) {
                writeGPR(x86_gpr_ax, -EINVAL);
                return;
            }
            uint16_t *sem_values = new uint16_t[host_ds.sem_nsems];
            size_t nbytes = 2 * host_ds.sem_nsems;
            if (nbytes!=process->get_memory()->read(sem_values, guest_semun.ptr, nbytes)) {
                delete[] sem_values;
                writeGPR(x86_gpr_ax, -EFAULT);
                return;
            }
            if (tracing(TRACE_SYSCALL)) {
                fprintf(tracing(TRACE_SYSCALL), "<continued...>\n");
                for (size_t i=0; i<host_ds.sem_nsems; i++) {
                    fprintf(tracing(TRACE_SYSCALL), "    value[%zu] = %"PRId16"\n", i, sem_values[i]);
                }
                fprintf(tracing(TRACE_SYSCALL), "%32s", "= ");
            }
#ifdef SYS_ipc  /* i686 */
            semun_native semun;
            semun.ptr = sem_values;
            result = syscall(SYS_ipc, 3/*SEMCTL*/, semid, semnum, cmd|version, &semun);
#else
            result = syscall(SYS_semctl, semid, semnum, cmd, sem_values);
#endif
            writeGPR(x86_gpr_ax, -1==result?-errno:result);
            delete[] sem_values;
            break;
        }

        case 11:        /* GETPID */
        case 12:        /* GETVAL */
        case 15:        /* GETZCNT */
        case 14: {      /* GETNCNT */
            int result = semctl(semid, semnum, cmd, NULL);
            writeGPR(x86_gpr_ax, -1==result?-errno:result);
            break;
        }

        case 16: {      /* SETVAL */
#ifdef SYS_ipc  /* i686 */
            int result = syscall(SYS_ipc, 3/*SEMCTL*/, semid, semnum, cmd|version, &guest_semun);
#else
            int result = syscall(SYS_semctl, semid, semnum, cmd, guest_semun.val);
#endif
            writeGPR(x86_gpr_ax, -1==result?-errno:result);
            break;
        }

        case 0: {       /* IPC_RMID */
#ifdef SYS_ipc /* i686 */
            semun_native host_semun;
            memset(&host_semun, 0, sizeof host_semun);
            int result = syscall(SYS_ipc, 3/*SEMCTL*/, semid, semnum, cmd|version, &host_semun);
#else
            int result = semctl(semid, semnum, cmd, NULL);
#endif
            writeGPR(x86_gpr_ax, -1==result?-errno:result);
            break;
        }

        default:
            writeGPR(x86_gpr_ax, -EINVAL);
            return;
    }
}


void
RSIM_Thread::sys_msgsnd(uint32_t msqid, uint32_t msgp_va, uint32_t msgsz, uint32_t msgflg)
{
    if (msgsz>65535) { /* 65535 >= MSGMAX; smaller limit errors are detected in actual syscall */
        writeGPR(x86_gpr_ax, -EINVAL);
        return;
    }

    /* Read the message buffer from the specimen. */
    uint8_t *buf = new uint8_t[msgsz+8]; /* msgsz does not include "long mtype", only "char mtext[]" */
    if (!buf) {
        writeGPR(x86_gpr_ax, -ENOMEM);
        return;
    }
    if (4+msgsz!=process->get_memory()->read(buf, msgp_va, 4+msgsz)) {
        delete[] buf;
        writeGPR(x86_gpr_ax, -EFAULT);
        return;
    }

    /* Message type must be positive */
    if (*(int32_t*)buf <= 0) {
        delete[] buf;
        writeGPR(x86_gpr_ax, -EINVAL);
        return;
    }

    /* Convert message type from four to eight bytes if necessary */
    if (4!=sizeof(long)) {
        ROSE_ASSERT(8==sizeof(long));
        memmove(buf+8, buf+4, msgsz);
        memset(buf+4, 0, 4);
    }

    /* Try to send the message */
    int result = msgsnd(msqid, buf, msgsz, msgflg);
    if (-1==result) {
        delete[] buf;
        writeGPR(x86_gpr_ax, -errno);
        return;
    }

    delete[] buf;
    writeGPR(x86_gpr_ax, result);
}

void
RSIM_Thread::sys_msgrcv(uint32_t msqid, uint32_t msgp_va, uint32_t msgsz, uint32_t msgtyp, uint32_t msgflg)
{
    if (msgsz>65535) { /* 65535 >= MSGMAX; smaller limit errors are detected in actual syscall */
        writeGPR(x86_gpr_ax, -EINVAL);
        return;
    }

    uint8_t *buf = new uint8_t[msgsz+8]; /* msgsz does not include "long mtype", only "char mtext[]" */
    int result = msgrcv(msqid, buf, msgsz, msgtyp, msgflg);
    if (-1==result) {
        delete[] buf;
        writeGPR(x86_gpr_ax, -errno);
        return;
    }

    if (4!=sizeof(long)) {
        ROSE_ASSERT(8==sizeof(long));
        uint64_t type = *(uint64_t*)buf;
        ROSE_ASSERT(0 == (type >> 32));
        memmove(buf+4, buf+8, msgsz);
    }

    if (4+msgsz!=process->get_memory()->write(buf, msgp_va, 4+msgsz)) {
        delete[] buf;
        writeGPR(x86_gpr_ax, -EFAULT);
        return;
    }

    delete[] buf;
    writeGPR(x86_gpr_ax, result);
}

void
RSIM_Thread::sys_msgget(uint32_t key, uint32_t msgflg)
{
    int result = msgget(key, msgflg);
    writeGPR(x86_gpr_ax, -1==result?-errno:result);
}

void
RSIM_Thread::sys_msgctl(uint32_t msqid, uint32_t cmd, uint32_t buf_va)
{
    int version = cmd & 0x0100/*IPC_64*/;
    cmd &= ~0x0100;

    switch (cmd) {
        case 3:    /* IPC_INFO */
        case 12: { /* MSG_INFO */
            writeGPR(x86_gpr_ax, -ENOSYS);              /* FIXME */
            break;
        }

        case 2:    /* IPC_STAT */
        case 11: { /* MSG_STAT */
            ROSE_ASSERT(0x0100==version); /* we're assuming ipc64_perm and msqid_ds from the kernel */
            static msqid_ds host_ds;
            int result = msgctl(msqid, cmd, &host_ds);
            if (-1==result) {
                writeGPR(x86_gpr_ax, -errno);
                break;
            }

            msqid64_ds_32 guest_ds;
            guest_ds.msg_perm.key = host_ds.msg_perm.__key;
            guest_ds.msg_perm.uid = host_ds.msg_perm.uid;
            guest_ds.msg_perm.gid = host_ds.msg_perm.gid;
            guest_ds.msg_perm.cuid = host_ds.msg_perm.cuid;
            guest_ds.msg_perm.cgid = host_ds.msg_perm.cgid;
            guest_ds.msg_perm.mode = host_ds.msg_perm.mode;
            guest_ds.msg_perm.pad1 = host_ds.msg_perm.__pad1;
            guest_ds.msg_perm.seq = host_ds.msg_perm.__seq;
            guest_ds.msg_perm.pad2 = host_ds.msg_perm.__pad2;
            guest_ds.msg_perm.unused1 = host_ds.msg_perm.__unused1;
            guest_ds.msg_perm.unused2 = host_ds.msg_perm.__unused2;
            guest_ds.msg_stime = host_ds.msg_stime;
#if 4==SIZEOF_LONG
            guest_ds.unused1 = host_ds.__unused1;
#endif
            guest_ds.msg_rtime = host_ds.msg_rtime;
#if 4==SIZEOF_LONG
            guest_ds.unused2 = host_ds.__unused2;
#endif
            guest_ds.msg_ctime = host_ds.msg_ctime;
#if 4==SIZEOF_LONG
            guest_ds.unused3 = host_ds.__unused3;
#endif
            guest_ds.msg_cbytes = host_ds.__msg_cbytes;
            guest_ds.msg_qnum = host_ds.msg_qnum;
            guest_ds.msg_qbytes = host_ds.msg_qbytes;
            guest_ds.msg_lspid = host_ds.msg_lspid;
            guest_ds.msg_lrpid = host_ds.msg_lrpid;
            guest_ds.unused4 = host_ds.__unused4;
            guest_ds.unused5 = host_ds.__unused5;

            if (sizeof(guest_ds)!=process->get_memory()->write(&guest_ds, buf_va, sizeof guest_ds)) {
                writeGPR(x86_gpr_ax, -EFAULT);
                break;
            }

            writeGPR(x86_gpr_ax, result);
            break;
        }

        case 0: { /* IPC_RMID */
            /* NOTE: syscall tracing will not show "IPC_RMID" if the IPC_64 flag is also present */
            int result = msgctl(msqid, cmd, NULL);
            writeGPR(x86_gpr_ax, -1==result?-errno:result);
            break;
        }

        case 1: { /* IPC_SET */
            msqid64_ds_32 guest_ds;
            if (sizeof(guest_ds)!=process->get_memory()->read(&guest_ds, buf_va, sizeof guest_ds)) {
                writeGPR(x86_gpr_ax, -EFAULT);
                break;
            }

            static msqid_ds host_ds;
            host_ds.msg_perm.__key = guest_ds.msg_perm.key;
            host_ds.msg_perm.uid = guest_ds.msg_perm.uid;
            host_ds.msg_perm.gid = guest_ds.msg_perm.gid;
            host_ds.msg_perm.cuid = guest_ds.msg_perm.cuid;
            host_ds.msg_perm.cgid = guest_ds.msg_perm.cgid;
            host_ds.msg_perm.mode = guest_ds.msg_perm.mode;
            host_ds.msg_perm.__seq = guest_ds.msg_perm.seq;
            host_ds.msg_stime = guest_ds.msg_stime;
            host_ds.msg_rtime = guest_ds.msg_rtime;
            host_ds.msg_ctime = guest_ds.msg_ctime;
            host_ds.__msg_cbytes = guest_ds.msg_cbytes;
            host_ds.msg_qnum = guest_ds.msg_qnum;
            host_ds.msg_qbytes = guest_ds.msg_qbytes;
            host_ds.msg_lspid = guest_ds.msg_lspid;
            host_ds.msg_lrpid = guest_ds.msg_lrpid;

            int result = msgctl(msqid, cmd, &host_ds);
            writeGPR(x86_gpr_ax, -1==result?-errno:result);
            break;
        }

        default: {
            writeGPR(x86_gpr_ax, -EINVAL);
            break;
        }
    }
}

void
RSIM_Thread::sys_shmdt(uint32_t shmaddr_va)
{
    const MemoryMap::MapElement *me = process->get_memory()->find(shmaddr_va);
    if (!me || me->get_va()!=shmaddr_va || me->get_offset()!=0 || me->is_anonymous()) {
        writeGPR(x86_gpr_ax, -EINVAL);
        return;
    }

    int result = shmdt(me->get_base());
    if (-1==result) {
        writeGPR(x86_gpr_ax, -errno);
        return;
    }

    process->get_memory()->erase(*me);
    writeGPR(x86_gpr_ax, result);
}

void
RSIM_Thread::sys_shmget(uint32_t key, uint32_t size, uint32_t shmflg)
{
    int result = shmget(key, size, shmflg);
    writeGPR(x86_gpr_ax, -1==result?-errno:result);
}

void
RSIM_Thread::sys_shmctl(uint32_t shmid, uint32_t cmd, uint32_t buf_va)
{
    int version = cmd & 0x0100/*IPC_64*/;
    cmd &= ~0x0100;

    switch (cmd) {
        case 13:  /* SHM_STAT */
        case 2: { /* IPC_STAT */
            ROSE_ASSERT(0x0100==version); /* we're assuming ipc64_perm and shmid_ds from the kernel */
            static shmid_ds host_ds;
            int result = shmctl(shmid, cmd, &host_ds);
            if (-1==result) {
                writeGPR(x86_gpr_ax, -errno);
                break;
            }

            shmid64_ds_32 guest_ds;
            guest_ds.shm_perm.key = host_ds.shm_perm.__key;
            guest_ds.shm_perm.uid = host_ds.shm_perm.uid;
            guest_ds.shm_perm.gid = host_ds.shm_perm.gid;
            guest_ds.shm_perm.cuid = host_ds.shm_perm.cuid;
            guest_ds.shm_perm.cgid = host_ds.shm_perm.cgid;
            guest_ds.shm_perm.mode = host_ds.shm_perm.mode;
            guest_ds.shm_perm.pad1 = host_ds.shm_perm.__pad1;
            guest_ds.shm_perm.seq = host_ds.shm_perm.__seq;
            guest_ds.shm_perm.pad2 = host_ds.shm_perm.__pad2;
            guest_ds.shm_perm.unused1 = host_ds.shm_perm.__unused1;
            guest_ds.shm_perm.unused2 = host_ds.shm_perm.__unused2;
            guest_ds.shm_segsz = host_ds.shm_segsz;
            guest_ds.shm_atime = host_ds.shm_atime;
#if 4==SIZEOF_LONG
            guest_ds.unused1 = host_ds.__unused1;
#endif
            guest_ds.shm_dtime = host_ds.shm_dtime;
#if 4==SIZEOF_LONG
            guest_ds.unused2 = host_ds.__unused2;
#endif
            guest_ds.shm_ctime = host_ds.shm_ctime;
#if 4==SIZEOF_LONG
            guest_ds.unused3 = host_ds.__unused3;
#endif
            guest_ds.shm_cpid = host_ds.shm_cpid;
            guest_ds.shm_lpid = host_ds.shm_lpid;
            guest_ds.shm_nattch = host_ds.shm_nattch;
            guest_ds.unused4 = host_ds.__unused4;
            guest_ds.unused5 = host_ds.__unused5;

            if (sizeof(guest_ds)!=process->get_memory()->write(&guest_ds, buf_va, sizeof guest_ds)) {
                writeGPR(x86_gpr_ax, -EFAULT);
                break;
            }

            writeGPR(x86_gpr_ax, result);
            break;
        }

        case 14: { /* SHM_INFO */
            shm_info host_info;
            int result = shmctl(shmid, cmd, (shmid_ds*)&host_info);
            if (-1==result) {
                writeGPR(x86_gpr_ax, -errno);
                break;
            }

            shm_info_32 guest_info;
            guest_info.used_ids = host_info.used_ids;
            guest_info.shm_tot = host_info.shm_tot;
            guest_info.shm_rss = host_info.shm_rss;
            guest_info.shm_swp = host_info.shm_swp;
            guest_info.swap_attempts = host_info.swap_attempts;
            guest_info.swap_successes = host_info.swap_successes;

            if (sizeof(guest_info)!=process->get_memory()->write(&guest_info, buf_va, sizeof guest_info)) {
                writeGPR(x86_gpr_ax, -EFAULT);
                break;
            }

            writeGPR(x86_gpr_ax, result);
            break;
        }

        case 3: { /* IPC_INFO */
            shminfo64_native host_info;
            int result = shmctl(shmid, cmd, (shmid_ds*)&host_info);
            if (-1==result) {
                writeGPR(x86_gpr_ax, -errno);
                return;
            }

            shminfo64_32 guest_info;
            guest_info.shmmax = host_info.shmmax;
            guest_info.shmmin = host_info.shmmin;
            guest_info.shmmni = host_info.shmmni;
            guest_info.shmseg = host_info.shmseg;
            guest_info.shmall = host_info.shmall;
            guest_info.unused1 = host_info.unused1;
            guest_info.unused2 = host_info.unused2;
            guest_info.unused3 = host_info.unused3;
            guest_info.unused4 = host_info.unused4;
            if (sizeof(guest_info)!=process->get_memory()->write(&guest_info, buf_va, sizeof guest_info)) {
                writeGPR(x86_gpr_ax, -EFAULT);
                return;
            }

            writeGPR(x86_gpr_ax, result);
            break;
        }

        case 11:   /* SHM_LOCK */
        case 12: { /* SHM_UNLOCK */
            int result = shmctl(shmid, cmd, NULL);
            writeGPR(x86_gpr_ax, -1==result?-errno:result);
            break;
        }

        case 1: { /* IPC_SET */
            ROSE_ASSERT(version!=0);
            shmid64_ds_32 guest_ds;
            if (sizeof(guest_ds)!=process->get_memory()->read(&guest_ds, buf_va, sizeof guest_ds)) {
                writeGPR(x86_gpr_ax, -EFAULT);
                return;
            }
            shmid_ds host_ds;
            host_ds.shm_perm.__key = guest_ds.shm_perm.key;
            host_ds.shm_perm.uid = guest_ds.shm_perm.uid;
            host_ds.shm_perm.gid = guest_ds.shm_perm.gid;
            host_ds.shm_perm.cuid = guest_ds.shm_perm.cuid;
            host_ds.shm_perm.cgid = guest_ds.shm_perm.cgid;
            host_ds.shm_perm.mode = guest_ds.shm_perm.mode;
            host_ds.shm_perm.__pad1 = guest_ds.shm_perm.pad1;
            host_ds.shm_perm.__seq = guest_ds.shm_perm.seq;
            host_ds.shm_perm.__pad2 = guest_ds.shm_perm.pad2;
            host_ds.shm_perm.__unused1 = guest_ds.shm_perm.unused1;
            host_ds.shm_perm.__unused2 = guest_ds.shm_perm.unused2;
            host_ds.shm_segsz = guest_ds.shm_segsz;
            host_ds.shm_atime = guest_ds.shm_atime;
#if 4==SIZEOF_LONG
            host_ds.__unused1 = guest_ds.unused1;
#endif
            host_ds.shm_dtime = guest_ds.shm_dtime;
#if 4==SIZEOF_LONG
            host_ds.__unused2 = guest_ds.unused2;
#endif
            host_ds.shm_ctime = guest_ds.shm_ctime;
#if 4==SIZEOF_LONG
            host_ds.__unused3 = guest_ds.unused3;
#endif
            host_ds.shm_cpid = guest_ds.shm_cpid;
            host_ds.shm_lpid = guest_ds.shm_lpid;
            host_ds.shm_nattch = guest_ds.shm_nattch;
            host_ds.__unused4 = guest_ds.unused4;
            host_ds.__unused5 = guest_ds.unused5;

            int result = shmctl(shmid, cmd, &host_ds);
            writeGPR(x86_gpr_ax, -1==result?-errno:result);
            break;
        }

        case 0: { /* IPC_RMID */
            int result = shmctl(shmid, cmd, NULL);
            writeGPR(x86_gpr_ax, -1==result?-errno:result);
            break;
        }

        default: {
            writeGPR(x86_gpr_ax, -EINVAL);
            break;
        }
    }
}

void
RSIM_Thread::sys_shmat(uint32_t shmid, uint32_t shmflg, uint32_t result_va, uint32_t shmaddr)
{
    if (0==shmaddr) {
        shmaddr = process->get_memory()->find_last_free();
    } else if (shmflg & SHM_RND) {
        shmaddr = ALIGN_DN(shmaddr, SHMLBA);
    } else if (ALIGN_DN(shmaddr, 4096)!=shmaddr) {
        writeGPR(x86_gpr_ax, -EINVAL);
        return;
    }

    /* We don't handle SHM_REMAP */
    if (shmflg & SHM_REMAP) {
        writeGPR(x86_gpr_ax, -EINVAL);
        return;
    }

    /* Map shared memory into the simulator */
    void *buf = shmat(shmid, NULL, shmflg);
    if (!buf) {
        writeGPR(x86_gpr_ax, -errno);
        return;
    }

    /* Map simulator's shared memory into the specimen */
    shmid_ds ds;
    int status = shmctl(shmid, IPC_STAT, &ds);
    ROSE_ASSERT(status>=0);
    ROSE_ASSERT(ds.shm_segsz>0);
    unsigned perms = MemoryMap::MM_PROT_READ | ((shmflg & SHM_RDONLY) ? 0 : MemoryMap::MM_PROT_WRITE);
    MemoryMap::MapElement shm(shmaddr, ds.shm_segsz, buf, 0, perms);
    shm.set_name("shmat("+StringUtility::numberToString(shmid)+")");
    process->get_memory()->insert(shm);

    /* Return values */
    if (4!=process->get_memory()->write(&shmaddr, result_va, 4)) {
        writeGPR(x86_gpr_ax, -EFAULT);
        return;
    }
    writeGPR(x86_gpr_ax, shmaddr);
}

void
RSIM_Thread::sys_socket(int family, int type, int protocol)
{
#ifdef SYS_socketcall /* i686 */
    ROSE_ASSERT(4==sizeof(int));
    int a[3];
    a[0] = family;
    a[1] = type;
    a[2] = protocol;
    int result = syscall(SYS_socketcall, 1/*SYS_SOCKET*/, a);
#else /* amd64 */
    int result = syscall(SYS_socket, family, type, protocol);
#endif
    writeGPR(x86_gpr_ax, -1==result?-errno:result);
}

void
RSIM_Thread::sys_bind(int fd, uint32_t addr_va, uint32_t addrlen)
{
    if (addrlen<1 || addrlen>4096) {
        writeGPR(x86_gpr_ax, -EINVAL);
        return;
    }
    uint8_t *addrbuf = new uint8_t[addrlen];
    if (addrlen!=process->get_memory()->read(addrbuf, addr_va, addrlen)) {
        writeGPR(x86_gpr_ax, -EFAULT);
        delete[] addrbuf;
        return;
    }

#ifdef SYS_socketcall /* i686 */
    ROSE_ASSERT(4==sizeof(int));
    ROSE_ASSERT(4==sizeof(void*));
    int a[3];
    a[0] = fd;
    a[1] = (int)addrbuf;
    a[2] = addrlen;
    int result = syscall(SYS_socketcall, 2/*SYS_BIND*/, a);
#else /* amd64 */
    int result = syscall(SYS_bind, fd, addrbuf, addrlen);
#endif
    writeGPR(x86_gpr_ax, -1==result?-errno:result);
    delete[] addrbuf;
}

void
RSIM_Thread::sys_listen(int fd, int backlog)
{
#ifdef SYS_socketcall /* i686 */
    ROSE_ASSERT(4==sizeof(int));
    int a[2];
    a[0] = fd;
    a[1] = backlog;
    int result = syscall(SYS_socketcall, 4/*SYS_LISTEN*/, a);
#else /* amd64 */
    int result = syscall(SYS_listen, fd, backlog);
#endif
    writeGPR(x86_gpr_ax, -1==result?-errno:result);
}

void
RSIM_Thread::syscall_arginfo(char format, uint32_t val, ArgInfo *info, va_list *ap)
{
    ROSE_ASSERT(info!=NULL);
    info->val = val;
    switch (format) {
        case 'f':       /*flags*/
        case 'e':       /*enum*/
            info->xlate = va_arg(*ap, const Translate*);
            break;
        case 's': {     /*NUL-terminated string*/
            info->str = read_string(val, 4096, &(info->str_fault));
            info->str_trunc = (info->str.size() >= 4096);
            break;
        }
        case 'b': {     /* buffer */
            size_t advertised_size = va_arg(*ap, size_t);
            info->struct_buf = new uint8_t[advertised_size];
            info->struct_nread = process->get_memory()->read(info->struct_buf, info->val, advertised_size);
            info->struct_size = 64; /* max print width, measured in columns of output */
            break;
        }
        case 'P': {     /*ptr to a struct*/
            info->struct_size = va_arg(*ap, size_t);
            info->struct_printer = va_arg(*ap, ArgInfo::StructPrinter);
            info->struct_buf = new uint8_t[info->struct_size];
            info->struct_nread = process->get_memory()->read(info->struct_buf, info->val, info->struct_size);
            break;
        }
    }
}

void
RSIM_Thread::syscall_enterv(uint32_t *values, const char *name, const char *format, va_list *app)
{
    static timeval first_call;

    if (tracing(TRACE_SYSCALL)) {
        timeval this_call;
        gettimeofday(&this_call, NULL);
        if (0==first_call.tv_sec)
            first_call = this_call;
        double elapsed = (this_call.tv_sec - first_call.tv_sec) + (this_call.tv_usec - first_call.tv_usec)/1e6;
        fprintf(tracing(TRACE_SYSCALL), "[pid %d] 0x%08"PRIx64" %8.4f: ", getpid(), readIP().known_value(), elapsed);
        ArgInfo args[6];
        for (size_t i=0; format[i]; i++)
            syscall_arginfo(format[i], values?values[i]:arg(i), args+i, app);
        print_enter(tracing(TRACE_SYSCALL), name, arg(-1), format, args);
    }
}

void
RSIM_Thread::syscall_enter(uint32_t *values, const char *name, const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    syscall_enterv(values, name, format, &ap);
    va_end(ap);
}

void
RSIM_Thread::syscall_enter(const char *name, const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    syscall_enterv(NULL, name, format, &ap);
    va_end(ap);
}

void
RSIM_Thread::syscall_leave(const char *format, ...) 
{
    va_list ap;
    va_start(ap, format);

    ROSE_ASSERT(strlen(format)>=1);
    if (tracing(TRACE_SYSCALL)) {
        /* System calls return an integer (negative error numbers, non-negative success) */
        ArgInfo info;
        uint32_t value = readGPR(x86_gpr_ax).known_value();
        syscall_arginfo(format[0], value, &info, &ap);
        print_leave(tracing(TRACE_SYSCALL), format[0], &info);

        /* Additionally, output any other buffer values that were filled in by a successful system call. */
        int result = (int)(uint32_t)(arg(-1));
        if (format[0]!='d' || -1!=result || -EINTR==result) {
            for (size_t i=1; format[i]; i++) {
                if ('-'!=format[i]) {
                    syscall_arginfo(format[i], arg(i-1), &info, &ap);
                    if ('P'!=format[i] || 0!=arg(i-1)) { /* no need to show null pointers */
                        fprintf(tracing(TRACE_SYSCALL), "    arg%zu = ", i-1);
                        print_single(tracing(TRACE_SYSCALL), format[i], &info);
                        fprintf(tracing(TRACE_SYSCALL), "\n");
                    }
                }
            }
        }
    }
}

uint32_t
RSIM_Thread::arg(int idx)
{
    switch (idx) {
        case -1: return readGPR(x86_gpr_ax).known_value();      /* syscall return value */
        case 0: return readGPR(x86_gpr_bx).known_value();
        case 1: return readGPR(x86_gpr_cx).known_value();
        case 2: return readGPR(x86_gpr_dx).known_value();
        case 3: return readGPR(x86_gpr_si).known_value();
        case 4: return readGPR(x86_gpr_di).known_value();
        case 5: return readGPR(x86_gpr_bp).known_value();
        default: assert(!"invalid argument number"); abort();
    }
}

/* Called asynchronously to make a signal pending. The signal will be dropped if it's action is to ignore. Otherwise it will be
 * made pending by setting the appropriate bit in the RSIM_Thread's signal_pending vector.  And yes, we know that this
 * function is not async signal safe when signal tracing is enabled. */
void
RSIM_Thread::signal_generate(int signo)
{
    ROSE_ASSERT(signo>0 && signo<_NSIG);
    uint64_t sigbit = (uint64_t)1 << (signo-1);
    bool is_masked = (0 != (signal_mask & sigbit));

    if (tracing(TRACE_SIGNAL)) {
        fprintf(tracing(TRACE_SIGNAL), " [generated ");
        print_enum(tracing(TRACE_SIGNAL), signal_names, signo);
        fprintf(tracing(TRACE_SIGNAL), "(%d)", signo);
    }
    
    if (signal_action[signo-1].handler_va==(uint32_t)(uint64_t)SIG_IGN) { /* double cast to avoid gcc warning */
        if (tracing(TRACE_SIGNAL))
            fputs(" ignored]", tracing(TRACE_SIGNAL));
    } else if (is_masked) {
        if (tracing(TRACE_SIGNAL))
            fputs(" masked]", tracing(TRACE_SIGNAL));
        signal_pending |= sigbit;
    } else {
        if (tracing(TRACE_SIGNAL))
            fputs("]", tracing(TRACE_SIGNAL));
        signal_pending |= sigbit;
        signal_reprocess = true;
    }
}

/* Deliver a pending, unmasked signal.  Posix doesn't specify an order, so we'll deliver the one with the lowest number. */
void
RSIM_Thread::signal_deliver_any()
{
    if (signal_reprocess) {
        signal_reprocess = false;
        for (size_t i=0; i<64; i++) {
            uint64_t sigbit = (uint64_t)1 << i;
            if ((signal_pending & sigbit) && 0==(signal_mask & sigbit)) {
                signal_pending &= ~sigbit;
                signal_deliver(i+1); /* bit N is signal N+1 */
                return;
            }
        }
    }
}

/* Deliver the specified signal. The signal is not removed from the signal_pending vector, nor is it added if it's masked. */
void
RSIM_Thread::signal_deliver(int signo)
{
    ROSE_ASSERT(signo>0 && signo<=64);

    if (tracing(TRACE_SIGNAL)) {
        fprintf(tracing(TRACE_SIGNAL), "0x%08"PRIx64": delivering ", readIP().known_value());
        print_enum(tracing(TRACE_SIGNAL), signal_names, signo);
        fprintf(tracing(TRACE_SIGNAL), "(%d)", signo);
    }

    if (signal_action[signo-1].handler_va==(uint32_t)(uint64_t)SIG_IGN) { /* double cast to avoid gcc warning */
        /* The signal action may have changed since the signal was generated, so we need to check this again. */
        if (tracing(TRACE_SIGNAL))
            fprintf(tracing(TRACE_SIGNAL), " ignored\n");
    } else if (signal_action[signo-1].handler_va==(uint32_t)(uint64_t)SIG_DFL) {
        if (tracing(TRACE_SIGNAL))
            fprintf(tracing(TRACE_SIGNAL), " default\n");
        switch (signo) {
            case SIGFPE:
            case SIGILL:
            case SIGSEGV:
            case SIGBUS:
            case SIGABRT:
            case SIGTRAP:
            case SIGSYS:
                /* Exit with core dump */
                process->dump_core(signo);
                throw Exit((signo & 0x7f) | __WCOREFLAG);
            case SIGTERM:
            case SIGINT:
            case SIGQUIT:
            case SIGKILL:
            case SIGHUP:
            case SIGALRM:
            case SIGVTALRM:
            case SIGPROF:
            case SIGPIPE:
            case SIGXCPU:
            case SIGXFSZ:
            case SIGUSR1:
            case SIGUSR2:
                /* Exit without core dump */
                throw Exit(signo & 0x7f);
            case SIGIO:
            case SIGURG:
            case SIGCHLD:
            case SIGCONT:
            case SIGSTOP:
            case SIGTTIN:
            case SIGTTOU:
            case SIGWINCH:
                /* Signal is ignored by default */
                return;
            default:
                /* Exit without a core dump */
                throw Exit(signo & 0x7f);
        }

    } else if (signal_mask & ((uint64_t)1 << signo)) {
        /* Masked, but do not adjust signal_pending vector. */
        if (tracing(TRACE_SIGNAL))
            fprintf(tracing(TRACE_SIGNAL), " masked (discarded)\n");
    } else {
        if (tracing(TRACE_SIGNAL))
            fprintf(tracing(TRACE_SIGNAL), " to 0x%08"PRIx32"\n", signal_action[signo-1].handler_va);

        uint32_t signal_return = readIP().known_value();
        push(signal_return);

        /* Switch to the alternate stack? */
        uint32_t signal_oldstack = readGPR(x86_gpr_sp).known_value();
        if (0==(signal_stack.ss_flags & SS_ONSTACK) && 0!=(signal_action[signo-1].flags & SA_ONSTACK))
            writeGPR(x86_gpr_sp, number<32>(signal_stack.ss_sp + signal_stack.ss_size));

        /* Push stuff that will be needed by the simulated sigreturn() syscall */
        push(signal_oldstack);
        push(signal_mask >> 32);
        push(signal_mask & 0xffffffff);

        /* Caller-saved registers */
        push(readGPR(x86_gpr_ax));
        push(readGPR(x86_gpr_bx));
        push(readGPR(x86_gpr_cx));
        push(readGPR(x86_gpr_dx));
        push(readGPR(x86_gpr_si));
        push(readGPR(x86_gpr_di));
        push(readGPR(x86_gpr_bp));

        /* New signal mask */
        signal_mask |= signal_action[signo-1].mask;
        signal_mask |= (uint64_t)1 << (signo-1);
        // signal_reprocess = true;  -- Not necessary because we're not clearing any */

        /* Signal handler arguments */
        push(readIP());
        push(number<32>(signo));

        /* Invoke signal handler */
        push(number<32>(SIGHANDLER_RETURN)); /* fake return address to trigger signal_cleanup() call */
        writeIP(number<32>(signal_action[signo-1].handler_va));
    }
}

/* Note: if the specimen's signal handler never returns then this function is never invoked.  The specimen may do a longjmp()
 * or siglongjmp(), in which case the original stack, etc are restored anyway. Additionally, siglongjmp() may do a system call
 * to set the signal mask back to the value saved by sigsetjmp(), if any. */
void
RSIM_Thread::signal_return()
{
    /* Discard handler arguments */
    int signo = pop().known_value(); /* signal number */
    pop(); /* signal address */

    if (tracing(TRACE_SIGNAL)) {
        fprintf(tracing(TRACE_SIGNAL), "[returning from ");
        print_enum(tracing(TRACE_SIGNAL), signal_names, signo);
        fprintf(tracing(TRACE_SIGNAL), " handler]\n");
    }

    /* Restore caller-saved registers */
    writeGPR(x86_gpr_bp, pop());
    writeGPR(x86_gpr_di, pop());
    writeGPR(x86_gpr_si, pop());
    writeGPR(x86_gpr_dx, pop());
    writeGPR(x86_gpr_cx, pop());
    writeGPR(x86_gpr_bx, pop());
    writeGPR(x86_gpr_ax, pop());

    /* Simulate the sigreturn system call (#119), the stack frame of which was set up by signal_deliver() */
    uint64_t old_sigmask = pop().known_value(); /* low bits */
    old_sigmask |= (uint64_t)pop().known_value() << 32; /* hi bits */
    if (old_sigmask!=signal_mask)
        signal_reprocess = true;
    signal_mask = old_sigmask;

    /* Simulate return from sigreturn */
    writeGPR(x86_gpr_sp, pop());        /* restore stack pointer */
    writeIP(pop());                     /* RET instruction */
}

/* Suspend execution until a signal arrives. The signal must not be masked, and must either terminate the process or have a
 * signal handler. */
void
RSIM_Thread::signal_pause()
{
    /* Signals that terminate a process by default */
    uint64_t terminating = (uint64_t)(-1);
    terminating &= ~((uint64_t)1 << (SIGIO-1));
    terminating &= ~((uint64_t)1 << (SIGURG-1));
    terminating &= ~((uint64_t)1 << (SIGCHLD-1));
    terminating &= ~((uint64_t)1 << (SIGCONT-1));
    terminating &= ~((uint64_t)1 << (SIGSTOP-1));
    terminating &= ~((uint64_t)1 << (SIGTTIN-1));
    terminating &= ~((uint64_t)1 << (SIGTTOU-1));
    terminating &= ~((uint64_t)1 << (SIGWINCH-1));

    /* What signals would unpause this syscall? */
    uint64_t unpause = 0;
    for (uint64_t i=0; i<64; i++) {
        uint64_t sigbit = (uint64_t)1 << i;
        if (signal_action[i].handler_va==(uint64_t)SIG_DFL && 0!=(sigbit & terminating)) {
            unpause |= sigbit;
        } else if (signal_action[i].handler_va!=0) {
            unpause |= sigbit;
        }
    }

    /* Pause until the simulator receives a signal that should be delivered to the specimen.  We violate the
     * semantics a tiny bit here: the pause() syscall returns before the signal handler is invoked.  I don't
     * think this matters much since the handler will be invoked before the instruction that follows the "INT 80". */
    while (0==(signal_pending & unpause & ~signal_mask))
        pause();
}

void
RSIM_Thread::writeSegreg(X86SegmentRegister sr, const VirtualMachineSemantics::ValueType<16> &val)
{
    ROSE_ASSERT(3 == (val.known_value() & 7)); /*GDT and privilege level 3*/
    VirtualMachineSemantics::Policy::writeSegreg(sr, val);
    process->load_segreg_shadow(sr, val.known_value()>>3);
}

