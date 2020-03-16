#ifndef ROSE_DISASSEMBLER_H
#define ROSE_DISASSEMBLER_H

#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

#include "BinaryCallingConvention.h"
#include "BinaryUnparser.h"
#include "Diagnostics.h"                                // Rose::Diagnostics
#include "MemoryMap.h"
#include "Registers.h"
#include "RoseException.h"
#include "integerOps.h"
#include "Map.h"
#include "BaseSemantics2.h"

#include <boost/serialization/access.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/version.hpp>

// REG_SP possibly defined on __sun
// REG_LINK possibly defined on Windows
#undef REG_SP
#undef REG_LINK

namespace Rose {
namespace BinaryAnalysis {

/** Virtual base class for instruction disassemblers.
 *
 *  The Disassembler class is a virtual class providing all non-architecture-specific functionality for disassembling
 *  instructions; architecture-specific components are in subclasses @ref DisassemblerArm, @ref DisassemblerPowerpc, @ref
 *  DisassemblerX86, and others. In general, there is no need to explicitly instantiate or call functions in any of these
 *  subclasses.  A @ref Disassembler is responsible for disassembling a single instruction at a time at some specified address,
 *  whereas the classes in @ref Rose::BinaryAnalysis::Partitioner2 are responsible for deciding what addresses should be
 *  disassembled.
 *
 *  The main interface to a @ref Disassembler is the @ref disassembleOne method. It tries to disassemble one instruction at the
 *  specified address from a supplied @ref MemoryMap.  A @ref MemoryMap object represents the data in a virtual address
 *  space. On success, it returns an instance of a subclass of @ref SgAsmInstruction, which serves as the root of an abstract
 *  syntax tree (AST) that containins information about the instruction.  The instruction operands are represented by the
 *  subclasses of @ref SgAsmExpression. If an error occurs during the disassembly of a single instruction, the disassembler
 *  will throw an exception.
 *
 *  New architectures can be added to ROSE without modifying any ROSE source code. One does this by subclassing an existing
 *  disassembler, overriding any necessary virtual methods, and registering an instance of the subclass with @ref
 *  register_subclass.  If the new subclass can handle multiple architectures then a disassembler should be registered for each
 *  of those architectures.When ROSE needs to disassemble something, it calls @ref lookup, which in turn calls the @ref
 *  can_disassemble method for all registered disassemblers.  The first disassembler whose @ref can_disassemble returns true is
 *  used for the disassembly. */
class Disassembler {
public:
    /** Exception thrown by the disassemblers. */
    class Exception: public Rose::Exception {
    public:
        /** A bare exception not bound to any particular instruction. */
        Exception(const std::string &reason)
            : Rose::Exception(reason), ip(0), bit(0), insn(NULL)
            {}

        /** An exception bound to a virtual address but no raw data or instruction. */
        Exception(const std::string &reason, rose_addr_t ip)
            : Rose::Exception(reason), ip(ip), bit(0), insn(NULL)
            {}

        /** An exception bound to a particular instruction being disassembled. */
        Exception(const std::string &reason, rose_addr_t ip, const SgUnsignedCharList &raw_data, size_t bit)
            : Rose::Exception(reason), ip(ip), bytes(raw_data), bit(bit), insn(NULL)
            {}

        /** An exception bound to a particular instruction being assembled. */
        Exception(const std::string &reason, SgAsmInstruction *insn)
            : Rose::Exception(reason), ip(insn->get_address()), bit(0), insn(insn)
            {}

        ~Exception() throw() {}

        void print(std::ostream&) const;
        friend std::ostream& operator<<(std::ostream &o, const Exception &e);

        rose_addr_t ip;                 /**< Virtual address where failure occurred; zero if no associated instruction */
        SgUnsignedCharList bytes;       /**< Bytes (partial) of failed disassembly, including byte at failure. Empty if the
                                         *   exception is not associated with a particular byte sequence, such as if an
                                         *   attempt was made to disassemble at an invalid address. */
        size_t bit;                     /**< Bit offset in instruction byte sequence where disassembly failed (bit/8 is the
                                         *   index into the "bytes" list, while bit%8 is the bit within that byte. */
        SgAsmInstruction *insn;         /**< Instruction associated with an assembly error. */
    };

