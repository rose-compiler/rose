#ifndef ROSE_BinaryAnalysis_Architecture_BasicTypes_H
#define ROSE_BinaryAnalysis_Architecture_BasicTypes_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Sawyer/Message.h>
#include <memory>
#include <set>
#include <string>
#include <vector>

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

/** Register a new architecture definition.
 *
 *  The specified definition is added to the ROSE library. When searching for an architecture, architectures registered later
 *  are preferred over architectures registered earlier.
 *
 *  Thread safety: This function is thread safe. */
void registerDefinition(const BasePtr&);

/** Remove the specified architecture from the list of registered architectures.
 *
 *  If the specified architecture object is found, then the latest such object is removed from the registration. This function
 *  is a no-ip if the argument is a null pointer.
 *
 *  Returns true if any architecture definition object was removed, false if the object was not found.
 *
 *  Thread safety: This function is thread safe. */
bool deregisterDefinition(const BasePtr&);

/** Registered architectures.
 *
 *  Returns the registered architectures in the order they were registered.
 *
 *  Thread safety: This function is thread safe. */
std::vector<BasePtr> registeredDefinitions();

/** Names of all registered architectures.
 *
 *  Returns the names of all registered architectures. This is returned as a set, although there is no requirement that the
 *  registered architectures have unique names.
 *
 *  Thread safety: This function is thread safe. */
std::set<std::string> registeredNames();

/** Look up a new architecture by name.
 *
 *  Returns the latest registered architecture having the specified name. If no matching architecture is found then a null pointer
 *  is returned.
 *
 *  Thread safety: This function is thread safe. */
BasePtr findByName(const std::string&);

/** Architecture name free function.
 *
 *  This is a convenient way to obtain an architecture definition's name without having to include "Base.h", and is therefore useful
 *  in header files that try to include a minimal number of type definitions. Returns a null pointer if the argument is a null
 *  pointer. */
const std::string& name(const BasePtr&);

} // namespace
} // namespace
} // namespace

#endif
#endif
