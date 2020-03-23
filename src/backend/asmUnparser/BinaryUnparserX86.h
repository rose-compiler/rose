#ifndef ROSE_BinaryAnalysis_UnparserX86_H
#define ROSE_BinaryAnalysis_UnparserX86_H

#include <BinaryUnparserBase.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

namespace Rose {
namespace BinaryAnalysis {
namespace Unparser {

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
