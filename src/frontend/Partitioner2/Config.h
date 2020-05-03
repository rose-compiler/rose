#ifndef ROSE_Partitioner2_Config_H
#define ROSE_Partitioner2_Config_H

#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

#include <FileSystem.h>
#include <Partitioner2/Function.h>
#include <Sawyer/Map.h>
#include <Sawyer/Optional.h>
#include <SourceLocation.h>
#include <set>
#include <string>

namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

/** Configuration information for a basic block. */
class BasicBlockConfig {
    rose_addr_t address_;
    std::string comment_;
    Sawyer::Optional<rose_addr_t> finalInsnVa_;
    std::set<rose_addr_t> successorVas_;
    SourceLocation sourceLocation_;

public:
    // default constructor needed by some STL containers
    BasicBlockConfig(): address_(0) {}

    /** Configuration information for a basic block. */
    explicit BasicBlockConfig(rose_addr_t va): address_(va) {}

    /** Property: starting address.
     *
     *  Addresses are read-only and specified in the constructor. */
    rose_addr_t address() const { return address_; }

    /** Property: comment.
     *
     *  A basic block may have a multi-line, plain-text, ASCII comment.
     *
     * @{ */
    const std::string& comment() const { return comment_; }
    BasicBlockConfig& comment(const std::string &s) { comment_ = s; return *this; }
    /** @} */

    /** Property: location of basic block in source code.
     *
     * @{ */
    const SourceLocation& sourceLocation() const { return sourceLocation_; }
    BasicBlockConfig& sourceLocation(const SourceLocation &loc) { sourceLocation_ = loc; return *this; }
    /** @} */

    /** Property: last instruction.
     *
     *  Virtual address of basic block's final instruction.  Basic blocks are usually constructed one instruction at a time, so
     *  specifying a final instruction is one way to force the end of a basic block.  A final instruction is required for some
     *  of the other properties to be well defined.
     *
     * @{ */
    Sawyer::Optional<rose_addr_t> finalInstructionVa() const { return finalInsnVa_; }
    BasicBlockConfig& finalInstructionVa(const Sawyer::Optional<rose_addr_t> &va) { finalInsnVa_ = va; return *this; }
    /** @} */

    /** Property: control flow successors.
     *
     *  A basic block's control flow successors are normally calculated automatically.  However, it is sometimes useful to be
     *  able to override them manually, such as when the calculation fails to do anything meaningful.  Successors in this list
     *  only apply when the basic block reaches its final instruction (see @sa finalInstructionVa).
     *
     * @{ */
    const std::set<rose_addr_t>& successorVas() const { return successorVas_; }
    std::set<rose_addr_t>& successorVas() { return successorVas_; }
    BasicBlockConfig& successorVas(const std::set<rose_addr_t> &vas) { successorVas_ = vas; return *this; }
    BasicBlockConfig& insertSuccessorVa(rose_addr_t va) { successorVas_.insert(va); return *this; }
    BasicBlockConfig& clearSuccessorVas() { successorVas_.clear(); return *this; }
    /** @} */
};


/** Configuration information for a data block. */
class DataBlockConfig {
    rose_addr_t address_;
    std::string name_;
    std::string comment_;
    SourceLocation sourceLocation_;

public:
    // Default constructor needed by some STL containers
    DataBlockConfig(): address_(0) {}

    /** Configuration information for a data block. */
    explicit DataBlockConfig(rose_addr_t va): address_(va) {}

    /** Property: starting address.
     *
     *  Addresses are read-only and specified in the constructor. */
    rose_addr_t address() const { return address_; }

    /** Property: name.
     *
     *  A data block may have an optional name.
     *
     * @{ */
    const std::string &name() const { return name_; }
    DataBlockConfig& name(const std::string &s) { name_ = s; return *this; }
    /** @} */

