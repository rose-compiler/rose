#ifndef ROSE_BinaryAnalysis_Architecture_Base_H
#define ROSE_BinaryAnalysis_Architecture_Base_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/Architecture/BasicTypes.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

/** Base class for architecture definitions. */
class Base {
public:
    using Ptr = BasePtr;
};

} // namespace
} // namespace
} // namespace

#endif
#endif
