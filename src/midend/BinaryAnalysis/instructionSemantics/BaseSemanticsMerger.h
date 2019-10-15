#ifndef ROSE_BinaryAnalysis_InstructionSemantics2_BaseSemantics_Merger_H
#define ROSE_BinaryAnalysis_InstructionSemantics2_BaseSemantics_Merger_H

#include <BaseSemanticsTypes.h>

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics2 {
namespace BaseSemantics {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Merging states
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Controls state merge operations.
 *
 *  This is the base class for objects that control the details of merge operations. A merge of two semantic values or semantic
 *  states happens when control flow joins together in data-flow analysis, and perhaps other operations.  An optional @ref
 *  Merger object is passed as an argument into the merge functions and contains settings and other details that might be
 *  necessary during the merge operation.
 *
 *  The base classes for register state and memory state allow an optional @ref Merger object to be stored in the
 *  state. Whenever a state is copied, its merger object pointer is also copied (shallow copy of merger).  The merger object is
 *  passed as an argument to each call of @ref SValue::createMerged or @ref SValue::createOptionalMerge.  The user-defined
 *  versions of these functions can access the merger object to decide how to merge. For example, the symbolic domain defines a
 *  merger that controls whether merging two different semantic values results in bottom or a set containing both values.
 *
 *  @ref Merger objects are allocated on the heap and have shared ownership like most other instruction semantics
 *  objects. Therefore they have no public C++ constructors but instead use factory methods named "instance". Users should not
 *  explicitly delete these objects -- they will be deleted automatically. */
class Merger: public Sawyer::SharedObject {
    bool memoryAddressesMayAlias_;
    bool memoryMergeDebugging_;

protected:
    Merger(): memoryAddressesMayAlias_(true), memoryMergeDebugging_(false) {}

public:
    /** Shared ownership pointer for @ref Merger. See @ref heap_object_shared_ownership. */
    typedef MergerPtr Ptr;

    /** Allocating constructor. */
    static Ptr instance() {
        return Ptr(new Merger);
    }

    /** Whether memory addresses can alias one another.
     *
     *  If true and the memory state supports it, then merging of two states will check for addresses that can alias one
     *  another and adjust the merge accordingly.
     *
     * @{ */
    bool memoryAddressesMayAlias() const { return memoryAddressesMayAlias_; }
    void memoryAddressesMayAlias(bool b) { memoryAddressesMayAlias_ = b; }
    /** @} */

    /** Turn on output for memory merge debugging.
     *
     *  If set and the @c Rose::BinaryAnalysis::InstructionSemantics2 diagnostic stream is also enabled, then memory merge
     *  operations produce debugging diagnostics to that stream.  Memory merge debugging is not normally enabled even if the
     *  stream is enabled because it can produce a very large amount of output.
     *
     *  @{ */
    bool memoryMergeDebugging() const { return memoryMergeDebugging_; }
    void memoryMergeDebugging(bool b) { memoryMergeDebugging_ = b; }
    /** @} */
};

} // namespace
} // namespace
} // namespace
} // namespace

#endif
