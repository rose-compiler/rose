#ifndef ROSE_BinaryAnalysis_Architecture_IntelPentium4_H
#define ROSE_BinaryAnalysis_Architecture_IntelPentium4_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Architecture/Base.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

/** Architecture-specific information for Intel Pentium4.
 *
 *
 * See also, https://en.wikipedia.org/wiki/Pentium_4 */
class IntelPentium4: public Base {
public:
    using Ptr = IntelPentium4Ptr;

protected:
    IntelPentium4();                                    // use `instance` instead
public:
    ~IntelPentium4();

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
