#ifndef ROSE_BinaryAnalysis_Unparser_Aarch64_H
#define ROSE_BinaryAnalysis_Unparser_Aarch64_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_ASM_AARCH64

#include <Rose/BinaryAnalysis/Unparser/Base.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Unparser {

/** %Settings specific to the ARM AArch64 unparser. */
struct Aarch64Settings: public Settings {};

/** ARM AArch64 A64 disassembler.
 *
 *  This class is able to unparse ARM AArch64 A64 instructions. */
class Aarch64: public Base {
    Aarch64Settings settings_;

protected:
    explicit Aarch64(const Architecture::BaseConstPtr&, const Aarch64Settings&);

public:
    ~Aarch64();

    static Ptr instance(const Architecture::BaseConstPtr&, const Aarch64Settings& = Aarch64Settings());

    Ptr copy() const override;

    const Aarch64Settings& settings() const override { return settings_; }
    Aarch64Settings& settings() override { return settings_; }

protected:
    void emitRegister(std::ostream&, RegisterDescriptor, State&) const override;
};

} // namespace
} // namespace
} // namespace

#endif
#endif
