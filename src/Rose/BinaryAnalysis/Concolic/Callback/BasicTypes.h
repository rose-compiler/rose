#ifndef ROSE_BinaryAnalysis_Concolic_Callback_BasicTypes_H
#define ROSE_BinaryAnalysis_Concolic_Callback_BasicTypes_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {
namespace Callback {

class MemoryExit;
using MemoryExitPtr = Sawyer::SharedPointer<MemoryExit>; /**< Shared ownership pointer. */

class MemoryInput;
using MemoryInputPtr = Sawyer::SharedPointer<MemoryInput>; /**< Shared ownership pointer */

class MemoryTime;
using MemoryTimePtr = Sawyer::SharedPointer<MemoryTime>; /**< Shared ownership pointer. */

} // namespace
} // namespace
} // namespace
} // namespace

#endif
#endif
