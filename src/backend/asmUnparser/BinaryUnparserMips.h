#ifndef ROSE_BinaryAnalysis_UnparserMips_H
#define ROSE_BinaryAnalysis_UnparserMips_H

#include <BinaryUnparserBase.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

namespace Rose {
namespace BinaryAnalysis {
namespace Unparser {

struct MipsSettings: public Settings {};

class Mips: public Base {
    MipsSettings settings_;

protected:
    explicit Mips(const MipsSettings &settings)
        : settings_(settings) {}

public:
    static Ptr instance(const MipsSettings &settings = MipsSettings()) {
        return Ptr(new Mips(settings));
    }

    Ptr copy() const ROSE_OVERRIDE {
        return instance(settings());
    }
    
    const MipsSettings& settings() const ROSE_OVERRIDE { return settings_; }
    MipsSettings& settings() ROSE_OVERRIDE { return settings_; }

protected:
    void emitInstruction(std::ostream&, SgAsmInstruction*, State&) const ROSE_OVERRIDE;
    void emitOperandBody(std::ostream&, SgAsmExpression*, State&) const ROSE_OVERRIDE;

private:
    void outputExpr(std::ostream&, SgAsmExpression*, State&) const;
};

} // namespace
} // namespace
} // namespace

#endif
#endif