    /** Property: comment.
     *
     *  A data block may have a multi-line, plain-text, ASCII comment.
     *
     * @{ */
    const std::string& comment() const { return comment_; }
    DataBlockConfig& comment(const std::string &s) { comment_ = s; return *this; }
    /** @} */

    /** Property: Location of data block in source code.
     *
     * @{ */
    const SourceLocation& sourceLocation() const { return sourceLocation_; }
    DataBlockConfig& sourceLocation(const SourceLocation &loc) { sourceLocation_ = loc; return *this; }
    /** @} */
};


/** Configuration information for a function. */
class FunctionConfig {
    Sawyer::Optional<rose_addr_t> address_;
    std::string name_, defaultName_, comment_;
    Sawyer::Optional<int64_t> stackDelta_;
    Sawyer::Optional<bool> mayReturn_;
    SourceLocation sourceLocation_;

public:
    // default constructor needed by some STL containers
    FunctionConfig() {}

    /** Configuration information for a function.
     *
     *  @{ */
    explicit FunctionConfig(rose_addr_t va, const std::string &name=""): address_(va), name_(name) {}
    explicit FunctionConfig(const std::string &name): name_(name) {}
    FunctionConfig(const Sawyer::Optional<rose_addr_t> &va, const std::string &name): address_(va), name_(name) {}
    /** @} */
    
    /** Property: address.
     *
     *  A function configuration has an optional address which is read-only, initialized by the constructor.  The return value
     *  can be used in a boolean context to test whether the address is present and dereferenced to get the address.  The
     *  Sawyer::Optional class has a number of other useful methods, such as @c getOrElse and @c assignTo:
     *
     *  @code
     *   FunctionConfig fconfig;
     *   if (fconfig.address())
     *       std::cout <<address is " <<*fconfig.address() <<"\n";
     *   std::cout <<"address is " <<fconfig.address().orElse(0) <<"\n";
     *   rose_addr_t va = 0;
     *   if (fconfig.address().assignTo(va))
     *       std::cout <<"address is " <<va <<"\n";
     * @endcode */
    Sawyer::Optional<rose_addr_t> address() const { return address_; }

    /** Property: name.
     *
     *  This is the name to use for the function regardless of what name has been given to this function's address by the
     *  ELF/PE container, symbol tables, etc.  The name is read-only if the config has no address.
     *
     * @sa defaultName
     *
     * @{ */
    const std::string& name() const { return name_; }
    FunctionConfig& name(const std::string &s);
    /** @} */

    /** Property: default name.
     *
     *  This is the name to use for a function if it hasn't been given a name by any other means.
     *
     * @{ */
    const std::string& defaultName() const { return defaultName_; }
    FunctionConfig& defaultName(const std::string &s) { defaultName_ = s; return *this; }
    /** @} */

    /** Property: comment.
     *
     *  A function may have a multi-line, plain-text, ASCII comment.
     *
     * @{ */
    const std::string& comment() const { return comment_; }
    FunctionConfig& comment(const std::string &s) { comment_ = s; return *this; }
    /** @} */

    /** Property: Location of function in source code.
     *
     * @{ */
    const SourceLocation& sourceLocation() const { return sourceLocation_; }
    FunctionConfig& sourceLocation(const SourceLocation &loc) { sourceLocation_ = loc; return *this; }
    /** @} */

    /** Property: stack delta.
     *
     *  The stack delta is a numeric value that indicates the net effect this function has on the stack pointer.  If a stack
     *  delta is specified then it will be used instead of performing a stack delta analysis.
     *
     *  This is an optional property; see @ref address for typical uses of the return value. The stack delta can be set by
     *  passing a signed value, and cleared by passing Sawyer::Nothing:
     *
     * @code
     *  FunctionConfig fconfig;
     *  fconfig.stackDelta(Sawyer::Nothing()); // clear the stack delta
     *  fconfig.stackDelta(4);                 // set the stack delta
     * @endcode
     *
     * @{ */
    Sawyer::Optional<int64_t> stackDelta() const { return stackDelta_; }
    FunctionConfig& stackDelta(const Sawyer::Optional<int64_t> &n) { stackDelta_ = n; return *this; }
    /** @} */

