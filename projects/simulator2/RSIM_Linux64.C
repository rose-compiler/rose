/* This file contains Linux-64 system call emulation.  Most of these functions are callbacks and have names like:
 *
 *    RSIM_Linux64::syscall_FOO_enter           -- prints syscall tracing info when the call is entered
 *    RSIM_Linux64::syscall_FOO                 -- implements the system call
 *    RSIM_Linux64::syscall_FOO_leave           -- prints syscall tracing info when the call returns
 */
#include "rose.h"
#include "RSIM_Private.h"

#ifdef ROSE_ENABLE_SIMULATOR

#include "RSIM_Linux64.h"
#include "BinaryDebugger.h"
#include "Diagnostics.h"

using namespace rose::Diagnostics;
using namespace rose::BinaryAnalysis;

void
RSIM_Linux64::init() {
    if (interpreterBaseVa() == 0) {
        // Linux seems to ignore the alignment constraints (0x200000) in the ld-linux-x86-64.so interpreter and uses 0x1000
        // instead. Unfortunately ROSE's BinaryLoader can't do that (2015-06-02) so we use a different load address instead.
        // interpreterBaseVa(0x00007ffff7fe1000ull); -- unsolvable alignment constraints against 0x200000
        interpreterBaseVa(0x00007ffff7000000ull);
    }
}

bool
RSIM_Linux64::isSupportedArch(SgAsmGenericHeader *fhdr) {
    return isSgAsmElfFileHeader(fhdr) && fhdr->get_word_size()==8;
}

void
RSIM_Linux64::loadSpecimenNative(RSIM_Process *process, Disassembler *disassembler) {
    process->mem_transaction_start("specimen main memory");
    BinaryDebugger debugger(exeArgs());
    process->get_memory().insertProcess(":noattach:" + StringUtility::numberToString(debugger.isAttached()));

    const RegisterDictionary *regs = disassembler->get_registers();
    initialRegs_.ax = debugger.readRegister(*regs->lookup("rax")).toInteger();
    initialRegs_.bx = debugger.readRegister(*regs->lookup("rbx")).toInteger();
    initialRegs_.cx = debugger.readRegister(*regs->lookup("rcx")).toInteger();
    initialRegs_.dx = debugger.readRegister(*regs->lookup("rdx")).toInteger();
    initialRegs_.si = debugger.readRegister(*regs->lookup("rsi")).toInteger();
    initialRegs_.di = debugger.readRegister(*regs->lookup("rdi")).toInteger();
    initialRegs_.flags = debugger.readRegister(*regs->lookup("rflags")).toInteger();
    initialRegs_.orig_ax = debugger.readRegister(*regs->lookup("rax")).toInteger();
    initialRegs_.ip = debugger.readRegister(*regs->lookup("rip")).toInteger();
    initialRegs_.sp = debugger.readRegister(*regs->lookup("rsp")).toInteger();
    initialRegs_.bp = debugger.readRegister(*regs->lookup("rbp")).toInteger();
    initialRegs_.cs = debugger.readRegister(*regs->lookup("cs")).toInteger();
    initialRegs_.ss = debugger.readRegister(*regs->lookup("ss")).toInteger();
    initialRegs_.r8 = debugger.readRegister(*regs->lookup("r8")).toInteger();
    initialRegs_.r9 = debugger.readRegister(*regs->lookup("r9")).toInteger();
    initialRegs_.r10 = debugger.readRegister(*regs->lookup("r10")).toInteger();
    initialRegs_.r11 = debugger.readRegister(*regs->lookup("r11")).toInteger();
    initialRegs_.r12 = debugger.readRegister(*regs->lookup("r12")).toInteger();
    initialRegs_.r13 = debugger.readRegister(*regs->lookup("r13")).toInteger();
    initialRegs_.r14 = debugger.readRegister(*regs->lookup("r14")).toInteger();
    initialRegs_.r15 = debugger.readRegister(*regs->lookup("r15")).toInteger();

    debugger.terminate();
    return;
}
    
PtRegs
RSIM_Linux64::initialRegistersArch() {
    if (settings().nativeLoad)
        return initialRegs_;

    PtRegs regs;
    memset(&regs, 0, sizeof regs);
    regs.sp = 0x00007ffffffff000ull;                    // high end of stack, exclusive
    regs.flags = 2;                                     // flag bit 1 is set, although this is a reserved bit
    return regs;
}

