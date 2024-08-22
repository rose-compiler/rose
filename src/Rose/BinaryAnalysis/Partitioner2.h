#ifndef ROSE_BinaryAnalysis_Partitioner2_H
#define ROSE_BinaryAnalysis_Partitioner2_H

#include <Rose/BinaryAnalysis/Partitioner2/AddressUsageMap.h>
#include <Rose/BinaryAnalysis/Partitioner2/BasicBlock.h>
#include <Rose/BinaryAnalysis/Partitioner2/BasicTypes.h>
#include <Rose/BinaryAnalysis/Partitioner2/CfgPath.h>
#include <Rose/BinaryAnalysis/Partitioner2/Configuration.h>
#include <Rose/BinaryAnalysis/Partitioner2/ControlFlowGraph.h>
#include <Rose/BinaryAnalysis/Partitioner2/DataBlock.h>
#include <Rose/BinaryAnalysis/Partitioner2/DataFlow.h>
#include <Rose/BinaryAnalysis/Partitioner2/Engine.h>
#include <Rose/BinaryAnalysis/Partitioner2/EngineBinary.h>
#include <Rose/BinaryAnalysis/Partitioner2/EngineJvm.h>
#include <Rose/BinaryAnalysis/Partitioner2/Exception.h>
#include <Rose/BinaryAnalysis/Partitioner2/Function.h>
#include <Rose/BinaryAnalysis/Partitioner2/FunctionCallGraph.h>
#include <Rose/BinaryAnalysis/Partitioner2/GraphViz.h>
#include <Rose/BinaryAnalysis/Partitioner2/Modules.h>
#include <Rose/BinaryAnalysis/Partitioner2/ModulesElf.h>
#include <Rose/BinaryAnalysis/Partitioner2/ModulesJvm.h>
#include <Rose/BinaryAnalysis/Partitioner2/ModulesLinux.h>
#include <Rose/BinaryAnalysis/Partitioner2/ModulesM68k.h>
#include <Rose/BinaryAnalysis/Partitioner2/ModulesMips.h>
#include <Rose/BinaryAnalysis/Partitioner2/ModulesPe.h>
#include <Rose/BinaryAnalysis/Partitioner2/ModulesPowerpc.h>
#include <Rose/BinaryAnalysis/Partitioner2/ModulesX86.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <Rose/BinaryAnalysis/Partitioner2/Reference.h>
#include <Rose/BinaryAnalysis/Partitioner2/Semantics.h>
#include <Rose/BinaryAnalysis/Partitioner2/Thunk.h>
#include <Rose/BinaryAnalysis/Partitioner2/Utility.h>

namespace Rose {
namespace BinaryAnalysis {

/** Binary function detection.
 *
 *  This namespace consists of two major parts and a number of smaller parts.  The major parts are:
 *
 *  @li @ref Partitioner2::Partitioner "Partitioner": The partitioner has two slightly conflated roles. Its primary role is to serve
 *  as a database that supports efficient queries for information that are needed during typical binary analysis. As such, it is
 *  able to describe functions, basic blocks, static data blocks, memory maps, control flow graphs, address usage maps, and more and
 *  has a fairly robust API.  Its second role, less important from the view of the end user, is to accumulate the above information
 *  in a process known as <em>partitioning</em>.
 *
 *  @li @ref Partitioner2::Engine "Engine": The engine contains the higher-level functionality that drives the process by which
 *  information is added to the partitioner, i.e., <em>partitioning</em>. The engine is customized by class derivation, and the base
 *  class (@ref Partitioner2::Engine) is abstract. */
namespace Partitioner2 {}

} // namespace
} // namespace

#endif
