#include "rose.h"
#include "RSIM_Private.h"

#ifdef ROSE_ENABLE_SIMULATOR

void
RSIM_SemanticPolicy::ctor()
{
    for (size_t i=0; i<VirtualMachineSemantics::State::n_gprs; i++)
        writeGPR((X86GeneralPurposeRegister)i, 0);
    for (size_t i=0; i<VirtualMachineSemantics::State::n_flags; i++)
        writeFlag((X86Flag)i, 0);
    writeIP(0);
    writeFlag((X86Flag)1, true_());

    writeGPR(x86_gpr_sp, 0xc0000000ul); /* high end of stack, exclusive */

    writeSegreg(x86_segreg_cs, 0x23);
    writeSegreg(x86_segreg_ds, 0x2b);
    writeSegreg(x86_segreg_es, 0x2b);
    writeSegreg(x86_segreg_ss, 0x2b);
    writeSegreg(x86_segreg_fs, 0);
    writeSegreg(x86_segreg_gs, 0);

}

RTS_Message *
RSIM_SemanticPolicy::tracing(TracingFacility what) const
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
RSIM_SemanticPolicy::cpuid()
{
    int code = readGPR(x86_gpr_ax).known_value();

    uint32_t dwords[4];
#if 0
    /* Prone to not compile */
    asm volatile("cpuid"
                 :
                 "=a"(*(dwords+0)),
                 "=b"(*(dwords+1)),
                 "=c"(*(dwords+2)),
                 "=d"(*(dwords+3))
                 :
                 "0"(code));
#else
    /* Return value based on an Intel model "Xeon X5680 @ 3.33GHz"; 3325.017GHz; stepping 2 */
    dwords[0] = 0x0000000b;
    dwords[1] = 0x756e6547;
    dwords[2] = 0x6c65746e;
    dwords[3] = 0x49656e69;
#endif

    /* Change "GenuineIntel" to "Genuine ROSE". Doing so should cause the caller to not execute any further CPUID
     * instructions since there's no well-known definition for the rest of our CPUID semantics. */
    if (0==code) {
        dwords[3] &= 0x00ffffff; dwords[3] |= 0x20000000;           /* ' ' */
        dwords[2] =  0x45534f52;                                    /* "ROSE" */
    } else {
        fprintf(stderr, "CPUID-%d probably should not have been executed!", code);
    }

    writeGPR(x86_gpr_ax, number<32>(dwords[0]));
    writeGPR(x86_gpr_bx, number<32>(dwords[1]));
    writeGPR(x86_gpr_cx, number<32>(dwords[2]));
    writeGPR(x86_gpr_dx, number<32>(dwords[3]));
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
RSIM_SemanticPolicy::dump_registers(RTS_Message *mesg) const
{
    /* lock, so entire state is all together in the output */
    RTS_MESSAGE(*mesg) {
        mesg->multipart("state",
                        "    eax=0x%08"PRIx64" ebx=0x%08"PRIx64" ecx=0x%08"PRIx64" edx=0x%08"PRIx64"\n",
                        readGPR(x86_gpr_ax).known_value(), readGPR(x86_gpr_bx).known_value(),
                        readGPR(x86_gpr_cx).known_value(), readGPR(x86_gpr_dx).known_value());
        mesg->more("    esi=0x%08"PRIx64" edi=0x%08"PRIx64" ebp=0x%08"PRIx64" esp=0x%08"PRIx64" eip=0x%08"PRIx64"\n",
                   readGPR(x86_gpr_si).known_value(), readGPR(x86_gpr_di).known_value(),
                   readGPR(x86_gpr_bp).known_value(), readGPR(x86_gpr_sp).known_value(),
                   get_ip().known_value());
        for (int i=0; i<6; i++) {
            X86SegmentRegister sr = (X86SegmentRegister)i;
            mesg->more("    %s=0x%04"PRIx64" base=0x%08"PRIx32" limit=0x%08"PRIx32" present=%s\n",
                       segregToString(sr), readSegreg(sr).known_value(),
                       sr_shadow[sr].base, sr_shadow[sr].limit, sr_shadow[sr].present?"yes":"no");
        }

        uint32_t eflags = get_eflags();
        mesg->more("    flags: 0x%08"PRIx32":", eflags);
        static const char *flag_name[] = {"cf",  "#1",  "pf",   "#3",    "af",    "#5",  "zf",  "sf",
                                          "tf",  "if",  "df",   "of", "iopl0", "iopl1",  "nt", "#15",
                                          "rf",  "vm",  "ac",  "vif",   "vip",    "id", "#22", "#23",
                                          "#24", "#25", "#26", "#27",   "#28",   "#29", "#30", "#31"};
        for (uint32_t i=0; i<32; i++) {
            if (eflags & (1u<<i))
                mesg->more(" %s", flag_name[i]);
        }
        mesg->more("\n");
        mesg->multipart_end();
    } RTS_MESSAGE_END(true);
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
RSIM_SemanticPolicy::set_eflags(uint32_t eflags)
{
    for (size_t i=0; i<VirtualMachineSemantics::State::n_flags; i++) {
        uint32_t mask = (uint32_t)1 << i;
        writeFlag((X86Flag)i, eflags & mask ? true_() : false_());
    }
}

void
RSIM_SemanticPolicy::push(VirtualMachineSemantics::ValueType<32> n)
{
    VirtualMachineSemantics::ValueType<32> new_sp = add(readGPR(x86_gpr_sp), number<32>((uint64_t)(int64_t)-4));
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
    ROSE_ASSERT(0==val.known_value() || 3 == (val.known_value() & 7)); /*GDT and privilege level 3*/
    VirtualMachineSemantics::Policy::writeSegreg(sr, val);
    load_sr_shadow(sr, val.known_value()>>3);
}

void
RSIM_SemanticPolicy::startInstruction(SgAsmInstruction* insn)
{
    RTS_Message *mesg = tracing(TRACE_INSN);
    if (mesg->get_file()) {
        if (isatty(fileno(mesg->get_file()))) {
            fprintf(mesg->get_file(), "\033[K\n%s\033[K\r\033[1A",
                    unparseInstructionWithAddress(insn).c_str());
        } else {
            mesg->mesg("%s", unparseInstruction(insn).c_str());
        }
    }
    VirtualMachineSemantics::Policy::startInstruction(insn);
}

void
RSIM_SemanticPolicy::load_sr_shadow(X86SegmentRegister sr, unsigned gdt_num)
{
    user_desc_32 *info = thread->gdt_entry(gdt_num);
    sr_shadow[sr] = *info;
    //ROSE_ASSERT(sr_shadow[sr].present); //checked when used
}

#endif /* ROSE_ENABLE_SIMULATOR */