    /** An AddressSet contains virtual addresses (alternatively, relative virtual addresses) for such things as specifying
     *  which virtual addresses should be disassembled. */
    typedef std::set<rose_addr_t> AddressSet;

    /** The InstructionMap is a mapping from (absolute) virtual address to disassembled instruction. */
    typedef Map<rose_addr_t, SgAsmInstruction*> InstructionMap;


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Data members
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
    CallingConvention::Dictionary callingConventions_;

protected:
    const RegisterDictionary *p_registers;              /**< Description of registers available for this platform. */
    RegisterDescriptor REG_IP, REG_SP, REG_SS, REG_SF, REG_LINK; /**< Register descriptors initialized during construction. */
    static std::vector<Disassembler*> disassemblers;    /**< List of disassembler subclasses. */
    ByteOrder::Endianness p_byteOrder;                  /**< Byte order of instructions in memory. */
    size_t p_wordSizeBytes;                             /**< Basic word size in bytes. */
    std::string p_name;                                 /**< Name by which this dissassembler is registered. */

    /** Prototypical dispatcher for creating real dispatchers */
    InstructionSemantics2::BaseSemantics::DispatcherPtr p_proto_dispatcher;

public:
    static Sawyer::Message::Facility mlog;              /**< Disassembler diagnostic streams. */


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Serialization
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned version) {
        s & BOOST_SERIALIZATION_NVP(p_registers);
        s & BOOST_SERIALIZATION_NVP(REG_IP);
        s & BOOST_SERIALIZATION_NVP(REG_SS);
        if (version >= 1)
            s & BOOST_SERIALIZATION_NVP(REG_SF);
        s & BOOST_SERIALIZATION_NVP(p_byteOrder);
        s & BOOST_SERIALIZATION_NVP(p_wordSizeBytes);
        s & BOOST_SERIALIZATION_NVP(p_name);
    }
#endif


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Constructors
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    Disassembler(): p_registers(NULL), p_byteOrder(ByteOrder::ORDER_LSB), p_wordSizeBytes(4) {}
    virtual ~Disassembler() {}


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Registration and lookup methods
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:
    /** Register a disassembler instance. More specific disassembler instances should be registered after more general
     *  disassemblers since the lookup() method will inspect disassemblers in reverse order of their registration.
     *
     *  Thread safety: Multiple threads can register disassemblers simultaneously.  However, one seldom does this because the
     *  order that disassemblers are registered determines which disassembler is returned by the lookup() class methods. */
    static void registerSubclass(Disassembler*);

    /** Predicate determining the suitability of a disassembler for a specific file header.  If this disassembler is capable
     *  of disassembling machine code described by the specified file header, then this predicate returns true, otherwise it
     *  returns false.
     *
     *  Thread safety: The thread safety of this virtual method depends on the implementation in the subclass. */
    virtual bool canDisassemble(SgAsmGenericHeader*) const = 0;

    /** Finds a suitable disassembler. Looks through the list of registered disassembler instances (from most recently
     *  registered to earliest registered) and returns the first one whose can_disassemble() predicate returns true.  Throws
     *  an exception if no suitable disassembler can be found.
     *
     *  Thread safety: Multiple threads can call this class method simultaneously even when other threads are registering
     *  additional disassemblers. */
    static Disassembler *lookup(SgAsmGenericHeader*);

    /** Property: Name by which disassembler is registered.
     *
     * @{ */
    const std::string& name() const {
        return p_name;
    }
    void name(const std::string &s) {
        p_name = s;
    }
    /** @} */

    /** List of names recognized by @ref lookup.
     *
     *  Returns the list of names that the @ref lookup method recognizes. */
    static std::vector<std::string> isaNames();

