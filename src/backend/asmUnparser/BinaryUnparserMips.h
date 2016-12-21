#ifndef ROSE_BinaryAnalysis_UnparserMips_H
#define ROSE_BinaryAnalysis_UnparserMips_H

#include <BinaryUnparserBase.h>

namespace rose {
namespace BinaryAnalysis {
namespace Unparser {

struct SettingsMips: public SettingsBase {};

class UnparserMips: public UnparserBase {
    SettingsMips settings_;

protected:
    UnparserMips() {}

    UnparserMips(const Partitioner2::Partitioner &p, const SettingsMips &settings)
        : UnparserBase(p), settings_(settings) {}

public:
    static Ptr instance() {
        return Ptr(new UnparserMips);
    }

    static Ptr instance(const Partitioner2::Partitioner &p, const SettingsMips &settings = SettingsMips()) {
        return Ptr(new UnparserMips(p, settings));
    }

    Ptr create(const Partitioner2::Partitioner &p) const ROSE_OVERRIDE {
        return instance(p);
    }
    
    const SettingsMips& settings() const { return settings_; }
    SettingsMips& settings() { return settings_; }

protected:
    void emitInstruction(std::ostream&, SgAsmInstruction*, State&) const ROSE_OVERRIDE;
    void emitOperandBody(std::ostream&, SgAsmExpression*, State&) const ROSE_OVERRIDE;
};

} // namespace
} // namespace
} // namespace

#endif
