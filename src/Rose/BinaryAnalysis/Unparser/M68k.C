#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Unparser/M68k.h>

#include <Rose/BinaryAnalysis/Architecture/Base.h>
#include <Rose/BinaryAnalysis/RegisterDictionary.h>

#include <SgAsmBinaryAdd.h>
#include <SgAsmBinaryMultiply.h>
#include <SgAsmIntegerValueExpression.h>
#include <SgAsmM68kInstruction.h>
#include <SgAsmMemoryReferenceExpression.h>
#include <SgAsmRegisterNames.h>
#include <SgAsmRegisterReferenceExpression.h>

#include <Cxx_GrammarDowncast.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Unparser {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Supporting functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::string
unparseM68kMnemonic(SgAsmM68kInstruction *insn) {
    assert(insn!=nullptr);
    return insn->get_mnemonic();
}

std::string
unparseM68kExpression(SgAsmExpression *expr, const LabelMap *labels, RegisterDictionary::Ptr registers) {
    if (!registers)
        registers = Architecture::findByName("nxp-coldfire").orThrow()->registerDictionary();
    RegisterNames name_of(registers);
    std::string result = "";
    if (expr==nullptr)
        return "BUGUS:NULL";

    if (SgAsmRegisterReferenceExpression *rre = isSgAsmRegisterReferenceExpression(expr)) {
        int adjustment = rre->get_adjustment();
        if (adjustment < 0)
            result = "--";
        result += name_of(rre->get_descriptor());
        if (adjustment > 0)
            result += "++";
    } else if (SgAsmMemoryReferenceExpression *mre = isSgAsmMemoryReferenceExpression(expr)) {
        result = "[" + unparseM68kExpression(mre->get_address(), labels, registers) + "]";
    } else if (SgAsmBinaryAdd *add = isSgAsmBinaryAdd(expr)) {
        result = unparseM68kExpression(add->get_lhs(), labels, registers) + "+" +
                 unparseM68kExpression(add->get_rhs(), labels, registers);
    } else if (SgAsmBinaryMultiply *mul = isSgAsmBinaryMultiply(expr)) {
        result = unparseM68kExpression(mul->get_lhs(), labels, registers) + "*" +
                 unparseM68kExpression(mul->get_rhs(), labels, registers);
    } else if (SgAsmIntegerValueExpression *ival = isSgAsmIntegerValueExpression(expr)) {
        assert(ival!=nullptr);
        uint64_t value = ival->get_absoluteValue(); // not sign extended
        result = StringUtility::signedToHex2(value, ival->get_significantBits());

        // Optional label.  Prefer a label supplied by the caller's LabelMap, but not for single-byte constants.  If
        // there's no caller-supplied label, then consider whether the value expression is relative to some other IR node.
        if (expr->get_comment().empty()) {
            std::string label;
            if (ival->get_significantBits()>8) {
                if (0!=value && labels!=nullptr) {
                    LabelMap::const_iterator li = labels->find(value);
                    if (li!=labels->end())
                        label = li->second;
                }
            }
            if (label.empty())
                label = ival->get_label();
            result = StringUtility::appendAsmComment(result, label);
        }

    } else if (SgAsmRegisterNames *regs = isSgAsmRegisterNames(expr)) {
        // The usual assembly is to show only an integer register mask.  That's not very friendly, especially since the meaning
        // of the bits is dependent on the addressing mode of the other instruction.  So we show the register names instead in
        // curly braces.
        int nregs = 0;
        result = "{";
        for (SgAsmRegisterReferenceExpression *rre: regs->get_registers())
            result += (nregs++ ? ", " : "") + unparseM68kExpression(rre, labels, registers);
        result += "}";
        if (regs->get_mask()!=0)
            result += "<" + StringUtility::toHex2(regs->get_mask(), 16, false, false) + ">";
    } else {
        result = "<UNHANDLED_EXPRESSION type=" + expr->class_name() + ">";
    }

    result = StringUtility::appendAsmComment(result, expr->get_comment());
    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// M68k
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

M68k::~M68k() {}

M68k::M68k(const Architecture::Base::ConstPtr &arch, const M68kSettings &settings)
    : Base(arch), settings_(settings) {}

M68k::Ptr
M68k::instance(const Architecture::Base::ConstPtr &arch, const M68kSettings &settings) {
    return Ptr(new M68k(arch, settings));
}

Base::Ptr
M68k::copy() const {
    return instance(architecture(), settings());
}

void
M68k::emitInstruction(std::ostream &out, SgAsmInstruction *insn_, State &state) const {
    SgAsmM68kInstruction *insn = isSgAsmM68kInstruction(insn_);
    ASSERT_always_not_null2(insn, "not an m68k instruction");
    Base::emitInstruction(out, insn_, state);
}

void
M68k::outputExpr(std::ostream &out, SgAsmExpression *expr, State &state) const {
    ASSERT_not_null(expr);
    std::vector<std::string> comments;

    if (SgAsmRegisterReferenceExpression *rre = isSgAsmRegisterReferenceExpression(expr)) {
        int adjustment = rre->get_adjustment();
        if (adjustment < 0)
            out <<"--";
        state.frontUnparser().emitRegister(out, rre->get_descriptor(), state);
        if (adjustment > 0)
            out <<"++";

    } else if (SgAsmMemoryReferenceExpression *mre = isSgAsmMemoryReferenceExpression(expr)) {
        state.frontUnparser().emitTypeName(out, expr->get_type(), state);
        out <<" [";
        outputExpr(out, mre->get_address(), state);
        out <<"]";

    } else if (SgAsmBinaryAdd *add = isSgAsmBinaryAdd(expr)) {
        outputExpr(out, add->get_lhs(), state);
        out <<"+";
        outputExpr(out, add->get_rhs(), state);

    } else if (SgAsmBinaryMultiply *mul = isSgAsmBinaryMultiply(expr)) {
        outputExpr(out, mul->get_lhs(), state);
        out <<"*";
        outputExpr(out, mul->get_rhs(), state);

    } else if (SgAsmIntegerValueExpression *ival = isSgAsmIntegerValueExpression(expr)) {
        comments = state.frontUnparser().emitSignedInteger(out, ival->get_bitVector(), state);

    } else if (SgAsmRegisterNames *regs = isSgAsmRegisterNames(expr)) {
        // The usual assembly is to show only an integer register mask.  That's not very friendly, especially since the meaning
        // of the bits is dependent on the addressing mode of the other instruction.  So we show the register names instead in
        // curly braces.
        int nregs = 0;
        out <<"{";
        for (SgAsmRegisterReferenceExpression *rre: regs->get_registers()) {
            if (++nregs > 1)
                out <<", ";
            outputExpr(out, rre, state);
        }
        out <<"}";
        if (regs->get_mask()!=0)
            comments.push_back(StringUtility::toHex2(regs->get_mask(), 16, false, false));

    } else {
        ASSERT_not_implemented(expr->class_name());
    }

    if (!expr->get_comment().empty())
        comments.push_back(expr->get_comment());
    if (!comments.empty())
        out <<"<" + boost::join(comments, ",") <<">";
}
void
M68k::emitOperandBody(std::ostream &out, SgAsmExpression *expr, State &state) const {
    outputExpr(out, expr, state);
}

} // namespace
} // namespace
} // namespace

#endif
