#ifndef ROSE_BinaryAnalysis_Architecture_IntelI386_H
#define ROSE_BinaryAnalysis_Architecture_IntelI386_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Architecture/Base.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

/** Architecture-specific information for Intel i386.
 *
 *  The Intel 386, originally released as 80386 and later renamed i386, is a 32-bit microprocessor introduced in 1985. The first
 *  versions had 275,000 transistors and were the central processing unit (CPU) of many workstations and high-end personal computers
 *  of the time.
 *
 *  The 32-bit i386 can correctly execute most code intended for the earlier 16-bit processors such as 8086 and 80286 that were
 *  ubiquitous in early PCs. As the original implementation of the 32-bit extension of the 80286 architecture, the i386 instruction
 *  set, programming model, and binary encodings are still the common denominator for all 32-bit x86 processors, which is termed the
 *  i386 architecture, x86, or IA-32, depending on context. Over the years, successively newer implementations of the same
 *  architecture have become several hundreds of times faster than the original 80386 (and thousands of times faster than the 8086).
 *
 * See also, https://en.wikipedia.org/wiki/I386 */
class IntelI386: public Base {
public:
    using Ptr = IntelI386Ptr;

protected:
    IntelI386();                                        // use `instance` instead
public:
    ~IntelI386();

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
