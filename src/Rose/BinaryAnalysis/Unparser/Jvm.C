#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Unparser/Jvm.h>

#include <Rose/BinaryAnalysis/Architecture/Base.h>

#include <SgAsmBinaryAdd.h>
#include <SgAsmBinaryMultiply.h>
#include <SgAsmIntegerValueExpression.h>
#include <SgAsmJvmInstruction.h>
#include <SgAsmMemoryReferenceExpression.h>

#include <Cxx_GrammarDowncast.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Unparser {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Supporting functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::string
unparseJvmMnemonic(SgAsmJvmInstruction* insn) {
    ASSERT_not_null(insn);
    return insn->get_mnemonic();
}

std::string
unparseJvmExpression(SgAsmExpression* expr, const LabelMap* labels, RegisterDictionary::Ptr registers) {
    std::string result = "";
    if (expr == nullptr) return "BOGUS:NULL";

    if (SgAsmMemoryReferenceExpression* mre = isSgAsmMemoryReferenceExpression(expr)) {
        result = "[" + unparseJvmExpression(mre->get_address(), labels, registers) + "]";
    } else if (SgAsmBinaryAdd* add = isSgAsmBinaryAdd(expr)) {
        result = unparseJvmExpression(add->get_lhs(), labels, registers) + "+" +
                 unparseJvmExpression(add->get_rhs(), labels, registers);
    } else if (SgAsmBinaryMultiply* mul = isSgAsmBinaryMultiply(expr)) {
        result = unparseJvmExpression(mul->get_lhs(), labels, registers) + "*" +
                 unparseJvmExpression(mul->get_rhs(), labels, registers);
    } else if (SgAsmIntegerValueExpression* ival = isSgAsmIntegerValueExpression(expr)) {
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
    } else {
        result = "<UNHANDLED_EXPRESSION type=" + expr->class_name() + ">";
    }

    result = StringUtility::appendAsmComment(result, expr->get_comment());
    return result;
}

Jvm::~Jvm() {}

Jvm::Jvm(const Architecture::Base::ConstPtr &arch, const JvmSettings &settings)
    : Base(arch), settings_(settings) {}

Jvm::Ptr
Jvm::instance(const Architecture::Base::ConstPtr &arch, const JvmSettings &settings) {
    return Ptr(new Jvm(arch, settings));
}

Base::Ptr
Jvm::copy() const {
    return instance(architecture(), settings());
}

void
Jvm::emitInstructionMnemonic(std::ostream &out, SgAsmInstruction *insn, State&) const {
    if (insn) {
        out << insn->get_mnemonic();
    }
}

void
Jvm::emitOperandBody(std::ostream &out, SgAsmExpression *expr, State &state) const {
    outputExpr(out, expr, state);
}

void
Jvm::outputExpr(std::ostream &out, SgAsmExpression *expr, State &state) const {
    ASSERT_not_null(expr);
    std::vector<std::string> comments;

    if (SgAsmMemoryReferenceExpression *mre = isSgAsmMemoryReferenceExpression(expr)) {
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

    } else {
        ASSERT_not_implemented(expr->class_name());
    }

    if (!expr->get_comment().empty())
        comments.push_back(expr->get_comment());
    if (!comments.empty())
        out <<"<" + boost::join(comments, ",") <<">";
}

} // namespace
} // namespace
} // namespace

#endif
