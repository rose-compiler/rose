#ifndef ROSE_BinaryAnalysis_Variables_StackFrame_H
#define ROSE_BinaryAnalysis_Variables_StackFrame_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/Variables/BasicTypes.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Variables {

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

} // namespace
} // namespace
} // namespace

#endif
#endif
