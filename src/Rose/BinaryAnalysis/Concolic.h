#ifndef ROSE_BinaryAnalysis_Concolic_H
#define ROSE_BinaryAnalysis_Concolic_H

// The one header to rule them all -- i.e., all the concolic testing.

#include <Rose/BinaryAnalysis/Concolic/Architecture.h>
#include <Rose/BinaryAnalysis/Concolic/BasicTypes.h>
#include <Rose/BinaryAnalysis/Concolic/Callback.h>
#include <Rose/BinaryAnalysis/Concolic/ConcolicExecutor.h>
#include <Rose/BinaryAnalysis/Concolic/ConcreteExecutor.h>
#include <Rose/BinaryAnalysis/Concolic/ConcreteResult.h>
#include <Rose/BinaryAnalysis/Concolic/Database.h>
#include <Rose/BinaryAnalysis/Concolic/Emulation.h>
#include <Rose/BinaryAnalysis/Concolic/ExecutionEvent.h>
#include <Rose/BinaryAnalysis/Concolic/ExecutionLocation.h>
#include <Rose/BinaryAnalysis/Concolic/ExecutionManager.h>
#include <Rose/BinaryAnalysis/Concolic/I386Linux.h>
#include <Rose/BinaryAnalysis/Concolic/InputVariables.h>
#include <Rose/BinaryAnalysis/Concolic/M68kSystem.h>
#include <Rose/BinaryAnalysis/Concolic/Settings.h>
#include <Rose/BinaryAnalysis/Concolic/SharedMemory.h>
#include <Rose/BinaryAnalysis/Concolic/Specimen.h>
#include <Rose/BinaryAnalysis/Concolic/SystemCall.h>
#include <Rose/BinaryAnalysis/Concolic/TestCase.h>
#include <Rose/BinaryAnalysis/Concolic/TestSuite.h>

namespace Rose {
namespace BinaryAnalysis {

/** Combined concrete and symbolic analysis. */
namespace Concolic {
} // namespace

} // namespace
} // namespace

#endif
