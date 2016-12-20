#ifndef ROSE_BinaryAnalysis_UnparserM68k_H
#define ROSE_BinaryAnalysis_UnparserM68k_H

#include <BinaryUnparserBase.h>

namespace rose {
namespace BinaryAnalysis {
namespace Unparser {

struct SettingsM68k: public SettingsBase {};

class UnparserM68k: public UnparserBase {
    SettingsM68k settings_;

public:
    UnparserM68k(const Partitioner2::Partitioner &p, const SettingsM68k &settings)
        : UnparserBase(p), settings_(settings) {}

    const SettingsM68k& settings() const { return settings_; }
    SettingsM68k& settings() { return settings_; }

protected:
    void emitOperandBody(std::ostream&, SgAsmExpression*, State&) const ROSE_OVERRIDE;
};

} // namespace
} // namespace
} // namespace

#endif
