#include "rose.h"
#include "RSIM_Private.h"

#ifdef ROSE_ENABLE_SIMULATOR

void
RSIM_SemanticPolicy::ctor()
{

    reg_eax = findRegister("eax", 32);
    reg_ebx = findRegister("ebx", 32);
    reg_ecx = findRegister("ecx", 32);
    reg_edx = findRegister("edx", 32);
    reg_esi = findRegister("esi", 32);
    reg_edi = findRegister("edi", 32);
    reg_eip = findRegister("eip", 32);
    reg_esp = findRegister("esp", 32);
    reg_ebp = findRegister("ebp", 32);

    reg_cs  = findRegister("cs", 16);
    reg_ds  = findRegister("ds", 16);
    reg_es  = findRegister("es", 16);
    reg_fs  = findRegister("fs", 16);
    reg_gs  = findRegister("gs", 16);
    reg_ss  = findRegister("ss", 16);

    reg_eflags = findRegister("eflags", 32);
    reg_df  = findRegister("df", 1);
    reg_tf  = findRegister("tf", 1);

    writeRegister<32>(reg_eip, 0);
    writeRegister<32>(reg_eax, 0);
    writeRegister<32>(reg_ebx, 0);
    writeRegister<32>(reg_ecx, 0);
    writeRegister<32>(reg_edx, 0);
    writeRegister<32>(reg_esi, 0);
    writeRegister<32>(reg_edi, 0);
    writeRegister<32>(reg_ebp, 0);
    writeRegister<32>(reg_esp, 0xc0000000ul); /* high end of stack, exclusive */
    writeRegister<32>(reg_eflags, 0x00000002); // flag bit 1 is set to one, although this is a reserved register

    writeRegister<16>(reg_cs, 0x23);
    writeRegister<16>(reg_ds, 0x2b);
    writeRegister<16>(reg_es, 0x2b);
    writeRegister<16>(reg_ss, 0x2b);
    writeRegister<16>(reg_fs, 0);
    writeRegister<16>(reg_gs, 0);
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
    int code = readRegister<32>(reg_eax).known_value();

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

    writeRegister(reg_eax, number<32>(dwords[0]));
    writeRegister(reg_ebx, number<32>(dwords[1]));
    writeRegister(reg_ecx, number<32>(dwords[2]));
    writeRegister(reg_edx, number<32>(dwords[3]));
}

void
RSIM_SemanticPolicy::sysenter()
{
    thread->emulate_syscall();

    /* On linux, SYSENTER is followed by zero or more NOPs, followed by a JMP back to just before the SYSENTER in order to
     * restart interrupted system calls, followed by POPs for the callee-saved registers. A non-interrupted system call should
     * return to the first POP instruction, which happens to be 9 bytes after the end of the SYSENTER. */
    writeRegister(reg_eip, add(readRegister<32>(reg_eip), number<32>(9)));
}

void
RSIM_SemanticPolicy::dump_registers(RTS_Message *mesg)
{
    /* lock, so entire state is all together in the output */
    RTS_MESSAGE(*mesg) {
        mesg->multipart("state",
                        "    eax=0x%08"PRIx64" ebx=0x%08"PRIx64" ecx=0x%08"PRIx64" edx=0x%08"PRIx64"\n",
                        readRegister<32>(reg_eax).known_value(), readRegister<32>(reg_ebx).known_value(),
                        readRegister<32>(reg_ecx).known_value(), readRegister<32>(reg_edx).known_value());
        mesg->more("    esi=0x%08"PRIx64" edi=0x%08"PRIx64" ebp=0x%08"PRIx64" esp=0x%08"PRIx64" eip=0x%08"PRIx64"\n",
                   readRegister<32>(reg_esi).known_value(), readRegister<32>(reg_edi).known_value(),
                   readRegister<32>(reg_ebp).known_value(), readRegister<32>(reg_esp).known_value(),
                   get_ip().known_value());

        static const char *segreg_name[] = {"cs", "ds", "es", "fs", "gs", "ss"};
        for (int i=0; i<6; i++) {
            mesg->more("    %s=0x%04"PRIx64" base=0x%08"PRIx32" limit=0x%08"PRIx32" present=%s\n",
                       segregToString((X86SegmentRegister)i), readRegister<16>(segreg_name[i]).known_value(),
                       sr_shadow[i].base, sr_shadow[i].limit, sr_shadow[i].present?"yes":"no");
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
RSIM_SemanticPolicy::get_eflags()
{
    return readRegister<32>(reg_eflags).known_value();
}

void
RSIM_SemanticPolicy::set_eflags(uint32_t eflags)
{
    writeRegister<32>(reg_eflags, eflags);
}

void
RSIM_SemanticPolicy::push(VirtualMachineSemantics::ValueType<32> n)
{
    VirtualMachineSemantics::ValueType<32> new_sp = add(readRegister<32>(reg_esp), number<32>((uint64_t)(int64_t)-4));
    writeMemory(x86_segreg_ss, new_sp, n, true_());
    writeRegister(reg_esp, new_sp);
}

VirtualMachineSemantics::ValueType<32>
RSIM_SemanticPolicy::pop()
{
    VirtualMachineSemantics::ValueType<32> old_sp = readRegister<32>(reg_esp);
    VirtualMachineSemantics::ValueType<32> retval = readMemory<32>(x86_segreg_ss, old_sp, true_());
    writeRegister(reg_esp, add(old_sp, number<32>(4)));
    return retval;
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
