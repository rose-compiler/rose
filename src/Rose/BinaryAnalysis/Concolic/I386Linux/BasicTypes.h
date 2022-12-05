#ifndef ROSE_BinaryAnalysis_Concolic_I386Linux_BasicTypes_H
#define ROSE_BinaryAnalysis_Concolic_I386Linux_BasicTypes_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING

#include <Sawyer/SharedPointer.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {
namespace I386Linux {

class Architecture;
using ArchitecturePtr = Sawyer::SharedPointer<Architecture>;

class ExitStatusExecutor;
using ExitStatusExecutorPtr = Sawyer::SharedPointer<ExitStatusExecutor>;

class ExitStatusManager;
using ExitStatusManagerPtr = Sawyer::SharedPointer<ExitStatusManager>;

class ExitStatusResult;
using ExitStatusResultPtr = Sawyer::SharedPointer<ExitStatusResult>;

class TracingExecutor;
using TracingExecutorPtr = Sawyer::SharedPointer<TracingExecutor>;

class TracingResult;
using TracingResultPtr = Sawyer::SharedPointer<TracingResult>;

} // namespace
} // namespace
} // namespace
} // namespace

#endif
#endif
