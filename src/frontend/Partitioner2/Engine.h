#ifndef ROSE_Partitioner2_Engine_H
#define ROSE_Partitioner2_Engine_H

#include <Partitioner2/Function.h>
#include <Partitioner2/Partitioner.h>
#include <Partitioner2/Utility.h>

namespace rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

/** Base class for engines driving the partitioner.
 *
 *  A partitioner engine defines the sequence of operations to perform on a partitioner in order to discover instructions,
 *  basic blocks, data blocks, and functions.  Users need not use an engine at all if they prefer to run each step explicitly,
 *  but if they do, there are a few ways to do so:
 *
 *  @li Use an existing engine directly without tweaking it in any way.  This is probably how ROSE @c frontend and may of the
 *      ROSE tools operate.
 *
 *  @li Subclass an engine and override some of its functions with new implementations.
 *
 *  @li Modify the partitioner on which the engine operates either by subclassing or by registering callbacks.
 *
 *  Although this class has quite a few methods, they are intended to be mostly simple things that are easy to replace in
 *  subclasses. */
class Engine {
public:
    virtual ~Engine() {}


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Some utilities
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Ensure specimen is loaded into memory.
     *
     *  If the specified interpretation's memory map is null then an appropriate BinaryLoader is obtained and used to map the
     *  interpretation's segments into virtual memory.  The loader only performs the load step, not dynamic linking.  The
     *  result is that the interpretation's memory map is non-null.
     *
     *  Additionally, if the specified disassembler is the null pointer then an appropriate disassembler is located, cloned,
     *  and returned; an <code>std::runtime_error</code> is thrown if a disassembler cannot be located.  Otherwise, when a
     *  disassembler is specified, that same disassembler pointer is returned. */
    virtual Disassembler* loadSpecimen(SgAsmInterpretation*, Disassembler *disassembler=NULL);

    /** Create a bare partitioner.
     *
     *  A bare partitioner, as far as the engine is concerned, is one that has characteristics that are common across all
     *  architectures but which is missing all architecture-specific functionality.  Using the partitioner's own constructor
     *  is not quite the same--that would produce an even more bare partitioner!   The disassembler and memory map arguments
     *  are passed along to the partitioner's constructor but not used for anything else. */
    virtual Partitioner createBarePartitioner(Disassembler*, const MemoryMap&);

    /** Create a generic partitioner.
     *
     *  A generic partitioner should work for any architecture but is not fine-tuned for any particular architecture. The
     *  disassembler and memory map arguments are passed along to the partitioner's constructor but not used for anything
     *  else. */
    virtual Partitioner createGenericPartitioner(Disassembler*, const MemoryMap&);

    /** Create a tuned partitioner.
     *
     *  Returns a partitioner that is tuned to operate on the architecture described by the specified disassembler. The
     *  disassembler and memory map are passed along to the partitioner's constructor. */
    virtual Partitioner createTunedPartitioner(Disassembler*, const MemoryMap&);


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  High-level methods that mostly call low-level stuff
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Does everything.
     *
     *  Builds a partitioner, calls @ref partition, and builds and returns an AST. */
    virtual SgAsmBlock* partition(SgAsmInterpretation*);

    /** Does almost everything.
     *
     *  Discovers instructions and organizes them into basic blocks and functions.  Many users will want to use lower-level
     *  methods for more control. */
    virtual void partition(Partitioner&, SgAsmInterpretation*);

    /** Discover as many basic blocks as possible.
     *
     *  Processes the "undiscovered" work list until the list becomes empty.  This list is the list of basic block placeholders
     *  for which no attempt has been made to discover instructions.  This method implements a recursive descent disassembler,
     *  although it does not process the control flow edges in any particular order. Subclasses are expected to override this
     *  to implement a more directed approach to discovering basic blocks. */
    virtual void discoverBasicBlocks(Partitioner&);

    /** Discover as many functions as possible.
     *
     *  Discover as many functions as possible by discovering as many basic blocks as possible (@ref discoverBasicBlocks) Each
     *  time we run out of basic blocks to try, we look for another function prologue pattern at the lowest possible address
     *  and then recursively discover more basic blocks.  When this procedure is exhausted a call to @ref
     *  attachBlocksToFunctions tries to attach each basic block to a function.
     *
     *  Returns a list of functions that need more attention.  These are functions for which the CFG is not well behaved--such
     *  as inter-function edges that are not function call edges. */
    virtual std::vector<Function::Ptr> discoverFunctions(Partitioner&);

