#ifndef ROSE_BinaryAnalysis_Partitioner2_EngineJvm_H
#define ROSE_BinaryAnalysis_Partitioner2_EngineJvm_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/ByteCode/Jvm.h>
#include <Rose/BinaryAnalysis/Partitioner2/Engine.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

/** Engine for Java Virtual Machine (JVM) specimens.
 *
 *  This engine is reponsible for creating a partitioner for a specimen that has byte code for the Java Virtual Machine.
 *
 *  This engine provides an @ref instance static member function that instantiates an engine of this type on the heap and returns a
 *  shared-ownership pointer to the instance. Refer to the base class, @ref Partitioner2::Engine, to learn how to instantiate
 *  engines from factories. */
class EngineJvm: public Engine {
public:
    /** Shared ownership pointer. */
    using Ptr = EngineJvmPtr;

  //TODO:DELETE_ME/MOVE_ME
    /** Errors from the engine. */
    class Exception: public Rose::Exception {
    public:
        Exception(const std::string &mesg)
            : Rose::Exception(mesg) {}
        ~Exception() throw () {}
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Internal data structures
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Data members
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
    // Mapping of class names to virtual address
    std::map<std::string, SgAsmGenericFile*> classes_;

    // Mapping of function names to virtual address
    std::map<std::string, rose_addr_t> functions_;

    // Mapping of unresolved (added to partitioner) function names to virtual address
    std::map<std::string, rose_addr_t> unresolvedFunctions_;

    static constexpr rose_addr_t vaDefaultIncrement{4*1024};
    rose_addr_t nextFunctionVa_;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Default constructor.  Constructor is deleted and class noncopyable. */
    EngineJvm() = delete;
    EngineJvm(const EngineJvm&) = delete;
    EngineJvm& operator=(const EngineJvm&) = delete;

protected:
    explicit EngineJvm(const Settings &settings);

public:
    ~EngineJvm();

    /** Allocating constructor with settings. */
    static Ptr instance(const Settings&);

    /** Allocate a factory. */
    static Ptr factory();

    virtual bool matchFactory(const std::vector<std::string> &specimen) const override;
    virtual EnginePtr instanceFromFactory(const Settings&) override;

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
    virtual SgAsmBlock* frontend(const std::vector<std::string> &args,
                                 const std::string &purpose, const std::string &description) override;
    /** @} */

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Basic top-level steps
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Command-line parsing
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    virtual std::pair<std::string, std::string> specimenNameDocumentation() override;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Container parsing
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
    virtual SgAsmInterpretation* parseContainers(const std::vector<std::string> &fileNames) override;

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

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Load specimens
    //
    // top-level: loadSpecimens
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:

    /** Returns true if specimens are loaded.
     *
     *  Specifically, returns true if the memory map is non-empty. */
    virtual bool areSpecimensLoaded() const override;

    /** Load interpretation.
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

    /** Load a class file by parsing its contents at the given address.
     *
     *  Returns the AST node representing the file.
     *
     * @{ */
    rose_addr_t loadClassFile(boost::filesystem::path, SgAsmGenericFileList*, rose_addr_t);

    /** Recursively find and load all super classes starting at the given address.
     *
     *  Returns the address of the last preloaded class.
     *
     * @{ */
    rose_addr_t loadSuperClasses(const std::string &, SgAsmGenericFileList*, rose_addr_t);

    /** Load classes discoverable from the file list starting at the given address.
     *
     *  Loads all classes and superclasses discovered by examining instructions.
     *
     *  Returns the next available address.
     *
     * @{ */
    rose_addr_t loadDiscoverableClasses(SgAsmGenericFileList*, rose_addr_t);

    /** Load class and super class starting at the given address.
     *
     *  Loads the class and super class from the file header and adds new files to the file list.
     *
     *  Returns the next available address.
     *
     * @{ */
    rose_addr_t loadClass(uint16_t, SgAsmJvmConstantPool*, SgAsmGenericFileList*, rose_addr_t);

    /** Path to the given class.
     *
     *  Returns the path to the class (may not exist).
     *
     * @{ */
    boost::filesystem::path pathToClass(const std::string &);

    /** Discover function calls (invoke instructions) made from a method.
     *
     *  Stores in a map of fully resolved function names to unique virtual addresses.
     *
     * @{ */
    void discoverFunctionCalls(SgAsmJvmMethod*, SgAsmJvmConstantPool*, std::map<std::string,rose_addr_t> &);

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

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Disassembler
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Partitioner high-level functions
    //
    // top-level: partition
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:

    /** Check that we have everything necessary to create a partitioner. */
    virtual void checkCreatePartitionerPrerequisites() const override;

    /** Create a partitioner for JVM.
     *
     *  Returns a partitioner that is tuned to operate on a JVM instruction set architecture. The engine must have @ref
     *  disassembler (if @ref doDisassemble property is set) and @ref memoryMap properties assigned already, either explicitly
     *  or as the result of earlier steps. */
    PartitionerPtr createJvmTunedPartitioner();

    /** Create partitioner.
     *
     *  This is the method usually called to create a new partitioner.  The base class just calls @ref
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
    // Partitioner mid-level functions
    //
    // These are the functions called by the partitioner high-level stuff.  These are sometimes overridden in subclasses,
    // although it is more likely that the high-level stuff is overridden.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:

    /** Discover as many basic blocks as possible.
     *
     *  Processes the "undiscovered" work list until the list becomes empty.  This list is the list of basic block placeholders
     *  for which no attempt has been made to discover instructions.  This method implements a recursive descent disassembler,
     *  although it does not process the control flow edges in any particular order. Subclasses are expected to override this
     *  to implement a more directed approach to discovering basic blocks. */
    virtual void discoverBasicBlocks(const PartitionerPtr&, const ByteCode::Method*);

    /** Discover as many functions as possible.
     *
     *  Discover as many functions as possible by discovering as many basic blocks as possible (@ref discoverBasicBlocks), Each
     *  time we run out of basic blocks to try, we look for another function prologue pattern at the lowest possible address
     *  and then recursively discover more basic blocks.  When this procedure is exhausted a call to @ref
     *  attachBlocksToFunctions tries to attach each basic block to a function. */
    virtual void discoverFunctions(const PartitionerPtr&, const ByteCode::Class*);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Partitioner low-level functions
    //
    // These are functions that a subclass seldom overrides, and maybe even shouldn't override because of their complexity or
    // the way the interact with one another.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Build AST
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
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

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Settings and properties
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
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
    //                                  Internal stuff
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
protected:
    // Similar to ::frontend but a lot less complicated.
    virtual SgProject* roseFrontendReplacement(const std::vector<boost::filesystem::path> &fileNames) override;
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
