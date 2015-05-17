#ifndef ROSE_Partitioner2_Engine_H
#define ROSE_Partitioner2_Engine_H

#include <BinaryLoader.h>
#include <Disassembler.h>
#include <FileSystem.h>
#include <Partitioner2/Function.h>
#include <Partitioner2/Partitioner.h>
#include <Partitioner2/Utility.h>
#include <sawyer/DistinctList.h>

namespace rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

/** Base class for engines driving the partitioner.
 *
 *  An engine serves these main purposes:
 *
 *  @li It holds configuration information related to disassembling and partitioning and provides methods for initializing that
 *      configuration information.
 *
 *  @li It provides methods for creating components (disassembler, partitioner) from stored configuration information.
 *
 *  @li It provides methods that implement the basic steps a program typically goes through in order to disassemble a specimen,
 *      such as parsing the binary container, mapping files and container sections into simulated specimen memory, calling a
 *      dynamic linker, disassembling and partitioning, and building an abstract syntax tree.
 *
 *  @li It provides a set of behaviors for the partitioner that are suitable for various situations.  For instance, the engine
 *      controls the order that functions and basic blocks are discovered, what happens when the partitioner encounters
 *      conflicts when assigning basic blocks to functions, etc.
 *
 *  Use of an engine is entirely optional.  All of the engine's actions are implemented in terms of public APIs on other
 *  objects such as @ref Disassembler and @ref Partitioner.  In fact, this particular engine base class is disigned so that
 *  users can pick and choose to use only those steps they need, or perhaps to call the main actions one step at a time with
 *  the user making adjustments between steps.
 *
 *  @section extensibility Custimization
 *
 *  The actions taken by an engine can be customized in a number of ways:
 *
 *  @li The engine can be subclassed. All object methods that are intended to be overridable in a subclass are declared as
 *      virtual.  Some simple functions, like those that return property values, are not virtual since they're not things that
 *      one normally overrides.  This engine's methods are also designed to be as modular as possible--each method does exactly
 *      one thing, and higher-level methods sew those things together into sequences.
 *
 *  @li Instead of calling one function that does everything from parsing the command-line to generating the final abstract
 *      syntax tree, the engine breaks things into steps. The user can invoke one step at a time and make adjustments between
 *      steps.  This is actually the most common custimization within the tools distributed with ROSE.
 *
 *  @li The behavior of the @ref Partitioner itself can be modified by attaching callbacks to it. In fact, if the engine is
 *      used to create a partitioner then certain engine-defined callbacks are added to the partitioner.
 *
 *  @section basic Basic usage
 *
 *  The most basic use case for the engine is to pass it the command-line arguments and have it do everything, eventually
 *  returning an abstract syntax tree.
 *
 *  @code
 *   #include <rose.h>
 *   #include <Partitioner2/Engine.h>
 *   using namespace rose;
 *   namespace P2 = rose::BinaryAnalysis::Partitioner2;
 *
 *   int main(int argc, char *argv[]) {
 *       std::string purpose = "disassembles a binary specimen";
 *       std::string description =
 *           "This tool disassembles the specified specimen and presents the "
 *           "results as a pseudo assembly listing, that is, a listing intended "
 *           "for human consumption rather than assembly.";
 *       SgProject *project = P2::Engine().frontend(argc, argv, purpose, description);
 *  @endcode
 *
 *  @section topsteps High level operations
 *
 *  While @ref frontend does everything, it's often useful to break it down to individual steps so that adjustments can be made
 *  along the way. This next level of steps are:
 *
 *  @li Parse the command-line to adjust engine settings. See @ref parseCommandLine.
 *
 *  @li Parse binary containers to create a container abstract syntax tree.  This step parses things like section tables,
 *      symbol tables, import and export tables, etc. but does not disassemble any instructions.  See @ref parseContainers.
 *
 *  @li Create a memory map that simulates process address space for the specimen.  This consists of running a dynamic linker,
 *      loading raw data into the simulated address space, and adjusting the memory map . All of these steps are optional. See
 *      @ref loadSpecimens.
 *
 *  @li Create a partitioner.  The partitioner is responsible for driving the disassembler based on control flow information
 *      obtained by examining previously disassembled instructions, user-specified configuration files, command-line switches,
 *      etc. See @ref createPartitioner.
 *
 *  @li Run partitioner.  This step uses the disassembler and partitioner to discover instructions, basic blocks, data blocks,
 *      and functions and updates the partitioner's internal data structures. See @ref runPartitioner.
 *
 *  @li Create an abstract syntax tree from the partitioner's data structures.  Most of ROSE is designed to operate on an AST,
 *      although many binary analysis capabilities are built directly on the more efficient partitioner data structures.
 *      Because of this, the partitioner also has a mechanism by which its data structures can be initialized from an AST.
 */
