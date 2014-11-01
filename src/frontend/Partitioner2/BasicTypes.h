#ifndef ROSE_Partitioner2_BasicTypes_H
#define ROSE_Partitioner2_BasicTypes_H

// Define this if you want extra invariant checks that are quite expensive. This only makes a difference if NDEBUG and
// SAWYER_NDEBUG are both undefined--if either one of them are defined then no expensive (or inexpensive) checks are
// performed.
//#define ROSE_PARTITIONER_EXPENSIVE_CHECKS

namespace rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

/** Partitioner control flow vertex types. */
enum VertexType {
    V_BASIC_BLOCK,                                      /**< A basic block or placeholder for a basic block. */
    V_UNDISCOVERED,                                     /**< The special "undiscovered" vertex. */
    V_INDETERMINATE,                                    /**< Special vertex destination for indeterminate edges. */
    V_NONEXISTING,                                      /**< Special vertex destination for non-existing basic blocks. */
};

/** Partitioner control flow edge types. */
enum EdgeType {
    E_NORMAL,                                           /**< Normal control flow edge, nothing special. */
    E_FUNCTION_CALL,                                    /**< Edge is a function call. */
    E_FUNCTION_RETURN,                                  /**< Edge is a function return. Such edges represent the actual
                                                         *   return-to-caller and usually originate from a return instruction
                                                         *   (e.g., x86 @c RET, m68k @c RTS, etc.). */
    E_CALL_RETURN,                                      /**< Edge is a function return from the call site. Such edges are from
                                                         *   a caller basic block to (probably) the fall-through address of the
                                                         *   call and don't actually exist directly in the specimen.  The
                                                         *   represent the fact that the called function eventually returns
                                                         *   even if the instructions for the called function are not available
                                                         *   to analyze. */
};

class Partitioner;

} // namespace
} // namespace
} // namespace

#endif
