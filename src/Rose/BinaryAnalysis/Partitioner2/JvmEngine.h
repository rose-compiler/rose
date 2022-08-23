#ifndef ROSE_BinaryAnalysis_Partitioner2_JvmEngine_H
#define ROSE_BinaryAnalysis_Partitioner2_JvmEngine_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/Disassembler/Base.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <Rose/Progress.h>
#include <Rose/Exception.h>
#include <stdexcept>

namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

/** Class for the JVM engine driving the partitioner.
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
 *   #include <Rose/BinaryAnalysis/Partitioner2/JvmEngine.h>
 *   using namespace Rose;
 *   namespace P2 = Rose::BinaryAnalysis::Partitioner2;
 *
 *   int main(int argc, char *argv[]) {
 *       std::string purpose = "disassembles a binary specimen";
 *       std::string description =
 *           "This tool disassembles the specified specimen and presents the "
 *           "results as a pseudo assembly listing, that is, a listing intended "
 *           "for human consumption rather than assembly.";
 *       SgAsmBlock *gblock = P2::JvmEngine().frontend(argc, argv, purpose, description);
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
class ROSE_DLL_API JvmEngine: private boost::noncopyable {
public:
    /** Settings for the engine.
     *
     *  The engine is configured by adjusting these settings, usually shortly after the engine is created. */
    struct Settings {
        DisassemblerSettings disassembler;              /**< Settings for creating the disassembler. */
        PartitionerSettings partitioner;                /**< Settings for creating a partitioner. */
        EngineSettings engine;                          /**< Settings that control engine behavior. */
        AstConstructionSettings astConstruction;        /**< Settings for constructing the AST. */
    };

    /** Errors from the engine. */
    class Exception: public Rose::Exception {
    public:
        Exception(const std::string &mesg)
            : Rose::Exception(mesg) {}
        ~Exception() throw () {}
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Internal data structures
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Data members
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
    Settings settings_;                                 // Settings for the partitioner.
    Disassembler::Base *disassembler_;                  // not ref-counted yet, but don't destroy it since user owns it
    Progress::Ptr progress_;                            // optional progress reporting

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Constructors
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Default constructor. */
    JvmEngine()
        : disassembler_(nullptr),
        progress_(Progress::instance()) {
        init();
    }

    /** Construct engine with settings. */
    explicit JvmEngine(const Settings &settings)
        : settings_(settings), disassembler_(nullptr),
        progress_(Progress::instance()) {
        init();
    }

    virtual ~JvmEngine() {}

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
     *  the interpretation, disassembler, and memory map so all the top-level steps get executed again. This is
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
     *  If an <code>std::runtime_exception</code> occurs and the @ref exitOnError property is set, then the exception is caught,
     *  its text is emitted to the partitioner's fatal error stream, and <code>exit(1)</code> is invoked.
     *
     * @{ */
    Sawyer::CommandLine::ParserResult parseCommandLine(int argc, char *argv[],
                                                       const std::string &purpose, const std::string &description) /*final*/;
    virtual Sawyer::CommandLine::ParserResult parseCommandLine(const std::vector<std::string> &args,
                                                               const std::string &purpose, const std::string &description);
    /** @} */

    /** Parse specimen binary containers.
     *
     *  Parses the Java class files to create an abstract syntax tree (AST).  If @p fileNames contains names that
     *  are either Java jar or class files.
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
    SgAsmInterpretation* parseContainers(const std::string &fileName) /*final*/;
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
    virtual Partitioner partition(const std::vector<std::string> &fileNames = std::vector<std::string>());
    Partitioner partition(const std::string &fileName) /*final*/;
    /** @} */

    /** Obtain an abstract syntax tree.
     *
     *  Constructs a new abstract syntax tree (AST) from partitioner information with these steps:
     *
     *  @li If the partitioner has not been run yet (according to @ref isPartitioned), then do that now with the same
     *      arguments.  The zero-argument version invokes the zero-argument @ref partition, which requires that the specimen
     *      has already been loaded by @ref loadSpecimens.
     *
     *  @li Call Modules::buildAst to build the AST.
     *
     *  If an <code>std::runtime_exception</code> occurs and the @ref exitOnError property is set, then the exception is caught,
     *  its text is emitted to the partitioner's fatal error stream, and <code>exit(1)</code> is invoked.
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
    virtual Disassembler::Base* obtainDisassembler(Disassembler::Base *hint=NULL);
    /** @} */


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Partitioner high-level functions
    //
    // top-level: partition
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Create partitioner.
     *
     *  This is the method usually called to create a new partitioner. */
    virtual Partitioner createPartitioner();

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
    virtual void labelAddresses(Partitioner&, const Configuration&);

    /** Discover as many basic blocks as possible.
     *
     *  Processes the "undiscovered" work list until the list becomes empty.  This list is the list of basic block placeholders
     *  for which no attempt has been made to discover instructions.  This method implements a recursive descent disassembler,
     *  although it does not process the control flow edges in any particular order. Subclasses are expected to override this
     *  to implement a more directed approach to discovering basic blocks. */
    virtual void discoverBasicBlocks(Partitioner&);

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
    /** Property: All settings.
     *
     *  Returns a reference to the engine settings structures.  Alternatively, each setting also has a corresponding engine
     *  member function to query or adjust the setting directly.
     *
     * @{ */
    const Settings& settings() const /*final*/ { return settings_; }
    Settings& settings() /*final*/ { return settings_; }
    /** @} */

    /** Property: Error handling.
     *
     *  If an exception occurs during certain high-level functions and this property is set, then the exception is caught,
     *  its text is written to a fatal error stream, and exit is called with a non-zero value.  Since the error message is more
     *  user-friendly and professional looking than the uncaught exception message produced by the C++ runtime, the default is
     *  that exceptions are caught.  If a tool needs to perform its own error handling, then it should clear this property.
     *
     * @{ */
    bool exitOnError() const /*final*/ { return settings_.engine.exitOnError; }
    virtual void exitOnError(bool b) { settings_.engine.exitOnError = b; }
    /** @} */

    /** Property: progress reporting.
     *
     *  The optional object to receive progress reports.
     *
     * @{ */
    Progress::Ptr progress() const /*final*/ { return progress_; }
    virtual void progress(const Progress::Ptr &progress) { progress_ = progress; }
    /** @} */

    /** Property: Perform disassembly.
     *
     *  If true, then disassembly is performed, otherwise it's skipped.
     *
     * @{ */
    bool doDisassemble() const /*final*/ { return settings_.disassembler.doDisassemble; }
    virtual void doDisassemble(bool b) { settings_.disassembler.doDisassemble = b; }
    /** @} */

    /** Property: Disassembler.
     *
     *  This property holds the disassembler to use whenever a new partitioner is created. If null, then the engine will choose
     *  a disassembler based on the binary container (unless @ref doDisassemble property is clear).
     *
     * @{ */
    Disassembler::Base *disassembler() const /*final*/ { return disassembler_; }
    virtual void disassembler(Disassembler::Base *d) { disassembler_ = d; }
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
    const std::vector<rose_addr_t>& functionStartingVas() const /*final*/ { return settings_.partitioner.functionStartingVas; }
    std::vector<rose_addr_t>& functionStartingVas() /*final*/ { return settings_.partitioner.functionStartingVas; }
    /** @} */

    /** Property: Whether to use instruction semantics.
     *
     *  If set, then instruction semantics are used to fine tune certain analyses that happen during partitioning, such as
     *  determining whether branch conditions are opaque.
     *
     * @{ */
    bool usingSemantics() const /*final*/ { return settings_.partitioner.base.usingSemantics; }
    virtual void usingSemantics(bool b) { settings_.partitioner.base.usingSemantics = b; }
    /** @} */

    /** Property: Whether unknown instructions are ignored.
     *
     *  If set, then instructions that cannot be disassembled are treated like no-ops for the purpose of building the global
     *  control flow graph (otherwise they terminate a basic block). This is useful when working with fixed-width instruction
     *  set architectures for which ROSE has an incomplete disassembler. For instance, PowerPC architectures that are augmented
     *  with additional undocumented co-processor instructions.
     *
     * @{ */
    bool ignoringUnknownInsns() const /*final*/ { return settings_.partitioner.base.ignoringUnknownInsns; }
    virtual void ignoringUnknownInsns(bool b) { settings_.partitioner.base.ignoringUnknownInsns = b; }
    /** @} */

    /** Property: Type of container for semantic memory.
     *
     *  Determines whether @ref Partitioner objects created by this engine will be configured to use list-based or map-based
     *  semantic memory states.  The list-based states are more precise, but they're also slower.
     *
     * @{ */
    SemanticMemoryParadigm semanticMemoryParadigm() const /*final*/ { return settings_.partitioner.semanticMemoryParadigm; }
    virtual void semanticMemoryParadigm(SemanticMemoryParadigm p) { settings_.partitioner.semanticMemoryParadigm = p; }
    /** @} */

    /** Property: Maximum size for basic blocks.
     *
     *  This property is the maximum size for basic blocks measured in number of instructions. Any basic block that would
     *  contain more than this number of instructions is split into multiple basic blocks.  Having smaller basic blocks makes
     *  some intra-block analysis faster, but they have less information.  A value of zero indicates no limit.
     *
     * @{ */
    size_t maxBasicBlockSize() const /*final*/ { return settings_.partitioner.maxBasicBlockSize; }
    virtual void maxBasicBlockSize(size_t n) { settings_.partitioner.maxBasicBlockSize = n; }
    /** @} */

    /** Property: CFG edge rewrite pairs.
     *
     *  This property is a list of old/new instruction pointer pairs that describe how to rewrite edges of the global control
     *  flow graph. Whenever an instruction has a successor whose address is an old address, it will be replaced with a successor
     *  edge that points to the new address.  This list must have an even number of elements where element <code>2*i+0</code> is
     *  and old address and element <code>2*i+1</code> is the corresponding new address.
     *
     * @{ */
    const std::vector<rose_addr_t>& ipRewrites() const /*final*/ { return settings_.partitioner.ipRewrites; }
    virtual void ipRewrites(const std::vector<rose_addr_t> &v) { settings_.partitioner.ipRewrites = v; }
    /** @} */

    /** Property: Whether to find dead code.
     *
     *  If ghost edges are being discovered (see @ref usingSemantics and @ref followingGhostEdges) and are not being inserted
     *  into the global CFG, then the target address of the ghost edges might not be used as code addresses during the code
     *  discovery phase.  This property, when true, will cause the target address of ghost edges to be used to discover additional
     *  instructions even if they have no incoming CFG edges.
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
     *  If positive, the partitioner will look for parts of memory that were not disassembled and occur between other parts of
     *  the same function, and will attempt to disassemble that missing part and link it into the surrounding function. It will
     *  perform up to @p n passes across the entire address space.
     *
     * @{ */
    size_t findingIntraFunctionCode() const /*final*/ { return settings_.partitioner.findingIntraFunctionCode; }
    virtual void findingIntraFunctionCode(size_t n) { settings_.partitioner.findingIntraFunctionCode = n; }
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
     *  If set, then each of the enabled post-partitioning analysis steps are executed.  Some of these can be quite expensive,
     *  but they can be enabled and disabled individually. Those that are enabled are only run if this property also is set.
     *
     * @{ */
    bool doingPostAnalysis() const /*final*/ { return settings_.partitioner.doingPostAnalysis; }
    virtual void doingPostAnalysis(bool b) { settings_.partitioner.doingPostAnalysis = b; }
    /** @} */

    /** Property: Whether to run the function may-return analysis.
     *
     *  Determines whether the may-return analysis is run when @ref doingPostAnalysis is true.
     *
     * @{ */
    bool doingPostFunctionMayReturn() const /*final*/ { return settings_.partitioner.doingPostFunctionMayReturn; }
    virtual void doingPostFunctionMayReturn(bool b) { settings_.partitioner.doingPostFunctionMayReturn = b; }
    /** @} */

    /** Property: Whether to run the function stack delta analysis.
     *
     *  Determines whether the stack delta analysis is run when @ref doingPostAnalysis is true.
     *
     * @{ */
    bool doingPostFunctionStackDelta() const /*final*/ { return settings_.partitioner.doingPostFunctionStackDelta; }
    virtual void doingPostFunctionStackDelta(bool b) { settings_.partitioner.doingPostFunctionStackDelta = b; }
    /** @} */

    /** Property: Whether to run calling-convention analysis.
     *
     *  Determines whether calling convention analysis is run on each function when @ref doingPostAnalysis is true.
     *
     * @{ */
    bool doingPostCallingConvention() const /*final*/ { return settings_.partitioner.doingPostCallingConvention; }
    virtual void doingPostCallingConvention(bool b) { settings_.partitioner.doingPostCallingConvention = b; }
    /** @} */

    /** Property: Whether to run no-op function analysis.
     *
     *  Determines whether function no-op analysis is run on each function when @ref doingPostAnalysis is true. This analysis
     *  determines whether a function is effectively a no-op and gives it a name indicative of a no-op if it is one.
     *
     * @{ */
    bool doingPostFunctionNoop() const /*final*/ { return settings_.partitioner.doingPostFunctionNoop; }
    virtual void doingPostFunctionNoop(bool b) { settings_.partitioner.doingPostFunctionNoop = b; }
    /** @} */

    /** Property: Whether to run the function may-return analysis.
     *
     *  The caller can decide whether may-return analysis runs, or if it runs whether an indeterminate result should be
     *  considered true or false.
     *
     * @{ */
    FunctionReturnAnalysis functionReturnAnalysis() const /*final*/ { return settings_.partitioner.functionReturnAnalysis; }
    virtual void functionReturnAnalysis(FunctionReturnAnalysis x) { settings_.partitioner.functionReturnAnalysis = x; }
    /** @} */

    /** Property: Maximum number of function may-return sorting operations.
     *
     *  If function may-return analysis is being run, the functions are normally sorted according to their call depth (after
     *  arbitrarily breaking cycles) and the analysis is run from the leaf functions to the higher functions in order to
     *  minimize forward dependencies. However, the functions need to be resorted each time a new function is discovered and/or
     *  when the global CFG is sufficiently modified. Therefore, the total cost of the sorting can be substantial for large
     *  specimens. This property limits the total number of sorting operations and reverts to unsorted analysis once the limit
     *  is reached. This allows smaller specimens to be handled as accurately as possible, but still allows large specimens to
     *  be processed in a reasonable amount of time.  The limit is based on the number of sorting operations rather than the
     *  specimen size.
     *
     * @{ */
    size_t functionReturnAnalysisMaxSorts() const /*final*/ { return settings_.partitioner.functionReturnAnalysisMaxSorts; }
    virtual void functionReturnAnalysisMaxSorts(size_t n) { settings_.partitioner.functionReturnAnalysisMaxSorts = n; }
    /** @} */

    /** Property: Whether to search for function calls between exiting functions.
     *
     *  If set, then @ref JvmEngine::makeFunctionFromInterFunctionCalls is invoked, which looks for call-like code between
     *  existing functions in order to create new functions at the call target addresses.
     *
     * @{ */
    bool findingInterFunctionCalls() const /*final*/ { return settings_.partitioner.findingInterFunctionCalls; }
    virtual void findingInterFunctionCalls(bool b) { settings_.partitioner.findingInterFunctionCalls = b; }
    /** @} */

    /** Property: Whether to turn function call targets into functions.
     *
     *  If set, then sequences of instructions that behave like a function call (including plain old function call
     *  instructions) will cause a function to be created at the call's target address under most circumstances.
     *
     * @{ */
    bool findingFunctionCallFunctions() const /*final*/ { return settings_.partitioner.findingFunctionCallFunctions; }
    virtual void findingFunctionCallFunctions(bool b) { settings_.partitioner.findingFunctionCallFunctions = b; }
    /** @} */

    /** Property: Whether to make functions at program entry points.
     *
     *  If set, then all program entry points are assumed to be the start of a function.
     *
     * @{ */
    bool findingEntryFunctions() const /*final*/ { return settings_.partitioner.findingEntryFunctions; }
    virtual void findingEntryFunctions(bool b) { settings_.partitioner.findingEntryFunctions = b; }
    /** @} */

    /** Property: Whether to make error handling functions.
     *
     *  If set and information is available about error handling and exceptions, then that information is used to create entry
     *  points for functions.
     *
     * @{ */
    bool findingErrorFunctions() const /*final*/ { return settings_.partitioner.findingErrorFunctions; }
    virtual void findingErrorFunctions(bool b) { settings_.partitioner.findingErrorFunctions = b; }
    /** @} */

    /** Property: Whether to make functions at import addresses.
     *
     *  If set and the file contains a table describing the addresses of imported functions, then each of those addresses is
     *  assumed to be the entry point of a function.
     *
     * @{ */
    bool findingImportFunctions() const /*final*/ { return settings_.partitioner.findingImportFunctions; }
    virtual void findingImportFunctions(bool b) { settings_.partitioner.findingImportFunctions = b; }
    /** @} */

    /** Property: Whether to make functions at export addresses.
     *
     *  If set and the file contains a table describing the addresses of exported functions, then each of those addresses is
     *  assumed to be the entry point of a function.
     *
     * @{ */
    bool findingExportFunctions() const /*final*/ { return settings_.partitioner.findingExportFunctions; }
    virtual void findingExportFunctions(bool b) { settings_.partitioner.findingExportFunctions = b; }
    /** @} */

    /** Property: Whether to make functions according to symbol tables.
     *
     *  If set and the file contains symbol tables, then symbols that define function addresses cause functions to be created
     *  at those addresses.
     *
     * @{ */
    bool findingSymbolFunctions() const /*final*/ { return settings_.partitioner.findingSymbolFunctions; }
    virtual void findingSymbolFunctions(bool b) { settings_.partitioner.findingSymbolFunctions = b; }
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

    /** Property: Whether to search existing instructions for function pointers.
     *
     *  If this property is set, then the partitioner scans existing instructions to look for constants that seem to be
     *  pointers to functions that haven't been discovered yet.
     *
     * @{ */
    bool findingCodeFunctionPointers() const /*final*/ { return settings_.partitioner.findingCodeFunctionPointers; }
    virtual void findingCodeFunctionPointers(bool b) { settings_.partitioner.findingCodeFunctionPointers = b; }
    /** @} */

    /** Property: Whether to look for function calls used as branches.
     *
     *  If this property is set, then function call instructions are not automatically assumed to be actual function calls.
     *
     * @{ */
    bool checkingCallBranch() const /*final*/ { return settings_.partitioner.base.checkingCallBranch; }
    virtual void checkingCallBranch(bool b) { settings_.partitioner.base.checkingCallBranch = b; }
    /** @} */

    /** Property: Automatically drop semantics for attached basic blocks.
     *
     *  Basic blocks normally cache their semantic state as they're being discovered so that the state does not need to be
     *  recomputed from the beginning of the block each time a new instruction is appended.  However, caching this information
     *  can consume a large number of symbolic expression nodes which are seldom needed once the basic block is fully
     *  discovered.  Therefore, setting this property to true will cause a basic block's semantic information to be forgotten
     *  as soon as the basic block is attached to the CFG.
     *
     *  @sa Partitioner::basicBlockDropSemantics
     *
     * @{ */
    bool basicBlockSemanticsAutoDrop() const /*final*/ { return settings_.partitioner.base.basicBlockSemanticsAutoDrop; }
    void basicBlockSemanticsAutoDrop(bool b) { settings_.partitioner.base.basicBlockSemanticsAutoDrop = b; }
    /** @} */

    /** Property: Configuration files.
     *
     *  This property holds a list of configuration files or directories.
     *
     * @{ */
    const std::vector<std::string>& configurationNames() /*final*/ const { return settings_.engine.configurationNames; }
    std::vector<std::string>& configurationNames() /*final*/ { return settings_.engine.configurationNames; }
    /** @} */

    /** Property: Give names to constants.
     *
     *  If this property is set, then the partitioner calls @ref Modules::nameConstants as part of its final steps.
     *
     * @{ */
    bool namingConstants() const /*final*/ { return settings_.partitioner.namingConstants; }
    virtual void namingConstants(bool b) { settings_.partitioner.namingConstants = b; }
    /** @} */

    /** Property: Addresses where strings might start.
     *
     *  Within instruction operands, any constants that fall within this set of addresses are checked to see if they point into
     *  an ASCII C-style NUL-terminated string. If so, and if the constant doesn't already have a comment, then a comment is
     *  attached describing the string.  Setting this to empty disables assigning string literal comments to integer values.
     *
     * @{ */
    const AddressInterval& namingStrings() const /*final*/ { return settings_.partitioner.namingStrings; }
    void namingStrings(const AddressInterval &where) { settings_.partitioner.namingStrings = where; }
    /** @} */

    /** Property: Give names to system calls.
     *
     *  If this property is set, then the partitioner makes a pass after the control flow graph is finalized and tries to give
     *  names to system calls using the @ref Rose::BinaryAnalysis::SystemCall analysis.
     *
     * @{ */
    bool namingSystemCalls() const /*final*/ { return settings_.partitioner.namingSyscalls; }
    virtual void namingSystemCalls(bool b) { settings_.partitioner.namingSyscalls = b; }
    /** @} */

    /** Property: Header file in which system calls are defined.
     *
     *  If this property is not empty, then the specified Linux header file is parsed to obtain the mapping between system call
     *  numbers and their names. Otherwise, any analysis that needs system call names obtains them by looking in predetermined
     *  system header files.
     *
     * @{ */
    const boost::filesystem::path& systemCallHeader() const /*final*/ { return settings_.partitioner.syscallHeader; }
    virtual void systemCallHeader(const boost::filesystem::path &filename) { settings_.partitioner.syscallHeader = filename; }
    /** @} */

    /** Property: Demangle names.
     *
     *  If this property is set, then names are passed through a demangle step, which generally converts them from a low-level
     *  format to a source language format.
     *
     * @{ */
    bool demangleNames() const /*final*/ { return settings_.partitioner.demangleNames; }
    virtual void demangleNames(bool b) { settings_.partitioner.demangleNames = b; }
    /** @} */

    /** Property: Whether to allow empty global block in the AST.
     *
     *  If partitioner has not detected any functions, then it will create an AST containing either a single global block with
     *  no children (true) or no global block at all (false).
     *
     * @{ */
    bool astAllowEmptyGlobalBlock() const /*final*/ { return settings_.astConstruction.allowEmptyGlobalBlock; }
    virtual void astAllowEmptyGlobalBlock(bool b) { settings_.astConstruction.allowEmptyGlobalBlock = b; }
    /** @} */

    /** Property: Whether to allow empty functions in the AST.
     *
     *  If a function has no basic blocks then the partitioner will create an AST containing either a function node (@ref
     *  SgAsmFunction) with no basic block children (true) or no function node at all (false).
     *
     * @{ */
    bool astAllowFunctionWithNoBasicBlocks() const /*final*/ {
        return settings_.astConstruction.allowFunctionWithNoBasicBlocks;
    }
    virtual void astAllowFunctionWithNoBasicBlocks(bool b) {
        settings_.astConstruction.allowFunctionWithNoBasicBlocks = b;
    }
    /** @} */

    /** Property: Whether to allow empty basic blocks in the AST.
     *
     *  If a basic block has no instructions then the partitioner will create an AST containing either a basic block node (@ref
     *  SgAsmNode) with no instruction children (true) or no basic block node at all (false).
     *
     * @{ */
    bool astAllowEmptyBasicBlock() const /*final*/ { return settings_.astConstruction.allowEmptyBasicBlocks; }
    virtual void astAllowEmptyBasicBlock(bool b) { settings_.astConstruction.allowEmptyBasicBlocks = b; }
    /** @} */

    /** Property: Whether to copy instructions when building the AST.
     *
     *  Determines whether each instruction is deep-copied into the AST from the instruction provider (true) or simply
     *  referenced (false).  Note that because the partitioner allows the same instruction to appear in more than one function,
     *  referencing instructions directly in the AST will violate the basic property of a tree: the instruction will be
     *  reachable in an AST depth-first traversal more than once although the instruction will have only one arbitrarily chosen
     *  basic block as its parent. Turning off the copying makes AST construction faster.
     *
     * @{ */
    bool astCopyAllInstructions() const /*final*/ { return settings_.astConstruction.copyAllInstructions; }
    virtual void astCopyAllInstructions(bool b) { settings_.astConstruction.copyAllInstructions = b; }
    /** @} */

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Python API support functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef ROSE_ENABLE_PYTHON_API

    // Similar to frontend, but returns a partitioner rather than an AST since the Python API doesn't yet support ASTs.
    Partitioner pythonParseVector(boost::python::list &pyArgs, const std::string &purpose, const std::string &description);
    Partitioner pythonParseSingle(const std::string &specimen, const std::string &purpose, const std::string &description);

#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Internal stuff
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
    void init();

    // Similar to ::frontend but a lot less complicated.
    SgProject* roseFrontendReplacement(const std::vector<boost::filesystem::path> &fileNames);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      JVM Module
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Disassembly and partitioning utility functions for JVM. */
namespace ModulesJvm {

/** True if named file is a Java class file.
 *
 *  Class files usually have names with a ".class" extension, although this function actually tries to open the file and parse
 *  the file header to make that determination. */
bool isJavaClassFile(const boost::filesystem::path&);

} // namespace

} // namespace
} // namespace
} // namespace

#endif
#endif