class Engine {
public:
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Settings.  All settings must act like properties, which means the following:
    //   1. Each setting must have a name that does not begin with a verb.
    //   2. Each setting must have a command-line switch to manipulate it.
    //   3. Each setting must have a method that queries the property (same name as the property and taking no arguments).
    //   4. Each setting must have a modifier method (same name as property but takes a value and returns void)
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /** Settings for loading specimens.
     *
     *  The runtime descriptions and command-line parser for these switches can be obtained from @ref loaderSwitches. */
    struct LoaderSettings {
        size_t deExecuteZeros;                          /**< Size threshold for removing execute permission from zero data. If
                                                         *   this data member is non-zero, then the memory map will be adjusted
                                                         *   by removing execute permission from any region of memory that has
                                                         *   at least this many consecutive zero bytes. */

        LoaderSettings()
            : deExecuteZeros(0) {}
    };

    /** Settings that control the disassembler.
     *
     *  The runtime descriptions and command-line parser for these switches can be obtained from @ref disassemblerSwitches. */
    struct DisassemblerSettings {
        std::string isaName;                            /**< Name of the instruction set architecture. Specifying a non-empty
                                                         *   ISA name will override the architecture that's chosen from the
                                                         *   binary container(s) such as ELF or PE. */
    };

    /** Settings that control creation of the partitioner.
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
        bool doingPostAnalysis;                         /**< Perform post-partitioning analysis phase? */
        bool functionReturnsAssumed;                    /**< Assume functions return if cannot prove otherwise? */
        bool findingDataFunctionPointers;               /**< Look for function pointers in static data. */

