#ifndef ROSE_BinaryAnalysis_Partitioner2_Engine_H
#define ROSE_BinaryAnalysis_Partitioner2_Engine_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BasicTypes.h>
#include <Rose/BinaryAnalysis/Partitioner2/Exception.h>
#include <Rose/BinaryAnalysis/Partitioner2/Modules.h>
#include <Rose/BinaryAnalysis/SerialIo.h>

#include <Sawyer/DistinctList.h>
#include <Sawyer/SharedObject.h>
#include <Sawyer/SharedPointer.h>

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
 *
 *       // Create an engine instance from the engine factory using an empty parser
 *       Sawyer::CommandLine::Parser parser = Rose::CommandLine::createEmptyParser(purpose, description);
 *       P2::Engine::EnginePtr engine = P2::Engine::forge(argc, argv, parser);
 *       // Create the AST
 *       SgAsmBlock *gblock = engine->frontend(argc, argv, purpose, description);
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
class Engine: public Sawyer::SharedObject {
public:
    /** Shared ownership pointer. */
    using EnginePtr = Sawyer::SharedPointer<Engine>;

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

    /** Errors from the engine. */
    class Exception: public Partitioner2::Exception {
    public:
        Exception(const std::string &mesg)
            : Partitioner2::Exception(mesg) {}
        ~Exception() throw () {}
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Internal data structures
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
protected:
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

private:
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

protected:
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
        rose_addr_t inProgress() {return inProgress_;}

        // Possibly insert more instructions into the work list when a basic block is added to the CFG
        virtual bool operator()(bool chain, const AttachedBasicBlock &attached) override;

        // Possibly remove instructions from the worklist when a basic block is removed from the CFG
        virtual bool operator()(bool chain, const DetachedBasicBlock &detached) override;

        // Return the next available constant if any.
        Sawyer::Optional<rose_addr_t> nextConstant(const PartitionerConstPtr &partitioner);

        // Address of instruction being examined.
        rose_addr_t inProgress() const { return inProgress_; }
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Data members
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
    std::string name_;                                  // factory name
    Settings settings_;                                 // Settings for the partitioner.
    SgAsmInterpretation *interp_;                       // interpretation set by loadSpecimen
    Disassembler::BasePtr disassembler_;                // not ref-counted yet, but don't destroy it since user owns it
    MemoryMapPtr map_;                                  // memory map initialized by load()
    BasicBlockWorkList::Ptr basicBlockWorkList_;        // what blocks to work on next
    CodeConstants::Ptr codeFunctionPointers_;           // generates constants that are found in instruction ASTs
    ProgressPtr progress_;                              // optional progress reporting
    std::vector<std::string> specimen_;                 // list of additional command line arguments (often file names)

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Constructors
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
protected:
    /** Default constructor.  Constructor is deleted and class noncopyable. */
    Engine() = delete;
    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;

protected:
    /** Allocating `instance` constructors are implemented by the non-abstract subclasses.
     *
     * @{ */
    explicit Engine(const std::string &name);      // for instantiating factories
    explicit Engine(const Settings &settings);
    /** @} */

    void init();

public:
    virtual ~Engine();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Factories
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Register an engine as a factory.
     *
     *  The specified engine is added to the end of a list of engine prototypical factory objects. When a new engine is
     *  needed, this list is scanned in reverse order until one of the @c matchFactory predicates for the prototypical object
     *  returns true, at which time a new copy of that object is created by passing the lookup arguments to its virtual @c
     *  instanceFromFactory constructor.
     *
     *  Thread safety: This method is thread safe. */
    static void registerFactory(const EnginePtr &factory);

    /** Remove a concrete engine factory from the registry.
     *
     *  The last occurrence of the specified factory is removed from the list of registered factories. This function returns
     *  true if a factory was removed, and false if no registered factories match.
     *
     *  Thread safety: This method is thread safe. */
    static bool deregisterFactory(const EnginePtr &factory);

    /** List of all registered factories.
     *
     *  The returned list contains the registered factories in the order they were registereed, which is the reverse order
     *  of how they're searched.
     *
     *  Thread safety: This method is thread safe. */
    static std::vector<EnginePtr> registeredFactories();

    /** Creates a suitable engine based on the specimen.
     *
     *  Scans the @ref registeredFactories list in the reverse order looking for a factory whose @ref matchFactory predicate
     *  (which accepts all but the first argument to this function) returns true. The first factory whose predicate returns
     *  true is used to create and return a new concrete engine object by invoking the factory's virtual @c
     *  instanceFromFactory constructor with the first argument of this function.
     *
     *  Thread safety: This method is thread safe. */
    static EnginePtr forge();
    static EnginePtr forge(const std::vector<std::string> &specimen);
    static EnginePtr forge(int argc, char *argv[], Sawyer::CommandLine::Parser&);

