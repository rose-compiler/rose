#ifndef ROSE_BinaryAnalysis_InstructionSemantics_Util_H
#define ROSE_BinaryAnalysis_InstructionSemantics_Util_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics {

/** Diagnostics logging facility for instruction semantics. */
extern Sawyer::Message::Facility mlog;

/** Initialize diagnostics for instruction semantics. */
void initDiagnostics();

} // namespace
} // namespace
} // namespace

#endif
#endif