        PartitionerSettings()
            : usingSemantics(false), followingGhostEdges(false), discontiguousBlocks(true), findingFunctionPadding(true),
              findingDeadCode(true), peScramblerDispatcherVa(0), findingIntraFunctionCode(true), findingIntraFunctionData(true),
              doingPostAnalysis(true), functionReturnsAssumed(true), findingDataFunctionPointers(false) {}
    };

    /** Settings for controling the engine behavior.
     *
     *  These settings control the behavior of the engine itself irrespective of how the partitioner is configured. The runtime
     *  descriptions and command-line parser for these switches can be obtained from @ref engineBehaviorSwitches. */
    struct EngineSettings {
        std::vector<std::string> configurationNames;    /**< List of configuration files and/or directories. */
    };

    /** Settings for the engine.
     *
     *  The engine is configured by adjusting these settings, usually shortly after the engine is created. */
    struct Settings {
        LoaderSettings loader;                          /**< Settings used during specimen loading. */
        DisassemblerSettings disassembler;              /**< Settings for creating the disassembler. */
        PartitionerSettings partitioner;                /**< Settings for creating a partitioner. */
        EngineSettings engine;                          /**< Settings that control engine behavior. */
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Internal data structures
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
    // Basic blocks that need to be worked on next. These lists are adjusted whenever a new basic block (or placeholder) is
    // inserted or erased from the CFG.
    class BasicBlockWorkList: public CfgAdjustmentCallback {
    private:
        Sawyer::Container::DistinctList<rose_addr_t> pendingCallReturn_;   // blocks that might need an E_CALL_RETURN edge
        Sawyer::Container::DistinctList<rose_addr_t> processedCallReturn_; // call sites whose may-return was indeterminate
        Sawyer::Container::DistinctList<rose_addr_t> finalCallReturn_;     // indeterminated call sites awaiting final analysis
        Sawyer::Container::DistinctList<rose_addr_t> undiscovered_;        // undiscovered basic block list (last-in-first-out)
    public:
        typedef Sawyer::SharedPointer<BasicBlockWorkList> Ptr;
        static Ptr instance() { return Ptr(new BasicBlockWorkList); }
        virtual bool operator()(bool chain, const AttachedBasicBlock &args) ROSE_OVERRIDE;
        virtual bool operator()(bool chain, const DetachedBasicBlock &args) ROSE_OVERRIDE;
        Sawyer::Container::DistinctList<rose_addr_t>& pendingCallReturn() { return pendingCallReturn_; }
        Sawyer::Container::DistinctList<rose_addr_t>& processedCallReturn() { return processedCallReturn_; }
        Sawyer::Container::DistinctList<rose_addr_t>& finalCallReturn() { return finalCallReturn_; }
        Sawyer::Container::DistinctList<rose_addr_t>& undiscovered() { return undiscovered_; }
        void moveAndSortCallReturn(const Partitioner&);
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Data members
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
    Settings settings_;                                 // Settings for the partitioner.
    SgAsmInterpretation *interp_;                       // interpretation set by loadSpecimen
    BinaryLoader *binaryLoader_;                        // how to remap, link, and fixup
    Disassembler *disassembler_;                        // not ref-counted yet, but don't destroy it since user owns it
    MemoryMap map_;                                     // memory map initialized by load()
    BasicBlockWorkList::Ptr basicBlockWorkList_;        // what blocks to work on next

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Constructors
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Default constructor. */
    Engine()
        : interp_(NULL), binaryLoader_(NULL), disassembler_(NULL), basicBlockWorkList_(BasicBlockWorkList::instance()) {
        init();
    }

    virtual ~Engine() {}

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
     * @{ */
    SgAsmBlock* frontend(int argc, char *argv[],
                         const std::string &purpose, const std::string &description);
    virtual SgAsmBlock* frontend(const std::vector<std::string> &args,
                                 const std::string &purpose, const std::string &description);
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
    void reset();

    /** Parse the command-line.
     *
     *  This method parses the command-line and uses it to update this engine's settings.  Since a command line is usually more
     *  than just engine-related switches, the more usual approach is for the user to obtain engine-related command-line switch
     *  declarations and parse the command-line in user code.
     *
     *  This function automatically applies the command-line when it's successfully parsed, thereby updating this engine's
     *  settings.  If something goes wrong with the command-line then an <code>std::runtime_error</code> is thrown.
     *
     *  The command-line can be provided as a typical @c argc and @c argv pair, or as a vector of arguments. In the latter
     *  case, the vector should not include <code>argv[0]</code> or <code>argv[argc]</code> (which is always a null pointer).
     *
     *  The @p purpose should be a single line string that will be shown in the title of the man page and should
     *  not start with an upper-case letter, a hyphen, white space, or the name of the command. E.g., a disassembler tool might
     *  specify the purpose as "disassembles a binary specimen".
     *
     *  The @p description is a full, multi-line description written in the Sawyer markup language where "@" characters have
     *  special meaning.
     *
     *  If the tool requires additional switches, an opportunity to adjust the parser, or other special handling, it can call
     *  @ref commandLineParser to obtain a parser and then call its @c parse and @c apply methods explicitly.
     *
     * @{ */
    Sawyer::CommandLine::ParserResult parseCommandLine(int argc, char *argv[],
                                                       const std::string &purpose, const std::string &description);
    virtual Sawyer::CommandLine::ParserResult parseCommandLine(const std::vector<std::string> &args,
                                                               const std::string &purpose, const std::string &description);
    /** @} */

    /** Parse specimen binary containers.
     *
     *  Parses the ELF and PE binary containers to create an abstract syntax tree (AST).  If @p fileNames contains names that
     *  are recognized as raw data or other non-containers then they are skipped over at this stage but processed during the
     *  @ref load stage.
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
     * @{ */
    virtual SgAsmInterpretation* parseContainers(const std::vector<std::string> &fileNames);
    SgAsmInterpretation* parseContainers(const std::string &fileName) /*final*/;
    /** @} */

    /** Load and/or link interpretation.
     *
     *  Loads and/or links the engine's interpretation according to the engine's binary loader with these steps:
     *
     *  @li Clears any existing memory map in the engine.
     *
     *  @li If the binary containers have not been parsed (@ref areContainersParsed returns false, i.e., engine has a null
     *      binary  interpretation) then @ref parseContainers is called with the same arguments.
     *
     *  @li If binary containers are present but the chosen binary interpretation's memory map is null or empty, then
     *      initialize the memory map by calling @ref loadContainers with the same arguments.
     *
     *  @li Continue initializing the memory map by processing all non-container arguments via @ref loadNonContainers.
     *
     *  Returns a reference to the engine's memory map.
     *
     * @{ */
    virtual MemoryMap& loadSpecimens(const std::vector<std::string> &fileNames = std::vector<std::string>());
    MemoryMap& loadSpecimens(const std::string &fileName) /*final*/;
    /** @} */

    /** Partition instructions into basic blocks and functions.
     *
     *  Disassembles and organizes instructions into basic blocks and functions with these steps:
     *
     *  @li If the specimen is not loaded (@ref areSpecimensLoaded) then call @ref loadSpecimens.
     *
     *  @li Obtain a disassembler by calling @ref obtainDisassembler.
     *
     *  @li Create a partitioner by calling @ref createPartitioner.
     *
     *  @li Run the partitioner by calling @ref runPartitioner.
     *
     *  Returns the partitioner that was used and which contains the results.
     *
     * @{ */
    virtual Partitioner partition(const std::vector<std::string> &fileNames = std::vector<std::string>());
    Partitioner partition(const std::string &fileName) /*final*/;
    /** @} */

    /** Obtain an abstract syntax tree.
     *
     *  Constructs a new abstract syntax tree (AST) from partitioner information with these steps:
     *
     *  @li If the partitioner has not been run yet (according to @ref isPartitioned), then do that now with the same
     *      arguments.
     *
     *  @li Call Modules::buildAst to build the AST.
     *
     * @{ */
    SgAsmBlock* buildAst(const std::vector<std::string> &fileNames = std::vector<std::string>()) /*final*/;
    SgAsmBlock* buildAst(const std::string &fileName) /*final*/;
    /** @} */

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Command-line parsing
    //
    // top-level: parseCommandLine
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Command-line switches related to the loader. */
    virtual Sawyer::CommandLine::SwitchGroup loaderSwitches();

    /** Command-line switches related to the disassembler. */
    virtual Sawyer::CommandLine::SwitchGroup disassemblerSwitches();

    /** Command-line switches related to the partitioner. */
    virtual Sawyer::CommandLine::SwitchGroup partitionerSwitches();

    /** Command-line switches related to engine behavior. */
    virtual Sawyer::CommandLine::SwitchGroup engineSwitches();

    /** Documentation for specimen names. */
    static std::string specimenNameDocumentation();

    /** Creates a command-line parser.
     *
     *  Creates and returns a command-line parser suitable for parsing command-line switches and arguments needed by the
     *  disassembler.
     *
     *  The @p purpose should be a single line string that will be shown in the title of the man page and should
     *  not start with an upper-case letter, a hyphen, white space, or the name of the command. E.g., a disassembler tool might
     *  specify the purpose as "disassembles a binary specimen".
     *
     *  The @p description is a full, multi-line description written in the Sawyer markup language where "@" characters have
     *  special meaning. */
    virtual Sawyer::CommandLine::Parser commandLineParser(const std::string &purpose, const std::string &description);

    /** Check settings after command-line is processed.
     *
     *  This does some checks and further configuration immediately after processing the command line. It's also called by most
     *  of the top-level operations.
     *
     *  If an ISA name is specified in the settings and no disassembler has been set yet, then a disassembler is allocated. */
    virtual void checkSettings();

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
    virtual bool isNonContainer(const std::string&);

    /** Returns true if containers are parsed.
     *
     *  Specifically, returns true if the engine has a non-null interpretation.  If it has a null interpretation then
     *  @ref parseContainers might have already been called but no binary containers specified, in which case calling it again
     *  with the same file names will have no effect. */
    virtual bool areContainersParsed() const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Load specimens
    //
    // top-level: loadSpecimens
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Returns true if specimens are loaded.
     *
     *  Specifically, returns true if the memory map is non-empty. */
    virtual bool areSpecimensLoaded() const;

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
     *  In any case, the @ref binaryLoader property is set to this method's return value. */
    virtual BinaryLoader *obtainLoader(BinaryLoader *hint=NULL);

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

    /** Adjust memory map post-loading.
     *
     *  Make adjustments to the memory map after the specimen is loaded. */
    virtual void adjustMemoryMap();

    /** Property: memory map
     *
     *  Returns the memory map resulting from the @ref loadSpecimens step.  This is a combination of the memory map created by
     *  the BinaryLoader (via @ref loadContainers) and stored in the interpretation, and the application of any memory map
     *  resources (via @ref loadNonContainers). During partitioning operations the memory map comes from the partitioner
     *  itself.  See @ref loadSpecimens.
     *
     *  The return value is a non-const reference so that the map can be manipulated directly if desired.
     *
     * @{ */
    MemoryMap& memoryMap() /*final*/ { return map_; }
    const MemoryMap& memoryMap() const /*final*/ { return map_; }
    virtual void memoryMap(const MemoryMap &m) { map_ = m; }
    /** @} */


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Disassembler
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Obtain a disassembler.
     *
     *  Chooses a disassembler based on one of the following (in this order):
     *
     *  @li If this engine's @ref disassembler property is non-null, then return that disassembler.
     *
     *  @li If this engine's ISA name setting is non-empty, then use it to obtain a disassembler.
     *
     *  @li If a binary container was parsed (@ref areContainersParsed returns true and @ref interpretation is non-null) then
     *      try to obtain a disassembler based on the interpretation.
     *
     *  @li If a @p hint is supplied, then use it.
     *
     *  @li Fail by throwing an <code>std::runtime_error</code>.
     *
     *  In any case, the @ref disassembler property is set to this method's return value. */
    virtual Disassembler* obtainDisassembler(Disassembler *hint=NULL);
    /** @} */


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Partitioner high-level functions
    //
    // top-level: partition
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Check that we have everything necessary to create a partitioner. */
    virtual void checkCreatePartitionerPrerequisites() const;

    /** Create a bare partitioner.
     *
     *  A bare partitioner, as far as the engine is concerned, is one that has characteristics that are common across all
     *  architectures but which is missing all architecture-specific functionality.  Using the partitioner's own constructor
     *  is not quite the same--that would produce an even more bare partitioner!  The engine must have @ref disassembler and
     *  @ref memoryMap properties already either assigned explicitly or as the result of earlier steps. */
    virtual Partitioner createBarePartitioner();

    /** Create a generic partitioner.
     *
     *  A generic partitioner should work for any architecture but is not fine-tuned for any particular architecture. The
     *  engine must have @ref disassembler and @ref memoryMap properties assigned already, either explicitly or as the result
     *  of earlier steps. */
    virtual Partitioner createGenericPartitioner();

    /** Create a tuned partitioner.
     *
     *  Returns a partitioner that is tuned to operate on a specific instruction set architecture. The engine must have @ref
     *  disassembler and @ref memoryMap properties assigned already, either explicitly or as the result of earlier steps. */
    virtual Partitioner createTunedPartitioner();

    /** Create a partitioner from an AST.
     *
     *  Partitioner data structures are often more useful and more efficient for analysis than an AST. This method initializes
     *  the engine and a new partitioner with information from the AST. */
    virtual Partitioner createPartitionerFromAst(SgAsmInterpretation*);

    /** Create partitioner.
     *
     *  This is the method usually called to create a new partitioner.  The base class just calls @ref
     *  createTunedPartitioner. */
    virtual Partitioner createPartitioner();

    /** Finds interesting things to work on initially.
     *
     *  Seeds the partitioner with addresses and functions where recursive disassembly should begin. */
    virtual void runPartitionerInit(Partitioner&);

    /** Runs the recursive part of partioning.
     *
     *  This is the long-running guts of the partitioner. */
    virtual void runPartitionerRecursive(Partitioner&);

    /** Runs the final parts of partitioning.
     *
     *  This does anything necessary after the main part of partitioning is finished. For instance, it might give names to some
     *  functions that don't have names yet. */
    virtual void runPartitionerFinal(Partitioner&);
    
    /** Partitions instructions into basic blocks and functions.
     *
     *  This method is a wrapper around a number of lower-level partitioning steps that uses the specified interpretation to
     *  instantiate functions and then uses the specified partitioner to discover basic blocks and use the CFG to assign basic
     *  blocks to functions.  It is often overridden by subclasses. */
    virtual void runPartitioner(Partitioner&);


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Partitioner mid-level functions
    //
    // These are the functions called by the partitioner high-level stuff.  These are sometimes overridden in subclasses,
    // although it is more likely that the high-level stuff is overridden.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Label addresses.
     *
     *  Labels addresses according to symbols, etc.  Address labels are used for things like giving an unnamed function a name
     *  when it's attached to the partitioner's CFG/AUM. */
    virtual void labelAddresses(Partitioner&);

    /** Make data blocks based on configuration.
     *
     *  NOTE: for now, all this does is label the datablock addresses. FIXME[Robb P. Matzke 2015-05-12] */
    virtual std::vector<DataBlock::Ptr> makeConfiguredDataBlocks(Partitioner&, const Configuration&);

    /** Make functions based on configuration information.
     *
     *  Uses the supplied function configuration information to make functions. */
    virtual std::vector<Function::Ptr> makeConfiguredFunctions(Partitioner&, const Configuration&);

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

    /** Make functions based on specimen container.
     *
     *  Traverses the specified interpretation parsed from, for example, related ELF or PE containers, and make functions at
     *  certain addresses that correspond to specimen entry points, imports and exports, symbol tables, etc.  This method only
     *  calls many of the other "make*Functions" methods and accumulates their results.
     *
     *  Returns a list of such functions, some of which may have existed prior to this call. */
    virtual std::vector<Function::Ptr> makeContainerFunctions(Partitioner&, SgAsmInterpretation*);

    /** Make functions from an interrupt vector.
     *
     *  Reads the interrupt vector and builds functions for its entries.  The functions are inserted into the partitioner's
     *  CFG/AUM.
     *
     *  Returns the list of such functions, some of which may have existed prior to this call. */
    virtual std::vector<Function::Ptr> makeInterruptVectorFunctions(Partitioner&, const AddressInterval &vector);

    /** Make a function at each specified address.
     *
     *  A function is created at each address and is attached to the partitioner's CFG/AUM. Returns a list of such functions,
     *  some of which may have existed prior to this call. */
    virtual std::vector<Function::Ptr> makeUserFunctions(Partitioner&, const std::vector<rose_addr_t>&);

    /** Discover as many basic blocks as possible.
     *
     *  Processes the "undiscovered" work list until the list becomes empty.  This list is the list of basic block placeholders
     *  for which no attempt has been made to discover instructions.  This method implements a recursive descent disassembler,
     *  although it does not process the control flow edges in any particular order. Subclasses are expected to override this
     *  to implement a more directed approach to discovering basic blocks. */
    virtual void discoverBasicBlocks(Partitioner&);

    /** Scan read-only data to find addresses.
     *
     *  Scans read-only data beginning at the specified address in order to find pointers to code, and makes a new function at
     *  when found.  The pointer must be word aligned and located in memory that's mapped read-only (not writable and not
     *  executable), and it must not point to an unknown instruction or an instruction that overlaps with any instruction
     *  that's already in the CFG/AUM.
     *
     *  Returns a pointer to a newly-allocated function that has not yet been attached to the CFG/AUM, or a null pointer if no
     *  function was found.  In any case, the startVa is updated so it points to the next read-only address to check. */
    virtual Function::Ptr makeNextDataReferencedFunction(const Partitioner&, rose_addr_t &startVa /*in,out*/);

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
     *  byte patterns and/or instruction patterns that indicate the start of a function.  When a pattern is found a function
     *  (or multiple functions, depending on the type of matcher) is created and inserted into the specified partitioner's
     *  CFG/AUM.
     *
     *  Patterns are found by calling the @ref Partitioner::nextFunctionPrologue method, which most likely invokes a variety of
     *  predefined and user-defined callbacks to search for the next pattern.
     *
     *  Returns a vector of non-null function pointers pointer for the newly inserted functions, otherwise returns an empty
     *  vector. */
    virtual std::vector<Function::Ptr> makeNextPrologueFunction(Partitioner&, rose_addr_t startVa);

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

    /** Attach  all possible intra-function basic blocks to functions.
     *
     *  This is similar to @ref attachSurroundedCodeToFunctions except it calls that method repeatedly until it cannot do
     *  anything more.  Between each call it also follows the CFG for the newly discovered blocks to discover as many blocks as
     *  possible, creates more functions by looking for function calls, and attaches additional basic blocks to functions by
     *  following the CFG for each function.
     *
     *  This method is called automatically by @ref runPartitioner if the @ref intraFunctionCodeSearch property is set.
     *
     *  Returns the sum from all the calls to @ref attachSurroundedCodeToFunctions. */
    virtual size_t attachAllSurroundedCodeToFunctions(Partitioner&);
    
    /** Attach intra-function basic blocks to functions.
     *
     *  This method scans the unused address intervals (those addresses that are not represented by the CFG/AUM). For each
     *  unused interval, if the interval is immediately surrounded by a single function then a basic block placeholder is
     *  created at the beginning of the interval and added to the function.
     *
     *  Returns the number of new placeholders created. */
    virtual size_t attachSurroundedCodeToFunctions(Partitioner&);

    /** Attach basic blocks to functions.
     *
     *  Calls @ref Partitioner::discoverFunctionBasicBlocks once for each known function the partitioner's CFG/AUM in a
     *  sophomoric attempt to assign existing basic blocks to functions.  Returns the list of functions that resulted in
     *  errors.  If @p reportProblems is set then emit messages to mlog[WARN] about problems with the CFG (that stream must
     *  also be enabled if you want to actually see the warnings). */
    virtual std::vector<Function::Ptr> attachBlocksToFunctions(Partitioner&, bool emitWarnings=false);

    /** Attach dead code to functions.
     *
     *  Calls @ref attachDeadCodeToFunction once for each function that exists in the specified partitioner's CFG/AUM, passing
     *  along @p maxIterations each time.
     *
     *  Returns the union of the dead code addresses discovered for each function. */
    virtual std::set<rose_addr_t> attachDeadCodeToFunctions(Partitioner&, size_t maxIterations=size_t(-1));

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

    /** Runs various analysis passes.
     *
     *  Runs each analysis over all functions to ensure that results are cached.  This should typically be done after functions
     *  are discovered and before the final AST is generated, otherwise the AST will not contain cached results for functions
     *  and blocks for which an analysis was not performed. */
    virtual void updateAnalysisResults(Partitioner&);


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
    virtual bool makeNextCallReturnEdge(Partitioner&, boost::logic::tribool assumeCallReturns);

    /** Discover basic block at next placeholder.
     *
     *  Discovers a basic block at some arbitrary placeholder.  Returns a pointer to the new basic block if a block was
     *  discovered, or null if no block is discovered.  A postcondition for a null return is that the CFG has no edges coming
     *  into the "undiscovered" vertex. */
    virtual BasicBlock::Ptr makeNextBasicBlockFromPlaceholder(Partitioner&);

    /** Discover a basic block.
     *
     *  Discovers another basic block if possible.  A variety of methods will be used to determine where to discover the next
     *  basic block:
     *
     *  @li Discover a block at a placeholder by calling @ref makeNextBasicBlockAtPlaceholder
     *
     *  @li Insert a new call-return (@ref E_CALL_RETURN) edge for a function call that may return.  Insertion of such an
     *      edge may result in a new placeholder for which this method then discovers a basic block.  The call-return insertion
     *      happens in two passes: the first pass only adds an edge for a callee whose may-return analysis is positive; the
     *      second pass relaxes that requirement and inserts an edge for any callee whose may-return is indeterminate (i.e., if
     *      ROSE can't prove that a callee never returns then assume it may return).
     *
     *  Returns the basic block that was discovered, or the null pointer if there are no pending undiscovered blocks. */
    virtual BasicBlock::Ptr makeNextBasicBlock(Partitioner&);


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Build AST
    //
    // top-level: buildAst
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    // no helpers necessary since this is implemented in the Modules


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Settings and properties
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Property: interpretation
     *
     *  The interpretation which is being analyzed. The interpretation is chosen when an ELF or PE container is parsed, and the
     *  user can set it to something else if desired. For instance, parsing a PE file will set the interpretation to PE, but
     *  the user can reset it to DOS to disassemble the DOS part of the executable.
     *
     * @{ */
    SgAsmInterpretation* interpretation() const /*final*/ { return interp_; }
    virtual void interpretation(SgAsmInterpretation *interp) { interp_ = interp; }
    /** @} */

    /** Property: binary loader.
     *
     *  The binary loader that maps a binary container's sections into simulated memory and optionally performs dynamic linking
     *  and relocation fixups.  If none is specified then the engine will choose one based on the container.
     *
     * @{ */
    BinaryLoader* binaryLoader() const /*final*/ { return binaryLoader_; }
    virtual void binaryLoader(BinaryLoader *loader) { binaryLoader_ = loader; }
    /** @} */

    /** Property: when to remove execute permission from zero bytes.
     *
     *  This is the number of consecutive zero bytes that must be present before execute permission is removed from this part
     *  of the memory map.  A value of zero disables this feature.
     *
     * @{ */
    size_t deExecuteZeros() const /*final*/ { return settings_.loader.deExecuteZeros; }
    virtual void deExecuteZeros(size_t n) { settings_.loader.deExecuteZeros = n; }
    /** @} */

    /** Property: Disassembler.
     *
     *  This property holds the disassembler to use whenever a new partitioner is created. If null, then the engine will choose
     *  a disassembler based on the binary container.
     *
     * @{ */
    Disassembler *disassembler() const /*final*/ { return disassembler_; }
    virtual void disassembler(Disassembler *d) { disassembler_ = d; }
    /** @} */

    /** Property: Instruction set architecture name.
     *
     *  The instruction set architecture name is used to obtain a disassembler and overrides the disassembler that would
     *  otherwise be found by examining the binary container.
     *
     * @{ */
    const std::string& isaName() const /*final*/ { return settings_.disassembler.isaName; }
    virtual void isaName(const std::string &s) { settings_.disassembler.isaName = s; }
    /** @} */

    /** Property: Starting addresses for disassembly.
     *
     *  This is a list of addresses where functions will be created in addition to those functions discovered by examining the
     *  binary container.
     *
     * @{ */
    const std::vector<rose_addr_t>& startingVas() const /*final*/ { return settings_.partitioner.startingVas; }
    std::vector<rose_addr_t>& startingVas() /*final*/ { return settings_.partitioner.startingVas; }
    /** @} */

    /** Property: Whether to use instruction semantics.
     *
     *  If set, then instruction semantics are used to fine tune certain analyses that happen during partitioning, such as
     *  determining whether branch conditions are opaque.
     *
     * @{ */
    bool usingSemantics() const /*final*/ { return settings_.partitioner.usingSemantics; }
    virtual void usingSemantics(bool b) { settings_.partitioner.usingSemantics = b; }
    /** @} */

    /**  Property: Whether to follow ghost edges.
     *
     *   If set, then "ghost" edges are followed during disassembly.  A ghost edge is a control flow edge from a branch
     *   instruction where the partitioner has decided according to instruction semantics that the branch cannot be taken at
     *   run time.  If semantics are disabled then ghost edges are always followed since its not possible to determine whether
     *   an edge is a ghost edge.
     *
     * @{ */
    bool followingGhostEdges() const /*final*/ { return settings_.partitioner.followingGhostEdges; }
    virtual void followingGhostEdges(bool b) { settings_.partitioner.followingGhostEdges = b; }
    /** @} */

    /** Property: Whether to allow discontiguous basic blocks.
     *
     *  ROSE's definition of a basic block allows two consecutive instructions, A and B, to be arranged in memory such that B
     *  does not immediately follow A.  Setting this property prevents this and would force A and B to belong to separate basic blocks.
     *
     * @{ */
    bool discontiguousBlocks() const /*final*/ { return settings_.partitioner.discontiguousBlocks; }
    virtual void discontiguousBlocks(bool b) { settings_.partitioner.discontiguousBlocks = b; }
    /** @} */

    /** Property: Whether to find function padding.
     *
     *  If set, then the partitioner will look for certain padding bytes appearing before the lowest address of a function and
     *  add those bytes to the function as static data.
     *
     * @{ */
    bool findingFunctionPadding() const /*final*/ { return settings_.partitioner.findingFunctionPadding; }
    virtual void findingFunctionPadding(bool b) { settings_.partitioner.findingFunctionPadding = b; }
    /** @} */

    /** Property: Whether to find dead code.
     *
     *  If set, then the partitioner looks for code that is reachable by ghost edges after all other code has been found.  This
     *  is different than @ref followingGhostEdges in that the former follows those edges immediately.
     *
     * @{ */
    bool findingDeadCode() const /*final*/ { return settings_.partitioner.findingDeadCode; }
    virtual void findingDeadCode(bool b) { settings_.partitioner.findingDeadCode = b; }
    /** @} */

    /** Property: PE-Scrambler dispatcher address.
     *
     *  If non-zero then the partitioner defeats PE-scrambled binary obfuscation by replacing control flow edges that go
     *  through this function with the de-scrambled control flow edge.
     *
     * @{ */
    rose_addr_t peScramblerDispatcherVa() const /*final*/ { return settings_.partitioner.peScramblerDispatcherVa; }
    virtual void peScramblerDispatcherVa(rose_addr_t va) { settings_.partitioner.peScramblerDispatcherVa = va; }
    /** @} */

    /** Property: Whether to find intra-function code.
     *
     *  If set, the partitioner will look for parts of memory that were not disassembled and occur between other parts of the
     *  same function, and will attempt to disassemble that missing part and link it into the surrounding function.
     *
     * @{ */
    bool findingIntraFunctionCode() const /*final*/ { return settings_.partitioner.findingIntraFunctionCode; }
    virtual void findingIntraFunctionCode(bool b) { settings_.partitioner.findingIntraFunctionCode = b; }
    /** @} */

    /** Property: Whether to find intra-function data.
     *
     *  If set, the partitioner will look for parts of memory that were not disassembled and occur between other parts of the
     *  same function, and will treat the missing part as static data belonging to that function.
     *
     * @{ */
    bool findingIntraFunctionData() const /*final*/ { return settings_.partitioner.findingIntraFunctionData; }
    virtual void findingIntraFunctionData(bool b) { settings_.partitioner.findingIntraFunctionData = b; }
    /** @} */

    /** Property: Location of machine interrupt vector.
     *
     *  If non-empty, the partitioner will treat the specified area as a machine interrupt vector. The effect of the vector
     *  varies by architecture.
     *
     * @{ */
    const AddressInterval& interruptVector() const /*final*/ { return settings_.partitioner.interruptVector; }
    virtual void interruptVector(const AddressInterval &i) { settings_.partitioner.interruptVector = i; }
    /** @} */

    /** Property: Whether to perform post-partitioning analysis steps.
     *
     *  If set, then various post-partitioning analysis steps are executed.  Some of these can be quite expensive.
     *
     * @{ */
    bool doingPostAnalysis() const /*final*/ { return settings_.partitioner.doingPostAnalysis; }
    virtual void doingPostAnalysis(bool b) { settings_.partitioner.doingPostAnalysis = b; }
    /** @} */

    /** Property: Whether to assume that functions return.
     *
     *  If the partitioner's may-return analysis fails to reach a conclusion about a function, then either assume that the
     *  function may return (the usual case) or that it doesn't.  If a function cannot return (e.g., GCC @c noreturn attribute,
     *  such as for the @c abort function in the C library) then the partitioner will not attempt to discover instructions
     *  after its call sites.
     *
     * @{ */
    bool functionReturnsAssumed() const /*final*/ { return settings_.partitioner.functionReturnsAssumed; }
    virtual void functionReturnsAssumed(bool b) { settings_.partitioner.functionReturnsAssumed = b; }
    /** @} */

    /** Property: Whether to search static data for function pointers.
     *
     *  If this property is set, then the partitioner will scan static data to look for things that might be pointers to
     *  functions.
     *
     * @{ */
    bool findingDataFunctionPointers() const /*final*/ { return settings_.partitioner.findingDataFunctionPointers; }
    virtual void findingDataFunctionPointers(bool b) { settings_.partitioner.findingDataFunctionPointers = b; }
    /** @} */

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Internal stuff
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
    void init();
};

} // namespace
} // namespace
} // namespace

#endif