    /** Property: may-return.
     *
     *  This property specifies whether the function might return to its caller.  If the function might return, the value
     *  should be true; if the function cannot possibly return then the value is false; indeterminate values indicate that a
     *  may-return analysis should be performed.
     *
     *  Be careful using this return value: the return value is a SawyerOptional, which when evaluated in a boolean context
     *  will tell you whether a may-return value is present, not what that value is!  In order to get the value, if present,
     *  you'll need to either dereference the return value or call something like @c orElse or @c assignTo.
     *
     * @{ */
    Sawyer::Optional<bool> mayReturn() const { return mayReturn_; }
    FunctionConfig& mayReturn(const Sawyer::Optional<bool> &b) { mayReturn_ = b; return *this; }
    /** @} */
};

/** Configuration for individual addresses.
 *
 *  If the address is the start of a function, basic block, or data block, then use one of those configuration objects instead. */
class AddressConfig {
    rose_addr_t address_;
    std::string name_;
    std::string comment_;
    SourceLocation sourceLocation_;

public:
    // Default constructor needed by STL containers
    AddressConfig() {}

    /** Construct a new address configuration object. */
    explicit AddressConfig(rose_addr_t va)
        : address_(va) {}

    /** Property: address.
     *
     *  The address property is read-only, set by the constructor. */
    rose_addr_t address() const { return address_; }

    /** Property: name.
     *
     * @{ */
    const std::string &name() const { return name_; }
    AddressConfig& name(const std::string &s) { name_ = s; return *this; }
    /** @} */
    
    /** Property: comment string.
     *
     * @{ */
    const std::string &comment() const { return comment_; }
    AddressConfig& comment(const std::string &s) { comment_ = s; return *this; }
    /** @} */

    /** Property: Location of address within source code.
     *
     * @{ */
    const SourceLocation& sourceLocation() const { return sourceLocation_; }
    AddressConfig& sourceLocation(const SourceLocation &loc) { sourceLocation_ = loc; return *this; }
    /** @} */
};

/** Holds configuration information. */
class Configuration {
public:
    typedef Sawyer::Container::Map<rose_addr_t, BasicBlockConfig> BasicBlockConfigs;
    typedef Sawyer::Container::Map<rose_addr_t, DataBlockConfig> DataBlockConfigs;
    typedef Sawyer::Container::Map<rose_addr_t, FunctionConfig> FunctionConfigsByAddress;
    typedef Sawyer::Container::Map<std::string, FunctionConfig> FunctionConfigsByName;
    typedef Sawyer::Container::Map<rose_addr_t, AddressConfig> AddressConfigs;

private:
    BasicBlockConfigs bblockConfigs_;
    DataBlockConfigs dblockConfigs_;
    FunctionConfigsByAddress functionConfigsByAddress_;
    FunctionConfigsByName functionConfigsByName_;
    AddressConfigs addressConfigs_;

public:
    /** Documentation string describing the file format. */
    static std::string fileFormatDoc();

    /** Loads configuration from a file. */
    void loadFromFile(const FileSystem::Path &fileName);

    /** All basic block configuration details.
     *
     * @{ */
    const BasicBlockConfigs& basicBlocks() const { return bblockConfigs_; }
    BasicBlockConfigs& basicBlocks() { return bblockConfigs_; }
    /** @} */

    /** Configuration for a particular basic block.
     *
     *  If no configuration exists for a basic block starting at the specified address, then a reference to an empty
     *  configuration record is returned. */
    const BasicBlockConfig& basicBlock(rose_addr_t) const;

