#ifndef ROSE_BinaryAnalysis_Partitioner2_IndirectControlFlow_H
#define ROSE_BinaryAnalysis_Partitioner2_IndirectControlFlow_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Partitioner2/BasicTypes.h>

#include <Sawyer/Message.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

/** Utilities for doing indirect control flow as part of partitioning. */
namespace IndirectControlFlow {

extern Sawyer::Message::Facility mlog;
void initDiagnostics();

bool analyzeJumpTables(const PartitionerPtr&);
bool analyzeFunctions(const PartitionerPtr&);

} // namespace
} // namespace
} // namespace
} // namespace

#endif
#endif
