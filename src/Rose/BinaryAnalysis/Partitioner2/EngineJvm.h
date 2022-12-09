#ifndef ROSE_BinaryAnalysis_Partitioner2_EngineJvm_H
#define ROSE_BinaryAnalysis_Partitioner2_EngineJvm_H

#include <Rose/BinaryAnalysis/Partitioner2/Engine.h>

#ifdef ROSE_ENABLE_BINARY_ANALYSIS

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
 *  objects such as @ref Disassembler and @ref Partitioner.  In fact, this particular engine base class is designed so that
 *  users can pick and choose to use only those steps they need, or perhaps to call the main actions one step at a time with
 *  the user making adjustments between steps.
 *
 *  @section EngineJvm_extensibility Customization
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
 *  @section EngineJvm_basic Basic usage
 *
 *  The most basic use case for the engine is to pass it the command-line arguments and have it do everything, eventually
 *  returning an abstract syntax tree.
 *
 *  @code
 *   #include <rose.h>
 *   #include <Rose/BinaryAnalysis/Partitioner2/EngineJvm.h>
 *   using namespace Rose;
 *   namespace P2 = Rose::BinaryAnalysis::Partitioner2;
 *
 *   int main(int argc, char *argv[]) {
 *       std::string purpose = "disassembles a binary specimen";
 *       std::string description =
 *           "This tool disassembles the specified specimen and presents the "
 *           "results as a pseudo assembly listing, that is, a listing intended "
 *           "for human consumption rather than assembly.";
 *       SgAsmBlock *gblock = P2::EngineJvm().frontend(argc, argv, purpose, description);
 *  @endcode
 *
 *  @section EngineJvm_topsteps High level operations
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
class ROSE_DLL_API EngineJvm: private boost::noncopyable {
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
    Disassembler::BasePtr disassembler_;                // not ref-counted yet, but don't destroy it since user owns it
    Progress::Ptr progress_;                            // optional progress reporting

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Constructors
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Default constructor. */
    EngineJvm();

    /** Construct engine with settings. */
    explicit EngineJvm(const Settings &settings);

    virtual ~EngineJvm();

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
    virtual PartitionerPtr partition(const std::vector<std::string> &fileNames = std::vector<std::string>());
    PartitionerPtr partition(const std::string &fileName) /*final*/;
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
    virtual Disassembler::BasePtr obtainDisassembler(const Disassembler::BasePtr &hint = Disassembler::BasePtr());
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
    virtual PartitionerPtr createPartitioner();

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

    /** Discover as many basic blocks as possible.
     *
     *  Processes the "undiscovered" work list until the list becomes empty.  This list is the list of basic block placeholders
     *  for which no attempt has been made to discover instructions.  This method implements a recursive descent disassembler,
     *  although it does not process the control flow edges in any particular order. Subclasses are expected to override this
     *  to implement a more directed approach to discovering basic blocks. */
    virtual void discoverBasicBlocks(const PartitionerPtr&);

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
    const Settings& settings() const /*final*/;
    Settings& settings() /*final*/;
    /** @} */

    /** Property: progress reporting.
     *
     *  The optional object to receive progress reports.
     *
     * @{ */
    Progress::Ptr progress() const /*final*/;
    virtual void progress(const Progress::Ptr&);
    /** @} */

    /** Property: Disassembler.
     *
     *  This property holds the disassembler to use whenever a new partitioner is created. If null, then the engine will choose
     *  a disassembler based on the binary container (unless @ref doDisassemble property is clear).
     *
     * @{ */
    Disassembler::BasePtr disassembler() const /*final*/;
    virtual void disassembler(const Disassembler::BasePtr&);
    /** @} */

    /** Property: Instruction set architecture name.
     *
     *  The instruction set architecture name is used to obtain a disassembler and overrides the disassembler that would
     *  otherwise be found by examining the binary container.
     *
     * @{ */
    const std::string& isaName() const /*final*/;
    virtual void isaName(const std::string&);
    /** @} */

    /** Property: Starting addresses for disassembly.
     *
     *  This is a list of addresses where functions will be created in addition to those functions discovered by examining the
     *  binary container.
     *
     * @{ */
    const std::vector<rose_addr_t>& functionStartingVas() const /*final*/;
    std::vector<rose_addr_t>& functionStartingVas() /*final*/;
    /** @} */

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Python API support functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef ROSE_ENABLE_PYTHON_API

    // Similar to frontend, but returns a partitioner rather than an AST since the Python API doesn't yet support ASTs.
    Partitioner::Ptr pythonParseVector(boost::python::list &pyArgs, const std::string &purpose, const std::string &description);
    Partitioner::Ptr pythonParseSingle(const std::string &specimen, const std::string &purpose, const std::string &description);

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
