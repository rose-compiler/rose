#ifndef ROSE_BinaryAnalysis_Unparser_Cil_H
#define ROSE_BinaryAnalysis_Unparser_Cil_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Unparser/Base.h>

#include <Rose/BinaryAnalysis/BasicTypes.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Unparser {

std::string unparseCilMnemonic(SgAsmCilInstruction*);
std::string unparseCilExpression(SgAsmExpression*, const LabelMap*, RegisterDictionaryPtr);

struct CilSettings: public Settings {};

class Cil: public Base {
    CilSettings settings_;

protected:
    explicit Cil(const CilSettings &settings)
        : settings_(settings) {}

public:
    static Ptr instance(const CilSettings &settings = CilSettings()) {
        return Ptr(new Cil(settings));
    }

    Ptr copy() const override {
        return instance(settings());
    }
    
    const CilSettings& settings() const override { return settings_; }
    CilSettings& settings() override { return settings_; }

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