    /** Finds a suitable disassembler. Looks through the list of registered disassembler instances (from most recently
     *  registered to earliest registered) and returns the first one whose can_disassemble() predicate returns true. This is
     *  done for each header contained in the interpretation and the disassembler for each header must match the other
     *  headers. An exception is thrown if no suitable disassembler can be found.
     *
     *  Thread safety: Multiple threads can call this class method simultaneously even when other threads are registering
     *  additional disassembles. However, no other thread can be changing attributes of the specified interpretation,
     *  particularly the list of file headers referenced by the interpretation. */
    static Disassembler *lookup(SgAsmInterpretation*);

    /** Finds a suitable disassembler.  Looks up a common disassembler by name.  If the name is the word "list" then a
     *  list of known names is printed to <code>std::cout</code>. */
    static Disassembler *lookup(const std::string&);

    /** Creates a new copy of a disassembler. The new copy has all the same settings as the original.
     *
     *  Thread safety: The thread safety of this virtual method depends on the implementation in the subclass. */
    virtual Disassembler *clone() const = 0;


    /***************************************************************************************************************************
     *                                          Disassembler properties and settings
     ***************************************************************************************************************************/
public:
    /** Unparser.
     *
     *  Returns an unparser suitable for unparsing the same instruction set architecture as recognized and produced by this
     *  disassembler. */
    virtual Unparser::BasePtr unparser() const = 0;

    /** Property: Byte order of instructions in memory.
     *
     * @{ */
    ByteOrder::Endianness byteOrder() const { return p_byteOrder; }
    void byteOrder(ByteOrder::Endianness sex) { p_byteOrder = sex; }
    /** @} */

    /** Property: Basic word size in bytes.
     *
     * @{ */
    size_t wordSizeBytes() const { return p_wordSizeBytes; }
    void wordSizeBytes(size_t nbytes) { p_wordSizeBytes = nbytes; }
    /** @} */

    /** Properties: Register dictionary.
     *
     *  Specifies the registers available on this architecture.  Rather than using hard-coded class, number, and position
     *  constants, the disassembler should perform a name lookup in this supplied register dictionary and use the values found
     *  therein. There's usually no need for the user to specify a value because either it will be obtained from an
     *  SgAsmInterpretation or the subclass will initialize it.
     *
     *  Thread safety: It is not safe to change the register dictionary while another thread is using this same Disassembler
     *  object.
     *
     * @{ */
    void registerDictionary(const RegisterDictionary *rdict) {
        p_registers = rdict;
    }
    const RegisterDictionary *registerDictionary() const {
        return p_registers;
    }

    /** Property: Calling convention dictionary.
     *
     *  This is a dictionary of the common calling conventions for this architecture.
     *
     * @{ */
    const CallingConvention::Dictionary& callingConventions() const { return callingConventions_; }
    CallingConvention::Dictionary& callingConventions() { return callingConventions_; }
    void callingConventions(const CallingConvention::Dictionary &d) { callingConventions_ = d; }
    /** @} */

    /** Returns the register that points to instructions. */
    virtual RegisterDescriptor instructionPointerRegister() const {
        ASSERT_forbid(REG_IP.isEmpty());
        return REG_IP;
    }

    /** Returns the register that points to the stack. */
    virtual RegisterDescriptor stackPointerRegister() const {
        ASSERT_forbid(REG_SP.isEmpty());
        return REG_SP;
    }

    /** Returns the register that ponts to the stack frame. */
    virtual RegisterDescriptor stackFrameRegister() const {
        return REG_SF;                                  // need not be valid
    }

    /** Returns the segment register for accessing the stack.  Not all architectures have this register, in which case the
     * default-constructed register descriptor is returned. */
    virtual RegisterDescriptor stackSegmentRegister() const {
        return REG_SS;                                  // need not be valid
    }

    /** Returns the register that holds the return address for a function.
     *
     *  If the architecture doesn't have such a register then a default constructed descriptor is returned. */
    virtual RegisterDescriptor callReturnRegister() const {
        return REG_LINK;                                // need not be valid
    }

