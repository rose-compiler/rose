#ifndef ROSE_BinaryAnalysis_Unparser_Aarch32_H
#define ROSE_BinaryAnalysis_Unparser_Aarch32_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_ASM_AARCH32

#include <BinaryUnparserBase.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Unparser {

struct Aarch32Settings: public Settings {};

/** ARM Aarch32 A32/T32 disassembler.
 *
 *  This class is able to unparse ARM AArch32 A32/T32 instructions. */
class Aarch32: public Base {
    Aarch32Settings settings_;

protected:
    explicit Aarch32(const Aarch32Settings &settings)
        : settings_(settings) {}

public:
    ~Aarch32() {}

    static Ptr instance(const Aarch32Settings &settings = Aarch32Settings()) {
        return Ptr(new Aarch32(settings));
    }

    Ptr copy() const ROSE_OVERRIDE {
        return instance(settings());
    }

    const Aarch32Settings& settings() const ROSE_OVERRIDE { return settings_; }
    Aarch32Settings& settings() ROSE_OVERRIDE { return settings_; }

protected:
    void emitInstructionMnemonic(std::ostream&, SgAsmInstruction*, State&) const override;
    void emitOperandBody(std::ostream&, SgAsmExpression*, State&) const override;

protected:
    void outputExpr(std::ostream&, SgAsmExpression*, State &) const;
    void outputRegister(std::ostream&, SgAsmRegisterReferenceExpression*, State&) const;
};

} // namespace
} // namespace
} // namespace

#endif
#endif
