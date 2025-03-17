#ifndef ROSE_BinaryAnalysis_Debugger_H
#define ROSE_BinaryAnalysis_Debugger_H

#include <Rose/BinaryAnalysis/Debugger/Base.h>
#include <Rose/BinaryAnalysis/Debugger/BasicTypes.h>
#include <Rose/BinaryAnalysis/Debugger/Exception.h>
#include <Rose/BinaryAnalysis/Debugger/Gdb.h>
#include <Rose/BinaryAnalysis/Debugger/GdbResponse.h>
#include <Rose/BinaryAnalysis/Debugger/Linux.h>
#include <Rose/BinaryAnalysis/Debugger/ThreadId.h>

namespace Rose {
namespace BinaryAnalysis {

/** Name space for dynamic debuggers.
 *
 *  This name space holds everything related to running a specimen under a debugger. The main class is @ref Base, from which
 *  various kinds of debuggers inherit. */
namespace Debugger {}

} // namespace
} // namespace

#endif
