#ifndef ROSE_BinaryAnalysis_Architecture_Intel8088_H
#define ROSE_BinaryAnalysis_Architecture_Intel8088_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Architecture/Base.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

/** Architecture-specific information for Intel 8088.
 *
 *  The Intel 8088 ("eighty-eighty-eight", also called iAPX 88) microprocessor is a variant of the Intel 8086. Introduced on June 1,
 *  1979, the 8088 has an eight-bit external data bus instead of the 16-bit bus of the 8086. The 16-bit registers and the one
 *  megabyte address range are unchanged, however. In fact, according to the Intel documentation, the 8086 and 8088 have the same
 *  execution unit (EU)—only the bus interface unit (BIU) is different. The 8088 was used in the original IBM PC and in IBM PC
 *  compatible clones.
 *
 * See also, https://en.wikipedia.org/wiki/Intel_8088 */
class Intel8088: public Base {
public:
    using Ptr = Intel8088Ptr;

protected:
    Intel8088();                                        // use `instance` instead
public:
    ~Intel8088();

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