    /** Predicate for matching a concrete engine factory by settings and specimen. */
    virtual bool matchFactory(const std::vector<std::string> &specimen) const = 0;

    /** Virtual constructor for factories.
     *
     *  This creates a new object by calling the class method @c instance for the class of which @c this is a type. All
     *  arguments are passed to @c instance. */
    virtual EnginePtr instanceFromFactory() = 0;

    /** Returns true if this object is a factory.
     *
     *  Factories are created by the @c factory class methods rather than the usual @c instance class methods. A factory
     *  object should only be used to create other (non-factory) objects by registering it as a factory and eventually
     *  calling (directly or indirectly) its @ref instanceFromFactory object method. */
    bool isFactory() const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Top-level, do everything functions
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
    SgAsmBlock* frontend(int argc, char *argv[],
                         const std::string &purpose, const std::string &description);
    virtual SgAsmBlock* frontend(const std::vector<std::string> &args,
                                 const std::string &purpose, const std::string &description) = 0;
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
    virtual void reset();

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
    virtual SgAsmBlock* buildAst(const std::vector<std::string> &fileNames = std::vector<std::string>()) = 0;
    /** @} */

    /** Save a partitioner and AST to a file.
     *
     *  The specified partitioner and the binary analysis components of the AST are saved into the specified file, which is
     *  created if it doesn't exist and truncated if it does exist. The name should end with a ".rba" extension. The file can
     *  be loaded by passing its name to the @ref partition function or by calling @ref loadPartitioner. */
    virtual void savePartitioner(const PartitionerConstPtr&, const boost::filesystem::path&, SerialIo::Format = SerialIo::BINARY);

//TODO: use Partitioner::instanceFromRbaFile() instead if not an engine?
protected:
    /** Load a partitioner and an AST from a file.
     *
     *  The specified RBA file is opened and read to create a new @ref Partitioner object and associated AST. The @ref
     *  partition function also understands how to open RBA files. */
    // instanceFromRbaFile() rename
    virtual PartitionerPtr loadPartitioner(const boost::filesystem::path&, SerialIo::Format = SerialIo::BINARY);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Command-line parsing
    //
    // top-level: parseCommandLine
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Parse specimen binary containers.
     *
     *  Parses the ELF and PE binary containers to create an abstract syntax tree (AST).  If @p fileNames contains names that
     *  are recognized as raw data or other non-containers then they are skipped over at this stage but processed during the
     *  @ref loadSpecimens stage.
     *
     *  This method tries to allocate a disassembler if none is set and an ISA name is specified in the settings, otherwise the
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
    virtual SgAsmInterpretation* parseContainers(const std::vector<std::string> &fileNames) = 0;
    SgAsmInterpretation* parseContainers(const std::string &fileName) /*final*/;
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
    virtual MemoryMapPtr loadSpecimens(const std::vector<std::string> &fileNames = std::vector<std::string>()) = 0;
    MemoryMapPtr loadSpecimens(const std::string &fileName) /*final*/;
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
    virtual PartitionerPtr partition(const std::vector<std::string> &fileNames = std::vector<std::string>()) = 0;
    PartitionerPtr partition(const std::string &fileName) /*final*/;
    /** @} */

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

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Container parsing
    //
    // top-level: parseContainers
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Determine whether a specimen is an RBA file.
     *
     *  Returns true if the name looks like a ROSE Binary Analysis file. Such files are not intended to be passed to ROSE's
     *  global @c ::frontend function but may be passed to this Engine's @ref Engine::frontend method. */
    virtual bool isRbaFile(const std::string&);

    /** Determine whether a specimen name is a non-container.
     *
     *  Certain strings are recognized as special instructions for how to adjust a memory map and are not intended to be passed
     *  to ROSE's @c frontend function.  This predicate returns true for such strings. */
    virtual bool isNonContainer(const std::string&) = 0;

    /** Returns true if containers are parsed.
     *
     *  Specifically, returns true if the engine has a non-null interpretation.  If it has a null interpretation then
     *  @ref parseContainers might have already been called but no binary containers specified, in which case calling it again
     *  with the same file names will have no effect. */
    virtual bool areContainersParsed() const = 0;

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
     *  architectures but which is missing all architecture-specific functionality.  Using the partitioner's own constructor is
     *  not quite the same--that would produce an even more bare partitioner!  The engine must have @ref disassembler (if @ref
     *  doDisassemble is set) and @ref memoryMap properties already either assigned explicitly or as the result of earlier
     *  steps. */
    virtual PartitionerPtr createBarePartitioner();

