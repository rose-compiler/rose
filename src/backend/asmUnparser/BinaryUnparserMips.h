#ifndef ROSE_BinaryAnalysis_UnparserMips_H
#define ROSE_BinaryAnalysis_UnparserMips_H

#include <BinaryUnparserBase.h>

namespace rose {
namespace BinaryAnalysis {
namespace Unparser {

struct SettingsMips: public SettingsBase {};

class UnparserMips: public UnparserBase {
    SettingsMips settings_;

public:
    UnparserMips(const Partitioner2::Partitioner &p, const SettingsMips &settings)
        : UnparserBase(p), settings_(settings) {}

    const SettingsMips& settings() const { return settings_; }
    SettingsMips& settings() { return settings_; }

protected:
    void emitOperandBody(std::ostream&, SgAsmExpression*, State&) const ROSE_OVERRIDE;
};

} // namespace
} // namespace
} // namespace

#endif
