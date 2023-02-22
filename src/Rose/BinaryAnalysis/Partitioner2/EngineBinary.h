#ifndef ROSE_BinaryAnalysis_Partitioner2_EngineBinary_H
#define ROSE_BinaryAnalysis_Partitioner2_EngineBinary_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#ifdef NOT_MOVED_TO_ENGINE
#include <Rose/BasicTypes.h>
#endif

#include <Rose/BinaryAnalysis/Partitioner2/Engine.h>
#include <Rose/BinaryAnalysis/Partitioner2/Exception.h>
#include <Rose/BinaryAnalysis/Partitioner2/ModulesLinux.h>
#include <Rose/BinaryAnalysis/Partitioner2/Utility.h>
#include <Rose/BinaryAnalysis/SerialIo.h>

#include <boost/noncopyable.hpp>
#include <boost/regex.hpp>
#ifdef NOT_MOVED_TO_ENGINE
#include <Sawyer/DistinctList.h>
#endif
#include <stdexcept>

#ifdef ROSE_ENABLE_PYTHON_API
#undef slots                                            // stupid Qt pollution
#include <boost/python.hpp>
#endif

namespace Rose {
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
 *  objects such as @ref Disassembler and @ref Partitioner.  In fact, this particular engine base class is designed so that
 *  users can pick and choose to use only those steps they need, or perhaps to call the main actions one step at a time with
 *  the user making adjustments between steps.
 *
 *  @section extensibility Customization
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
 *      steps.  This is actually the most common customization within the tools distributed with ROSE.
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
 *       SgAsmBlock *gblock = P2::Engine().frontend(argc, argv, purpose, description);
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
class EngineBinary: public Engine {
public:
    /** Shared ownership pointer. */
    using EngineBinaryPtr = Sawyer::SharedPointer<EngineBinary>;

#ifdef NOT_MOVED_TO_ENGINE
    /** Settings for the engine.
     *
     *  The engine is configured by adjusting these settings, usually shortly after the engine is created. */
    struct Settings {
        LoaderSettings loader;                          /**< Settings used during specimen loading. */
        DisassemblerSettings disassembler;              /**< Settings for creating the disassembler. */
        PartitionerSettings partitioner;                /**< Settings for creating a partitioner. */
        EngineSettings engine;                          /**< Settings that control engine behavior. */
        AstConstructionSettings astConstruction;        /**< Settings for constructing the AST. */

    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, unsigned version) {
            s & loader & disassembler & partitioner & engine & astConstruction;
        }
    };
#endif

    /** Errors from the engine. */
    class Exception: public Partitioner2::Exception {
    public:
        Exception(const std::string &mesg)
            : Partitioner2::Exception(mesg) {}
        ~Exception() throw () {}
    };

#ifdef NOT_MOVED_TO_ENGINE
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Internal data structures
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
    // Engine callback for handling instructions added to basic blocks.  This is called when a basic block is discovered,
    // before it's attached to a partitioner, so it shouldn't really be modifying any state in the engine, but rather only
    // preparing the basic block to be processed.
    class BasicBlockFinalizer: public BasicBlockCallback {
        typedef Sawyer::Container::Map<rose_addr_t /*target*/, std::vector<rose_addr_t> /*sources*/> WorkList;
    public:
        static Ptr instance() { return Ptr(new BasicBlockFinalizer); }
        virtual bool operator()(bool chain, const Args &args) override;
    private:
        void fixFunctionReturnEdge(const Args&);
        void fixFunctionCallEdges(const Args&);
        void addPossibleIndeterminateEdge(const Args&);
    };

