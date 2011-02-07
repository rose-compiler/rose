#include "sage3basic.h"
#include "RSIM_Simulator.h"

FILE *
RSIM_SemanticPolicy::tracing(unsigned what) const
{
    return thread->tracing(what);
}

void
RSIM_SemanticPolicy::interrupt(uint8_t num)
{
    if (num != 0x80) {
        fprintf(stderr, "Bad interrupt\n");
        abort();
    }
    thread->emulate_syscall();
}

void
RSIM_SemanticPolicy::sysenter()
{
    thread->emulate_syscall();

    /* On linux, SYSENTER is followed by zero or more NOPs, followed by a JMP back to just before the SYSENTER in order to
     * restart interrupted system calls, followed by POPs for the callee-saved registers. A non-interrupted system call should
     * return to the first POP instruction, which happens to be 9 bytes after the end of the SYSENTER. */
    writeIP(add(readIP(), number<32>(9)));
}

void
RSIM_SemanticPolicy::dump_registers(FILE *f) const
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
                segregToString(sr), readSegreg(sr).known_value(), thread->process->segreg_shadow[sr].base,
                thread->process->segreg_shadow[sr].limit,
                thread->process->segreg_shadow[sr].present?"yes":"no");
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
RSIM_SemanticPolicy::get_eflags() const
{
    uint32_t eflags = 0;
    for (size_t i=0; i<VirtualMachineSemantics::State::n_flags; i++) {
        if (readFlag((X86Flag)i).is_known())
            eflags |= readFlag((X86Flag)i).known_value() ? 1u<<i : 0u;
    }
    return eflags;
}

void
RSIM_SemanticPolicy::push(VirtualMachineSemantics::ValueType<32> n)
{
    VirtualMachineSemantics::ValueType<32> new_sp = add(readGPR(x86_gpr_sp), number<32>(-4));
    writeMemory(x86_segreg_ss, new_sp, n, true_());
    writeGPR(x86_gpr_sp, new_sp);
}

VirtualMachineSemantics::ValueType<32>
RSIM_SemanticPolicy::pop()
{
    VirtualMachineSemantics::ValueType<32> old_sp = readGPR(x86_gpr_sp);
    VirtualMachineSemantics::ValueType<32> retval = readMemory<32>(x86_segreg_ss, old_sp, true_());
    writeGPR(x86_gpr_sp, add(old_sp, number<32>(4)));
    return retval;
}

void
RSIM_SemanticPolicy::writeSegreg(X86SegmentRegister sr, const VirtualMachineSemantics::ValueType<16> &val)
{
    ROSE_ASSERT(3 == (val.known_value() & 7)); /*GDT and privilege level 3*/
    VirtualMachineSemantics::Policy::writeSegreg(sr, val);
    thread->process->load_segreg_shadow(sr, val.known_value()>>3);
}

void
RSIM_SemanticPolicy::startInstruction(SgAsmInstruction* insn)
{
    if (tracing(TRACE_INSN)) {
        if (isatty(fileno(tracing(TRACE_INSN)))) {
            fprintf(tracing(TRACE_INSN), "\033[K\n[%07zu] %s\033[K\r\033[1A",
                    get_ninsns(), unparseInstructionWithAddress(insn).c_str());
        } else {
            fprintf(tracing(TRACE_INSN),
                    "[%07zu] 0x%08"PRIx64": %s\n", get_ninsns(), insn->get_address(), unparseInstruction(insn).c_str());
        }
    }
    VirtualMachineSemantics::Policy::startInstruction(insn);
}
