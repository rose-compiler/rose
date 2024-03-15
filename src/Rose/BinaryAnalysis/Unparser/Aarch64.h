#ifndef ROSE_BinaryAnalysis_Unparser_Aarch64_H
#define ROSE_BinaryAnalysis_Unparser_Aarch64_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_ASM_AARCH64

#include <Rose/BinaryAnalysis/BasicTypes.h>
#include <Rose/BinaryAnalysis/InstructionEnumsAarch64.h>
#include <Rose/BinaryAnalysis/Unparser/Base.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Unparser {

std::string unparseAarch64Mnemonic(SgAsmAarch64Instruction*);
std::string unparseAarch64Expression(SgAsmExpression*, const LabelMap*, RegisterDictionaryPtr);

/** %Settings specific to the ARM AArch64 unparser. */
struct Aarch64Settings: public Settings {};

/** ARM AArch64 A64 disassembler.
 *
 *  This class is able to unparse ARM AArch64 A64 instructions. */
class Aarch64: public Base {
    Aarch64Settings settings_;

protected:
    explicit Aarch64(const Architecture::BaseConstPtr&, const Aarch64Settings&);

public:
    ~Aarch64();

    static Ptr instance(const Architecture::BaseConstPtr&, const Aarch64Settings& = Aarch64Settings());

    Ptr copy() const override;

    const Aarch64Settings& settings() const override { return settings_; }
    Aarch64Settings& settings() override { return settings_; }

protected:
    void emitInstructionMnemonic(std::ostream&, SgAsmInstruction*, State&) const override;
    void emitOperandBody(std::ostream&, SgAsmExpression*, State&) const override;

protected:
    static std::string unparseArmCondition(Aarch64InstructionCondition);
    void outputExpr(std::ostream&, SgAsmExpression*, State&) const;
    void outputRegister(std::ostream&, SgAsmRegisterReferenceExpression*, State&) const;

private:
    // Precedence for the sake of emitting expressions. Higher return value is higher precedence.
    static int operatorPrecedence(SgAsmExpression*);

    // Parentheses for emitting expressions with inverted precedences
    struct Parens {
        std::string left, right;
        Parens() {}
        Parens(const std::string &left, const std::string &right)
            : left(left), right(right) {}
    };

    // What parens to use when the operator has rootPrec and the left or right operand is as specified.
    static Parens parensForPrecedence(int rootPrec, SgAsmExpression*);
};

} // namespace
} // namespace
} // namespace

#endif
#endif
