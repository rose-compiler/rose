#ifndef ROSE_BinaryAnalysis_Partitioner2_EngineBinary_H
#define ROSE_BinaryAnalysis_Partitioner2_EngineBinary_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/Architecture/BasicTypes.h>
#include <Rose/BinaryAnalysis/Partitioner2/Engine.h>
#include <Rose/BinaryAnalysis/Partitioner2/ModulesLinux.h>
#include <Rose/BinaryAnalysis/Partitioner2/Utility.h>
#include <Rose/BinaryAnalysis/SerialIo.h>

#include <boost/noncopyable.hpp>
#include <boost/regex.hpp>
#include <stdexcept>

#ifdef ROSE_ENABLE_PYTHON_API
#undef slots                                            // stupid Qt pollution
#include <boost/python.hpp>
#endif

namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

/** Engine for specimens containing machine instructions.
 *
 *  This engine is reponsible for creating a partitioner for a specimen that has machine instructions such as the Intel x86 family
 *  of instruction sets, Arm instruction sets, PowerPC instructio sets, Motorola instruction sets, MIPS instruction sets, etc. It is
 *  specifically not to be used for byte code targeting the likes of the Java Virtual Machine (JVM) or the Common Language Runtime
 *  (CLR).
 *
 *  This engine provides an @ref instance static member function that instantiates an engine of this type on the heap and returns a
 *  shared-ownership pointer to the instance. Refer to the base class, @ref Partitioner2::Engine, to learn how to instantiate
 *  engines from factories.
 *
 *  This engine uses a hybrid approach combining linear and recursvie diassembly. Linear disassembly progresses by starting at some
 *  low address in the specimen address space, disassembling one instruction, and then moving on to the next (fallthrough) address
 *  and repeating.  This approach is quite good at disassembling everything (especially for fixed length instructions) but makes no
 *  attempt to organize instructions according to flow of control.  On the other hand, recursive disassembly uses a work list
 *  containing known instruction addresses, disassembles an instruction from the worklist, determines its control flow successors,
 *  and adds those addresses to the work list. As a side effect, it produces a control flow graph. ROSE's hybrid approach uses
 *  linear disassembly to find starting points using heuristics such as common compiler function prologues and epilogues, references
 *  from symbol tables of various types, and other available data. Once starting points are known, ROSE uses recursive disassembly
 *  to follow the control flow. Various kinds of analysis and heuristics are used to control the finer points of recursive
 *  disassembly. Part of the trick to a successful and accurate disassembly of what is essentially equivalent to the halting
 *  problem, depends on finding the right balance between the pure recursive approach and the heuristics and analyses. This balance
 *  is often different for each kind of specimens. */
class EngineBinary: public Engine {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Types
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Shared ownership pointer. */
    using Ptr = EngineBinaryPtr;

private:
    using Super = Engine;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Data members
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
    BinaryLoaderPtr binaryLoader_;                      // how to remap, link, and fixup
    ModulesLinux::LibcStartMain::Ptr libcStartMain_;    // looking for "main" by analyzing libc_start_main?
    ThunkPredicatesPtr functionMatcherThunks_;          // predicates to find thunks when looking for functions
    ThunkPredicatesPtr functionSplittingThunks_;        // predicates for splitting thunks from front of functions

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
protected:
    /** Default constructor. Deleted, use factory method instance() instead. */
    EngineBinary() = delete;

    explicit EngineBinary(const Settings&);

public:
    virtual ~EngineBinary();

    /** Allocating constructor. */
    static Ptr instance();

    /** Allocating constructor with settings. */
    static Ptr instance(const Settings&);

    /** Allocate a factory. */
    static Ptr factory();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Command-line parsing
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:

    /** Command-line switches related to the general engine behavior.
     *
     *  The switches are configured to adjust the specified settings object when parsed. */
    static Sawyer::CommandLine::SwitchGroup engineSwitches(EngineSettings&);

