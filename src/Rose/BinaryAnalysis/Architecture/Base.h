#ifndef ROSE_BinaryAnalysis_Architecture_Base_H
#define ROSE_BinaryAnalysis_Architecture_Base_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/Architecture/BasicTypes.h>
#include <Rose/BinaryAnalysis/CallingConvention.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/BasicTypes.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

/** Base class for architecture definitions. */
class Base {
public:
    using Ptr = BasePtr;

private:
    std::string name_;                                  // name of architecture
    Disassembler::BasePtr instructionDecoderFactory_;
    CallingConvention::Dictionary callingConventions_;

protected:
    Base();
    virtual ~Base();

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
     *  Thread safety: Not thread safe.  It is assumed that an architecture is given a name when it's being defined, and then the
     *  name is not changed once the architecture is registered with the ROSE library.
     *
     * @{ */
    const std::string& name() const;
    void name(const std::string&);
    /** @} */

    /** Property: Instruction decoder factory..
     *
     *  The instruction decoder is responsible for decoding a machine instruction from memory into an internal representation (@ref
     *  SgAsmInstruction) for a single instruction at a time.
     *
     *  This property points to an architecture decoder factory, not an actual decoder (though they both have the same type). When
     *  an actual decoder is needed, the factory's @ref Disassembler::Base::clone "clone" method is called. For convenence, see the
     *  @ref newInstructionDecoder method.
     *
     *  Thread safety: Not thread safe. It is assumed that an architecture is given an instruction decoder when it's being defined
     *  and then the decoder is not changed once the architecture is registered with the ROSE library.
     *
     * @{ */
    virtual const Disassembler::BasePtr& instructionDecoderFactory() const;
    virtual void instructionDecoderFactory(const Disassembler::BasePtr&);
    /** @} */

    /** Return a new instruction decoder.
     *
     *  The instruction decoder is obtained from the @ref instructionDecoderFactory by calling its @ref Disassembler::Base::clone
     *  "clone" method method. This function throws a @ref NotFound exception if there is no instruction decoder. */
    virtual Disassembler::BasePtr newInstructionDecoder() const;

    /** Property: Register dictionary.
     *
     *  The register dictionary defines a mapping between register names and register descriptors (@ref RegisterDescriptor), and
     *  thus how the registers map into hardware.
     *
     *  This property comes from the instruction decoder factory ans is read-only. A @ref NotFound exception is thrown if the @ref
     *  instructionDecoderFactory property is null.
     *
     *  Thread safety: Not thread safe. It is assumed that an architecture is given a register dictionary when it's being defined,
     *  and then the register dictionary is not changed once the architecture is registered with the ROSE library. */
    virtual RegisterDictionaryPtr registerDictionary() const;

    /** Property: Word size.
     *
     *  This is the natural word size for the architecture, measured in bits or bytes (depending on the property name).
     *
     *  This property comes from the instruction decoder factory and is read-only. A @ref NotFound exception is thrown if the @ref
     *  instructionDecoderFactory property is null.
     *
     *  Thread safety: Not thread safe. It is assumed that this property is given a value when the architecture is being defined and
     *  then this value is not changed once the architecture is registered with the ROSE library.
     *
     * @{ */
    virtual size_t wordSizeBytes() const;
    virtual size_t wordSizeBits() const;
    /** @} */

    /** Property: Byte order for memory.
     *
     *  When multi-byte values (such as 32-bit integral values) are stored in memory, this property is the order in which the
     *  value's bytes are stored. If the order is little endian, then the least significant byte is stored at the lowest address; if
     *  the order is big endian then the most significant byte is stored at the lowest address.
     *
     *  This property comes from the instruction decoder factory ans is read-only. A @ref NotFound exception is thrown if the @ref
     *  instructionDecoderFactory property is null.
     *
     *  Thread safety: Not thread safe. It is assumed that this property is given a value when the architecture is being defined and
     *  then this value is not changed once the architecture is registered with the ROSE library. */
    virtual ByteOrder::Endianness byteOrder() const;

    /** Property: Instruction alignment.
     *
     *  The alignment that's required for instruction addresses. The return value is a positive number of bytes.
     *
     *  This property comes from the instruction decoder factory ans is read-only. A @ref NotFound exception is thrown if the @ref
     *  instructionDecoderFactory property is null.
     *
     *  Thread safety: Not thread safe. It is assumed that this property is given a value when the architecture is being defined and
     *  then this value is not changed once the architecture is registered with the ROSE library. */
    virtual size_t instructionAlignment() const;

    /** Property: Returns the register that points to instructions.
     *
     *  This function will return a valid register descriptor since all architectures need to be able to point to instructions.
     *
     *  This property comes from the instruction decoder factory ans is read-only. A @ref NotFound exception is thrown if the @ref
     *  instructionDecoderFactory property is null.
     *
     *  Thread safety: Not thread safe. It is assumed that this property is given a value when the architecture is being defined and
     *  then this value is not changed once the architecture is registered with the ROSE library. */
    virtual RegisterDescriptor instructionPointerRegister() const;

    /** Returns the register that points to the stack.
     *
     *  This function will return a valid register descriptor if the architecture has such a register.
     *
     *  This property comes from the instruction decoder factory ans is read-only. A @ref NotFound exception is thrown if the @ref
     *  instructionDecoderFactory property is null.
     *
     *  Thread safety: Not thread safe. It is assumed that this property is given a value when the architecture is being defined and
     *  then this value is not changed once the architecture is registered with the ROSE library. */
    virtual RegisterDescriptor stackPointerRegister() const;

    /** Returns the register that ponts to the stack frame.
     *
     *  This function will return a valid register descriptor if the architecture has such a register, otherwise it returns an empty
     *  descriptor.
     *
     *  This property comes from the instruction decoder factory ans is read-only. A @ref NotFound exception is thrown if the @ref
     *  instructionDecoderFactory property is null.
     *
     *  Thread safety: Not thread safe. It is assumed that this property is given a value when the architecture is being defined and
     *  then this value is not changed once the architecture is registered with the ROSE library. */
    virtual RegisterDescriptor stackFrameRegister() const;

    /** Returns the segment register for accessing the stack.
     *
     *  This function will return a valid register descriptor if the architecture has such a register, otherwise it returns an empty
     *  descriptor.
     *
     *  This property comes from the instruction decoder factory ans is read-only. A @ref NotFound exception is thrown if the @ref
     *  instructionDecoderFactory property is null.
     *
     *  Thread safety: Not thread safe. It is assumed that this property is given a value when the architecture is being defined and
     *  then this value is not changed once the architecture is registered with the ROSE library. */
    virtual RegisterDescriptor stackSegmentRegister() const;

    /** Returns the register that holds the return address for a function.
     *
     *  This function will return a valid register descriptor if the architecture has such a register, otherwise it returns an empty
     *  descriptor.
     *
     *  This property comes from the instruction decoder factory ans is read-only. A @ref NotFound exception is thrown if the @ref
     *  instructionDecoderFactory property is null.
     *
     *  Thread safety: Not thread safe. It is assumed that this property is given a value when the architecture is being defined and
     *  then this value is not changed once the architecture is registered with the ROSE library. */
    virtual RegisterDescriptor callReturnRegister() const;

    /** Construct instruction dispatcher for specified domain.
     *
     *  Constructs a new instruction dispatcher for instruction semantics using the specified domain. The domain defines the type of
     *  values stored in machine states and operated on by the instruction semantics. Examples are concrete domains whose primitive
     *  bit values are 0 or 1; and symbolic domains whose primitive bit values are 0, 1, or a variable; interval domains; taint
     *  domains; user defined domains, etc.
     *
     *  Returns a pointer to a new instruction dispatcher if this architecture has instruction semantics, or a null pointer if this
     *  architecture has no semantics.  Even if a non-null pointer is returned, the object may not be able to handle all
     *  instructions of the architecture.
     *
     *  Thread safety: The implementation must be thread safe. */
    virtual InstructionSemantics::BaseSemantics::DispatcherPtr
    newInstructionDispatcher(const InstructionSemantics::BaseSemantics::RiscOperatorsPtr&) const;

    /** Property: Calling convention definitions.
     *
     *  This is a list of calling convention definitions for this architecture.
     *
     *  Thread safety: Not thread safe. It is assumed that this property is given a value when the architecture is being defined and
     *  then this value is not changed once the architecture is registered with the ROSE library.
     *
     * @{ */
    const CallingConvention::Dictionary& callingConventions() const;
    CallingConvention::Dictionary& callingConventions();
    void callingConventions(const CallingConvention::Dictionary&);
    /** @} */

};

} // namespace
} // namespace
} // namespace

#endif
#endif