rose_addr_t
RSIM_Linux64::pushAuxVector(RSIM_Process *process, rose_addr_t sp, rose_addr_t execfn_va, SgAsmElfFileHeader *fhdr,
                            FILE *trace) {
    static const char *platform = "x86_64";
    sp -= strlen(platform)+1;
    uint64_t platformVa = sp;
    process->mem_write(platform, platformVa, strlen(platform)+1);

    // 16-bytes of random data. We use hard-coded values for reproducibility with a pattern one might recognize when debugging.
    static const uint8_t randomData[] = {
        0x00, 0x11, 0x22, 0x33,
        0xff, 0xee, 0xdd, 0xcc,
        0x88, 0x99, 0xaa, 0xbb,
        0x77, 0x66, 0x55, 0x44
    };
    sp -= sizeof randomData;
    uint64_t randomDataVa = sp;
    process->mem_write(randomData, randomDataVa, sizeof randomData);

    // Initialize the stack with auxv, where each is two words. The order and values were determined by running the simulator
    // as "x86_64 -R x86sim --show-auxv a.out" on my devel machine, Linux 2.6.32-5-amd64. [Robb P. Matzke 2015-06-02]
    auxv_.clear();
    if (vdsoMappedVa() != 0) {
        auxv_.push_back(33);
        auxv_.push_back(vdsoMappedVa());
        if (trace)
            fprintf(trace, "AT_SYSINFO_PHDR(0x21):  0x%016"PRIx64"\n", auxv_.back());
    }

    auxv_.push_back(0x10);
    auxv_.push_back(0xbfebfbfful);
    if (trace)
        fprintf(trace, "AT_HWCAP(0x10):         0x%016"PRIx64"\n", auxv_.back());

    auxv_.push_back(0x06);
    auxv_.push_back(4096);
    if (trace)
        fprintf(trace, "AT_PAGESZ(0x06):        %"PRId64"\n", auxv_.back());

    auxv_.push_back(0x11);
    auxv_.push_back(100);
    if (trace)
        fprintf(trace, "AT_CLKTCK(0x11):        %"PRId64"\n", auxv_.back());

    auxv_.push_back(3);
    auxv_.push_back(segmentTableVa(fhdr));
    if (trace)
        fprintf(trace, "AT_PHDR(0x03):          0x%016"PRIx64"\n", auxv_.back());

    auxv_.push_back(4);
    auxv_.push_back(fhdr->get_phextrasz() + sizeof(SgAsmElfSegmentTableEntry::Elf64SegmentTableEntry_disk));
    if (trace)
        fprintf(trace, "AT_PHENT(0x04):         0x%"PRId64"\n", auxv_.back());

    auxv_.push_back(5);
    auxv_.push_back(fhdr->get_e_phnum());
    if (trace)
        fprintf(trace, "AT_PHNUM(0x05):         %"PRId64"\n", auxv_.back());

    auxv_.push_back(7);
    auxv_.push_back(fhdr->get_section_by_name(".interp") ? interpreterBaseVa() : 0);
    if (trace)
        fprintf(trace, "AT_BASE(0x07):          0x%016"PRIx64"\n", auxv_.back());
        
    auxv_.push_back(8);
    auxv_.push_back(0);
    if (trace)
        fprintf(trace, "AT_FLAGS(0x08):         0x%016"PRIx64"\n", auxv_.back());

    auxv_.push_back(9);
    auxv_.push_back(fhdr->get_entry_rva() + fhdr->get_base_va());
    if (trace)
        fprintf(trace, "AT_ENTRY(0x09):         0x%016"PRIx64"\n", auxv_.back());

    auxv_.push_back(0x0b);
    auxv_.push_back(getuid());
    if (trace)
        fprintf(trace, "AT_UID(0x0b):           %"PRId64"\n", auxv_.back());

    auxv_.push_back(0x0c);
    auxv_.push_back(geteuid());
    if (trace)
        fprintf(trace, "AT_EUID(0x0c):          %"PRId64"\n", auxv_.back());

    auxv_.push_back(0x0d);
    auxv_.push_back(getgid());
    if (trace)
        fprintf(trace, "AT_GID(0x0d):           %"PRId64"\n", auxv_.back());

    auxv_.push_back(0x0e);
    auxv_.push_back(getegid());
    if (trace)
        fprintf(trace, "AT_EGID(0x0e):          %"PRId64"\n", auxv_.back());

    auxv_.push_back(0x17);
    auxv_.push_back(false);
    if (trace)
        fprintf(trace, "AT_SECURE(0x17):        %"PRId64"\n", auxv_.back());

    auxv_.push_back(0x19);
    auxv_.push_back(randomDataVa);
    if (trace)
        fprintf(trace, "AT_RANDOM(0x19):        0x%016"PRIx64"\n", auxv_.back());

    auxv_.push_back(0x1f);
    auxv_.push_back(execfn_va);
    if (trace)
        fprintf(trace, "AT_EXECFN(0x1f):        0x%016"PRIx64" (%s)\n", auxv_.back(), exeArgs()[0].c_str());

    auxv_.push_back(0x0f);
    auxv_.push_back(platformVa);
    if (trace)
        fprintf(trace, "AT_PLATFORM(0x0f):      0x%016"PRIx64" (%s)\n", auxv_.back(), platform);

    // AT_NULL
    auxv_.push_back(0);
    auxv_.push_back(0);

    // Write auxv pairs
    ASSERT_require(8 == sizeof(auxv_[0]));
    sp -= 8 * auxv_.size();
    sp &= ~0xf;                                         // align to 16 byte boundary
    process->mem_write(&auxv_[0], sp, 8*auxv_.size());

    return sp;
}

#endif
