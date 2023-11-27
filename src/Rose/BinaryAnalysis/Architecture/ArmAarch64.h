#ifndef ROSE_BinaryAnalysis_Architecture_ArmAarch64_H
#define ROSE_BinaryAnalysis_Architecture_ArmAarch64_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_ASM_AARCH64
#include <Rose/BinaryAnalysis/Architecture/Base.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

/** Architecture-specific information for ARM AArch64.
 *
 *  AArch64 or ARM64 is the 64-bit extension of the ARM architecture family.  It was first introduced with the Armv8-A
 *  architecture. Arm releases a new extension every year. */
class ArmAarch64: public Base {
public:
    using Ptr = ArmAarch64Ptr;

protected:
    ArmAarch64();                                       // use `instance` instead
public:
    ~ArmAarch64();

public:
    /** Allocating constructor. */
    static Ptr instance();

public:
    RegisterDictionary::Ptr registerDictionary() const override;
    bool matchesHeader(SgAsmGenericHeader*) const override;
    Disassembler::BasePtr newInstructionDecoder() const override;
    Unparser::BasePtr newUnparser() const override;
};

} // namespace
} // namespace
} // namespace

#endif
#endif
