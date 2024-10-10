#ifndef ROSE_BinaryAnalysis_Variables_VariableFinder_H
#define ROSE_BinaryAnalysis_Variables_VariableFinder_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/BasicTypes.h>
#include <Rose/BinaryAnalysis/Variables/GlobalVariable.h>
#include <Rose/BinaryAnalysis/Variables/StackVariable.h>

#include <Sawyer/CommandLine.h>

class SgAsmInstruction;

namespace Rose {
namespace BinaryAnalysis {
namespace Variables {

/** Analysis to find variable locations.
 *
 *  See @ref Variables "namespace" for more information and caveats. */
class VariableFinder {
public:
    /** Settings that control this analysis. */
    struct Settings {
        /** Max time to spend in Method 1 global variable analysis per function. */
        std::chrono::seconds gvarMethod1MaxTimePerFunction;

        Settings()
            : gvarMethod1MaxTimePerFunction(30) {}
    };

    /** Shared ownership pointer for @ref VariableFinder.
     *
     * @{ */
    using Ptr = VariableFinderPtr;
    using ConstPtr = VariableFinderConstPtr;
    /** @} */

private:
    Settings settings_;

protected:
    explicit VariableFinder(const Settings&);
public:
    ~VariableFinder();

public:
    /** Allocating constructor. */
    static Ptr instance(const Settings &settings = Settings());

    /** Settings for this analysis.
     *
     * @{ */
    const Settings& settings() const { return settings_; }
    Settings& settings() { return settings_; }
    /** @} */

    /** Command-line switches for analysis settings.
     *
     *  This function captures a reference to the specified settings so that when the command-line is parsed later, those settings
     *  are modified. */
    static Sawyer::CommandLine::SwitchGroup commandLineSwitches(Settings&);

    /** Find local variables in a function.
     *
     *  Analyzes the given function to find the local (stack) variables, caches and returns the list.  If this information
     *  is already cached, then this method just returns the cached value.
     *
     *  The second argument can be either a function or an instruction. When it's an instruction, we arbitrarily choose one
     *  of the functions that owns the instruction. Usually each instruction is owned by only one function, but there are
     *  exceptions.
     *
     * @{ */
    StackVariables findStackVariables(const Partitioner2::PartitionerConstPtr&, const Partitioner2::FunctionPtr&);
    StackVariables findStackVariables(const Partitioner2::PartitionerConstPtr&, SgAsmInstruction*);
    /** @} */

    /** Find global variables.
     *
     *  Finds global variables by examining all functions represented by the given partitioner. Global variables can be found
     *  only if they're read or written by some code, and not through a pointer. For example, this method detects variable @c g1
     *  but not @c g2 in the following compiled C code:
     *
     * @code
     *     int g1, g2;
     *
     *     int foo() {
     *         return g1;
     *     }
     *
     *     int bar(int *ptr) {
     *         return *ptr; // no static address even if ptr points to g2
     *     }
     * @endcode
     *
     * If previous results are already cached then they're returned, otherwise new results are computed, cached, and returned. */
    GlobalVariables findGlobalVariables(const Partitioner2::PartitionerConstPtr&);

    /** Removed cached information.
     *
     *  If a function is supplied as an argument, remove information cached by this analysis for that function. If a
     *  partitioner is specified then remove information from all functions.
     *
     * @{ */
    void evict(const Partitioner2::FunctionPtr&);
    void evict(const Partitioner2::PartitionerConstPtr&);
    /** @} */

    /** Test whether local variable information is cached.
     *
     *  Returns true if @ref findStackVariables has been run for the specified function and the results are currently
     *  cached in that function. */
    bool isCached(const Partitioner2::FunctionPtr&);

