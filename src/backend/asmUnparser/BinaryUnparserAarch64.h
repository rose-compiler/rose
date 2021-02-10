#ifndef ROSE_BinaryAnalysis_Unparser_Aarch64_H
#define ROSE_BinaryAnalysis_Unparser_Aarch64_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_ASM_AARCH64

#include <BinaryUnparserBase.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Unparser {

struct Aarch64Settings: public Settings {};

/** ARM AArch64 A64 disassembler.
 *
 *  This class is able to unparse ARM AArch64 A64 instructions. */
class Aarch64: public Base {
    Aarch64Settings settings_;

protected:
    explicit Aarch64(const Aarch64Settings &settings)
        : settings_(settings) {}

public:
    ~Aarch64() {}

    static Ptr instance(const Aarch64Settings &settings = Aarch64Settings()) {
        return Ptr(new Aarch64(settings));
    }

    Ptr copy() const ROSE_OVERRIDE {
        return instance(settings());
    }
    
    const Aarch64Settings& settings() const ROSE_OVERRIDE { return settings_; }
    Aarch64Settings& settings() ROSE_OVERRIDE { return settings_; }

protected:
    void emitInstructionMnemonic(std::ostream&, SgAsmInstruction*, State&) const ROSE_OVERRIDE;
    void emitOperandBody(std::ostream&, SgAsmExpression*, State&) const ROSE_OVERRIDE;

protected:
    static std::string unparseArmCondition(Aarch64InstructionCondition);
    void outputExpr(std::ostream&, SgAsmExpression*, State&) const;
    void outputRegister(std::ostream&, SgAsmRegisterReferenceExpression*, State&) const;
};

} // namespace
} // namespace
} // namespace

#endif
#endif
