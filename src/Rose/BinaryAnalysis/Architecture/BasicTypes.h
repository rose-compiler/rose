#ifndef ROSE_BinaryAnalysis_Architecture_BasicTypes_H
#define ROSE_BinaryAnalysis_Architecture_BasicTypes_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <memory>

// Basic types needed by almost all architectures

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

class Base;

/** Reference counted pointer for architecture definitions. */
using BasePtr = std::shared_ptr<Base>;

} // namespace
} // namespace
} // namespace

#endif
#endif
