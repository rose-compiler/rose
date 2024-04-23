#ifndef ROSE_BinaryAnalysis_Architecture_Base_H
#define ROSE_BinaryAnalysis_Architecture_Base_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/Architecture/BasicTypes.h>
#include <Rose/BinaryAnalysis/ByteOrder.h>
#include <Rose/BinaryAnalysis/CallingConvention/BasicTypes.h>
#include <Rose/BinaryAnalysis/Disassembler/BasicTypes.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/BasicTypes.h>
#include <Rose/BinaryAnalysis/Unparser/Base.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

/** Base class for architecture definitions. */
class Base: public std::enable_shared_from_this<Base> {
public:
    /** Reference counting pointer. */
    using Ptr = BasePtr;

    /** Reference counting pointer to const object. */
    using ConstPtr = BaseConstPtr;

private:
    std::string name_;                                  // name of architecture
    size_t bytesPerWord_ = 0;
    ByteOrder::Endianness byteOrder_ = ByteOrder::ORDER_UNSPECIFIED;

protected:
    Sawyer::Cached<RegisterDictionaryPtr> registerDictionary_;
    Sawyer::Cached<CallingConvention::Dictionary> callingConventions_;
    Sawyer::Cached<Unparser::Base::Ptr> insnToString_, insnToStringNoAddr_;

protected:
    Base(const std::string &name, size_t bytesPerWord, ByteOrder::Endianness byteOrder);
    virtual ~Base();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Properties
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Property: Architecture definition name.
     *
     *  The name is used for lookups, but it need not be unique since lookups prefer the latest registered architecture. I.e., if
     *  two architectures A, and B, have the same name, and B was registered after A, then lookup by the name will return
     *  architecture B.
     *
     *  A best practice is to use only characters that are not special in shell scripts since architecture names often appear as
     *  arguments to command-line switches. Also, try to use only lower-case letters, decimal digits and hyphens for consistency
     *  across all architecture names. See the list of ROSE built-in architecture names for ideas (this list can be obtained from
     *  many binary analysis tools, or the @ref Architecture::registeredNames function).
     *
     *  Thread safety: Thread safe. The name is specified during construction and is thereafter read-only. */
    const std::string& name() const;

    /** Property: Word size.
     *
     *  This is the natural word size for the architecture, measured in bits or bytes (depending on the property name).
     *
     *  Thread safety: Thread safe. This property is set during construction and is thereafter read-only.
     *
     * @{ */
    size_t bytesPerWord() const;
    size_t bitsPerWord() const;
    /** @} */

    /** Property: Byte order for memory.
     *
     *  When multi-byte values (such as 32-bit integral values) are stored in memory, this property is the order in which the
     *  value's bytes are stored. If the order is little endian, then the least significant byte is stored at the lowest address; if
     *  the order is big endian then the most significant byte is stored at the lowest address.
     *
     *  Thread safety: Thread safe. This property is set during construction and is thereafter read-only. */
    ByteOrder::Endianness byteOrder() const;

    /** Property: Register dictionary.
     *
     *  The register dictionary defines a mapping between register names and register descriptors (@ref RegisterDescriptor), and
     *  thus how the registers map into hardware.
     *
     *  Since dictionaries are generally not modified, it is permissible for this function to return the same dictionary every time
     *  it's called. The dictionary can be constructed on the first call.
     *
     *  Thread safety: Thread safe. */
    virtual RegisterDictionaryPtr registerDictionary() const = 0;

    /** Property: Calling convention definitions.
     *
     *  Returns a list of calling convention definitions used by this architecture. Since definitions are generally not modified,
     *  it is permissible for this function to return the same definitions every time it's called. The list can be constructed on
     *  the first call.
     *
     *  The default implementation returns an empty list.
     *
     *  Thread safety: Thread safe. */
    virtual const CallingConvention::Dictionary& callingConventions() const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Lookup functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Tests whether this architecture matches a name.
     *
     *  Returns true if this architecture matches the specified name, and false otherwise.
     *
     *  The default implementation matches the name exactly, which is what one usually wants. */
    virtual bool matchesName(const std::string&) const;

