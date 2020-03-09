#ifndef ROSE_BinaryAnalysis_UnparserArm_H
#define ROSE_BinaryAnalysis_UnparserArm_H

#include <BinaryUnparserBase.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

namespace Rose {
namespace BinaryAnalysis {
namespace Unparser {

struct ArmSettings: public Settings {};

class Arm: public Base {
    ArmSettings settings_;

protected:
    explicit Arm(const ArmSettings &settings)
        : settings_(settings) {}

public:
    ~Arm() {}

    static Ptr instance(const ArmSettings &settings = ArmSettings()) {
        return Ptr(new Arm(settings));
    }

    Ptr copy() const ROSE_OVERRIDE {
        return instance(settings());
    }
    
    const ArmSettings& settings() const ROSE_OVERRIDE { return settings_; }
    ArmSettings& settings() ROSE_OVERRIDE { return settings_; }

protected:
    void emitInstructionMnemonic(std::ostream&, SgAsmInstruction*, State&) const ROSE_OVERRIDE;
    void emitOperandBody(std::ostream&, SgAsmExpression*, State&) const ROSE_OVERRIDE;

protected:
    static std::string unparseArmCondition(ArmInstructionCondition);
    void outputExpr(std::ostream&, SgAsmExpression*, State&, std::string sign, std::string *suffix = NULL) const;
};

} // namespace
} // namespace
} // namespace

#endif
#endif