    /** Return an instruction semantics dispatcher if possible.
     *
     *  If instruction semantics are implemented for this architecure then return a pointer to a dispatcher. The dispatcher
     *  will have no attached RISC operators and can only be used to create a new dispatcher via its virtual constructor.  If
     *  instruction semantics are not implemented then the null pointer is returned. */
    const Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::DispatcherPtr& dispatcher() const {
        return p_proto_dispatcher;
    }

    /** Initializes and registers disassembler diagnostic streams. See Diagnostics::initialize(). */
    static void initDiagnostics();

    /***************************************************************************************************************************
     *                                          Instruction disassembly functions
     ***************************************************************************************************************************/
public:
    /** This is the lowest level disassembly function and is implemented in the architecture-specific subclasses. It
     *  disassembles one instruction at the specified virtual address. The @p map is a mapping from virtual addresses to
     *  buffer and enables instructions to span file segments that are mapped contiguously in virtual memory by the loader but
     *  which might not be contiguous in the file.  The instruction's successor virtual addresses are added to the optional
     *  successor set (note that successors of an individual instruction can also be obtained via
     *  SgAsmInstruction::getSuccessors). If the instruction cannot be disassembled then an exception is thrown and the
     *  successors set is not modified.
     *
     *  Thread safety:  The safety of this method depends on its implementation in the subclass. In any case, no other thread
     *  can be modifying the MemoryMap or successors set at the same time. */
    virtual SgAsmInstruction *disassembleOne(const MemoryMap::Ptr &map, rose_addr_t start_va, AddressSet *successors=NULL) = 0;

    /** Similar in functionality to the disassembleOne method that takes a MemoryMap argument, except the content buffer is
     *  mapped 1:1 to virtual memory beginning at the specified address.
     *
     *  Thread safety:  The safety of this method depends on the implementation of the disassembleOne() defined in the
     *  subclass. If the subclass is thread safe then this method can be called in multiple threads as long as the supplied
     *  buffer and successors set are not being modified by another thread. */
    SgAsmInstruction *disassembleOne(const unsigned char *buf, rose_addr_t buf_va, size_t buf_size, rose_addr_t start_va,
                                     AddressSet *successors=NULL);


    /***************************************************************************************************************************
     *                                          Miscellaneous methods
     ***************************************************************************************************************************/
public:
    /** Makes an unknown instruction from an exception.
     *
     *  Thread safety: The safety of this method depends on its implementation in the subclass. */
    virtual SgAsmInstruction* makeUnknownInstruction(const Exception&) = 0;

    /** Marks parts of the file that correspond to instructions as having been referenced.
     *
     *  Thread safety: This method is not thread safe. */
    void mark_referenced_instructions(SgAsmInterpretation*, const MemoryMap::Ptr&, const InstructionMap&);

    /** Calculates the successor addresses of a basic block and adds them to a successors set. The successors is always
     *  non-null when called. If the function is able to determine the complete set of successors then it should set @p
     *  complete to true before returning.
     *
     *  Thread safety: Thread safe provided no other thread is modifying the specified instruction map. */
    AddressSet get_block_successors(const InstructionMap&, bool *complete);

private:
    /** Initialize class (e.g., register built-in disassemblers). This class method is thread safe, using class_mutex. */
    static void initclass();
    static void initclassHelper();

    /** Finds the highest-address instruction that contains the byte at the specified virtual address. Returns null if no such
     *  instruction exists.
     *
     *  Thread safety: This class method is thread safe provided no other thread is modifying the instruction map nor the
     *  instructions to which the map points, particularly the instructions' virtual address and raw bytes. */
    static SgAsmInstruction *find_instruction_containing(const InstructionMap &insns, rose_addr_t va);
};

} // namespace
} // namespace

// Class versions must be at global scope
BOOST_CLASS_VERSION(Rose::BinaryAnalysis::Disassembler, 1);

#endif
#endif
