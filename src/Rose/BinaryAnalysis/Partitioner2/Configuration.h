#ifndef ROSE_BinaryAnalysis_Partitioner2_Configuration_H
#define ROSE_BinaryAnalysis_Partitioner2_Configuration_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Partitioner2/BasicTypes.h>

#include <Rose/FileSystem.h>
#include <Sawyer/Map.h>
#include <Sawyer/Optional.h>
#include <Rose/SourceLocation.h>
#include <set>
#include <string>

namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

/** Configuration information for a basic block. */
class BasicBlockConfiguration {
    Address address_ = 0;
    std::string comment_;
    Sawyer::Optional<Address> finalInsnVa_;
    std::set<Address> successorVas_;
    SourceLocation sourceLocation_;

public:
    // default constructor needed by some STL containers
    BasicBlockConfiguration() {}

    /** Configuration information for a basic block. */
    explicit BasicBlockConfiguration(Address va): address_(va) {}

    /** Property: starting address.
     *
     *  Addresses are read-only and specified in the constructor. */
    Address address() const { return address_; }

    /** Property: comment.
     *
     *  A basic block may have a multi-line, plain-text, ASCII comment.
     *
     * @{ */
    const std::string& comment() const { return comment_; }
    BasicBlockConfiguration& comment(const std::string &s) { comment_ = s; return *this; }
    /** @} */

    /** Property: location of basic block in source code.
     *
     * @{ */
    const SourceLocation& sourceLocation() const { return sourceLocation_; }
    BasicBlockConfiguration& sourceLocation(const SourceLocation &loc) { sourceLocation_ = loc; return *this; }
    /** @} */

    /** Property: last instruction.
     *
     *  Virtual address of basic block's final instruction.  Basic blocks are usually constructed one instruction at a time, so
     *  specifying a final instruction is one way to force the end of a basic block.  A final instruction is required for some
     *  of the other properties to be well defined.
     *
     * @{ */
    Sawyer::Optional<Address> finalInstructionVa() const { return finalInsnVa_; }
    BasicBlockConfiguration& finalInstructionVa(const Sawyer::Optional<Address> &va) { finalInsnVa_ = va; return *this; }
    /** @} */

    /** Property: control flow successors.
     *
     *  A basic block's control flow successors are normally calculated automatically.  However, it is sometimes useful to be
     *  able to override them manually, such as when the calculation fails to do anything meaningful.  Successors in this list
     *  only apply when the basic block reaches its final instruction (see @sa finalInstructionVa).
     *
     * @{ */
    const std::set<Address>& successorVas() const { return successorVas_; }
    std::set<Address>& successorVas() { return successorVas_; }
    BasicBlockConfiguration& successorVas(const std::set<Address> &vas) { successorVas_ = vas; return *this; }
    BasicBlockConfiguration& insertSuccessorVa(Address va) { successorVas_.insert(va); return *this; }
    BasicBlockConfiguration& clearSuccessorVas() { successorVas_.clear(); return *this; }
    /** @} */
};


/** Configuration information for a data block. */
class DataBlockConfiguration {
    Address address_;
    std::string name_;
    std::string comment_;
    SourceLocation sourceLocation_;

public:
    // Default constructor needed by some STL containers
    DataBlockConfiguration(): address_(0) {}

    /** Configuration information for a data block. */
    explicit DataBlockConfiguration(Address va): address_(va) {}

    /** Property: starting address.
     *
     *  Addresses are read-only and specified in the constructor. */
    Address address() const { return address_; }

    /** Property: name.
     *
     *  A data block may have an optional name.
     *
     * @{ */
    const std::string &name() const { return name_; }
    DataBlockConfiguration& name(const std::string &s) { name_ = s; return *this; }
    /** @} */

    /** Property: comment.
     *
     *  A data block may have a multi-line, plain-text, ASCII comment.
     *
     * @{ */
    const std::string& comment() const { return comment_; }
    DataBlockConfiguration& comment(const std::string &s) { comment_ = s; return *this; }
    /** @} */

    /** Property: Location of data block in source code.
     *
     * @{ */
    const SourceLocation& sourceLocation() const { return sourceLocation_; }
    DataBlockConfiguration& sourceLocation(const SourceLocation &loc) { sourceLocation_ = loc; return *this; }
    /** @} */
};


