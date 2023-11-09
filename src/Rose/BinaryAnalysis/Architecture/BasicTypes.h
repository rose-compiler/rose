#ifndef ROSE_BinaryAnalysis_Architecture_BasicTypes_H
#define ROSE_BinaryAnalysis_Architecture_BasicTypes_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Sawyer/Message.h>
#include <memory>

// Basic types needed by almost all architectures

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

// Forward declarations
class Base;
class Exception;

/** Reference counted pointer for architecture definitions. */
using BasePtr = std::shared_ptr<Base>;

/** Diagnostic facility for architecture definitions. */
extern Sawyer::Message::Facility mlog;

/** Initialize and registers architecture diagnostic streams.
 *
 *  See @ref Rose::Diagnostics::initialize. */
void initDiagnostics();

} // namespace
} // namespace
} // namespace

#endif
#endif
