#ifndef ROSE_BinaryAnalysis_UnparserM68k_H
#define ROSE_BinaryAnalysis_UnparserM68k_H

#include <BinaryUnparserBase.h>

namespace rose {
namespace BinaryAnalysis {
namespace Unparser {

struct SettingsM68k: public SettingsBase {};

class UnparserM68k: public UnparserBase {
    SettingsM68k settings_;

protected:
    UnparserM68k() {}

    UnparserM68k(const Partitioner2::Partitioner &p, const SettingsM68k &settings)
        : UnparserBase(p), settings_(settings) {}

public:
    static Ptr instance() {
        return Ptr(new UnparserM68k);
    }

    static Ptr instance(const Partitioner2::Partitioner &p, const SettingsM68k &settings = SettingsM68k()) {
        return Ptr(new UnparserM68k(p, settings));
    }

    Ptr create(const Partitioner2::Partitioner &p) const ROSE_OVERRIDE {
        return instance(p);
    }
    
    const SettingsM68k& settings() const { return settings_; }
    SettingsM68k& settings() { return settings_; }

protected:
    void emitInstruction(std::ostream&, SgAsmInstruction*, State&) const ROSE_OVERRIDE;
    void emitOperandBody(std::ostream&, SgAsmExpression*, State&) const ROSE_OVERRIDE;
};

} // namespace
} // namespace
} // namespace

#endif
