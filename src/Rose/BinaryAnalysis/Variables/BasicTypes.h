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

/** Information about a stack frame.
 *
 *  A stack frame is a contiguous area of the stack with one frame for each function in a call chain. As functions are called
 *  and return, frames are pushed onto and popped from the stack. Typically, when a frame is pushed onto the stack, a pointer
 *  is also added so that the frames form a linked list from most recent function call to the oldest function call.
 *
 *  Typically, a dedicated register points to the most recent frame. E.g., for x86 this is usually the bp/ebp/rbp
 *  register. Sometimes there is no dedicated frame pointer register (such as when x86 is compiled with GCC/LLVM
 *  -fomit-frame-pointer). In these cases, the compiler keeps track of the frame address and when needed, emits code that finds
 *  the frame based on the current value of the stack pointer register (which can change throughout a function's execution).
 *  The frame pointer (explicit or calculated) defines the zero offset for the frame. The frame itself extends in positive
 *  and/or negative directions from this anchor point.
 *
 *  The frame pointer register can point to any position within the frame. For instance, x86 rbp register points to the top of
 *  the frame, whereas PowerPC's r31 register points to the bottom. Likewise, the pointers forming the linked list can point to
 *  any position within the frame. These pointer offsets into the frame are known by the compiler, which adds/subtracts the
 *  appropriate constant to access the desired bytes in the frame.
 *
 *  A stack containing the frames can grow (when frames are pushed) in a positive or negative direction.
 *
 *  A function can access areas of the stack that are outside its own frame.  For instance, x86 function stack arguments are
 *  stored in the caller's frame. Pointers to local variables can point into earlier frames.  The latest function can use parts
 *  of the stack that are after the latest frame and which therefore out outside any frame. */
class StackFrame {
public:
    enum Direction {
        GROWS_UP,                                       /**< New frames are added at higher addresses than old frames. */
        GROWS_DOWN                                      /**< New frames are added at lower addresses than old frames. */
    };

    Direction growthDirection = GROWS_DOWN;             /**< Direction that the stack grows when pushing a new frame. */
    Sawyer::Optional<int64_t> maxOffset;                /**< Maximum frame offset w.r.t. function's initial stack pointer. */
    Sawyer::Optional<int64_t> minOffset;                /**< Minimum frame offset w.r.t. function's initial stack pointer. */
    Sawyer::Optional<uint64_t> size;                    /**< Size of the frame in bytes if known. */
    std::string rule;                                   /**< Informal rule name used to detect frame characteristics. */
};

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
