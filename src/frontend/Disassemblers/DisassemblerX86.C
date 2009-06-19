#include "rose.h"

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

/* See header file for full documentation. */

Disassembler::AddressSet
SgAsmx86Instruction::get_successors() {
    Disassembler::AddressSet retval;
    rose_addr_t va;

    /* call, jmp, ja, jae, jb, jbe, jcxz, jecxz, jrcxz, je, jg, jge, jl, jle, jne, jno, jns, jo,
     * jpe, jpo, js, loop, loopnz, loopz */
    if (x86GetKnownBranchTarget(this, va/*out*/))
        retval.insert(va);
        
    /* Fall through address. */
    switch (get_kind()) {
        case x86_ret:
        case x86_iret:
        case x86_farcall:
        case x86_farjmp:
        case x86_hlt:
        case x86_jmp:
        case x86_int1:
        case x86_int3:
        case x86_into:
        case x86_retf:
        case x86_rsm:
        case x86_ud2:
            break;
        default:
            retval.insert(get_address() + get_raw_bytes().size());
            break;
    }
    return retval;
}

bool
SgAsmx86Instruction::terminatesBasicBlock() {
    return x86InstructionIsControlTransfer(this);
}

Disassembler *
DisassemblerX86::can_disassemble(SgAsmGenericHeader *header) const
{
    SgAsmExecutableFileFormat::InsSetArchitecture isa = header->get_isa();
    if (isSgAsmDOSFileHeader(header))
        return new DisassemblerX86(header);
    if ((isa & SgAsmExecutableFileFormat::ISA_FAMILY_MASK) == SgAsmExecutableFileFormat::ISA_IA32_Family)
        return new DisassemblerX86(header);
    if ((isa & SgAsmExecutableFileFormat::ISA_FAMILY_MASK) == SgAsmExecutableFileFormat::ISA_X8664_Family)
        return new DisassemblerX86(header);
    return NULL;
}

void
DisassemblerX86::init(SgAsmGenericHeader *header)
{
    SgAsmExecutableFileFormat::InsSetArchitecture isa = header->get_isa();
    if (isSgAsmDOSFileHeader(header)) {
        params = X86Disassembler::Parameters(0, x86_insnsize_16);
        set_wordsize(2);
        set_alignment(1);
        set_sex(SgAsmExecutableFileFormat::ORDER_LSB);
    } else if ((isa & SgAsmExecutableFileFormat::ISA_FAMILY_MASK) == SgAsmExecutableFileFormat::ISA_IA32_Family) {
        params = X86Disassembler::Parameters(0, x86_insnsize_32);
        insnSize = x86_insnsize_32;
        set_wordsize(4);
        set_alignment(1);
        set_sex(SgAsmExecutableFileFormat::ORDER_LSB);
    } else if ((isa & SgAsmExecutableFileFormat::ISA_FAMILY_MASK) == SgAsmExecutableFileFormat::ISA_X8664_Family) {
        params = X86Disassembler::Parameters(0, x86_insnsize_64);
        insnSize = x86_insnsize_64;
        set_wordsize(8);
        set_alignment(1);
        set_sex(SgAsmExecutableFileFormat::ORDER_LSB);
    } else {
        ROSE_ASSERT(!"unknown x86 sub-architecture");
        abort();
    }
}

/* This is a bit of a kludge for now because we're trying to use an unmodified version of the X86Disassembler name space. */
SgAsmInstruction *
DisassemblerX86::disassembleOne(const unsigned char *buf, const RvaFileMap &map, rose_addr_t start_va, 
                                AddressSet *successors)
{
    /* Starting virtual address was not available when the DisassemblerX86 class was instantiated, so set it now. */
    params.ip = start_va;

    /* The old X86Disassembler::disassemble() function doesn't understand RvaFileMap mappings. Therefore, remap the next few
     * bytes (enough for at least one instruction) into a temporary buffer. The longest x86 instruction is 15 bytes in 16-bit
     * mode and 13 bytes in 32-bit mode:
     * 
     * [16-bit]: 66 67 F0 3E 81 04 4E 01234567 89ABCDEF: add [ds:esi+ecx*2+0x67452301], 0xEFCDAB89
     * [32-bit]:       F0 3E 81 04 4E 01234567 89ABCDEF: add [ds:esi+ecx*2+0x67452301], 0xEFCDAB89
     *
     * In theory, by adding all appropriate prefix bytes you can obtain an instruction that is up to 16 bytes long. However,
     * the x86 will generate an exception if the instruction length exceeds 15 bytes. */
    unsigned char temp[16];
    size_t tempsz = map.read(temp, buf, start_va, sizeof temp);

    /* Disassemble the instruction */
    SgAsmx86Instruction *insn = NULL;
    AddressSet suc1;
    try {
        insn = X86Disassembler::disassemble(params, temp, tempsz, 0, &suc1);
    } catch(const X86Disassembler::OverflowOfInstructionVector&) {
        throw Exception("short read");
    } catch(const X86Disassembler::BadInstruction&) {
        throw Exception("bad instruction");
    } catch(...) {
        throw Exception("cannot disassemble");
    }
    if (successors)
        successors->insert(suc1.begin(), suc1.end());

#if 1 /*DEBUG*/
    /* Make sure the successors returned by the disassembler match the successors returned by the instruction since they are
     * built two different ways. When this code is removed, we can optimize the decoding above to not need the temporary suc1. */
    AddressSet suc2 = insn->get_successors();
    switch (insn->get_kind()) {
        case x86_farjmp:
        case x86_hlt:
        case x86_jmp:
        case x86_ret:
            /* Old method adds the following instruction address to the successors */
            break;
        default:
            if (suc1!=suc2) {
                fprintf(stderr, "Successors do not match for instruction: %s\n", unparseInstructionWithAddress(insn).c_str());
                fprintf(stderr, "  Old method:\n");
                for (AddressSet::iterator si=suc1.begin(); si!=suc1.end(); si++)
                    fprintf(stderr, "    0x%08"PRIx64"\n", *si);
                fprintf(stderr, "  New method:\n");
                for (AddressSet::iterator si=suc2.begin(); si!=suc2.end(); si++)
                    fprintf(stderr, "    0x%08"PRIx64"\n", *si);
                ROSE_ASSERT(suc1==suc2);
            }
            break;
    }
#endif


    ROSE_ASSERT(insn);
    return insn;
}
