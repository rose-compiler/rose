#ifndef ROSE_BinaryAnalysis_Architecture_Intel80286_H
#define ROSE_BinaryAnalysis_Architecture_Intel80286_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Architecture/Base.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

/** Architecture specific information for Intel 80286.
 *
 *  The Intel 80286 (also marketed as the iAPX 286 and often called Intel 286) is a 16-bit microprocessor that was introduced on
 *  February 1, 1982. It was the first 8086-based CPU with separate, non-multiplexed address and data buses and also the first with
 *  memory management and wide protection abilities. The 80286 used approximately 134,000 transistors in its original nMOS (HMOS)
 *  incarnation and, just like the contemporary 80186, it could correctly execute most software written for the earlier Intel 8086
 *  and 8088 processors.
 *
 *  The 80286 was employed for the IBM PC/AT, introduced in 1984, and then widely used in most PC/AT compatible computers until the
 *  early 1990s. In 1987, Intel shipped its five-millionth 80286 microprocessor.
 *
 *  See https://en.wikipedia.org/wiki/Intel_80286 */
class Intel80286: public Base {
public:
    using Ptr = Intel80286Ptr;

public:
    ~Intel80286();
protected:
    Intel80286();                                       // use `instance` instead

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
