#ifndef ROSE_BinaryAnalysis_UnparserArm_H
#define ROSE_BinaryAnalysis_UnparserArm_H

#include <BinaryUnparserBase.h>

namespace rose {
namespace BinaryAnalysis {
namespace Unparser {

struct SettingsArm: public SettingsBase {};

class UnparserArm: public UnparserBase {
    SettingsArm settings_;
public:
    UnparserArm(const Partitioner2::Partitioner &p, const SettingsArm &settings)
        : UnparserBase(p), settings_(settings) {}

    const SettingsArm& settings() const { return settings_; }
    SettingsArm& settings() { return settings_; }

protected:
    void emitInstructionMnemonic(std::ostream&, SgAsmInstruction*, State&) const ROSE_OVERRIDE;
    void emitOperandBody(std::ostream&, SgAsmExpression*, State&) const ROSE_OVERRIDE;

protected:
    static std::string unparseArmCondition(ArmInstructionCondition);
    void emitExpr(std::ostream&, SgAsmExpression*, State&, std::string sign, std::string *suffix = NULL) const;
};

} // namespace
} // namespace
} // namespace

#endif