/** Configuration information for a function. */
class FunctionConfiguration {
    Sawyer::Optional<Address> address_;
    std::string name_, defaultName_, comment_;
    Sawyer::Optional<int64_t> stackDelta_;
    Sawyer::Optional<bool> mayReturn_;
    SourceLocation sourceLocation_;

public:
    // default constructor needed by some STL containers
    FunctionConfiguration() {}

    /** Configuration information for a function.
     *
     *  @{ */
    explicit FunctionConfiguration(Address va, const std::string &name=""): address_(va), name_(name) {}
    explicit FunctionConfiguration(const std::string &name): name_(name) {}
    FunctionConfiguration(const Sawyer::Optional<Address> &va, const std::string &name): address_(va), name_(name) {}
    /** @} */
    
    /** Property: address.
     *
     *  A function configuration has an optional address which is read-only, initialized by the constructor.  The return value
     *  can be used in a boolean context to test whether the address is present and dereferenced to get the address.  The
     *  Sawyer::Optional class has a number of other useful methods, such as @c getOrElse and @c assignTo:
     *
     *  @code
     *   FunctionConfiguration fconfig;
     *   if (fconfig.address())
     *       std::cout <<address is " <<*fconfig.address() <<"\n";
     *   std::cout <<"address is " <<fconfig.address().orElse(0) <<"\n";
     *   Address va = 0;
     *   if (fconfig.address().assignTo(va))
     *       std::cout <<"address is " <<va <<"\n";
     * @endcode */
    Sawyer::Optional<Address> address() const { return address_; }

    /** Property: name.
     *
     *  This is the name to use for the function regardless of what name has been given to this function's address by the
     *  ELF/PE container, symbol tables, etc.  The name is read-only if the config has no address.
     *
     * @sa defaultName
     *
     * @{ */
    const std::string& name() const { return name_; }
    FunctionConfiguration& name(const std::string &s);
    /** @} */

    /** Property: default name.
     *
     *  This is the name to use for a function if it hasn't been given a name by any other means.
     *
     * @{ */
    const std::string& defaultName() const { return defaultName_; }
    FunctionConfiguration& defaultName(const std::string &s) { defaultName_ = s; return *this; }
    /** @} */

    /** Property: comment.
     *
     *  A function may have a multi-line, plain-text, ASCII comment.
     *
     * @{ */
    const std::string& comment() const { return comment_; }
    FunctionConfiguration& comment(const std::string &s) { comment_ = s; return *this; }
    /** @} */

    /** Property: Location of function in source code.
     *
     * @{ */
    const SourceLocation& sourceLocation() const { return sourceLocation_; }
    FunctionConfiguration& sourceLocation(const SourceLocation &loc) { sourceLocation_ = loc; return *this; }
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
     *  FunctionConfiguration fconfig;
     *  fconfig.stackDelta(Sawyer::Nothing()); // clear the stack delta
     *  fconfig.stackDelta(4);                 // set the stack delta
     * @endcode
     *
     * @{ */
    Sawyer::Optional<int64_t> stackDelta() const { return stackDelta_; }
    FunctionConfiguration& stackDelta(const Sawyer::Optional<int64_t> &n) { stackDelta_ = n; return *this; }
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
    FunctionConfiguration& mayReturn(const Sawyer::Optional<bool> &b) { mayReturn_ = b; return *this; }
    /** @} */
};

/** Configuration for individual addresses.
 *
 *  If the address is the start of a function, basic block, or data block, then use one of those configuration objects instead. */
class AddressConfiguration {
    Address address_ = 0;
    std::string name_;
    std::string comment_;
    SourceLocation sourceLocation_;

public:
    // Default constructor needed by STL containers
    AddressConfiguration() {}

    /** Construct a new address configuration object. */
    explicit AddressConfiguration(Address va)
        : address_(va) {}

    /** Property: address.
     *
     *  The address property is read-only, set by the constructor. */
    Address address() const { return address_; }

    /** Property: name.
     *
     * @{ */
    const std::string &name() const { return name_; }
    AddressConfiguration& name(const std::string &s) { name_ = s; return *this; }
    /** @} */
    
    /** Property: comment string.
     *
     * @{ */
    const std::string &comment() const { return comment_; }
    AddressConfiguration& comment(const std::string &s) { comment_ = s; return *this; }
    /** @} */

