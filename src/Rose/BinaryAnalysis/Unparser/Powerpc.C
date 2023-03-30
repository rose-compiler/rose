#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Unparser/Powerpc.h>

#include <Rose/BinaryAnalysis/RegisterDictionary.h>
#include <integerOps.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Unparser {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Supporting functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static std::string
unparsePowerpcRegister(SgAsmInstruction *insn, RegisterDescriptor rdesc, RegisterDictionary::Ptr registers) {
    if (!registers)
        registers = RegisterDictionary::instancePowerpc32();
    std::string name = registers->lookup(rdesc);
    if (name.empty())
        name = invalidRegister(insn, rdesc, registers);
    return name;
}

static std::string
unparsePowerpcExpression(SgAsmExpression* expr, const LabelMap *labels, const RegisterDictionary::Ptr &registers, bool useHex) {
    std::string result = "";
    if (expr == nullptr) return "BOGUS:NULL";
    switch (expr->variantT()) {
        case V_SgAsmBinaryAdd:
            result = unparsePowerpcExpression(isSgAsmBinaryExpression(expr)->get_lhs(), labels, registers, false) + " + " +
                     unparsePowerpcExpression(isSgAsmBinaryExpression(expr)->get_rhs(), labels, registers, false);
            break;
        case V_SgAsmMemoryReferenceExpression: {
            SgAsmMemoryReferenceExpression* mr = isSgAsmMemoryReferenceExpression(expr);
            SgAsmExpression* addr = mr->get_address();
            switch (addr->variantT()) {
                case V_SgAsmBinaryAdd: {
                    SgAsmBinaryAdd* a = isSgAsmBinaryAdd(addr);
                    std::string lhs = unparsePowerpcExpression(a->get_lhs(), labels, registers, false);
                    if (isSgAsmValueExpression(a->get_rhs())) {
                        // Sign-extend from 16 bits
                        SgAsmValueExpression *ve = isSgAsmValueExpression(a->get_rhs());
                        ASSERT_not_null(ve);
                        result = boost::lexical_cast<std::string>(
                                   (int64_t)IntegerOps::signExtend<16, 64>(SageInterface::getAsmConstant(ve)));
                        result += "(" + lhs + ")";
                    } else {
                        result = lhs + ", " + unparsePowerpcExpression(a->get_rhs(), labels, registers, false);
                    }
                    break;
                }
                default:
                    result = "(" + unparsePowerpcExpression(addr, labels, registers, false) + ")";
                    break;
            }
            break;
        }
        case V_SgAsmDirectRegisterExpression: {
            SgAsmInstruction *insn = SageInterface::getEnclosingNode<SgAsmInstruction>(expr);
            SgAsmDirectRegisterExpression* rr = isSgAsmDirectRegisterExpression(expr);
            result = unparsePowerpcRegister(insn, rr->get_descriptor(), registers);
            break;
        }
        case V_SgAsmIntegerValueExpression: {
            uint64_t v = isSgAsmIntegerValueExpression(expr)->get_absoluteValue();
            if (useHex) {
                result = StringUtility::intToHex(v);
            } else {
                result = StringUtility::numberToString(v);
            }
            if (expr->get_comment().empty() && labels) {
                LabelMap::const_iterator li = labels->find(v);
                if (li!=labels->end())
                    result = StringUtility::appendAsmComment(result, li->second);
            }
            break;
        }
        default: {
            ASSERT_not_reachable("invalid PowerPC expression: " + expr->class_name());
        }
    }
    result = StringUtility::appendAsmComment(result, expr->get_comment());
    return result;
}

std::string
unparsePowerpcMnemonic(SgAsmPowerpcInstruction *insn) {
    ASSERT_not_null(insn);
    return insn->get_mnemonic();
}

std::string
unparsePowerpcExpression(SgAsmExpression *expr, const LabelMap *labels, const RegisterDictionary::Ptr &registers) {
    /* Find the instruction with which this expression is associated. */
    SgAsmPowerpcInstruction *insn = nullptr;
    for (SgNode *node=expr; !insn && node; node=node->get_parent()) {
        insn = isSgAsmPowerpcInstruction(node);
    }
    ASSERT_not_null(insn);

    PowerpcInstructionKind kind = insn->get_kind();
    bool isBranchTarget = (((kind == powerpc_b ||
                             kind == powerpc_bl ||
                             kind == powerpc_ba ||
                             kind == powerpc_bla) &&
                            expr==insn->operand(0)) ||
                           ((kind == powerpc_bc ||
                             kind == powerpc_bcl ||
                             kind == powerpc_bca ||
                             kind == powerpc_bcla) &&
                            insn->nOperands() >= 3 &&
                            expr==insn->operand(2)));
    return unparsePowerpcExpression(expr, labels, registers, isBranchTarget);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Powerpc
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
Powerpc::emitInstruction(std::ostream &out, SgAsmInstruction *insn_, State &state) const {
    ASSERT_not_null(isSgAsmPowerpcInstruction(insn_));
    Base::emitInstruction(out, insn_, state);
}

void
Powerpc::outputExpr(std::ostream &out, SgAsmExpression *expr, State &state) const {
    ASSERT_not_null(expr);
    std::vector<std::string> comments;

    if (SgAsmBinaryAdd *add = isSgAsmBinaryAdd(expr)) {
        outputExpr(out, add->get_lhs(), state);

        // Print the "+" and RHS only if RHS is non-zero
        SgAsmIntegerValueExpression *ival = isSgAsmIntegerValueExpression(add->get_rhs());
        if (!ival || !ival->get_bitVector().isAllClear()) {
            out <<" + ";
            outputExpr(out, add->get_rhs(), state);
        }

    } else if (SgAsmMemoryReferenceExpression *mre = isSgAsmMemoryReferenceExpression(expr)) {
        state.frontUnparser().emitTypeName(out, mre->get_type(), state);
        out <<" [";
        outputExpr(out, mre->get_address(), state);
        out <<"]";

    } else if (SgAsmDirectRegisterExpression *dre = isSgAsmDirectRegisterExpression(expr)) {
        state.frontUnparser().emitRegister(out, dre->get_descriptor(), state);

    } else if (SgAsmIntegerValueExpression *ive = isSgAsmIntegerValueExpression(expr)) {
        comments = state.frontUnparser().emitSignedInteger(out, ive->get_bitVector(), state);
    } else {
        ASSERT_not_implemented(expr->class_name());
    }

    if (!expr->get_comment().empty())
        comments.push_back(expr->get_comment());
    if (!comments.empty())
        out <<"<" + boost::join(comments, ",") <<">";
}

void
Powerpc::emitOperandBody(std::ostream &out, SgAsmExpression *expr, State &state) const {
    outputExpr(out, expr, state);
}

} // namespace
} // namespace
} // namespace

#endif
