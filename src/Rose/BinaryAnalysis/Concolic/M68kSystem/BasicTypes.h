#ifndef ROSE_BinaryAnalysis_Concolic_M68kSystem_BasicTypes_H
#define ROSE_BinaryAnalysis_Concolic_M68kSystem_BasicTypes_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING

#include <Sawyer/SharedPointer.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {
namespace M68kSystem {

class Architecture;
using ArchitecturePtr = Sawyer::SharedPointer<Architecture>;

class TracingExecutor;
using TracingExecutorPtr = Sawyer::SharedPointer<TracingExecutor>;

class TracingManager;
using TracingManagerPtr = Sawyer::SharedPointer<TracingManager>;

class TracingResult;
using TracingResultPtr = Sawyer::SharedPointer<TracingResult>;

} // namespace
} // namespace
} // namespace
} // namespace

#endif
#endif