    // Basic blocks that need to be worked on next. These lists are adjusted whenever a new basic block (or placeholder) is
    // inserted or erased from the CFG.
    class BasicBlockWorkList: public CfgAdjustmentCallback {
        // The following lists are used for adding outgoing E_CALL_RETURN edges to basic blocks based on whether the basic
        // block is a call to a function that might return.  When a new basic block is inserted into the CFG (or a previous
        // block is removed, modified, and re-inserted), the operator() is called and conditionally inserts the block into the
        // "pendingCallReturn" list (if the block is a function call that lacks an E_CALL_RETURN edge and the function is known
        // to return or the analysis was incomplete).
        //
        // When we run out of other ways to create basic blocks, we process the pendingCallReturn list from back to front. If
        // the back block (which gets popped) has a positive may-return result then an E_CALL_RETURN edge is added to the CFG
        // and the normal recursive BB discovery is resumed. Otherwise if the analysis is incomplete the basic block is moved
        // to the processedCallReturn list.  The entire pendingCallReturn list is processed before proceeding.
        //
        // If there is no more pendingCallReturn work to be done, then the processedCallReturn blocks are moved to the
        // finalCallReturn list and finalCallReturn is sorted by approximate CFG height (i.e., leafs first). The contents
        // of the finalCallReturn list is then analyzed and the result (or the default may-return value for failed analyses)
        // is used to decide whether a new CFG edge should be created, possibly adding new basic block addresses to the
        // list of undiscovered blocks.
        //
        Sawyer::Container::DistinctList<rose_addr_t> pendingCallReturn_;   // blocks that might need an E_CALL_RETURN edge
        Sawyer::Container::DistinctList<rose_addr_t> processedCallReturn_; // call sites whose may-return was indeterminate
        Sawyer::Container::DistinctList<rose_addr_t> finalCallReturn_;     // indeterminate call sites awaiting final analysis

        Sawyer::Container::DistinctList<rose_addr_t> undiscovered_;        // undiscovered basic block list (last-in-first-out)
        Engine *engine_;                                                   // engine to which this callback belongs
        size_t maxSorts_;                                                  // max sorts before using unsorted lists
    protected:
        BasicBlockWorkList(Engine *engine, size_t maxSorts): engine_(engine), maxSorts_(maxSorts) {}
    public:
        typedef Sawyer::SharedPointer<BasicBlockWorkList> Ptr;
        static Ptr instance(Engine *engine, size_t maxSorts) { return Ptr(new BasicBlockWorkList(engine, maxSorts)); }
        virtual bool operator()(bool chain, const AttachedBasicBlock &args) override;
        virtual bool operator()(bool chain, const DetachedBasicBlock &args) override;
        Sawyer::Container::DistinctList<rose_addr_t>& pendingCallReturn() { return pendingCallReturn_; }
        Sawyer::Container::DistinctList<rose_addr_t>& processedCallReturn() { return processedCallReturn_; }
        Sawyer::Container::DistinctList<rose_addr_t>& finalCallReturn() { return finalCallReturn_; }
        Sawyer::Container::DistinctList<rose_addr_t>& undiscovered() { return undiscovered_; }
        void moveAndSortCallReturn(const PartitionerConstPtr&);
    };

    // A work list providing constants from instructions that are part of the CFG.
    class CodeConstants: public CfgAdjustmentCallback {
    public:
        typedef Sawyer::SharedPointer<CodeConstants> Ptr;

    private:
        std::set<rose_addr_t> toBeExamined_;            // instructions waiting to be examined
        std::set<rose_addr_t> wasExamined_;             // instructions we've already examined
        rose_addr_t inProgress_;                        // instruction that is currently in progress
        std::vector<rose_addr_t> constants_;            // constants for the instruction in progress

    protected:
        CodeConstants(): inProgress_(0) {}

    public:
        static Ptr instance() { return Ptr(new CodeConstants); }

        // Possibly insert more instructions into the work list when a basic block is added to the CFG
        virtual bool operator()(bool chain, const AttachedBasicBlock &attached) override;

        // Possibly remove instructions from the worklist when a basic block is removed from the CFG
        virtual bool operator()(bool chain, const DetachedBasicBlock &detached) override;

        // Return the next available constant if any.
        Sawyer::Optional<rose_addr_t> nextConstant(const PartitionerConstPtr &partitioner);

