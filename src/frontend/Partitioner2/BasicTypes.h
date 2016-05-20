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
    V_USER_DEFINED,                                     /**< User defined vertex. These vertices don't normally appear in the
                                                         *   global control flow graph but might appear in other kinds of
                                                         *   graphs that are closely related to a CFG, such as a paths graph. */
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
    E_FUNCTION_XFER,                                    /**< Edge is a function call transfer. A function call transfer is
                                                         *   similar to @ref E_FUNCTION_CALL except the entire call frame is
                                                         *   transferred to the target function and this function is no longer
                                                         *   considered part of the call stack; a return from the target
                                                         *   function will skip over this function. Function call transfers
                                                         *   most often occur as the edge leaving a thunk. */
    E_USER_DEFINED,                                     /**< User defined edge.  These edges don't normally appear in the
                                                         *   global control flow graph but might appear in other kinds of
                                                         *   graphs that are closely related to a CFG, such as a paths graph. */
};

/** How sure are we of something. */
enum Confidence {
    ASSUMED,                                            /**< The value is an assumption without any proof. */
    PROVED,                                             /**< The value was somehow proved. */
};

/** Organization of semantic memory. */
enum SemanticMemoryParadigm {
    LIST_BASED_MEMORY,                                  /**< Precise but slow. */
    MAP_BASED_MEMORY                                    /**< Fast but not precise. */
};

/** Settings that control building the AST.
 *
 *  The runtime descriptions and command-line parser for these switches can be obtained from @ref
 *  astConstructionSwitches. */
struct AstConstructionSettings {
    /** Whether to allow an empty global block.
     *
     *  If the partitioner contains no functions then either create an empty global block (top-level @ref SgAsmBlock) when
     *  this setting is true, or return a null global block pointer when this setting is false. */
    bool allowEmptyGlobalBlock;

    /** Whether to allow functions with no basic blocks.
     *
     *  If the the partitioner knows about a function but was unable to produce any basic blocks then we have two choices
     *  for constructing the @ref SgAsmFunction node in the AST: if this setting is true, then create a function node with
     *  no @ref SgAsmBlock children; otherwise return a null pointer and do not add ths function to the AST. */
    bool allowFunctionWithNoBasicBlocks;

    /** Whether to allow a basic block to be empty.
     *
     *  If the partitioner contains a basic block with no instructions, such as a block whose starting address is not
     *  mapped, then we have two choices when creating the corresponding @ref SgAsmBlock node in the AST: if this setting
     *  is true, then create a basic block with no @ref SgAsmInstruction children; otherwise return a null pointer and do
     *  not add the basic block to the AST. */
    bool allowEmptyBasicBlocks;

    /** Whether to allow shared instructions in the AST.
     *
     *  This setting controls how an instruction that is shared between two or more functions by virtue of its basic block
     *  being part of both functions is represented in the AST.  If this setting is true, instruction ASTs (rooted at @ref
     *  SgAsmInstruction) are deep-copied into the AST at each place they occur.
     *
     *  The partitioner allows an instruction to be shared by two or functions by virtue of the instruction's basic block
     *  being shared by those functions.  If the copying is not performed then the AST will no longer be a tree (it will be
     *  a lattice) but each instruction can point to only one parent basic block (chosen arbitrarily). Thus, a depth-first
     *  traversal of the AST will find the same @ref SgAsmInstruction node more than once, yet following the instruction's
     *  parent pointer will always return the same basic block. */
    bool copyAllInstructions;

    /** Default constructor. */
    AstConstructionSettings()
        : allowEmptyGlobalBlock(false), allowFunctionWithNoBasicBlocks(false), allowEmptyBasicBlocks(false),
          copyAllInstructions(true) {}

    /** Default strict settings.
     *
     *  These settings try to construct an AST that will work with all old AST-based analyses. Some information represented
     *  in the partitioner might not be copied into the AST. */
    static AstConstructionSettings strict() {
        AstConstructionSettings s;
        s.allowEmptyGlobalBlock = false;
        s.allowFunctionWithNoBasicBlocks = false;
        s.allowEmptyBasicBlocks = false;
        s.copyAllInstructions = true;
        return s;
    }

