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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/** Functions for analyzing static jump tables. */
namespace StaticJumpTable {

/** Attempt to resolve indeterminate jumps related to static jump tables.
 *
 *  Scans through all discovered basic blocks in the CFG and processes each block that has a CFG edge pointing to an indeterminate
 *  address. This analysis attempts to resolve the indirect control flow by discovering and using statically created jump tables,
 *  such as what a C compiler typically creates for `switch` statements.
 *
 *  For each such block having an indeterminate outgoing CFG edge, a dataflow graph is created from nearby basic blocks. The
 *  dataflow analysis uses instruction semantics to update the machine state associated with each dataflow graph vertex. A special
 *  merge operation merges states when control flow merges, such as at the end of an `if` statement. The analysis examines the
 *  final outgoing state for the basic block in question to ascertain whether a static jump table was used, what its starting
 *  address might be, and how entries in the table are used to compute target addresses for the jump instruction.
 *
 *  Besides containing the typical symbolic registers and memory, the state also contains constraints imposed by the execution path.
 *  These constraints are used to limit the size of the jump table. Ideally, even if the compiler emits two consecutive tables for
 *  two different `switch` statements, the basic block for each `switch` statement will have only successors that are relevant to
 *  that statement. */
bool analyzeAllBlocks(const Settings&, const PartitionerPtr&);

} // namespace
} // namespace
} // namespace
} // namespace
} // namespace

#endif
#endif
