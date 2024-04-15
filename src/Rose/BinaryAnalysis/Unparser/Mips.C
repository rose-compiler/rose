#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Unparser/Mips.h>

#include <Rose/AST/Traversal.h>
#include <Rose/BinaryAnalysis/Architecture/Base.h>
#include <Rose/BinaryAnalysis/RegisterDictionary.h>

#include <SgAsmBinaryAdd.h>
#include <SgAsmDirectRegisterExpression.h>
#include <SgAsmIntegerType.h>
#include <SgAsmIntegerValueExpression.h>
#include <SgAsmMemoryReferenceExpression.h>
#include <SgAsmMipsInstruction.h>

#include <Cxx_GrammarDowncast.h>

using namespace Sawyer::Message::Common;

namespace Rose {
namespace BinaryAnalysis {
namespace Unparser {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Supporting functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Returns a string containing everthing before the first operand in a typical x86 assembly statement. */
std::string
unparseMipsMnemonic(SgAsmMipsInstruction *insn) {
    ASSERT_not_null(insn);
    return insn->get_mnemonic();
}

/** Returns the name of a MIPS register.
 *
 * FIXME: This assumes MIPS32 */
std::string
unparseMipsRegister(SgAsmInstruction *insn, RegisterDescriptor reg, RegisterDictionary::Ptr registers) {
    using namespace StringUtility;
    if (!registers)
        registers = Architecture::findByName("mips32-be").orThrow()->registerDictionary();
    std::string name = registers->lookup(reg);
    if (name.empty())
        name = invalidRegister(insn, reg, registers);
    return name;
}

static std::string
mipsValToLabel(uint64_t val, const LabelMap *labels) {
    if (!val || !labels)
        return "";

    LabelMap::const_iterator li = labels->find(val);
    if (li==labels->end())
        return "";

    return li->second;
}

static std::string
mipsTypeToPtrName(SgAsmType* ty) {
    if (nullptr==ty) {
        mlog[ERROR] <<"mipsTypeToPtrName: null type\n";
        return "BAD_TYPE";
    }

    if (SgAsmIntegerType *it = isSgAsmIntegerType(ty)) {
        switch (it->get_nBits()) {
            case 8: return "BYTE";
            case 16: return "HALFWORD";
            case 32: return "WORD";
            case 64: return "DOUBLEWORD";
        }
    }
    ASSERT_not_reachable("invalid MIPS type: " + ty->toString());
}

std::string
unparseMipsExpression(SgAsmExpression *expr, const LabelMap *labels, const RegisterDictionary::Ptr &registers) {
    std::string result = "";
    if (expr == nullptr) return "BOGUS:NULL";

    switch (expr->variantT()) {
        case V_SgAsmBinaryAdd:
            result = unparseMipsExpression(isSgAsmBinaryExpression(expr)->get_lhs(), labels, registers) + " + " +
                     unparseMipsExpression(isSgAsmBinaryExpression(expr)->get_rhs(), labels, registers);
            break;

        case V_SgAsmMemoryReferenceExpression: {
            SgAsmMemoryReferenceExpression* mr = isSgAsmMemoryReferenceExpression(expr);
            result = mipsTypeToPtrName(mr->get_type()) + " [" +
                     unparseMipsExpression(mr->get_address(), labels, registers) + "]";
            break;
        }

        case V_SgAsmDirectRegisterExpression: {
            SgAsmInstruction *insn = AST::Traversal::findParentTyped<SgAsmInstruction>(expr);
            SgAsmDirectRegisterExpression* rr = isSgAsmDirectRegisterExpression(expr);
            result = unparseMipsRegister(insn, rr->get_descriptor(), registers);
            break;
        }

        case V_SgAsmIntegerValueExpression: {
            SgAsmIntegerValueExpression *ival = isSgAsmIntegerValueExpression(expr);
            ASSERT_not_null(ival);
            uint64_t value = ival->get_absoluteValue(); // not sign extended
            result = StringUtility::signedToHex2(value, ival->get_significantBits());

            // Optional label.  Prefer a label supplied by the caller's LabelMap, but not for single-byte constants.  If
            // there's no caller-supplied label, then consider whether the value expression is relative to some other IR node.
            if (expr->get_comment().empty()) {
                std::string label;
                if (ival->get_significantBits()>8)
                    label = mipsValToLabel(value, labels);
                if (label.empty())
                    label = ival->get_label();
                result = StringUtility::appendAsmComment(result, label);
            }
            break;
        }

        default: {
            ASSERT_not_reachable("invalid MIPS expression: " + expr->class_name());
        }
    }

    result = StringUtility::appendAsmComment(result, expr->get_comment());
    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Mips
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Mips::~Mips() {}

Mips::Mips(const Architecture::Base::ConstPtr &arch, const MipsSettings &settings)
    : Base(arch), settings_(settings) {}

Mips::Ptr
Mips::instance(const Architecture::Base::ConstPtr &arch, const MipsSettings &settings) {
    return Ptr(new Mips(arch, settings));
}

Base::Ptr
Mips::copy() const {
    return instance(architecture(), settings());
}

void
Mips::emitInstruction(std::ostream &out, SgAsmInstruction *insn_, State &state) const {
    ASSERT_not_null(isSgAsmMipsInstruction(insn_));
    Base::emitInstruction(out, insn_, state);
}

void
Mips::outputExpr(std::ostream &out, SgAsmExpression *expr, State &state) const {
    ASSERT_not_null(expr);
    std::vector<std::string> comments;

    if (SgAsmBinaryAdd *add = isSgAsmBinaryAdd(expr)) {
        outputExpr(out, add->get_lhs(), state);
        out <<" + ";
        outputExpr(out, add->get_rhs(), state);

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
Mips::emitOperandBody(std::ostream &out, SgAsmExpression *expr, State &state) const {
    outputExpr(out, expr, state);
}

} // namespace
} // namespace
} // namespace

#endif