    /** Property: Location of address within source code.
     *
     * @{ */
    const SourceLocation& sourceLocation() const { return sourceLocation_; }
    AddressConfiguration& sourceLocation(const SourceLocation &loc) { sourceLocation_ = loc; return *this; }
    /** @} */
};

/** Holds configuration information. */
class Configuration {
public:
    typedef Sawyer::Container::Map<Address, BasicBlockConfiguration> BasicBlockConfigurations;
    typedef Sawyer::Container::Map<Address, DataBlockConfiguration> DataBlockConfigurations;
    typedef Sawyer::Container::Map<Address, FunctionConfiguration> FunctionConfigurationsByAddress;
    typedef Sawyer::Container::Map<std::string, FunctionConfiguration> FunctionConfigurationsByName;
    typedef Sawyer::Container::Map<Address, AddressConfiguration> AddressConfigurations;

private:
    BasicBlockConfigurations bblockConfigurations_;
    DataBlockConfigurations dblockConfigurations_;
    FunctionConfigurationsByAddress functionConfigurationsByAddress_;
    FunctionConfigurationsByName functionConfigurationsByName_;
    AddressConfigurations addressConfigurations_;

public:
    /** Documentation string describing the file format. */
    static std::string fileFormatDoc();

    /** Loads configuration from a file. */
    void loadFromFile(const FileSystem::Path &fileName);

    /** All basic block configuration details.
     *
     * @{ */
    const BasicBlockConfigurations& basicBlocks() const { return bblockConfigurations_; }
    BasicBlockConfigurations& basicBlocks() { return bblockConfigurations_; }
    /** @} */

    /** Configuration for a particular basic block.
     *
     *  If no configuration exists for a basic block starting at the specified address, then a reference to an empty
     *  configuration record is returned. */
    const BasicBlockConfiguration& basicBlock(Address) const;

    /** All data block configuration details.
     *
     * @{ */
    const DataBlockConfigurations& dataBlocks() const { return dblockConfigurations_; }
    DataBlockConfigurations& dataBlocks() { return dblockConfigurations_; }
    /** @} */

    /** Configuration for a particular data block.
     *
     *  If no configuration exists for a data block starting at the specified address, then a reference to an empty
     *  configuration record is returned. */
    const DataBlockConfiguration& dataBlock(Address) const;

    /** All function configuration details for function configs that have addresses.
     *
     * @{ */
    const FunctionConfigurationsByAddress& functionConfigurationsByAddress() const { return functionConfigurationsByAddress_; }
    FunctionConfigurationsByAddress& functionConfigurationsByAddress() { return functionConfigurationsByAddress_; }
    /** @} */

    /** All function configuration details for configs that have no address.
     *
     * @{ */
    const FunctionConfigurationsByName& functionConfigurationsByName() const { return functionConfigurationsByName_; }
    FunctionConfigurationsByName& functionConfigurationsByName() { return functionConfigurationsByName_; }
    /** @} */

    /** Configuration for a particular function.
     *
     *  If no configuration exists for a function block starting at the specified address or having the specified name, then a
     *  reference to an empty configuration record is returned.
     *
     * @{ */
    const FunctionConfiguration& function(Address) const;
    const FunctionConfiguration& function(const std::string &name) const;
    /** @} */

    /** All address configuration details.
     *
     * @{ */
    const AddressConfigurations& addresses() const { return addressConfigurations_; }
    AddressConfigurations& addresses() { return addressConfigurations_; }
    /** @} */

    /** Configuration for a particular address.
     *
     *  If no configuration exists for the specified address, then a reference to an empty configuration record is returned. */
    const AddressConfiguration& address(Address) const;

    /** Lookup or insert a basic block.
     *
     *  If the basic block exists then return a reference to its configuration, otherwise create a new configuration for it. */
    BasicBlockConfiguration& insertMaybeBasicBlock(Address va);

    /** Lookup or insert a data block.
     *
     *  If the data block exists then return a reference to its configuration, otherwise create a new configuration for it. */
    DataBlockConfiguration& insertMaybeDataBlock(Address va);

    /** Lookup or insert a function.
     *
     *  If the function exists then return a reference to its configuration, otherwise create a new configuration for it.
     *
     * @{ */
    FunctionConfiguration& insertMaybeFunction(Address va, const std::string &name="");
    FunctionConfiguration& insertMaybeFunction(const std::string &name);
    /** @} */

    /** Lookup or insert address details.
     *
     *  If the address exists then return a reference to its configuration, otherwise create a new configuration for it. */
    AddressConfiguration& insertMaybeAddress(Address va);