    /** Default permissive settings.
     *
     *  These settings allow as much of the partitioner's information as possible to be copied into the AST even if it means
     *  that AST might violate some invariants that are expected by old analyses.  For instance, it will allow creation of a
     *  basic block with no instructions if the block exists at a virtual addresses that could not be disassembled. */
    static AstConstructionSettings permissive() {
        AstConstructionSettings s;
        s.allowEmptyGlobalBlock = true;
        s.allowFunctionWithNoBasicBlocks = true;
        s.allowEmptyBasicBlocks = true;
        s.copyAllInstructions = true;               // true keeps the AST a tree instead of a lattice
        return s;
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Settings.  All settings must act like properties, which means the following:
//   1. Each setting must have a name that does not begin with a verb.
//   2. Each setting must have a command-line switch to manipulate it.
//   3. Each setting must have a method that queries the property (same name as the property and taking no arguments).
//   4. Each setting must have a modifier method (same name as property but takes a value and returns void)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** How the partitioner should globally treat memory. */
enum MemoryDataAdjustment {
    DATA_IS_CONSTANT,                               /**< Treat all memory as if it were constant. This is accomplished by
                                                     *   removing @ref MemoryMap::READABLE from all segments. */
    DATA_IS_INITIALIZED,                            /**< Treat all memory as if it were initialized. This is a little
                                                     *   weaker than @ref MEMORY_IS_CONSTANT in that it allows the
                                                     *   partitioner to read the value from memory as if it were constant,
                                                     *   but also marks the value as being indeterminate. This is
                                                     *   accomplished by adding @ref MemoryMap::INITIALIZED to all
                                                     *   segments. */
    DATA_NO_CHANGE,                                 /**< Do not make any global changes to the memory map. */
};

/** Settings for loading specimens.
 *
 *  The runtime descriptions and command-line parser for these switches can be obtained from @ref loaderSwitches. */
struct LoaderSettings {
    size_t deExecuteZerosThreshold;                 /**< Size threshold for removing execute permission from zero data. If
                                                     *   this data member is non-zero, then the memory map will be adjusted
                                                     *   by removing execute permission from any region of memory that has
                                                     *   at least this many consecutive zero bytes. The affected regions
                                                     *   are adjusted by the @ref deExecuteZerosLeaveAtFront and @ref
                                                     *   deExecuteZerosLeaveAtBack data members. This happens after the
                                                     *   @ref memoryIsExecutable property is processed. */
    size_t deExecuteZerosLeaveAtFront;              /**< Number of bytes at the beginning of each zero area to leave
                                                     *   unaffected. */
    size_t deExecuteZerosLeaveAtBack;               /**< Number of bytes at the end of each zero area to leave
                                                     *   unaffected. */
    MemoryDataAdjustment memoryDataAdjustment;      /**< How to globally adjust memory segment access bits for data
                                                     *   areas. See the enum for details. The default is @ref
                                                     *   DATA_NO_CHANGE, which causes the partitioner to use the
                                                     *   user-supplied memory map without changing anything. */
    bool memoryIsExecutable;                        /**< Determines whether all of memory should be made executable. The
                                                     *   executability bit controls whether the partitioner is able to make
                                                     *   instructions at that address.  The default, false, means that the
                                                     *   engine will not modify executable bits in memory, but rather use
                                                     *   the bits already set in the memory map. This happens before the
                                                     *   @ref deExecuteZeros property is processed. */

    LoaderSettings()
        : deExecuteZerosThreshold(0), deExecuteZerosLeaveAtFront(16), deExecuteZerosLeaveAtBack(1),
          memoryDataAdjustment(DATA_IS_INITIALIZED), memoryIsExecutable(false) {}
};

/** Settings that control the disassembler.
 *
 *  The runtime descriptions and command-line parser for these switches can be obtained from @ref disassemblerSwitches. */
struct DisassemblerSettings {
    std::string isaName;                            /**< Name of the instruction set architecture. Specifying a non-empty
                                                     *   ISA name will override the architecture that's chosen from the
                                                     *   binary container(s) such as ELF or PE. */
};

/** Controls whether the function may-return analysis runs. */
enum FunctionReturnAnalysis {
    MAYRETURN_DEFAULT_YES,                          /**< Assume a function returns if the may-return analysis cannot
                                                     *   decide whether it may return. */
    MAYRETURN_DEFAULT_NO,                           /**< Assume a function cannot return if the may-return analysis cannot
                                                     *   decide whether it may return. */
    MAYRETURN_ALWAYS_YES,                           /**< Assume that all functions return without ever running the
                                                     *   may-return analysis. */
    MAYRETURN_ALWAYS_NO,                            /**< Assume that a function cannot return without ever running the
                                                     *   may-return analysis. */
};

/** Settings that control the partitioner.
 *
 *  The runtime descriptions and command-line parser for these switches can be obtained from @ref partitionerSwitches. */
struct PartitionerSettings {
    std::vector<rose_addr_t> startingVas;           /**< Addresses at which to start recursive disassembly. These
                                                     *   addresses are in addition to entry addresses, addresses from
                                                     *   symbols, addresses from configuration files, etc. */
    bool usingSemantics;                            /**< Whether instruction semantics are used. If semantics are used,
                                                     *   then the partitioner will have more accurate reasoning about the
                                                     *   control flow graph.  For instance, semantics enable the detection
                                                     *   of certain kinds of opaque predicates. */
    bool followingGhostEdges;                       /**< Should ghost edges be followed during disassembly?  A ghost edge
                                                     *   is a CFG edge that is apparent from the instruction but which is
                                                     *   not taken according to semantics. For instance, a branch
                                                     *   instruction might have two outgoing CFG edges apparent by looking
                                                     *   at the instruction syntax, but a semantic analysis might determine
                                                     *   that only one of those edges can ever be taken. Thus, the branch
                                                     *   has an opaque predicate with one actual edge and one ghost edge. */
    bool discontiguousBlocks;                       /**< Should basic blocks be allowed to be discontiguous. If set, then
                                                     *   the instructions of a basic block do not need to follow one after
                                                     *   the other in memory--the block can have internal unconditional
                                                     *   branches. */
    bool findingFunctionPadding;                    /**< Look for padding before each function entry point? */
    bool findingDeadCode;                           /**< Look for unreachable basic blocks? */
    rose_addr_t peScramblerDispatcherVa;            /**< Run the PeDescrambler module if non-zero. */
    bool findingIntraFunctionCode;                  /**< Suck up unused addresses as intra-function code. */
    bool findingIntraFunctionData;                  /**< Suck up unused addresses as intra-function data. */
    AddressInterval interruptVector;                /**< Table of interrupt handling functions. */
    bool doingPostAnalysis;                         /**< Perform enabled post-partitioning analyses? */
    bool doingPostFunctionMayReturn;                /**< Run function-may-return analysis if doingPostAnalysis is set? */
    bool doingPostFunctionStackDelta;               /**< Run function-stack-delta analysis if doingPostAnalysis is set? */
    bool doingPostCallingConvention;                /**< Run calling-convention analysis if doingPostAnalysis is set? */
    bool doingPostFunctionNoop;                     /**< Find and name functions that are effectively no-ops. */
    FunctionReturnAnalysis functionReturnAnalysis;  /**< How to run the function may-return analysis. */
    bool findingDataFunctionPointers;               /**< Look for function pointers in static data. */
    bool findingThunks;                             /**< Look for common thunk patterns in undiscovered areas. */
    bool splittingThunks;                           /**< Split thunks into their own separate functions. */
    SemanticMemoryParadigm semanticMemoryParadigm;  /**< Container used for semantic memory states. */
    bool namingConstants;                           /**< Give names to constants by calling @ref Modules::nameConstants. */
    bool namingStrings;                             /**< Give labels to constants that are string literal addresses. */

    PartitionerSettings()
        : usingSemantics(false), followingGhostEdges(false), discontiguousBlocks(true), findingFunctionPadding(true),
          findingDeadCode(true), peScramblerDispatcherVa(0), findingIntraFunctionCode(true), findingIntraFunctionData(true),
          doingPostAnalysis(true), doingPostFunctionMayReturn(true), doingPostFunctionStackDelta(true),
          doingPostCallingConvention(false), doingPostFunctionNoop(false), functionReturnAnalysis(MAYRETURN_DEFAULT_YES),
          findingDataFunctionPointers(false), findingThunks(true), splittingThunks(false),
          semanticMemoryParadigm(LIST_BASED_MEMORY), namingConstants(true), namingStrings(true) {}
};

/** Settings for controling the engine behavior.
 *
 *  These settings control the behavior of the engine itself irrespective of how the partitioner is configured. The runtime
 *  descriptions and command-line parser for these switches can be obtained from @ref engineBehaviorSwitches. */
struct EngineSettings {
    std::vector<std::string> configurationNames;    /**< List of configuration files and/or directories. */
};

// Additional declarations w/out definitions yet.
class Partitioner;

} // namespace
} // namespace
} // namespace

#endif