        // Address of instruction being examined.
        rose_addr_t inProgress() const { return inProgress_; }
    };
#endif
    
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

#ifdef NOT_DEPRECATED
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
     *  If an <code>std::runtime_exception</code> occurs and the @ref exitOnError property is set, then the exception is caught,
     *  its text is emitted to the partitioner's fatal error stream, and <code>exit(1)</code> is invoked.
     *
     * @{ */
    Sawyer::CommandLine::ParserResult parseCommandLine(int argc, char *argv[],
                                                       const std::string &purpose, const std::string &description) /*final*/;
    virtual Sawyer::CommandLine::ParserResult parseCommandLine(const std::vector<std::string> &args,
                                                               const std::string &purpose, const std::string &description);
    /** @} */
#endif

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
    virtual SgAsmBlock* buildAst(const std::vector<std::string> &fileNames = std::vector<std::string>()) override;
    /** @} */

#ifdef NOT_DEPRECATED
    /** Save a partitioner and AST to a file.
     *
     *  The specified partitioner and the binary analysis components of the AST are saved into the specified file, which is
     *  created if it doesn't exist and truncated if it does exist. The name should end with a ".rba" extension. The file can
     *  be loaded by passing its name to the @ref partition function or by calling @ref loadPartitioner. */
    virtual void savePartitioner(const PartitionerConstPtr&, const boost::filesystem::path&, SerialIo::Format = SerialIo::BINARY);

    /** Load a partitioner and an AST from a file.
     *
     *  The specified RBA file is opened and read to create a new @ref Partitioner object and associated AST. The @ref
     *  partition function also understands how to open RBA files. */
    virtual PartitionerPtr loadPartitioner(const boost::filesystem::path&, SerialIo::Format = SerialIo::BINARY);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Command-line parsing
    //
    // top-level: parseCommandLine
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Command-line switches related to the loader.
     *
     * @{ */
    virtual Sawyer::CommandLine::SwitchGroup loaderSwitches();
    static Sawyer::CommandLine::SwitchGroup loaderSwitches(LoaderSettings&);
    /** @} */

    /** Command-line switches related to the disassembler.
     *
     * @{ */
    virtual Sawyer::CommandLine::SwitchGroup disassemblerSwitches();
    static Sawyer::CommandLine::SwitchGroup disassemblerSwitches(DisassemblerSettings&);
    /** @} */

    /** Command-line switches related to the partitioner.
     *
     * @{ */
    virtual Sawyer::CommandLine::SwitchGroup partitionerSwitches();
    static Sawyer::CommandLine::SwitchGroup partitionerSwitches(PartitionerSettings&);
    /** @} */

    /** Command-line switches related to engine behavior.
     *
     * @{ */
    virtual Sawyer::CommandLine::SwitchGroup engineSwitches();
    static Sawyer::CommandLine::SwitchGroup engineSwitches(EngineSettings&);
    /** @} */

    /** Command-line switches related to AST construction.
     *
     * @{ */
    virtual Sawyer::CommandLine::SwitchGroup astConstructionSwitches();
    static Sawyer::CommandLine::SwitchGroup astConstructionSwitches(AstConstructionSettings&);
    /** @} */

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
#endif

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

#ifdef NOT_DEPRECATED
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
     * @{ */
    MemoryMapPtr memoryMap() const /*final*/;
    virtual void memoryMap(const MemoryMapPtr&);
    /** @} */
#endif


#ifdef NOT_DEPRECATED
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
     *  @li If the engine @ref doDisassemble property is false, return no disassembler (nullptr).
     *
     *  @li Fail by throwing an <code>std::runtime_error</code>.
     *
     *  In any case, the @ref disassembler property is set to this method's return value. */
    virtual Disassembler::BasePtr obtainDisassembler();
    virtual Disassembler::BasePtr obtainDisassembler(const Disassembler::BasePtr &hint);
    /** @} */
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Partitioner high-level functions
    //
    // top-level: partition
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
#ifdef NOT_MOVED_TO_ENGINE
    /** Check that we have everything necessary to create a partitioner. */
    virtual void checkCreatePartitionerPrerequisites() const;
#endif

#ifdef NOT_MOVED_TO_ENGINE
    /** Create a bare partitioner.
     *
     *  A bare partitioner, as far as the engine is concerned, is one that has characteristics that are common across all
     *  architectures but which is missing all architecture-specific functionality.  Using the partitioner's own constructor is
     *  not quite the same--that would produce an even more bare partitioner!  The engine must have @ref disassembler (if @ref
     *  doDisassemble is set) and @ref memoryMap properties already either assigned explicitly or as the result of earlier
     *  steps. */
    virtual PartitionerPtr createBarePartitioner();
#endif

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

#ifdef NOT_MOVED_TO_ENGINE
    /** Partitions instructions into basic blocks and functions.
     *
     *  This method is a wrapper around a number of lower-level partitioning steps that uses the specified interpretation to
     *  instantiate functions and then uses the specified partitioner to discover basic blocks and use the CFG to assign basic
     *  blocks to functions.  It is often overridden by subclasses. */
    virtual void runPartitioner(const PartitionerPtr&);
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Partitioner mid-level functions
    //
    // These are the functions called by the partitioner high-level stuff.  These are sometimes overridden in subclasses,
    // although it is more likely that the high-level stuff is overridden.
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
#ifdef NOT_MOVED_TO_ENGINE
    /** Label addresses.
     *
     *  Labels addresses according to symbols, etc.  Address labels are used for things like giving an unnamed function a name
     *  when it's attached to the partitioner's CFG/AUM. */
    virtual void labelAddresses(const PartitionerPtr&, const Configuration&);

    /** Make data blocks based on configuration.
     *
     *  NOTE: for now, all this does is label the datablock addresses. FIXME[Robb P. Matzke 2015-05-12] */
    virtual std::vector<DataBlockPtr> makeConfiguredDataBlocks(const PartitionerPtr&, const Configuration&);

    /** Make functions based on configuration information.
     *
     *  Uses the supplied function configuration information to make functions. */
    virtual std::vector<FunctionPtr> makeConfiguredFunctions(const PartitionerPtr&, const Configuration&);
#endif

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

#ifdef NOT_MOVED_TO_ENGINE
    /** Runs various analysis passes.
     *
     *  Runs each analysis over all functions to ensure that results are cached.  This should typically be done after functions
     *  are discovered and before the final AST is generated, otherwise the AST will not contain cached results for functions
     *  and blocks for which an analysis was not performed. */
    virtual void updateAnalysisResults(const PartitionerPtr&);
#endif

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


#ifdef NOT_MOVED_TO_ENGINE
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Build AST
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    // Used internally by ROSE's ::frontend disassemble instructions to build the AST that goes under each SgAsmInterpretation.
    static void disassembleForRoseFrontend(SgAsmInterpretation*);
#endif


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Settings and properties
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
#ifdef NOT_MOVED_TO_ENGINE
    /** Property: All settings.
     *
     *  Returns a reference to the engine settings structures.  Alternatively, some settings also have a corresponding engine
     *  member function to query or adjust the setting directly.
     *
     * @{ */
    const Settings& settings() const /*final*/ { return settings_; }
    Settings& settings() /*final*/ { return settings_; }
    /** @} */

    /** Property: Disassembler.
     *
     *  This property holds the disassembler to use whenever a new partitioner is created. If null, then the engine will choose
     *  a disassembler based on the binary container (unless @ref doDisassemble property is clear).
     *
     * @{ */
    Disassembler::BasePtr disassembler() const;
    virtual void disassembler(const Disassembler::BasePtr&);
    /** @} */

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

    /** Property: progress reporting.
     *
     *  The optional object to receive progress reports.
     *
     * @{ */
    ProgressPtr progress() const /*final*/;
    virtual void progress(const ProgressPtr&);
    /** @} */
#endif

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