    /** All data block configuration details.
     *
     * @{ */
    const DataBlockConfigs& dataBlocks() const { return dblockConfigs_; }
    DataBlockConfigs& dataBlocks() { return dblockConfigs_; }
    /** @} */

    /** Configuration for a particular data block.
     *
     *  If no configuration exists for a data block starting at the specified address, then a reference to an empty
     *  configuration record is returned. */
    const DataBlockConfig& dataBlock(rose_addr_t) const;

    /** All function configuration details for function configs that have addresses.
     *
     * @{ */
    const FunctionConfigsByAddress& functionConfigsByAddress() const { return functionConfigsByAddress_; }
    FunctionConfigsByAddress& functionConfigsByAddress() { return functionConfigsByAddress_; }
    /** @} */

    /** All function configuration details for configs that have no address.
     *
     * @{ */
    const FunctionConfigsByName& functionConfigsByName() const { return functionConfigsByName_; }
    FunctionConfigsByName& functionConfigsByName() { return functionConfigsByName_; }
    /** @} */

    /** Configuration for a particular function.
     *
     *  If no configuration exists for a function block starting at the specified address or having the specified name, then a
     *  reference to an empty configuration record is returned.
     *
     * @{ */
    const FunctionConfig& function(rose_addr_t) const;
    const FunctionConfig& function(const std::string &name) const;
    /** @} */

    /** All address configuration details.
     *
     * @{ */
    const AddressConfigs& addresses() const { return addressConfigs_; }
    AddressConfigs& addresses() { return addressConfigs_; }
    /** @} */

    /** Configuration for a particular address.
     *
     *  If no configuration exists for the specified address, then a reference to an empty configuration record is returned. */
    const AddressConfig& address(rose_addr_t) const;

    /** Lookup or insert a basic block.
     *
     *  If the basic block exists then return a reference to its configuration, otherwise create a new configuration for it. */
    BasicBlockConfig& insertMaybeBasicBlock(rose_addr_t va);

    /** Lookup or insert a data block.
     *
     *  If the data block exists then return a reference to its configuration, otherwise create a new configuration for it. */
    DataBlockConfig& insertMaybeDataBlock(rose_addr_t va);

    /** Lookup or insert a function.
     *
     *  If the function exists then return a reference to its configuration, otherwise create a new configuration for it.
     *
     * @{ */
    FunctionConfig& insertMaybeFunction(rose_addr_t va, const std::string &name="");
    FunctionConfig& insertMaybeFunction(const std::string &name);
    /** @} */

    /** Lookup or insert address details.
     *
     *  If the address exists then return a reference to its configuration, otherwise create a new configuration for it. */
    AddressConfig& insertMaybeAddress(rose_addr_t va);

    /** Insert basic block configuration information.
     *
     *  Inserts basic block configuration information, overwriting any config information that was already present for the same
     *  basic block address.  Returns true if information was inserted rather than overwritten. */
    bool insertConfiguration(const BasicBlockConfig&);

    /** Insert data block configuration information.
     *
     *  Inserts data block configuration information, overwriting any config information that was already present for the same
     *  data block address.  Returns true if information was inserted rather than overwritten. */
    bool insertConfiguration(const DataBlockConfig&);

    /** Insert function configuration information.
     *
     *  Inserts function configuration information, overwriting any config information that was already present at the same
     *  address or name. Returns true if information was inserted rather than overwritten. */
    bool insertConfiguration(const FunctionConfig&);

    /** Insert address configuration information.
     *
     *  Inserts address configuration information, overwriting any config information that was already present at the same
     *  address.  Returns true if information was inserted rather than overwritten. */
    bool insertConfiguration(const AddressConfig&);

    /** Basic block comment.
     *
     *  Returns the comment configured for the basic block at the specified address, or an empty string.
     *
     *  See also, @ref comment. */
    std::string basicBlockComment(rose_addr_t bblockVa) const;

