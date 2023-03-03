#ifndef ROSE_BinaryAnalysis_Partitioner2_EngineBinary_H
#define ROSE_BinaryAnalysis_Partitioner2_EngineBinary_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

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

/** Base class for binary engines driving the partitioner.
 *
 *  A binary engine operates on purely binary, not bytecode (JVM or CIL) specimens.
 *
 *  @section creation Binary engine instance
 *
 *  An engine instance is obtained from the engine factory via a call to a forge(args...) function,
 *  the type returned is a Rose::BinaryAnalysis::Partitioner2::Engine::Ptr. The default engine type for
 *  a call to forge() with no arguments is an Engine::Ptr to an EngineBinary instance.
 *
 *  @code
 *   #include <rose.h>
 *   #include <Rose/BinaryAnalysis/Partitioner2/Engine.h>
 *   using namespace Rose;
 *   namespace P2 = Rose::BinaryAnalysis::Partitioner2;
 *
 *   int main(int argc, char *argv[]) {
 *       std::string purpose = "disassembles a binary specimen";
 *       std::string description =
 *           "This tool disassembles the specified specimen and presents the "
 *           "results as a pseudo assembly listing, that is, a listing intended "
 *           "for human consumption rather than assembly.";
         P2::Engine::Ptr engine = P2::Engine::forge();
 *       SgAsmBlock *gblock = engine->frontend(argc, argv, purpose, description);
 *  @endcode
 *
 */
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

    explicit EngineBinary(const std::string &name); // for creating a factory
    explicit EngineBinary(const Settings &settings);

