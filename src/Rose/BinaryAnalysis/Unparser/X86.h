#ifndef ROSE_BinaryAnalysis_Unparser_X86_H
#define ROSE_BinaryAnalysis_Unparser_X86_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Unparser/Base.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Unparser {

std::string unparseX86Mnemonic(SgAsmX86Instruction*);
std::string unparseX86Register(SgAsmInstruction*, RegisterDescriptor, const RegisterDictionary*);
std::string unparseX86Register(RegisterDescriptor, const RegisterDictionary*);
std::string unparseX86Expression(SgAsmExpression*, const LabelMap*, const RegisterDictionary*, bool leaMode);
std::string unparseX86Expression(SgAsmExpression*, const LabelMap*, const RegisterDictionary*);

struct X86Settings: public Settings {};

class X86: public Base {
    X86Settings settings_;

protected:
    explicit X86(const X86Settings &settings)
        : settings_(settings) {}

public:
    static Ptr instance(const X86Settings &settings = X86Settings()) {
        return Ptr(new X86(settings));
    }

    Ptr copy() const ROSE_OVERRIDE {
        return instance(settings());
    }

    const X86Settings& settings() const ROSE_OVERRIDE { return settings_; }
    X86Settings& settings() ROSE_OVERRIDE { return settings_; }

protected:
    void emitInstructionMnemonic(std::ostream&, SgAsmInstruction*, State&) const ROSE_OVERRIDE;
    void emitOperandBody(std::ostream&, SgAsmExpression*, State&) const ROSE_OVERRIDE;
    void emitTypeName(std::ostream&, SgAsmType*, State&) const ROSE_OVERRIDE;

private:
    void outputExpr(std::ostream&, SgAsmExpression*, State&) const;
};

} // namespace
} // namespace
} // namespace

#endif
#endif