    /** Basic block final instruction address.
     *
     *  Returns the optional final instruction address for a basic block. If no such configuration information is avilable then
     *  nothing is returned. */
    Sawyer::Optional<rose_addr_t> basicBlockFinalInstructionVa(rose_addr_t bblockVa) const;

    /** Basic block successor addresses.
     *
     *  Returns the set of basic block successors. This set is only meaningful at the block's final instruction as returned by
     *  @ref basicBlockFinalInstructionVa. */
    std::set<rose_addr_t> basicBlockSuccessorVas(rose_addr_t bblockVa) const;

    /** Data block name.
     *
     *  Returns the name configured for a data block at the specified address, or an empty string. */
    std::string dataBlockName(rose_addr_t dblockVa) const;

    /** Data block comment.
     *
     *  Returns the comment configured for the data block at the specified address, or an empty string.
     *
     *  See also, @ref comment. */
    std::string dataBlockComment(rose_addr_t dblockVa) const;

    /** Function name.
     *
     *  Returns the configured name for a function at the specified address.
     *
     *  @sa functionDefaultName */
    std::string functionName(rose_addr_t functionVa) const;

    /** Function default name.
     *
     *  Returns the configured default name for a function at the specified address.
     *
     *  @sa functionName */
    std::string functionDefaultName(rose_addr_t functionVa) const;

    /** Function comment.
     *
     *  Returns the configured comment for a function at the specified address or having the specified name.  Function
     *  configuration is looked up by address or name-only (i.e., asking for config information for function "main" will not
     *  return anything if "main" has an address in the configuration).
     *
     *  See also, @ref comment.
     *
     * @{ */
    std::string functionComment(rose_addr_t functionVa) const;
    std::string functionComment(const std::string &functionName) const;
    std::string functionComment(const Function::Ptr&) const;
    /** @} */

    /** Function stack delta.
     *
     *  Returns the configured stack delta for a function at the specified address or having the specified name.  Function
     *  configuration is looked up by address or name-only (i.e., asking for config information for function "main" will not
     *  return anything if "main" has an address in the configuration).
     *
     * @{ */
    Sawyer::Optional<int64_t> functionStackDelta(rose_addr_t functionVa) const;
    Sawyer::Optional<int64_t> functionStackDelta(const std::string &functionName) const;
    Sawyer::Optional<int64_t> functionStackDelta(const Function::Ptr&) const;
    /** @} */

    /** Function may-return status.
     *
     *  Returns the configured may-return status for a function at the specified address or having the specified name.
     *  Function configuration is looked up by address or name-only (i.e., asking for config information for function "main"
     *  will not return anything if "main" has an address in the configuration).
     *
     *  Be careful using this return value: the return value is a SawyerOptional, which when evaluated in a boolean context
     *  will tell you whether a may-return value is present, not what that value is!  In order to get the value, if present,
     *  you'll need to either dereference the return value or call something like @c orElse or @c assignTo.
     *
     * @{ */
    Sawyer::Optional<bool> functionMayReturn(rose_addr_t functionVa) const;
    Sawyer::Optional<bool> functionMayReturn(const std::string &functionName) const;
    Sawyer::Optional<bool> functionMayReturn(const Function::Ptr&) const;
    /** @} */

    /** Address comment.
     *
     *  Returns the comment associated with the details for the specified address, if any.
     *
     *  See also, @ref comment. */
    std::string addressComment(rose_addr_t va) const;

    /** Address comment.
     *
     *  Returns the comment for the specified address by search first within the address details, then basic block details,
     *  then data block details, and finally function details. Returns the first non-empty comment that's found, or the empty
     *  string. */
    std::string comment(rose_addr_t va) const;

    /** Generate a YAML file. */
    void print(std::ostream&) const;
};

} // namespace
} // namespace
} // namespace

std::ostream& operator<<(std::ostream&, const Rose::BinaryAnalysis::Partitioner2::Configuration&);

#endif
#endif
