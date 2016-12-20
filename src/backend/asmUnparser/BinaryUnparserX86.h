#ifndef ROSE_BinaryAnalysis_UnparserX86_H
#define ROSE_BinaryAnalysis_UnparserX86_H

#include <BinaryUnparserBase.h>

namespace rose {
namespace BinaryAnalysis {
namespace Unparser {

struct SettingsX86: public SettingsBase {};

class UnparserX86: public UnparserBase {
    SettingsX86 settings_;

public:
    UnparserX86(const Partitioner2::Partitioner &p, const SettingsX86 &settings)
        : UnparserBase(p), settings_(settings) {}

    const SettingsX86& settings() const { return settings_; }
    SettingsX86& settings() { return settings_; }

protected:
    void emitInstructionMnemonic(std::ostream&, SgAsmInstruction*, State&) const ROSE_OVERRIDE;
    void emitOperandBody(std::ostream&, SgAsmExpression*, State&) const ROSE_OVERRIDE;
    void emitTypeName(std::ostream&, SgAsmType*, State&) const ROSE_OVERRIDE;
};

} // namespace
} // namespace
} // namespace

#endif
