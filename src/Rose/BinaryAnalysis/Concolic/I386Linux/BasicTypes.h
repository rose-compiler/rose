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

class ConcreteExecutor;
using ConcreteExecutorPtr = Sawyer::SharedPointer<ConcreteExecutor>;

} // namespace
} // namespace
} // namespace
} // namespace

#endif
#endif
