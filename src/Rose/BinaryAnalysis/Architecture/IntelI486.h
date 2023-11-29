#ifndef ROSE_BinaryAnalysis_Architecture_IntelI486_H
#define ROSE_BinaryAnalysis_Architecture_IntelI486_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Architecture/X86.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

/** Architecture-specific information for Intel i486.
 *
 *  The Intel 486, officially named i486 and also known as 80486, is a microprocessor. It is a higher-performance follow-up to the
 *  Intel 386. The i486 was introduced in 1989. It represents the fourth generation of binary compatible CPUs following the 8086 of
 *  1978, the Intel 80286 of 1982, and 1985's i386.
 *
 *  It was the first tightly-pipelined x86 design as well as the first x86 chip to include more than one million transistors. It
 *  offered a large on-chip cache and an integrated floating-point unit.
 *
 *  When it was announced, the initial performance was originally published between 15 and 20 VAX MIPS, between 37,000 and 49,000
 *  dhrystones per second, and between 6.1 and 8.2 double-precision megawhetstones per second for both 25 and 33 MHz version. A
 *  typical 50 MHz i486 executes around 40 million instructions per second (MIPS), reaching 50 MIPS peak performance. It is
 *  approximately twice as fast as the i386 or i286 per clock cycle. The i486's improved performance is thanks to its five-stage
 *  pipeline with all stages bound to a single cycle. The enhanced FPU unit on the chip was significantly faster than the i387 FPU
 *  per cycle. The intel 80387 FPU ("i387") was a separate, optional math coprocessor that was installed in a motherboard socket
 *  alongside the i386.
 *
 *  The i486 was succeeded by the original Pentium. */
class IntelI486: public X86 {
public:
    using Ptr = IntelI486Ptr;

protected:
    IntelI486();                                        // use `instance` instead
public:
    ~IntelI486();

public:
    /** Allocating constructor. */
    static Ptr instance();

public:
    RegisterDictionary::Ptr registerDictionary() const override;
    bool matchesHeader(SgAsmGenericHeader*) const override;
};

} // namespace
} // namespace
} // namespace

#endif
#endif
