#ifndef ROSE_BinaryAnalysis_InstructionSemantics_BaseSemantics_AddressSpace_H
#define ROSE_BinaryAnalysis_InstructionSemantics_BaseSemantics_AddressSpace_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/BasicTypes.h>

#include <Rose/BinaryAnalysis/Address.h>
#include <Rose/BinaryAnalysis/RegisterDescriptor.h>
#include <Combinatorics.h>                              // ROSE

#include <boost/enable_shared_from_this.hpp>

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/shared_ptr.hpp>
#endif

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics {
namespace BaseSemantics {

/** Addresses for base address space.
 *
 *  Address spaces have many different types of addresses that fall into the following categories:
 *
 *  @li A @ref RegisterDescriptor is used as an address for some register address spaces.
 *  @li Concrete addresses are often used for address spaces that have no aliasing, such as those that store intermediate results.
 *  @li Abstract semantic values (@ref BaseSemantics::SValue) are common for memory address spaces.
 *
 *  This class encapsulates address types and is usually used as an argument for functions that operate on addresses. */
class AddressSpaceAddress {
private:
    RegisterDescriptor reg_;
    Sawyer::Optional<Address> concrete_;
    SValuePtr abstract_;

public:
    ~AddressSpaceAddress();

    /** Construct a null address. */
    AddressSpaceAddress();

    /** Construct an address that's a concrete register. */
    AddressSpaceAddress(RegisterDescriptor);            // implicit

    /** Construct an address that's a concrete value. */
    AddressSpaceAddress(Address);                       // implicit

    /** Construct an address that's an abstract value. */
    AddressSpaceAddress(const SValuePtr&);              // implicit

    /** True if the address is null. */
    bool isEmpty() const;

    /** Return the underlying address.
     *
     *  Since the return value can be used in a Boolean context, this function can also serve as a predicate to test whether the
     *  address is in the specified form.
     *
     * @{ */
    RegisterDescriptor registerDescriptor() const;
    const Sawyer::Optional<Address>& concrete() const;
    SValuePtr abstract() const;
    /** @} */

    /** Convert an address to a concrete value if possible.
     *
     *  If the address is already concrete, then that value is returned. If the address is a register descriptor, then the
     *  descriptor is converted to a byte offset. If the address is abstract then we try to convert it to an unsigned concrete
     *  value. */
    Sawyer::Optional<Address> toConcrete() const;

    /** Convert an address to an abstract value if possible.
     *
     *  If the address is already abstract, then that value is returned. If the address is a register descriptor, then the
     *  descriptor is converted to a byte offset. If the address is concrete then an abstract address is created from it. If the
     *  address is empty or cannot be converted to an abstract value then null is returned.
     *
     *  The supplied address operators are used to perform any necessary arithmetic and construction of the return value. The
     *  returned value, if not null, is 64 bits in width by unsigned extension or truncation. */
    SValuePtr toAbstract(RiscOperators &addrOps) const;
};


/** Base class for address spaces.
 *
 *  An address space stores information about memory, registers, or other kinds of states that have a mapping from an address to a
 *  value. A semantic state typically has multiple address spaces. */
class AddressSpace: public boost::enable_shared_from_this<AddressSpace> {
public:
    /** Shared-ownership pointer. */
    using Ptr = AddressSpacePtr;

    /** Purpose for the address space. */
    using Purpose = AddressSpacePurpose;

private:
    Purpose purpose_;
    std::string name_;

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_NVP(purpose_);
        s & BOOST_SERIALIZATION_NVP(name_);
    }
#endif

public:
    virtual ~AddressSpace();
protected:
    AddressSpace();                                     // for de-serialization
    AddressSpace(Purpose, const std::string &name);
    AddressSpace(const AddressSpace&);
    AddressSpace& operator=(const AddressSpace&) = delete;

public:
    /** Deep-copy of this address space. */
    virtual Ptr clone() const = 0;

public:
    /** Property: Purpose of this address space.
     *
     *  Every address space has a purpose to describe what kind of data it stores. It is permissible for a semantic state to have
     *  multiple address spaces that have the same value for this property but have slightly different purposes. For instance, the
     *  state might have two @ref Purpose `MEMORY` address spaces where one is for the processor's main address bus and another is
     *  used internally to store temporary values.
     *
     *  Thread safety: It is safe to have multiple threads reading the purpose as long as none are modifying the purpose.
     *
     * @{ */
    Purpose purpose() const;
    void purpose(Purpose);
    /** @} */

    /** Property: Name for this address space.
     *
     *  Every address space has a name (possibly an empty string) that is used in diagnostic messages and sometimes used to look
     *  up an address space within a semantic state.
     *
     *  Thread safety: It is safe for multiple threads to read the name as long as none are modifying the name.
     *
     * @{ */
    const std::string& name() const;
    void name(const std::string&);
    /** @} */

    /** Remove all values.
     *
     *  All address and value pairs are removed from the address space, making the space completely empty. */
    virtual void clear() = 0;

    /** Read a value from the address space.
     *
     *  Reading a value may have side effects. For instance, reading from a certain address that hasn't previously been defined may
     *  instantiate a new value and save it at that address so that a subsequent read from the same address can return the same
     *  value.  The provided default is used for this purpose. */
    virtual SValuePtr read(const AddressSpaceAddress&, const SValuePtr &dflt, RiscOperators &addrOps, RiscOperators &valOps);

    /** Read without causing side effects.
     *
     *  Attempts to read a value from the specified address of this address space. If the space does not hold a value for that address
     *  then the specified default is returned. The address space is not modified by this operation. */
    virtual SValuePtr peek(const AddressSpaceAddress&, const SValuePtr &dflt, RiscOperators &addrOps, RiscOperators &valOps);

    /** Write a value to an address space. */
    virtual void write(const AddressSpaceAddress&, const SValuePtr &value, RiscOperators &addrOps, RiscOperators &valOps);

    /** Hash this address space.
     *
     *  Hashes this address space by appending its data to the specified hasher. The goal is to identify when two address spaces are
     *  the "same" and allow certain kinds of analyses to take shortcuts. The address space purpose and name are not included in the
     *  hash. Separate RISC operators are passed for the addresses and values of this address space and are used if the hashing
     *  requires any operations on the addresses or values. */
    virtual void hash(Combinatorics::Hasher&, RiscOperators *addrOps, RiscOperators *valOps) const = 0;

    /** Merge address spaces for data flow analysis.
     *
     *  Merges the @p other address space into this address space, returning true if this space changed. */
    virtual bool merge(const AddressSpacePtr &other, RiscOperators *addrOps, RiscOperators *valOps) = 0;

    /** Printable name for this address space.
     *
     *  Returns a string with the name of the address space. If the address space name is a single word then the word is printed
     *  without quotes. Otherwise quotes and appropriate C-style escapes are included. */
    std::string printableName() const;

    /** Print an address space.
     *
     *  The output most likely occupies more than one line and each emitted line will be terminated with a line feed.
     *
     * @{ */
    void print(std::ostream&, const std::string &prefix = "") const;
    virtual void print(std::ostream&, Formatter&) const = 0;
    /** @} */

    /** Address space with formatter. See with_format(). */
    class WithFormatter {
        AddressSpacePtr obj;
        Formatter &fmt;
    public:
        ~WithFormatter();
        WithFormatter() = delete;
        WithFormatter(const AddressSpacePtr&, Formatter&);
        void print(std::ostream&) const;
    };

    /** Used for printing address spaces with formatting. The usual way to use this is:
     *
     * @code
     *  AddressSpace::Ptr obj = ...;
     *  Formatter fmt = ...;
     *  std::cout <<"The value is: " <<(*obj+fmt) <<"\n";
     * @endcode
     *
     * Since specifying a line prefix string for indentation purposes is such a common use case, the indentation can be given
     * instead of a format, as in the following code that indents each line of the expression with four spaces.
     *
     * @code
     *  std::cout <<"Memory state:\n" <<(*obj + "    ");
     * @endcode
     * @{ */
    WithFormatter with_format(Formatter&);
    WithFormatter operator+(Formatter&);
    WithFormatter operator+(const std::string &linePrefix);
    /** @} */
};

std::ostream& operator<<(std::ostream&, const AddressSpace&);
std::ostream& operator<<(std::ostream&, const AddressSpace::WithFormatter&);

} // namespace
} // namespace
} // namespace
} // namespace

#endif
#endif
