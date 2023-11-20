#ifndef ROSE_BinaryAnalysis_Architecture_Motorola68040_H
#define ROSE_BinaryAnalysis_Architecture_Motorola68040_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Architecture/Base.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

/** Architecture-specific information for Motorola 68040'
 *
 *  The Motorola 68040 ("sixty-eight-oh-forty") is a 32-bit microprocessor in the Motorola 68000 series, released in 1990. It is the
 *  successor to the 68030 and is followed by the 68060, skipping the 68050. In keeping with general Motorola naming, the 68040 is
 *  often referred to as simply the '040 (pronounced oh-four-oh or oh-forty).
 *
 *  The 68040 was the first 680x0 family member with an on-chip Floating-Point Unit (FPU). It thus included all of the functionality
 *  that previously required external chips, namely the FPU and Memory Management Unit (MMU), which was added in the 68030. It also
 *  had split instruction and data caches of 4 kilobytes each. It was fully pipelined, with six stages.
 *
 *  Versions of the 68040 were created for specific market segments, including the 68LC040, which removed the FPU, and the 68EC040,
 *  which removed both the FPU and MMU. Motorola had intended the EC variant for embedded use, but embedded processors during the
 *  68040's time did not need the power of the 68040, so EC variants of the 68020 and 68030 continued to be common in designs.
 *
 *  Motorola produced several speed grades. The 16 MHz and 20 MHz parts were never qualified (XC designation) and used as
 *  prototyping samples. 25 MHz and 33 MHz grades featured across the whole line, but until around 2000 the 40 MHz grade was only
 *  for the "full" 68040. A planned 50 MHz grade was canceled after it exceeded the thermal design envelope.
 *
 *  See also, https://en.wikipedia.org/wiki/Motorola_68040 */
class Motorola68040: public Base {
public:
    using Ptr = Motorola68040Ptr;

protected:
    explicit Motorola68040();                           // use `instance` instead
public:
    ~Motorola68040();

public:
    /** Allocating constructor. */
    static Ptr instance();

public:
    RegisterDictionary::Ptr registerDictionary() const override;
};

} // namespace
} // namespace
} // namespace

#endif
#endif
