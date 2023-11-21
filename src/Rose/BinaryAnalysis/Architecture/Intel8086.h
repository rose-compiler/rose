#ifndef ROSE_BinaryAnalysis_Architecture_Intel8086_H
#define ROSE_BinaryAnalysis_Architecture_Intel8086_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Architecture/Base.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

/** Architecture-specific information for Intel 8086.
 *
 *  The 8086 (also called iAPX 86) is a 16-bit microprocessor chip designed by Intel between early 1976 and June 8, 1978, when it
 *  was released. The Intel 8088, released July 1, 1979, is a slightly modified chip with an external 8-bit data bus (allowing the
 *  use of cheaper and fewer supporting ICs), and is notable as the processor used in the original IBM PC design.
 *
 *  The 8086 gave rise to the x86 architecture, which eventually became Intel's most successful line of processors. On June 5, 2018,
 *  Intel released a limited-edition CPU celebrating the 40th anniversary of the Intel 8086, called the Intel Core i7-8086K. */
class Intel8086: public Base {
public:
    using Ptr = Intel8086Ptr;

protected:
    Intel8086();                                        // use `instance` instead
public:
    ~Intel8086();

public:
    /** Allocating constructor. */
    static Ptr instance();

public:
    RegisterDictionary::Ptr registerDictionary() const override;
    bool matchesHeader(SgAsmGenericHeader*) const override;
    Disassembler::BasePtr newInstructionDecoder() const override;
};

} // namespace
} // namespace
} // namespace

#endif
#endif
