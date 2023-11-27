#ifndef ROSE_BinaryAnalysis_Unparser_Mips_H
#define ROSE_BinaryAnalysis_Unparser_Mips_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Unparser/Base.h>

#include <Rose/BinaryAnalysis/BasicTypes.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Unparser {

std::string unparseMipsMnemonic(SgAsmMipsInstruction*);
std::string unparseMipsRegister(SgAsmInstruction*, RegisterDescriptor, RegisterDictionaryPtr);
std::string unparseMipsExpression(SgAsmExpression*, const LabelMap*, const RegisterDictionaryPtr&);

/** %Settings specific to the MIP unparser. */
struct MipsSettings: public Settings {};

/** %Unparser for the MIPS instruction set. */
class Mips: public Base {
    MipsSettings settings_;

protected:
    explicit Mips(const Architecture::BaseConstPtr&, const MipsSettings&);

public:
    ~Mips();

public:
    static Ptr instance(const Architecture::BaseConstPtr&, const MipsSettings& = MipsSettings());

    Ptr copy() const override;

    const MipsSettings& settings() const override { return settings_; }
    MipsSettings& settings() override { return settings_; }

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
