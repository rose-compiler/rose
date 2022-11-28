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

class ConcreteExecutor;
using ConcreteExecutorPtr = Sawyer::SharedPointer<ConcreteExecutor>;

} // namespace
} // namespace
} // namespace
} // namespace

#endif
#endif