    /** Insert basic block configuration information.
     *
     *  Inserts basic block configuration information, overwriting any config information that was already present for the same
     *  basic block address.  Returns true if information was inserted rather than overwritten. */
    bool insertConfiguration(const BasicBlockConfiguration&);

    /** Insert data block configuration information.
     *
     *  Inserts data block configuration information, overwriting any config information that was already present for the same
     *  data block address.  Returns true if information was inserted rather than overwritten. */
    bool insertConfiguration(const DataBlockConfiguration&);

    /** Insert function configuration information.
     *
     *  Inserts function configuration information, overwriting any config information that was already present at the same
     *  address or name. Returns true if information was inserted rather than overwritten. */
    bool insertConfiguration(const FunctionConfiguration&);

    /** Insert address configuration information.
     *
     *  Inserts address configuration information, overwriting any config information that was already present at the same
     *  address.  Returns true if information was inserted rather than overwritten. */
    bool insertConfiguration(const AddressConfiguration&);

    /** Basic block comment.
     *
     *  Returns the comment configured for the basic block at the specified address, or an empty string.
     *
     *  See also, @ref comment. */
    std::string basicBlockComment(Address bblockVa) const;

    /** Basic block final instruction address.
     *
     *  Returns the optional final instruction address for a basic block. If no such configuration information is avilable then
     *  nothing is returned. */
    Sawyer::Optional<Address> basicBlockFinalInstructionVa(Address bblockVa) const;

    /** Basic block successor addresses.
     *
     *  Returns the set of basic block successors. This set is only meaningful at the block's final instruction as returned by
     *  @ref basicBlockFinalInstructionVa. */
    std::set<Address> basicBlockSuccessorVas(Address bblockVa) const;

    /** Data block name.
     *
     *  Returns the name configured for a data block at the specified address, or an empty string. */
    std::string dataBlockName(Address dblockVa) const;

    /** Data block comment.
     *
     *  Returns the comment configured for the data block at the specified address, or an empty string.
     *
     *  See also, @ref comment. */
    std::string dataBlockComment(Address dblockVa) const;

    /** Function name.
     *
     *  Returns the configured name for a function at the specified address.
     *
     *  @sa functionDefaultName */
    std::string functionName(Address functionVa) const;

    /** Function default name.
     *
     *  Returns the configured default name for a function at the specified address.
     *
     *  @sa functionName */
    std::string functionDefaultName(Address functionVa) const;

    /** Function comment.
     *
     *  Returns the configured comment for a function at the specified address or having the specified name.  Function
     *  configuration is looked up by address or name-only (i.e., asking for config information for function "main" will not
     *  return anything if "main" has an address in the configuration).
     *
     *  See also, @ref comment.
     *
     * @{ */
    std::string functionComment(Address functionVa) const;
    std::string functionComment(const std::string &functionName) const;
    std::string functionComment(const FunctionPtr&) const;
    /** @} */

    /** Function stack delta.
     *
     *  Returns the configured stack delta for a function at the specified address or having the specified name.  Function
     *  configuration is looked up by address or name-only (i.e., asking for config information for function "main" will not
     *  return anything if "main" has an address in the configuration).
     *
     * @{ */
    Sawyer::Optional<int64_t> functionStackDelta(Address functionVa) const;
    Sawyer::Optional<int64_t> functionStackDelta(const std::string &functionName) const;
    Sawyer::Optional<int64_t> functionStackDelta(const FunctionPtr&) const;
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
    Sawyer::Optional<bool> functionMayReturn(Address functionVa) const;
    Sawyer::Optional<bool> functionMayReturn(const std::string &functionName) const;
    Sawyer::Optional<bool> functionMayReturn(const FunctionPtr&) const;
    /** @} */

    /** Address comment.
     *
     *  Returns the comment associated with the details for the specified address, if any.
     *
     *  See also, @ref comment. */
    std::string addressComment(Address va) const;

    /** Address comment.
     *
     *  Returns the comment for the specified address by search first within the address details, then basic block details,
     *  then data block details, and finally function details. Returns the first non-empty comment that's found, or the empty
     *  string. */
    std::string comment(Address va) const;

    /** Generate a YAML file. */
    void print(std::ostream&) const;
};

} // namespace
} // namespace
} // namespace

std::ostream& operator<<(std::ostream&, const Rose::BinaryAnalysis::Partitioner2::Configuration&);

#endif
#endif
