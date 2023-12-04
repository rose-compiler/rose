// SgAsmMipsInstruction member definitions.
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

using namespace Rose;
using namespace Rose::BinaryAnalysis;

unsigned
SgAsmMipsInstruction::get_anyKind() const {
    return p_kind;
}

// see base class
AddressSet
SgAsmMipsInstruction::getSuccessors(bool &complete)
{
    complete = false;
    AddressSet successors;

    switch (get_kind()) {
        case mips_break:
        case mips_j:
        case mips_jal:
        case mips_jalr:
        case mips_jalx:
        case mips_jr:
        case mips_jr_hb:
        case mips_syscall:
            // unconditional branch
            if (Sawyer::Optional<rose_addr_t> target = branchTarget()) {
                successors.insert(*target);
                complete = true;
            }
            break;

        case mips_beq:
        case mips_beql:
        case mips_bgez:
        case mips_bgezal:
        case mips_bgezall:
        case mips_bgezl:
        case mips_bgtz:
        case mips_bgtzl:
        case mips_blez:
        case mips_blezl:
        case mips_bltz:
        case mips_bltzal:
        case mips_bltzall:
        case mips_bltzl:
        case mips_bne:
        case mips_bnel:
        case mips_teq:
        case mips_teqi:
        case mips_tge:
        case mips_tgei:
        case mips_tgeiu:
        case mips_tgeu:
        case mips_tlt:
        case mips_tlti:
        case mips_tltiu:
        case mips_tltu:
        case mips_tne:
        case mips_tnei:
            // conditional branch
            if (Sawyer::Optional<rose_addr_t> target = branchTarget()) {
                successors.insert(*target);
                complete = true;
            }
            successors.insert(get_address() + get_size()); // fall through address
            break;

        default:
            // fall through
            successors.insert(get_address() + get_size());
            complete = true;
            break;
    }
    return successors;
}

// see base class
bool
SgAsmMipsInstruction::isUnknown() const
{
    return mips_unknown_instruction == get_kind();
}

Sawyer::Optional<rose_addr_t>
SgAsmMipsInstruction::branchTarget()
{
    SgAsmExpressionPtrList &args = get_operandList()->get_operands();
    switch (get_kind()) {
        case mips_j:
        case mips_jal:
        case mips_jalx:
            // target address stored in first argument
            ASSERT_require(args.size() >= 1);
            ASSERT_require(isSgAsmIntegerValueExpression(args[0]));
            return isSgAsmIntegerValueExpression(args[0])->get_absoluteValue();

        case mips_bgez:
        case mips_bgezal:
        case mips_bgezall:
        case mips_bgezl:
        case mips_bgtz:
        case mips_bgtzl:
        case mips_blez:
        case mips_blezl:
        case mips_bltz:
        case mips_bltzal:
        case mips_bltzall:
        case mips_bltzl:
            // target address stored in the second argument
            ASSERT_require(args.size() >= 2);
            ASSERT_require(isSgAsmIntegerValueExpression(args[1]));
            return isSgAsmIntegerValueExpression(args[1])->get_absoluteValue();

        case mips_beq:
        case mips_beql:
        case mips_bne:
        case mips_bnel:
            // target address stored in the third argument
            ASSERT_require(args.size() >= 3);
            ASSERT_require(isSgAsmIntegerValueExpression(args[2]));
            return isSgAsmIntegerValueExpression(args[2])->get_absoluteValue();

        default:
            // no known target
            return Sawyer::Nothing();
    }
}


#endif