    /** Figure out attributes describing the stack frame for the specified function. */
    StackFrame detectFrameAttributes(const Partitioner2::PartitionerConstPtr&, const Partitioner2::FunctionPtr&,
                                     StackVariable::Boundaries&/*in,out*/);

//    /** Initilialize offsets for function prologue.
//     *
//     *  At the start of a function, we sometimes know where certain things are on the stack and their sizes. For instance, for
//     *  powerpc after the function prologue sets up the stack frame, we know that the stack frame header contain two 4-byte
//     *  quantities: the pointer to the parent frame, and the LR save area for callees and therefore we can add the three offsets
//     *  that delimit the boundaries of these two "variables". */
//    void initializeFrameBoundaries(const StackFrame&, const Partitioner2::PartitionerConstPtr&, const Partitioner2::FunctionPtr&,
//                                   StackVariable::Boundaries &boundaries /*in,out*/);
//
//    /** Find stack variable addresses.
//     *
//     *  Given an instruction, look for operand subexpressions that reference memory based from a stack frame pointer, such as
//     *  x86 "mov eax, [ebp - 12]". Returns the set of offsets from the frame pointer. */
//    std::set<int64_t> findFrameOffsets(const StackFrame&, const Partitioner2::PartitionerConstPtr&, SgAsmInstruction*);
//
//    /** Function that owns an instruction.
//     *
//     *  Given an instruction, return one of the owning functions chosen arbitrarily.  This is the method used by the version
//     *  of @ref findStackVariables that takes an instruction argument. */
//    Partitioner2::FunctionPtr functionForInstruction(const Partitioner2::PartitionerConstPtr&, SgAsmInstruction*);
//
    /** Find global variable addresses.
     *
     *  Returns a list of addresses that are possibly the beginning of global variables. We're only able to find global
     *  variables that are referenced by instructions that read or write to the variable.  If the global variable is an array
     *  then one of two things happen:
     *
     *  @li If the array elements are only accessed with indices that are unknown statically, then the address of the beginning
     *      of the array is returned.  This applies to C code like:
     *
     * @code
     *      int get(int i) {
     *          return array[i];
     *      }
     *      int sum(int n) {
     *          int s = 0;
     *          for (int i=0; i < n; ++i)
     *              s += array[i];
     *          return s;
     *      }
     * @endcode
     *
     *  @li If the array elements are accessed with constant indices, then it's impossible to distinguish between accessing an
     *      array with constant indexes and accessing a non-array variable. This applies to C code like:
     *
     * @code
     *      int get() {
     *          return array[3];
     *      }
     *      int sum() {
     *          int s = 0;
     *          for (int i=0; i < 4; ++i) // assume loop unrolling optimization
     *              s += array[i];
     *          return s;
     *      }
     * @endcode */
    AddressToAddresses findGlobalVariableVas(const Partitioner2::PartitionerConstPtr&);

    /** Find address constants in an expression.
     *
     *  Given a symbolic expression, return all the constants that appear in it that have a potential for being addresses. */
    std::set<Address> findConstants(const SymbolicExpression::Ptr&);

    /** Find constants syntactically in an instruction. */
    std::set<Address> findConstants(SgAsmInstruction*);

    /** Find addresses in memory state.
     *
     *  Given a cell-based symbolic memory state, return all the memory addresses that appear in it. */
    std::set<SymbolicExpression::Ptr> getMemoryAddresses(const InstructionSemantics::BaseSemantics::MemoryCellStatePtr&);

    /** Find constants in memory.
     *
     *  Given an cell-based symbolic memory state, return all constants that appear in the cell addresses. */
    std::set<Address> findAddressConstants(const InstructionSemantics::BaseSemantics::MemoryCellStatePtr&);

    /** Remove boundaries that are outside a stack frame.
     *
     *  If the frame's lowest address is known, then boundaries that begin before the frame are removed. Except if there is no
     *  boundary at the beginning of the frame, then the greatest boundary before the frame is moved to the beginning of the
     *  frame instead of being removed entirely.
     *
     *  If the frame's upper address is known, then any boundary above that address is removed from the list. */
    void removeOutliers(const StackFrame&, StackVariable::Boundaries &sortedBoundaries /*in,out*/);

    /** True if memory region contains any decoded instructions. */
    static bool regionContainsInstructions(const Partitioner2::PartitionerConstPtr&, const AddressInterval&);

    /** True if memory region is fully mapped. */
    static bool regionIsFullyMapped(const Partitioner2::PartitionerConstPtr&, const AddressInterval&);

    /** True if memory region is fully mapped with read and write access. */
    static bool regionIsFullyReadWrite(const Partitioner2::PartitionerConstPtr&, const AddressInterval&);

private:
    // Searches for global variables using a per-function data-flow analysis and distinguishing between global variable
    // addresses based on which instructions wrote to those addresses. The data-flow is inter-procedural only for calls to
    // certain functions (such as the x86 get_pc_thunk variety of functions).
    //
    // The return value is a list of global variable addresses and the instructions at which each global variable address
    // was detected.
    AddressToAddresses findGlobalVariableVasMethod1(const Partitioner2::PartitionerConstPtr&);

    // Searches for global variables by running each instruction individually in symbolic semantics and then searching
    // the memory state addresses. All constants found in address expressions are gathered together.
    //
    // The return value is a list of global variable addresses and the instructions at which each global address was detected.
    AddressToAddresses findGlobalVariableVasMethod2(const Partitioner2::PartitionerConstPtr&);

    // Searches for global variables by looking for constants in instruction ASTs.
    //
    // The retun value is the list of constants and the instructions in which each constant appeared.
    AddressToAddresses findGlobalVariableVasMethod3(const Partitioner2::PartitionerConstPtr&);

    // Merge one set of addresses and their defining instructions into another.
    static void merge(AddressToAddresses&, const AddressToAddresses&);
};

} // namespace
} // namespace
} // namespace

#endif
#endif
