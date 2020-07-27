#ifndef ROSE_BinaryAnalysis_UnparserArm_H
#define ROSE_BinaryAnalysis_UnparserArm_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_ASM_A64

#include <BinaryUnparserBase.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Unparser {

struct ArmSettings: public Settings {};

/** ARM AArch64 A64 disassembler.
 *
 *  This class is able to decode a single ARM AArch64 A64 instruction at a time. */
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
    static std::string unparseArmCondition(A64InstructionCondition);
    void outputExpr(std::ostream&, SgAsmExpression*, State&) const;
    void outputRegister(std::ostream&, SgAsmRegisterReferenceExpression*, State&) const;
};

} // namespace
} // namespace
} // namespace

#endif
#endif
