#ifndef ROSE_BinaryAnalysis_UnparserX86_H
#define ROSE_BinaryAnalysis_UnparserX86_H

#include <BinaryUnparserBase.h>

namespace rose {
namespace BinaryAnalysis {
namespace Unparser {

struct SettingsX86: public SettingsBase {};

class UnparserX86: public UnparserBase {
    SettingsX86 settings_;

protected:
    UnparserX86() {}
    
    UnparserX86(const Partitioner2::Partitioner &p, const SettingsX86 &settings)
        : UnparserBase(p), settings_(settings) {}

public:
    static Ptr instance() {
        return Ptr(new UnparserX86);
    }

    static Ptr instance(const Partitioner2::Partitioner &p, const SettingsX86 &settings = SettingsX86()) {
        return Ptr(new UnparserX86(p, settings));
    }

    Ptr create(const Partitioner2::Partitioner &p) const ROSE_OVERRIDE {
        return instance(p);
    }

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