    /** Create partitioner.
     *
     *  This is the method usually called to create a new partitioner. */
    virtual PartitionerPtr createPartitioner() = 0;

    /** Finds interesting things to work on initially.
     *
     *  Seeds the partitioner with addresses and functions where recursive disassembly should begin. */
    virtual void runPartitionerInit(const PartitionerPtr&) = 0;

    /** Runs the recursive part of partioning.
     *
     *  This is the long-running guts of the partitioner. */
    virtual void runPartitionerRecursive(const PartitionerPtr&) = 0;

    /** Runs the final parts of partitioning.
     *
     *  This does anything necessary after the main part of partitioning is finished. For instance, it might give names to some
     *  functions that don't have names yet. */
    virtual void runPartitionerFinal(const PartitionerPtr&) = 0;

    /** Partitions instructions into basic blocks and functions.
     *
     *  This method is a wrapper around a number of lower-level partitioning steps that uses the specified interpretation to
     *  instantiate functions and then uses the specified partitioner to discover basic blocks and use the CFG to assign basic
     *  blocks to functions.  It is often overridden by subclasses. */
    virtual void runPartitioner(const PartitionerPtr&);

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
    virtual void labelAddresses(const PartitionerPtr&, const Configuration&);

    /** Make data blocks based on configuration.
     *
     *  NOTE: for now, all this does is label the datablock addresses. FIXME[Robb P. Matzke 2015-05-12] */
    virtual std::vector<DataBlockPtr> makeConfiguredDataBlocks(const PartitionerPtr&, const Configuration&);

    /** Make functions based on configuration information.
     *
     *  Uses the supplied function configuration information to make functions. */
    virtual std::vector<FunctionPtr> makeConfiguredFunctions(const PartitionerPtr&, const Configuration&);

    /** Runs various analysis passes.
     *
     *  Runs each analysis over all functions to ensure that results are cached.  This should typically be done after functions
     *  are discovered and before the final AST is generated, otherwise the AST will not contain cached results for functions
     *  and blocks for which an analysis was not performed. */
    virtual void updateAnalysisResults(const PartitionerPtr&);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Partitioner low-level functions
    //
    // These are functions that a subclass seldom overrides, and maybe even shouldn't override because of their complexity or
    // the way the interact with one another.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Build AST
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    // Used internally by ROSE's ::frontend disassemble instructions to build the AST that goes under each SgAsmInterpretation.
    static void disassembleForRoseFrontend(SgAsmInterpretation*);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Settings and properties
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Property: Name.
     *
     *  The name of the engine comes from the engine factory, will be empty if not a factory.
     *
     * @{ */
    const std::string& name() const /*final*/ {return name_;}
    void name(std::string n) {name_ = n;}
    /** @} */

    /** Property: All settings.
     *
     *  Returns a reference to the engine settings structures.  Alternatively, some settings also have a corresponding engine
     *  member function to query or adjust the setting directly.
     *
     * @{ */
    const Settings& settings() const /*final*/ { return settings_; }
    Settings& settings() /*final*/ { return settings_; }
    /** @} */

    /** Property: BasicBlock work list.
     *
     *  This property holds the list of what blocks to work on next.
     *
     * @{ */
    BasicBlockWorkList::Ptr basicBlockWorkList() const /*final*/ { return basicBlockWorkList_; }
    void basicBlockWorkList(const BasicBlockWorkList::Ptr &l) /*final*/ { basicBlockWorkList_ = l; }

    /** Property: Instruction AST constants.
     *
     *  This property holds constants that are found in instruction ASTs.
     *
     * @{ */
    CodeConstants::Ptr codeFunctionPointers() const /*final*/ { return codeFunctionPointers_; }
    void codeFunctionPointers(const CodeConstants::Ptr &c) /*final*/ { codeFunctionPointers_ = c; }
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

    /** Property: specimen.
     *
     *  The specimen is a list of additional command line arguments. It is often a list of file names.
     *
     * @{ */
    std::vector<std::string> specimen() const /*final*/ { return specimen_; }
    virtual void specimen(const std::vector<std::string> &specimen) {specimen_ = specimen; }
    /** @} */

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Internal stuff
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
protected:
    // Similar to ::frontend but a lot less complicated.
    virtual SgProject* roseFrontendReplacement(const std::vector<boost::filesystem::path> &fileNames) = 0;
};

} // namespace
} // namespace
} // namespace

#endif
#endif
