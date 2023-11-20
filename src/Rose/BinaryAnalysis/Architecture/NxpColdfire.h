#ifndef ROSE_BinaryAnalysis_Architecture_NxpColdfire_H
#define ROSE_BinaryAnalysis_Architecture_NxpColdfire_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Architecture/Base.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

/** Architecture-specific information for NXP ColdFire'
 *
 *  The NXP ColdFire is a microprocessor that derives from the Motorola 68000 family architecture, manufactured for embedded systems
 *  development by NXP Semiconductors. It was formerly manufactured by Freescale Semiconductor (formerly the semiconductor division
 *  of Motorola) which merged with NXP in 2015.
 *
 *  See also, https://en.wikipedia.org/wiki/NXP_Coldfire */
class NxpColdfire: public Base {
public:
    using Ptr = NxpColdfirePtr;

protected:
    explicit NxpColdfire();                             // use `instance` instead
public:
    ~NxpColdfire();

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