    /** Runs post-partitioning fixups.
     *
     *  This method is normally run after the CFG/AUM is built. It does things like give names to some functions. The binary
     *  interpretation argument is optional, although some functionality is reduced when it is null. */
    virtual void postPartitionFixups(Partitioner&, SgAsmInterpretation*);
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Methods to make basic blocks
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Discover a basic block.
     *
     *  Obtains an arbitrary basic block address from the list of undiscovered basic blocks (i.e., placeholders that have no
     *  basic block assigned to them yet) and attempts to discover the instructions that belong to that block.  The basic block
     *  is then added to the specified partitioner's CFG/AUM.
     *
     *  Returns the basic block that was discovered, or the null pointer if there are no pending undiscovered blocks. */
    virtual BasicBlock::Ptr makeNextBasicBlock(Partitioner&);


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Methods to make functions.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Make functions based on specimen container.
     *
     *  Traverses the specified interpretation parsed from, for example, related ELF or PE containers, and make functions at
     *  certain addresses that correspond to specimen entry points, imports and exports, symbol tables, etc.  This method only
     *  calls many of the other "make*Functions" methods and accumulates their results.
     *
     *  Returns a list of such functions, some of which may have existed prior to this call. */
    virtual std::vector<Function::Ptr> makeContainerFunctions(Partitioner&, SgAsmInterpretation*);

    /** Make functions at specimen entry addresses.
     *
     *  A function is created at each specimen entry address for all headers in the specified interpretation and adds them to
     *  the specified partitioner's CFG/AUM.
     *
     *  Returns a list of such functions, some of which may have existed prior to this call. */
    virtual std::vector<Function::Ptr> makeEntryFunctions(Partitioner&, SgAsmInterpretation*);

    /** Make functions at error handling addresses.
     *
     *  Makes a function at each error handling address in the specified interpratation and inserts the function into the
     *  specified partitioner's CFG/AUM.
     *
     *  Returns the list of such functions, some of which may have existed prior to this call. */
    virtual std::vector<Function::Ptr> makeErrorHandlingFunctions(Partitioner&, SgAsmInterpretation*);

    /** Make functions at import trampolines.
     *
     *  Makes a function at each import trampoline and inserts them into the specified partitioner's CFG/AUM. An import
     *  trampoline is a thunk that branches to a dynamically loaded/linked function. Since ROSE does not necessarily load/link
     *  dynamic functions, they often don't appear in the executable.  Therefore, this function can be called to create
     *  functions from the trampolines and give them the same name as the function they would have called had the link step
     *  been performed.
     *
     *  Returns a list of such functions, some of which may have existed prior to this call. */
    virtual std::vector<Function::Ptr> makeImportFunctions(Partitioner&, SgAsmInterpretation*);

    /** Make functions at export addresses.
     *
     *  Makes a function at each address that is indicated as being an exported function, and inserts them into the specified
     *  partitioner's CFG/AUM.
     *
     *  Returns a list of such functions, some of which may have existed prior to this call. */
    virtual std::vector<Function::Ptr> makeExportFunctions(Partitioner&, SgAsmInterpretation*);

    /** Make functions for symbols.
     *
     *  Makes a function for each function symbol in the various symbol tables under the specified interpretation and inserts
     *  them into the specified partitioner's CFG/AUM.
     *
     *  Returns a list of such functions, some of which may have existed prior to this call. */
    virtual std::vector<Function::Ptr> makeSymbolFunctions(Partitioner&, SgAsmInterpretation*);

    /** Make functions for function call edges.
     *
     *  Scans the partitioner's CFG to find edges that are marked as function calls and makes a function at each target address
     *  that is concrete.  The function is added to the specified partitioner's CFG/AUM.
     *
     *  Returns a list of such functions, some of which may have existed prior to this call. */
    virtual std::vector<Function::Ptr> makeCalledFunctions(Partitioner&);

