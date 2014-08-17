#ifndef ROSE_Partitioner2_BasicTypes_H
#define ROSE_Partitioner2_BasicTypes_H

namespace rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

/** Partitioner control flow vertex types. */
enum VertexType {
    V_BASICBLOCK,                                       /**< A basic block or placeholder for a basic block. */
    V_UNDISCOVERED,                                     /**< The special "undiscovered" vertex. */
    V_INDETERMINATE,                                    /**< Special vertex destination for indeterminate edges. */
    V_NONEXISTING,                                      /**< Special vertex destination for non-existing basic blocks. */
};

/** Partitioner control flow edge types. */
enum EdgeType {
    E_NORMAL,                                           /**< Normal control flow edge, nothing special. */
    E_FCALL,                                            /**< Edge is a function call. */
    E_FRET,                                             /**< Edge is a function return from the call site. */
};

class Partitioner;

} // namespace
} // namespace
} // namespace

#endif
