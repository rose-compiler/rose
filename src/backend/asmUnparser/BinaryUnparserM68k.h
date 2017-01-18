#ifndef ROSE_BinaryAnalysis_UnparserM68k_H
#define ROSE_BinaryAnalysis_UnparserM68k_H

#include <BinaryUnparserBase.h>

namespace rose {
namespace BinaryAnalysis {
namespace Unparser {

struct M68kSettings: public Settings {};

class M68k: public Base {
    M68kSettings settings_;

protected:
    explicit M68k(const M68kSettings &settings)
        : settings_(settings) {}

public:
    static Ptr instance(const M68kSettings &settings = M68kSettings()) {
        return Ptr(new M68k(settings));
    }

    Ptr copy() const ROSE_OVERRIDE {
        return instance(settings());
    }
    
    const M68kSettings& settings() const { return settings_; }
    M68kSettings& settings() { return settings_; }

protected:
    void emitInstruction(std::ostream&, SgAsmInstruction*, State&) const ROSE_OVERRIDE;
    void emitOperandBody(std::ostream&, SgAsmExpression*, State&) const ROSE_OVERRIDE;
};

} // namespace
} // namespace
} // namespace

#endif
