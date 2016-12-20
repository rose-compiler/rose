#ifndef ROSE_BinaryAnalysis_UnparserPowerpc_H
#define ROSE_BinaryAnalysis_UnparserPowerpc_H

#include <BinaryUnparserBase.h>

namespace rose {
namespace BinaryAnalysis {
namespace Unparser {

struct SettingsPowerpc: public SettingsBase {};

class UnparserPowerpc: public UnparserBase {
    SettingsPowerpc settings_;

public:
    UnparserPowerpc(const Partitioner2::Partitioner &p, const SettingsPowerpc &settings)
        : UnparserBase(p), settings_(settings) {}

    const SettingsPowerpc& settings() const { return settings_; }
    SettingsPowerpc& settings() { return settings_; }

protected:
    void emitOperandBody(std::ostream&, SgAsmExpression*, State &state) const ROSE_OVERRIDE;
};

} // namespace
} // namespace
} // namespace

#endif

