#ifndef ROSE_BinaryAnalysis_Variables_BasicTypes_H
#define ROSE_BinaryAnalysis_Variables_BasicTypes_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/AddressSet.h>

#include <Sawyer/Attribute.h>
#include <Sawyer/BitFlags.h>
#include <Sawyer/Message.h>
#include <Sawyer/Optional.h>

#include <map>

namespace Rose {
namespace BinaryAnalysis {
namespace Variables {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Types
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class BaseVariable;
class GlobalVariable;
class InstructionAccess;
class StackFrame;
class StackVariable;
class VariableFinder;

/** Shared ownership pointer to @ref VariableFinder.
 *
 * @{ */
using VariableFinderPtr = std::shared_ptr<VariableFinder>;
using VariableFinderConstPtr = std::shared_ptr<const VariableFinder>;
/** @} */

/** Mapping from stack offsets to address sets. */
using OffsetToAddress = std::map<int64_t /*offset*/, AddressSet>;

/** Mapping from addresses to address sets. */
using AddressToAddresses = std::map<Address, AddressSet>;

/** Interval of signed offsets. */
using OffsetInterval = Sawyer::Container::Interval<int64_t>;

/** Whether a variable is read or written. */
enum class Access {
    READ          = 0x0001,                             /**< Variable is read.  */
    WRITE         = 0x0002,                             /**< Variable is written. */
};

/** Whether a variable is read and/or written by an instruction. */
using AccessFlags = Sawyer::BitFlags<Access, uint64_t>;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Global variables
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Diagnostic facility for variables. */
extern Sawyer::Message::Facility mlog;

// Key for storing uint64_t frame sizes in P2::Function objects.
extern Sawyer::Attribute::Id ATTR_FRAME_SIZE;

// Key for storing StackVariables in a P2::Function.
extern Sawyer::Attribute::Id ATTR_LOCAL_VARS;

// Key for storing GlobalVariables in a P2::Partitioner.
extern Sawyer::Attribute::Id ATTR_GLOBAL_VARS;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Initialize diagnostic output.
 *
 *  This is called automatically when ROSE is initialized. */
void initDiagnostics();

/** Initialize the namespace.
 *
 *  This is called automatically when ROSE is initialized. */
void initNamespace();

/** Format a stack offset as a string. */
std::string offsetStr(int64_t offset);

/** Format size as a string. */
std::string sizeStr(uint64_t size);

} // namespace
} // namespace
} // namespace

#endif
#endif
