#ifndef ROSE_BinaryAnalysis_Unparser_X86_H
#define ROSE_BinaryAnalysis_Unparser_X86_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Unparser/Base.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Unparser {

/** %Settings specific to the x86 unparser. */
struct X86Settings: public Settings {};

/** %Unparser for x86 instruction sets. */
class X86: public Base {
    X86Settings settings_;

protected:
    explicit X86(const Architecture::BaseConstPtr&, const X86Settings&);

public:
    ~X86();

public:
    static Ptr instance(const Architecture::BaseConstPtr&, const X86Settings& = X86Settings());

    Ptr copy() const override;

    const X86Settings& settings() const override { return settings_; }
    X86Settings& settings() override { return settings_; }

protected:
    void emitInstructionMnemonic(std::ostream&, SgAsmInstruction*, State&) const override;
    void emitTypeName(std::ostream&, SgAsmType*, State&) const override;
    std::vector<std::string> emitMemoryReferenceExpression(std::ostream&, SgAsmMemoryReferenceExpression*, State&) const override;
    std::vector<std::string> emitIndirectRegisterExpression(std::ostream&, SgAsmIndirectRegisterExpression*, State&) const override;
};

} // namespace
} // namespace
} // namespace

#endif
#endif