    /** Command-line switches related to loading specimen into memory.
     *
     *  The switches are configured to adjust the specified settings object when parsed. */
    static Sawyer::CommandLine::SwitchGroup loaderSwitches(LoaderSettings&);

    /** Command-line switches related to decoding instructions.
     *
     *  The switches are configured to adjust the specified settings object when parsed. */
    static Sawyer::CommandLine::SwitchGroup disassemblerSwitches(DisassemblerSettings&);

    /** Command-line switches related to partitioning instructions.
     *
     *  The switches are configured to adjust the specified settings object when parsed. */
    static Sawyer::CommandLine::SwitchGroup partitionerSwitches(PartitionerSettings&);

    /** Command-line switches related to constructing an AST from the partitioner.
     *
     *  The switches are configured to adjust the specified settings object when parsed. */
    static Sawyer::CommandLine::SwitchGroup astConstructionSwitches(AstConstructionSettings&);


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Container parsing
    //
    // top-level: parseContainers
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Parses a vxcore specification and initializes memory.
     *
     *  Parses a VxWorks core dump in the format defined by Jim Leek and loads the data into ROSE's analysis memory. The argument
     *  should be everything after the first colon in the URL "vxcore:[MEMORY_ATTRS]:[FILE_ATTRS]:FILE_NAME". */
    virtual void loadVxCore(const std::string &spec);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Load specimens
    //
    // top-level: loadSpecimens
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Obtain a binary loader.
     *
     *  Find a suitable binary loader by one of the following methods (in this order):
     *
     *  @li If this engine's @ref binaryLoader property is non-null, then return that loader.
     *
     *  @li If a binary container was parsed (@ref areContainersParsed returns true and @ref interpretation is non-null) then
     *      a loader is chosen based on the interpretation, and configured to map container sections into memory but not
     *      perform any linking or relocation fixups.
     *
     *  @li If a @p hint is supplied, use it.
     *
     *  @li Fail by throwing an `std::runtime_error`.
     *
     *  In any case, the @ref binaryLoader property is set to this method's return value.
     *
     * @{ */
    virtual BinaryLoaderPtr obtainLoader(const BinaryLoaderPtr &hint);
    virtual BinaryLoaderPtr obtainLoader();
    /** @} */

    /** Loads memory from binary containers.
     *
     *  If the engine has an interpretation whose memory map is missing or empty, then the engine obtains a binary loader via
     *  @ref obtainLoader and invokes its @c load method on the interpretation.  It then copies the interpretation's memory map
     *  into the engine (if present, or leaves it as is). */
    virtual void loadContainers(const std::vector<std::string> &fileNames);

    /** Loads memory from non-containers.
     *
     *  Processes each non-container string (as determined by @ref isNonContainer) and modifies the memory map according to the
     *  string. */
    virtual void loadNonContainers(const std::vector<std::string> &names);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Partitioner high-level functions
    //
    // top-level: partition
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Create a tuned partitioner.
     *
     *  Returns a partitioner that is tuned to operate on a specific instruction set architecture. A @ref memoryMap must be assigned
     *  already, either explicitly or as the result of earlier steps. */
    virtual PartitionerPtr createTunedPartitioner();

    /** Create a partitioner from an AST.
     *
     *  Partitioner data structures are often more useful and more efficient for analysis than an AST. This method initializes
     *  the engine and a new partitioner with information from the AST. */
    virtual PartitionerPtr createPartitionerFromAst(SgAsmInterpretation*);

    /** Partition any sections containing CIL code.
     *
     *  Decodes and partitions any sections of type SgAsmCliHeader. These sections contain CIL byte code.
     *
     *  Returns true if a section containing CIL code was found, false otherwise. */
    virtual bool partitionCilSections(const PartitionerPtr&);

    /** Determine whether the interpretation header contains a CIL code section
     *
     *  When the interpretation has a header with a section named "CLR Runtime Header", it
     *  contains CIL code.  This predicate returns true for such interpretations. */
    bool hasCilCodeSection();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Partitioner mid-level functions
    //
    // These are the functions called by the partitioner high-level stuff.  These are sometimes overridden in subclasses,
    // although it is more likely that the high-level stuff is overridden.
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Make functions at specimen entry addresses.
     *
     *  A function is created at each specimen entry address for all headers in the specified interpretation and adds them to
     *  the specified partitioner's CFG/AUM.
     *
     *  Returns a list of such functions, some of which may have existed prior to this call. */
    virtual std::vector<FunctionPtr> makeEntryFunctions(const PartitionerPtr&, SgAsmInterpretation*);

    /** Make functions at error handling addresses.
     *
     *  Makes a function at each error handling address in the specified interpratation and inserts the function into the
     *  specified partitioner's CFG/AUM.
     *
     *  Returns the list of such functions, some of which may have existed prior to this call. */
    virtual std::vector<FunctionPtr> makeErrorHandlingFunctions(const PartitionerPtr&, SgAsmInterpretation*);

    /** Make functions at import trampolines.
     *
     *  Makes a function at each import trampoline and inserts them into the specified partitioner's CFG/AUM. An import
     *  trampoline is a thunk that branches to a dynamically loaded/linked function. Since ROSE does not necessarily load/link
     *  dynamic functions, they often don't appear in the executable.  Therefore, this function can be called to create
     *  functions from the trampolines and give them the same name as the function they would have called had the link step
     *  been performed.
     *
     *  Returns a list of such functions, some of which may have existed prior to this call. */
    virtual std::vector<FunctionPtr> makeImportFunctions(const PartitionerPtr&, SgAsmInterpretation*);

    /** Make functions at export addresses.
     *
     *  Makes a function at each address that is indicated as being an exported function, and inserts them into the specified
     *  partitioner's CFG/AUM.
     *
     *  Returns a list of such functions, some of which may have existed prior to this call. */
    virtual std::vector<FunctionPtr> makeExportFunctions(const PartitionerPtr&, SgAsmInterpretation*);

    /** Make functions for symbols.
     *
     *  Makes a function for each function symbol in the various symbol tables under the specified interpretation and inserts
     *  them into the specified partitioner's CFG/AUM.
     *
     *  Returns a list of such functions, some of which may have existed prior to this call. */
    virtual std::vector<FunctionPtr> makeSymbolFunctions(const PartitionerPtr&, SgAsmInterpretation*);

    /** Make functions based on specimen container.
     *
     *  Traverses the specified interpretation parsed from, for example, related ELF or PE containers, and make functions at
     *  certain addresses that correspond to specimen entry points, imports and exports, symbol tables, etc.  This method only
     *  calls many of the other "make*Functions" methods and accumulates their results.
     *
     *  Returns a list of such functions, some of which may have existed prior to this call. */
    virtual std::vector<FunctionPtr> makeContainerFunctions(const PartitionerPtr&, SgAsmInterpretation*);

    /** Make functions from an interrupt vector.
     *
     *  Reads the interrupt vector and builds functions for its entries.  The functions are inserted into the partitioner's
     *  CFG/AUM.
     *
     *  Returns the list of such functions, some of which may have existed prior to this call. */
    virtual std::vector<FunctionPtr> makeInterruptVectorFunctions(const PartitionerPtr&, const AddressInterval &vector);

    /** Make a function at each specified address.
     *
     *  A function is created at each address and is attached to the partitioner's CFG/AUM. Returns a list of such functions,
     *  some of which may have existed prior to this call. */
    virtual std::vector<FunctionPtr> makeUserFunctions(const PartitionerPtr&, const std::vector<Address>&);

    /** Discover as many basic blocks as possible.
     *
     *  Processes the "undiscovered" work list until the list becomes empty.  This list is the list of basic block placeholders
     *  for which no attempt has been made to discover instructions.  This method implements a recursive descent disassembler,
     *  although it does not process the control flow edges in any particular order. Subclasses are expected to override this
     *  to implement a more directed approach to discovering basic blocks. */
    virtual void discoverBasicBlocks(const PartitionerPtr&);

    /** Scan read-only data to find function pointers.
     *
     *  Scans read-only data beginning at the specified address in order to find pointers to code, and makes a new function at
     *  when found.  The pointer must be word aligned and located in memory that's mapped read-only (not writable and not
     *  executable), and it must not point to an unknown instruction or an instruction that overlaps with any instruction
     *  that's already in the CFG/AUM.
     *
     *  Returns a pointer to a newly-allocated function that has not yet been attached to the CFG/AUM, or a null pointer if no
     *  function was found.  In any case, the startVa is updated so it points to the next read-only address to check.
     *
     *  Functions created in this manner have the @c SgAsmFunction::FUNC_SCAN_RO_DATA reason. */
    virtual FunctionPtr makeNextDataReferencedFunction(const PartitionerConstPtr&, Address &startVa /*in,out*/);

    /** Scan instruction ASTs to function pointers.
     *
     *  Scans each instruction to find pointers to code and makes a new function when found.  The pointer must be word aligned
     *  and located in memory that's mapped read-only (not writable and not executable), and it most not point to an unknown
     *  instruction of an instruction that overlaps with any instruction that's already in the CFG/AUM.
     *
     *  This function requires that the partitioner has been initialized to track instruction ASTs as they are added to and
     *  removed from the CFG/AUM.
     *
     *  Returns a pointer to a newly-allocated function that has not yet been attached to the CFG/AUM, or a null pointer if no
     *  function was found.
     *
     *  Functions created in this manner have the @c SgAsmFunction::FUNC_INSN_RO_DATA reason. */
    virtual FunctionPtr makeNextCodeReferencedFunction(const PartitionerConstPtr&);

    /** Make functions for function call edges.
     *
     *  Scans the partitioner's CFG to find edges that are marked as function calls and makes a function at each target address
     *  that is concrete.  The function is added to the specified partitioner's CFG/AUM.
     *
     *  Returns a list of such functions, some of which may have existed prior to this call. */
    virtual std::vector<FunctionPtr> makeCalledFunctions(const PartitionerPtr&);

    /** Make function at prologue pattern.
     *
     *  Scans executable memory starting at the specified address and which is not represented in the CFG/AUM and looks for
     *  byte patterns and/or instruction patterns that indicate the start of a function.  When a pattern is found a function
     *  (or multiple functions, depending on the type of matcher) is created and inserted into the specified partitioner's
     *  CFG/AUM.
     *
     *  Patterns are found by calling the @ref Partitioner::nextFunctionPrologue method, which most likely invokes a variety of
     *  predefined and user-defined callbacks to search for the next pattern.
     *
     *  Returns a vector of non-null function pointers pointer for the newly inserted functions, otherwise returns an empty
     *  vector. If the @p lastSearchedVa is provided, it will be set to the highest address at which a function prologue was
     *  searched.
     *
     * @{ */
    virtual std::vector<FunctionPtr> makeNextPrologueFunction(const PartitionerPtr&, Address startVa);
    virtual std::vector<FunctionPtr> makeNextPrologueFunction(const PartitionerPtr&, Address startVa, Address &lastSearchedVa);
    /** @} */

    /** Make functions from inter-function calls.
     *
     *  This method scans the unused executable areas between existing functions to look for additional function calls and
     *  creates new functions for those calls.  It starts the scan at @p startVa which is updated upon return to be the next
     *  address that needs to be scanned. The @p startVa is never incremented past the end of the address space (i.e., it never
     *  wraps back around to zero), so care should be taken to not call this in an infinite loop when the end of the address
     *  space is reached.
     *
     *  The scanner tries to discover new basic blocks in the unused portion of the address space. These basic blocks are not
     *  allowed to overlap with existing, attached basic blocks, data blocks, or functions since that is an indication that we
     *  accidentally disassembled non-code.  If the basic block looks like a function call and the target address(es) is not
     *  pointing into the middle of an existing basic block, data-block, or function then a new function is created at the
     *  target address. The basic blocks which were scanned are not explicitly attached to the partitioner's CFG since we
     *  cannot be sure we found their starting address, but they might be later attached by following the control flow from the
     *  functions we did discover.
     *
     *  Returns the new function(s) for the first basic block that satisfied the requirements outlined above, and updates @p
     *  startVa to be a greater address which is not part of the basic block that was scanned. */
    virtual std::vector<FunctionPtr>
    makeFunctionFromInterFunctionCalls(const PartitionerPtr&, Address &startVa /*in,out*/);

    /** Discover as many functions as possible.
     *
     *  Discover as many functions as possible by discovering as many basic blocks as possible (@ref discoverBasicBlocks), Each
     *  time we run out of basic blocks to try, we look for another function prologue pattern at the lowest possible address
     *  and then recursively discover more basic blocks.  When this procedure is exhausted a call to @ref
     *  attachBlocksToFunctions tries to attach each basic block to a function. */
    virtual void discoverFunctions(const PartitionerPtr&);

    /** Attach dead code to function.
     *
     *  Examines the ghost edges for the basic blocks that belong to the specified function in order to discover basic blocks
     *  that are not reachable according the CFG, adds placeholders for those basic blocks, and causes the function to own
     *  those blocks.
     *
     *  If @p maxIterations is larger than one then multiple iterations are performed.  Between each iteration @ref
     *  makeNextBasicBlock is called repeatedly to recursively discover instructions for all pending basic blocks, and then the
     *  CFG is traversed to add function-reachable basic blocks to the function.  The loop terminates when the maximum number
     *  of iterations is reached, or when no more dead code can be found within this function.
     *
     *  Returns the set of newly discovered addresses for unreachable code.  These are the ghost edge target addresses
     *  discovered at each iteration of the loop and do not include addresses of basic blocks that are reachable from the ghost
     *  target blocks. */
    virtual std::set<Address> attachDeadCodeToFunction(const PartitionerPtr&, const FunctionPtr&, size_t maxIterations=size_t(-1));

    /** Attach function padding to function.
     *
     *  Examines the memory immediately prior to the specified function's entry address to determine if it is alignment
     *  padding.  If so, it creates a data block for the padding and adds it to the function.
     *
     *  Returns the padding data block, which might have existed prior to this call.  Returns null if the function apparently
     *  has no padding. */
    virtual DataBlockPtr attachPaddingToFunction(const PartitionerPtr&, const FunctionPtr&);

    /** Attach padding to all functions.
     *
     *  Invokes @ref attachPaddingToFunction for each known function and returns the set of data blocks that were returned by
     *  the individual calls. */
    virtual std::vector<DataBlockPtr> attachPaddingToFunctions(const PartitionerPtr&);

    /** Attach  all possible intra-function basic blocks to functions.
     *
     *  This is similar to @ref attachSurroundedCodeToFunctions except it calls that method repeatedly until it cannot do
     *  anything more.  Between each call it also follows the CFG for the newly discovered blocks to discover as many blocks as
     *  possible, creates more functions by looking for function calls, and attaches additional basic blocks to functions by
     *  following the CFG for each function.
     *
     *  This method is called automatically by @ref Engine::runPartitioner if the @ref PartitionerSettings::findingIntraFunctionCode
     *  property is set.
     *
     *  Returns the sum from all the calls to @ref attachSurroundedCodeToFunctions. */
    virtual size_t attachAllSurroundedCodeToFunctions(const PartitionerPtr&);

    /** Attach intra-function basic blocks to functions.
     *
     *  This method scans the unused address intervals (those addresses that are not represented by the CFG/AUM). For each
     *  unused interval, if the interval is immediately surrounded by a single function then a basic block placeholder is
     *  created at the beginning of the interval and added to the function.
     *
     *  Returns the number of new placeholders created. */
    virtual size_t attachSurroundedCodeToFunctions(const PartitionerPtr&);

    /** Attach basic blocks to functions.
     *
     *  Calls @ref Partitioner::discoverFunctionBasicBlocks once for each known function the partitioner's CFG/AUM in a
     *  sophomoric attempt to assign existing basic blocks to functions. */
    virtual void attachBlocksToFunctions(const PartitionerPtr&);

    /** Attach dead code to functions.
     *
     *  Calls @ref attachDeadCodeToFunction once for each function that exists in the specified partitioner's CFG/AUM, passing
     *  along @p maxIterations each time.
     *
     *  Returns the union of the dead code addresses discovered for each function. */
    virtual std::set<Address> attachDeadCodeToFunctions(const PartitionerPtr&, size_t maxIterations=size_t(-1));

    /** Attach intra-function data to functions.
     *
     *  Looks for addresses that are not part of the partitioner's CFG/AUM and which are surrounded immediately below and above
     *  by the same function and add that address interval as a data block to the surrounding function.  Returns the list of
     *  such data blocks added.
     *
     *  @todo In @ref attachSurroundedDataToFunctions: We can add a single-function version of this if necessary. It was done
     *  this way because it is more efficient to iterate over all unused addresses and find the surrounding functions than it
     *  is to iterate over one function's unused addresses at a time. [Robb P. Matzke 2014-09-08] */
    virtual std::vector<DataBlockPtr> attachSurroundedDataToFunctions(const PartitionerPtr&);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Partitioner low-level functions
    //
    // These are functions that a subclass seldom overrides, and maybe even shouldn't override because of their complexity or
    // the way the interact with one another.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Insert a call-return edge and discover its basic block.
     *
     *  Inserts a call-return (@ref E_CALL_RETURN) edge for some function call that lacks such an edge and for which the callee
     *  may return.  The @p assumeCallReturns parameter determines whether a call-return edge should be added or not for
     *  callees whose may-return analysis is indeterminate.  If @p assumeCallReturns is true then an indeterminate callee will
     *  have a call-return edge added; if false then no call-return edge is added; if indeterminate then no call-return edge is
     *  added at this time but the vertex is saved so it can be reprocessed later.
     *
     *  Returns true if a new call-return edge was added to some call, or false if no such edge could be added. A post
     *  condition for a false return is that the pendingCallReturn list is empty. */
    virtual bool makeNextCallReturnEdge(const PartitionerPtr&, boost::logic::tribool assumeCallReturns);

    /** Discover basic block at next placeholder.
     *
     *  Discovers a basic block at some arbitrary placeholder.  Returns a pointer to the new basic block if a block was
     *  discovered, or null if no block is discovered.  A postcondition for a null return is that the CFG has no edges coming
     *  into the "undiscovered" vertex. */
    virtual BasicBlockPtr makeNextBasicBlockFromPlaceholder(const PartitionerPtr&);

    /** Discover a basic block.
     *
     *  Discovers another basic block if possible.  A variety of methods will be used to determine where to discover the next
     *  basic block:
     *
     *  @li Discover a block at a placeholder by calling @ref makeNextBasicBlockFromPlaceholder
     *
     *  @li Insert a new call-return (@ref E_CALL_RETURN) edge for a function call that may return.  Insertion of such an
     *      edge may result in a new placeholder for which this method then discovers a basic block.  The call-return insertion
     *      happens in two passes: the first pass only adds an edge for a callee whose may-return analysis is positive; the
     *      second pass relaxes that requirement and inserts an edge for any callee whose may-return is indeterminate (i.e., if
     *      ROSE can't prove that a callee never returns then assume it may return).
     *
     *  Returns the basic block that was discovered, or the null pointer if there are no pending undiscovered blocks. */
    virtual BasicBlockPtr makeNextBasicBlock(const PartitionerPtr&);


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Build AST
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Settings and properties
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Property: binary loader.
     *
     *  The binary loader that maps a binary container's sections into simulated memory and optionally performs dynamic linking
     *  and relocation fixups.  If none is specified then the engine will choose one based on the container.
     *
     * @{ */
    BinaryLoaderPtr binaryLoader() const /*final*/;
    virtual void binaryLoader(const BinaryLoaderPtr&);
    /** @} */

    /** Property: Predicate for finding functions that are thunks.
     *
     *  This collective predicate is used when searching for function prologues in order to create new functions. Its purpose
     *  is to try to match sequences of instructions that look like thunks and then create a function at that address. A suitable
     *  default list of predicates is created when the engine is initialized, and can either be replaced by a new list, an empty
     *  list, or the list itself can be adjusted.  The list is consulted only when @ref PartitionerSettings::findingThunks is set.
     *
     * @{ */
    ThunkPredicatesPtr functionMatcherThunks() const /*final*/;
    virtual void functionMatcherThunks(const ThunkPredicatesPtr&);
    /** @} */

    /** Property: Predicate for finding thunks at the start of functions.
     *
     *  This collective predicate is used when searching for thunks at the beginnings of existing functions in order to split
     *  those thunk instructions into their own separate function.  A suitable default list of predicates is created when the
     *  engine is initialized, and can either be replaced by a new list, an empty list, or the list itself can be adjusted.
     *  The list is consulted only when @ref PartitionerSettings::splittingThunks is set.
     *
     * @{ */
    ThunkPredicatesPtr functionSplittingThunks() const /*final*/;
    virtual void functionSplittingThunks(const ThunkPredicatesPtr&);
    /** @} */

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Python API support functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef ROSE_ENABLE_PYTHON_API

    // Similar to frontend, but returns a partitioner rather than an AST since the Python API doesn't yet support ASTs.
    PartitionerPtr pythonParseVector(boost::python::list &pyArgs, const std::string &purpose, const std::string &description);
    PartitionerPtr pythonParseSingle(const std::string &specimen, const std::string &purpose, const std::string &description);

#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Internal stuff
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
    void init();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Overrides documented in the base class
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    virtual bool matchFactory(const std::vector<std::string> &specimen) const override;
    virtual EnginePtr instanceFromFactory(const Settings&) override;
    virtual void reset() override;

    using Engine::frontend;
    SgAsmBlock* frontend(const std::vector<std::string> &args,
                         const std::string &purpose, const std::string &description) override;

    using Engine::parseContainers;
    virtual SgAsmInterpretation* parseContainers(const std::vector<std::string> &fileNames) override;

    using Engine::loadSpecimens;
    virtual MemoryMapPtr loadSpecimens(const std::vector<std::string> &fileNames = std::vector<std::string>()) override;

    using Engine::partition;
    virtual PartitionerPtr partition(const std::vector<std::string> &fileNames = std::vector<std::string>()) override;

    using Engine::buildAst;
    virtual SgAsmBlock* buildAst(const std::vector<std::string> &fileNames = std::vector<std::string>()) override;

    virtual std::list<Sawyer::CommandLine::SwitchGroup> commandLineSwitches() override;
    virtual std::pair<std::string, std::string> specimenNameDocumentation() override;
    virtual bool isNonContainer(const std::string&) override;
    virtual bool areContainersParsed() const override;
    virtual PartitionerPtr createPartitioner() override;
    virtual void runPartitionerInit(const PartitionerPtr&) override;
    virtual void runPartitionerRecursive(const PartitionerPtr&) override;
    virtual void runPartitionerFinal(const PartitionerPtr&) override;
    virtual SgProject* roseFrontendReplacement(const std::vector<boost::filesystem::path> &fileNames) override;
};

} // namespace
} // namespace
} // namespace

#endif
#endif