    /** Tests whether this architecture matches a file header.
     *
     *  Returns true if this architecture matches the specified file header, and false otherwise.
     *
     *  The default implementation always returns false. */
    virtual bool matchesHeader(SgAsmGenericHeader*) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Factories
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Construct and return a new instruction decoder.
     *
     *  Returns a new decoder for this architecture if possible, otherwise a null pointer.
     *
     *  Thread safety: Thread safe. */
    virtual Disassembler::BasePtr newInstructionDecoder() const = 0;

    /** Construct and return a new instruction unparser.
     *
     *  An unparser is responsible for generating pseudo assembly listings.
     *
     *  Thread safety: Thread safe. */
    virtual Unparser::BasePtr newUnparser() const = 0;

    /** Construct and return a new instruction dispatcher.
     *
     * The dispatcher knows the semantics for instructions, but not the low-level operators (arithmetic, memory I/O, etc), nor the
     * domain (concrete, symbolic, etc) on which those operators operate. These other things are supplied by the argument, which
     * also points to the states that are modified by executing the instructions.
     *
     * The default implementation returns a null pointer, signifying that instruction semantics are not known.
     *
     * Thread safety: Thread safe. */
    virtual InstructionSemantics::BaseSemantics::DispatcherPtr
    newInstructionDispatcher(const InstructionSemantics::BaseSemantics::RiscOperatorsPtr&) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Instruction characteristics
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Unparse an instruction to a string.
     *
     *  The returned string is a simple, one-line string with no leading or trailing white space and no line termination. If the
     *  instruction is null, then the word "null" is printed.
     *
     *  Thread safety: Thread safe.
     *
     * @{ */
    virtual std::string toString(const SgAsmInstruction*) const;
    virtual std::string toStringNoAddr(const SgAsmInstruction*) const;
    /** @} */

    /** Description for an instruction.
     *
     *  Returns the description for a particular instruction. The description must be a single line with no leading or trailing
     *  white space, no line termination characters, and no non-printable characters.  Most subclasses will just return a string
     *  based on the instruction mnemonic, such as "push a value onto the stack" for a `PUSH` instruction.  The instruction argument
     *  must not be a null pointer and must be valid for this architecture.
     *
     *  The default implementation returns an empty string.
     *
     *  Thread safety: Thread safe. */
    virtual std::string instructionDescription(const SgAsmInstruction*) const;

    /** Returns true if the instruction is the special "unknown" instruction.
     *
     *  Each instruction architecture in ROSE defines an "unknown" instruction to be used when the disassembler is unable to create
     *  a real instruction.  This can happen, for instance, if the bit pattern does not represent a valid instruction for the
     *  architecture. The instruction must not be a null pointer, and must be valid for this architecture.
     *
     *  Thread safety: Thread safe. */
    virtual bool isUnknown(const SgAsmInstruction*) const = 0;

    /** Determines whether the specified instruction normally terminates a basic block.
     *
     *  The analysis generally only looks at the individual instruction and therefore is not very sophisticated.  For instance, a
     *  conditional branch will always terminate a basic block by this method even if its condition is opaque. The instruction
     *  argument must not be a null pointer and must be valid for this architecture.
     *
     *  Thread safety: Thread safe. */
    virtual bool terminatesBasicBlock(SgAsmInstruction*) const = 0;

    /** Returns true if the specified basic block looks like a function call.
     *
     *  If the basic block looks like a function call then this method returns true.  If (and only if) the target address is known
     *  (i.e., the address of the called function) then @p target is set to this address (otherwise @p target is unmodified). If the
     *  return address is known or can be guessed, then return_va is initialized to the return address, which is normally the
     *  fall-through address of the last instruction; otherwise the return_va is unmodified.
     *
     *  The "fast" and "slow" versions differ only in what kind of anlysis they do.  The "fast" version typically looks only at
     *  instruction patterns while the slow version might incur more expense by looking at instruction semantics.
     *
     *  The base implementation of the fast method always returns false. The base implementation of the slow method just calls the
     *  fast method.
     *
     *  Thread safety: Thread safe.
     *
     * @{ */
    virtual bool isFunctionCallFast(const std::vector<SgAsmInstruction*>&, rose_addr_t *target, rose_addr_t *ret) const;
    virtual bool isFunctionCallSlow(const std::vector<SgAsmInstruction*>&, rose_addr_t *target, rose_addr_t *ret) const;
    /** @} */

    /** Returns true if the specified basic block looks like a function return.
     *
     *  The "fast" and "slow" versions differ only in what kind of anlysis they do.  The "fast" version typically looks only at
     *  instruction patterns while the slow version might incur more expense by looking at instruction semantics.
     *
     *  The base implementaiton of the fast method always returns false. The base implementation of the slow method just calls the
     *  fast method.
     *
     *  Thread safety: Thread safe.
     *
     * @{ */
    virtual bool isFunctionReturnFast(const std::vector<SgAsmInstruction*>&) const;
    virtual bool isFunctionReturnSlow(const std::vector<SgAsmInstruction*>&) const;
    /** @} */

    /** Obtains the virtual address for a branching instruction.
     *
     *  Returns the branch target address if the specified instruction is a branching instruction and the target is known; otherwise
     *  returns nothing.
     *
     *  The default implementation returns nothing.
     *
     *  Thread safety: Thread safe. */
    virtual Sawyer::Optional<rose_addr_t> branchTarget(SgAsmInstruction*) const;

    /** Control flow successors for a single instruction.
     *
     *  The return value does not consider neighboring instructions, and therefore is quite naive.  It returns only the information
     *  it can glean from this single instruction.  If the returned set of virtual instructions is fully known then the @p complete
     *  argument will be set to true, otherwise false. The instruction must not be null, and must be valid for this architecture.
     *
     *  The default implementation always returns an empty set and clears @p complete.
     *
     *  Thread safety: Thread saafe. */
    virtual AddressSet getSuccessors(SgAsmInstruction*, bool &complete) const;

    /** Control flow successors for a basic block.
     *
     *  The @p basicBlock argument is a vector of instructions that is assumed to be a basic block that is entered only at the first
     *  instruction and exits only at the last instruction.  A memory map can supply initial values for the analysis' memory state.
     *  The return value is a set of control flow successor virtual addresses, and the @p complete argument return value indicates
     *  whether the returned set is known to be complete (aside from interrupts, faults, etc).
     *
     *  The default implementation calls the single-instruction version, so architecture-specific subclasses might want to override
     *  this to do something more sophisticated. However, if the basic block is empty then this function instead returns an empty
     *  set and sets @p complete to true.
     *
     *  Thread safety: Thread safe.
     *
     *  @{ */
    AddressSet getSuccessors(const std::vector<SgAsmInstruction*> &basicBlock, bool &complete) const;
    virtual AddressSet getSuccessors(const std::vector<SgAsmInstruction*> &basicBlock, bool &complete,
                                     const MemoryMapPtr &initial_memory) const;
    /** @} */

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Architecture-specific stuff for a partitioning engine.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Instruction patterns matching function prologues.
     *
     * Returns a list of matchers that match sequences of instructions that are often generated by compilers as part of instruction
     * prologues.
     *
     * The default implementation returns an empty list. */
    virtual std::vector<Partitioner2::FunctionPrologueMatcherPtr>
    functionPrologueMatchers(const Partitioner2::EnginePtr&) const;

    /** Architecture-specific basic block callbacks for partitioning.
     *
     *  Returns a list of basic block callbacks used by the partitioner during disassembly.
     *
     *  The default implementation returns an empty list. */
    virtual std::vector<Partitioner2::BasicBlockCallbackPtr>
    basicBlockCreationHooks(const Partitioner2::EnginePtr&) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Miscellaneous
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
protected:
    Ptr ptr();
    ConstPtr constPtr() const;
};

} // namespace
} // namespace
} // namespace

#endif
#endif