    /** Make function at prologue pattern.
     *
     *  Scans executable memory starting at the specified address and which is not represented in the CFG/AUM and looks for
     *  byte patterns and/or instruction patterns that indicate the start of a function.  When a pattern is found a function is
     *  created and inserted into the specified partitioner's CFG/AUM.
     *
     *  Patterns are found by calling the @ref Partitioner::nextFunctionPrologue method, which most likely invokes a variety of
     *  predefined and user-defined callbacks to search for the next pattern.
     *
     *  Returns a pointer to the newly inserted function if one was found, otherwise returns the null pointer. */
    virtual Function::Ptr makeNextPrologueFunction(Partitioner&, rose_addr_t startVa);


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Methods that adjust existing functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Attach basic blocks to functions.
     *
     *  Calls @ref Partitioner::discoverFunctionBasicBlocks once for each known function the partitioner's CFG/AUM in a
     *  sophomoric attempt to assign existing basic blocks to functions.  Returns the list of functions that resulted in
     *  errors.  If @p reportProblems is set then emit messages to mlog[WARN] about problems with the CFG (that stream must
     *  also be enabled if you want to actually see the warnings). */
    virtual std::vector<Function::Ptr> attachBlocksToFunctions(Partitioner&, bool emitWarnings=false);

    /** Attach dead code to function.
     *
     *  Examines the ghost edges for the basic blocks that belong to the specified function in order to discover basic blocks
     *  that are not reachable according the CFG, adds placeholders for those basic blocks, and causes the function to own
     *  those blocks.
     *
     *  If @p maxIterations is larger than one then multiple iterations are performed.  Between each iteration @ref
     *  makeNextBasicBlock is called repeatedly to recursively discover instructions for all pending basic blocks, and then the
     *  CFG is traversed to add function-reachable basic blocks to the function.  The loop terminates when the maximum number
     *  of iterations is reached, or when no more dead code can be found within this function, or when the CFG reaches a state
     *  that has non-call inter-function edges.  In the last case, @ref Partitioner::discoverFunctionBasicBlocks can be called
     *  to by the user to determine what's wrong with the CFG.
     *
     *  Returns the set of newly discovered addresses for unreachable code.  These are the ghost edge target addresses
     *  discovered at each iteration of the loop and do not include addresses of basic blocks that are reachable from the ghost
     *  target blocks. */
    virtual std::set<rose_addr_t> attachDeadCodeToFunction(Partitioner&, const Function::Ptr&, size_t maxIterations=size_t(-1));

    /** Attach dead code to functions.
     *
     *  Calls @ref attachDeadCodeToFunction once for each function that exists in the specified partitioner's CFG/AUM, passing
     *  along @p maxIterations each time.
     *
     *  Returns the union of the dead code addresses discovered for each function. */
    virtual std::set<rose_addr_t> attachDeadCodeToFunctions(Partitioner&, size_t maxIterations=size_t(-1));

    /** Attach function padding to function.
     *
     *  Examines the memory immediately prior to the specified function's entry address to determine if it is alignment
     *  padding.  If so, it creates a data block for the padding and adds it to the function.
     *
     *  Returns the padding data block, which might have existed prior to this call.  Returns null if the function apparently
     *  has no padding. */
    virtual DataBlock::Ptr attachPaddingToFunction(Partitioner&, const Function::Ptr&);

    /** Attach padding to all functions.
     *
     *  Invokes @ref attachPaddingToFunction for each known function and returns the set of data blocks that were returned by
     *  the individual calls. */
    virtual std::vector<DataBlock::Ptr> attachPaddingToFunctions(Partitioner&);

    /** Attach intra-function data to functions.
     *
     *  Looks for addresses that are not part of the partitioner's CFG/AUM and which are surrounded immediately below and above
     *  by the same function and add that address interval as a data block to the surrounding function.  Returns the list of
     *  such data blocks added.
     *
     *  @todo In @ref attachSurroundedDataToFunctions: We can add a single-function version of this if necessary. It was done
     *  this way because it is more efficient to iterate over all unused addresses and find the surrounding functions than it
     *  is to iterate over one function's unused addresses at a time. [Robb P. Matzke 2014-09-08] */
    virtual std::vector<DataBlock::Ptr> attachSurroundedDataToFunctions(Partitioner&);
};

} // namespace
} // namespace
} // namespace

#endif
