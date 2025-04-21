#ifndef ROSE_BinaryAnalysis_Partitioner2_IndirectControlFlow_H
#define ROSE_BinaryAnalysis_Partitioner2_IndirectControlFlow_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Partitioner2/BasicTypes.h>

#include <Sawyer/CommandLine.h>
#include <Sawyer/Message.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

/** Utilities for doing indirect control flow as part of partitioning. */
namespace IndirectControlFlow {

extern Sawyer::Message::Facility mlog;
void initDiagnostics();

// `struct Settings` is located in <BasicTypes.h>

/** Define command-line switches.
 *
 *  Returns a switch group containing definitions for command-line switches related to indirect control flow recovery. The switches
 *  are bound to the specified settings so that when the command-line is parsed and its results are applied, those settings are
 *  adjusted. */
Sawyer::CommandLine::SwitchGroup commandLineSwitches(Settings&);

/** Attempt to resolve indirect control flow using a variety of strategies.
 *
 *  Scans through all discovered basic blocks in the CFG and processes each block that has a CFG edge pointing to an indeterminate
 *  address. It then uses various strategies to find concrete CFG successors and updates the successors for the basic block in
 *  question.
 *
 *  Some of the strategies use a dataflow analysis. The dataflow graph is constructed in two phases: first, starting at the basic
 *  block in question, the CFG edges are followed in reverse up to a certain distance without traversing function calls. This
 *  usually obtains the entry point(s) of the function containing the basic block in question. Second, starting at the dataflow
 *  vertices that have no incoming edges, a forward traversal inlines function calls to a specified depth, either bringing their
 *  definitions into the dataflow graph, or inserting a representative vertex for the call. The @ref Settings argument controls
 *  some of the details for this process. */
bool analyzeAllBlocks(const Settings&, const PartitionerPtr&);

} // namespace
} // namespace
} // namespace
} // namespace

#endif
#endif