public:
    virtual ~EngineBinary();

    /** Allocating constructor. */
    static EngineBinaryPtr instance();

    /** Allocating constructor with settings. */
    static EngineBinaryPtr instance(const Settings&);

    /** Allocate a factory. */
    static EngineBinaryPtr factory();

    virtual bool matchFactory(const std::vector<std::string> &specimen) const override;
    virtual EnginePtr instanceFromFactory() override;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  The very top-level use case
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Most basic usage of the partitioner.
     *
     *  This method does everything from parsing the command-line to generating an abstract syntax tree. If all is successful,
     *  then an abstract syntax tree is returned.  The return value is a SgAsmBlock node that contains all the detected
     *  functions. If the specimen consisted of an ELF or PE container then the parent nodes of the returned AST will lead
     *  eventually to an SgProject node.
     *
     *  The command-line can be provided as a typical @c argc and @c argv pair, or as a vector of arguments. In the latter
     *  case, the vector should not include <code>argv[0]</code> or <code>argv[argc]</code> (which is always a null pointer).
     *
     *  The command-line supports a "--help" (or "-h") switch to describe all other switches and arguments, essentially
     *  generating output like a Unix man(1) page.
     *
     *  The @p purpose should be a single line string that will be shown in the title of the man page and should
     *  not start with an upper-case letter, a hyphen, white space, or the name of the command. E.g., a disassembler tool might
     *  specify the purpose as "disassembles a binary specimen".
     *
     *  The @p description is a full, multi-line description written in the Sawyer markup language where "@" characters have
     *  special meaning.
     *
     *  If an <code>std::runtime_exception</code> occurs and the @ref exitOnError property is set, then the exception is caught,
     *  its text is emitted to the partitioner's fatal error stream, and <code>exit(1)</code> is invoked.
     *
     * @{ */
    using Engine::frontend;
    SgAsmBlock* frontend(const std::vector<std::string> &args,
                         const std::string &purpose, const std::string &description) override;
    /** @} */

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Basic top-level steps
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Reset the engine to its initial state.
     *
     *  This does not reset the settings properties since that can be done easily by constructing a new engine.  It only resets
     *  the interpretation, binary loader, disassembler, and memory map so all the top-level steps get executed again. This is
     *  a useful way to re-use the same partitioner to process multiple specimens. */
    virtual void reset() override;

    /** Parse specimen binary containers.
     *
     *  Parses the ELF and PE binary containers to create an abstract syntax tree (AST).  If @p fileNames contains names that
     *  are recognized as raw data or other non-containers then they are skipped over at this stage but processed during the
     *  @ref loadSpecimens stage.
     *
     *  This method tries to allocate a disassember if none is set and an ISA name is specified in the settings, otherwise the
     *  disassembler is chosen later.  It also resets the interpretation to be the return value (see below), and clears the
     *  memory map.
     *
     *  Returns a binary interpretation (perhaps one of many). ELF files have only one interpretation; PE files have a DOS and
     *  a PE interpretation and this method will return the PE interpretation. The user may, at this point, select a different
     *  interpretation. If the list of names has nothing suitable for ROSE's @c frontend function (the thing that does the
     *  container parsing) then the null pointer is returned.
     *
     *  If an <code>std::runtime_exception</code> occurs and the @ref exitOnError property is set, then the exception is caught,
     *  its text is emitted to the partitioner's fatal error stream, and <code>exit(1)</code> is invoked.
     *
     * @{ */
    using Engine::parseContainers;
    virtual SgAsmInterpretation* parseContainers(const std::vector<std::string> &fileNames) override;
    /** @} */

    /** Load and/or link interpretation.
     *
     *  Loads and/or links the engine's interpretation according to the engine's binary loader with these steps:
     *
     *  @li Clears any existing memory map in the engine.
     *
     *  @li If the binary containers have not been parsed (@ref areContainersParsed returns false, i.e., engine has a null
     *      binary interpretation) then @ref parseContainers is called with the same arguments.
     *
     *  @li If binary containers are present but the chosen binary interpretation's memory map is null or empty, then
     *      initialize the memory map by calling @ref loadContainers with the same arguments.
     *
     *  @li Continue initializing the memory map by processing all non-container arguments via @ref loadNonContainers.
     *
     *  Returns a reference to the engine's memory map.
     *
     *  If an <code>std::runtime_exception</code> occurs and the @ref exitOnError property is set, then the exception is caught,
     *  its text is emitted to the partitioner's fatal error stream, and <code>exit(1)</code> is invoked.
     *
     * @{ */
    using Engine::loadSpecimens;
    virtual MemoryMapPtr loadSpecimens(const std::vector<std::string> &fileNames = std::vector<std::string>()) override;
    /** @} */

    /** Partition instructions into basic blocks and functions.
     *
     *  Disassembles and organizes instructions into basic blocks and functions with these steps:
     *
     *  @li If the specimen is not loaded (@ref areSpecimensLoaded) then call @ref loadSpecimens. The no-argument version of
     *  this function requires that specimens have already been loaded.
     *
     *  @li Obtain a disassembler by calling @ref obtainDisassembler.
     *
     *  @li Create a partitioner by calling @ref createPartitioner.
     *
     *  @li Run the partitioner by calling @ref runPartitioner.
     *
     *  Returns the partitioner that was used and which contains the results.
     *
     *  If an <code>std::runtime_exception</code> occurs and the @ref exitOnError property is set, then the exception is caught,
     *  its text is emitted to the partitioner's fatal error stream, and <code>exit(1)</code> is invoked.
     *
     * @{ */
    using Engine::partition;
    virtual PartitionerPtr partition(const std::vector<std::string> &fileNames = std::vector<std::string>()) override;
    /** @} */

    /** Obtain an abstract syntax tree.
     *
     *  Constructs a new abstract syntax tree (AST) from partitioner information with these steps:
     *
     *  @li If the partitioner has not been run yet, then do that now with the same arguments.  The zero-argument version
     *      invokes the zero-argument @ref partition, which requires that the specimen has already been loaded by @ref
     *      loadSpecimens.
     *
     *  @li Call Modules::buildAst to build the AST.
     *
     *  If an <code>std::runtime_exception</code> occurs and the @ref exitOnError property is set, then the exception is caught,
     *  its text is emitted to the partitioner's fatal error stream, and <code>exit(1)</code> is invoked.
     *
     * @{ */
    using Engine::buildAst;
    virtual SgAsmBlock* buildAst(const std::vector<std::string> &fileNames = std::vector<std::string>()) override;
    /** @} */

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Command-line parsing
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    virtual std::list<Sawyer::CommandLine::SwitchGroup> commandLineSwitches() override;
    virtual std::pair<std::string, std::string> specimenNameDocumentation() override;

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
    /** Determine whether a specimen name is a non-container.
     *
     *  Certain strings are recognized as special instructions for how to adjust a memory map and are not intended to be passed
     *  to ROSE's @c frontend function.  This predicate returns true for such strings. */
    virtual bool isNonContainer(const std::string&) override;

    /** Returns true if containers are parsed.
     *
     *  Specifically, returns true if the engine has a non-null interpretation.  If it has a null interpretation then
     *  @ref parseContainers might have already been called but no binary containers specified, in which case calling it again
     *  with the same file names will have no effect. */
    virtual bool areContainersParsed() const override;

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
     *  @li Fail by throwing an <code>std::runtime_error</code>.
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
    /** Create a generic partitioner.
     *
     *  A generic partitioner should work for any architecture but is not fine-tuned for any particular architecture. The
     *  engine must have @ref disassembler (if @ref doDisassemble property is set) and @ref memoryMap properties assigned
     *  already, either explicitly or as the result of earlier steps. */
    virtual PartitionerPtr createGenericPartitioner();

    /** Create a tuned partitioner.
     *
     *  Returns a partitioner that is tuned to operate on a specific instruction set architecture. The engine must have @ref
     *  disassembler (if @ref doDisassemble property is set) and @ref memoryMap properties assigned already, either explicitly
     *  or as the result of earlier steps. */
    virtual PartitionerPtr createTunedPartitioner();

    /** Create a partitioner from an AST.
     *
     *  Partitioner data structures are often more useful and more efficient for analysis than an AST. This method initializes
     *  the engine and a new partitioner with information from the AST. */
    virtual PartitionerPtr createPartitionerFromAst(SgAsmInterpretation*);

    /** Create partitioner.
     *
     *  This is the method usually called to create a new partitioner.  The binary engine class just calls @ref
     *  createTunedPartitioner. */
    virtual PartitionerPtr createPartitioner() override;

    /** Finds interesting things to work on initially.
     *
     *  Seeds the partitioner with addresses and functions where recursive disassembly should begin. */
    virtual void runPartitionerInit(const PartitionerPtr&) override;

    /** Runs the recursive part of partioning.
     *
     *  This is the long-running guts of the partitioner. */
    virtual void runPartitionerRecursive(const PartitionerPtr&) override;

    /** Runs the final parts of partitioning.
     *
     *  This does anything necessary after the main part of partitioning is finished. For instance, it might give names to some
     *  functions that don't have names yet. */
    virtual void runPartitionerFinal(const PartitionerPtr&) override;

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
    virtual std::vector<FunctionPtr> makeUserFunctions(const PartitionerPtr&, const std::vector<rose_addr_t>&);

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
    virtual FunctionPtr makeNextDataReferencedFunction(const PartitionerConstPtr&, rose_addr_t &startVa /*in,out*/);

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
    virtual std::vector<FunctionPtr> makeNextPrologueFunction(const PartitionerPtr&, rose_addr_t startVa);
    virtual std::vector<FunctionPtr> makeNextPrologueFunction(const PartitionerPtr&, rose_addr_t startVa,
                                                              rose_addr_t &lastSearchedVa);
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
    makeFunctionFromInterFunctionCalls(const PartitionerPtr&, rose_addr_t &startVa /*in,out*/);

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
    virtual std::set<rose_addr_t> attachDeadCodeToFunction(const PartitionerPtr&, const FunctionPtr&,
                                                           size_t maxIterations=size_t(-1));

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
     *  This method is called automatically by @ref Engine::runPartitioner if the @ref findingIntraFunctionCode property is set.
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
    virtual std::set<rose_addr_t> attachDeadCodeToFunctions(const PartitionerPtr&, size_t maxIterations=size_t(-1));

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
     *  list, or the list itself can be adjusted.  The list is consulted only when @ref findingThunks is set.
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
     *  The list is consulted only when @ref splittingThunks is set.
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

    // Similar to ::frontend but a lot less complicated.
    virtual SgProject* roseFrontendReplacement(const std::vector<boost::filesystem::path> &fileNames) override;
};

} // namespace
} // namespace
} // namespace

#endif
#endif
