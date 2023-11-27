#ifndef ROSE_BinaryAnalysis_Unparser_M68k_H
#define ROSE_BinaryAnalysis_Unparser_M68k_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Unparser/Base.h>

#include <Rose/BinaryAnalysis/BasicTypes.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Unparser {

std::string unparseM68kMnemonic(SgAsmM68kInstruction*);
std::string unparseM68kExpression(SgAsmExpression*, const LabelMap*, RegisterDictionaryPtr);

/** %Settings specific to the M68k unparser. */
struct M68kSettings: public Settings {};

/** %Unparser for Motorola M68k and related instruction sets. */
class M68k: public Base {
    M68kSettings settings_;

protected:
    explicit M68k(const Architecture::BaseConstPtr&, const M68kSettings&);

public:
    ~M68k();

public:
    static Ptr instance(const Architecture::BaseConstPtr&, const M68kSettings& = M68kSettings());

    Ptr copy() const override;

    const M68kSettings& settings() const override { return settings_; }
    M68kSettings& settings() override { return settings_; }

protected:
    void emitInstruction(std::ostream&, SgAsmInstruction*, State&) const override;
    void emitOperandBody(std::ostream&, SgAsmExpression*, State&) const override;

private:
    void outputExpr(std::ostream&, SgAsmExpression*, State&) const;
};

} // namespace
} // namespace
} // namespace

#endif
#endif
