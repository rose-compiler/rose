#ifndef ROSE_BinaryAnalysis_Unparser_Aarch32_H
#define ROSE_BinaryAnalysis_Unparser_Aarch32_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_ASM_AARCH32

#include <Rose/BinaryAnalysis/BasicTypes.h>
#include <Rose/BinaryAnalysis/Unparser/Base.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Unparser {

std::string unparseAarch32Mnemonic(SgAsmAarch32Instruction*);
std::string unparseAarch32Expression(SgAsmExpression*, const LabelMap*);

/** %Settings specific to the ARM AArch32 unparser. */
struct Aarch32Settings: public Settings {};

/** ARM Aarch32 A32/T32 disassembler.
 *
 *  This class is able to unparse ARM AArch32 A32/T32 instructions. */
class Aarch32: public Base {
    Aarch32Settings settings_;

protected:
    explicit Aarch32(const Architecture::BaseConstPtr&, const Aarch32Settings&);

public:
    ~Aarch32();

    static Ptr instance(const Architecture::BaseConstPtr&, const Aarch32Settings& = Aarch32Settings());

    Ptr copy() const override;

    const Aarch32Settings& settings() const override { return settings_; }
    Aarch32Settings& settings() override { return settings_; }

protected:
    void emitInstructionMnemonic(std::ostream&, SgAsmInstruction*, State&) const override;
    void emitOperandBody(std::ostream&, SgAsmExpression*, State&) const override;

protected:
    void outputExpr(std::ostream&, SgAsmExpression*, State &) const;
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
