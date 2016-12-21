#ifndef ROSE_BinaryAnalysis_UnparserPowerpc_H
#define ROSE_BinaryAnalysis_UnparserPowerpc_H

#include <BinaryUnparserBase.h>

namespace rose {
namespace BinaryAnalysis {
namespace Unparser {

struct SettingsPowerpc: public SettingsBase {};

class UnparserPowerpc: public UnparserBase {
    SettingsPowerpc settings_;

protected:
    UnparserPowerpc() {}

    UnparserPowerpc(const Partitioner2::Partitioner &p, const SettingsPowerpc &settings)
        : UnparserBase(p), settings_(settings) {}

public:
    static Ptr instance() {
        return Ptr(new UnparserPowerpc);
    }

    static Ptr instance(const Partitioner2::Partitioner &p, const SettingsPowerpc &settings = SettingsPowerpc()) {
        return Ptr(new UnparserPowerpc(p, settings));
    }

    Ptr create(const Partitioner2::Partitioner &p) const ROSE_OVERRIDE {
        return instance(p);
    }
    
    const SettingsPowerpc& settings() const { return settings_; }
    SettingsPowerpc& settings() { return settings_; }

protected:
    void emitInstruction(std::ostream&, SgAsmInstruction*, State&) const ROSE_OVERRIDE;
    void emitOperandBody(std::ostream&, SgAsmExpression*, State &state) const ROSE_OVERRIDE;
};

} // namespace
} // namespace
} // namespace

#endif

