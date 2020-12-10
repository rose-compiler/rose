#ifndef ROSE_BinaryAnalysis_UnparserPowerpc_H
#define ROSE_BinaryAnalysis_UnparserPowerpc_H

#include <BinaryUnparserBase.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

namespace Rose {
namespace BinaryAnalysis {
namespace Unparser {

struct PowerpcSettings: public Settings {};

class Powerpc: public Base {
    PowerpcSettings settings_;

protected:
    explicit Powerpc(const PowerpcSettings &settings)
        : settings_(settings) {}

public:
    static Ptr instance(const PowerpcSettings &settings = PowerpcSettings()) {
        return Ptr(new Powerpc(settings));
    }

    Ptr copy() const ROSE_OVERRIDE {
        return instance(settings());
    }
    
    const PowerpcSettings& settings() const ROSE_OVERRIDE { return settings_; }
    PowerpcSettings& settings() ROSE_OVERRIDE { return settings_; }

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
