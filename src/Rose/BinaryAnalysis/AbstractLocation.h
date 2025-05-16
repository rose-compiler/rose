#ifndef ROSE_BinaryAnalysis_AbstractLocation_H
#define ROSE_BinaryAnalysis_AbstractLocation_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/Location.h>
#include <Rose/BinaryAnalysis/BasicTypes.h>
#include <Rose/BinaryAnalysis/SmtSolver.h>

#include <ostream>
#include <string>

namespace Rose {
namespace BinaryAnalysis {

/** Abstract location.
 *
 *  An abstract location represents either a register name or memory address. Registers are represented by RegisterDescriptor
 *  and addresses are represented by semantic values (subclasses of BaseSemantics::SValue).
 *
 *  For instance, to represent abstract locations where memory addresses have symbolic values, one uses:
 *
 *  @code
 *   using namespace Rose::BinaryAnalysis;
 *   AbstractLocation aloc1(REG_EAX); // REG_EAX is a RegisterDescriptor
 *   AbstractLocation aloc2(addr); // addr is an SValuePtr for the symbolic semantics domain
 *  @endcode
 *
 *  Abstract locations are immutable objects.
 *
 *  See also, @ref ConcreteLocation. */
class AbstractLocation: public Location {
public:
    using Address = InstructionSemantics::BaseSemantics::SValuePtr; /**< Type of memory address. */

private:
    RegisterDescriptor reg_;
    Address addr_;
    size_t nBytes_ = 0;                                 // size of memory location, or zero if unknown
    RegisterDictionaryPtr regdict_;                     // not hashed or compared; only used to generate string output

public:
    /** Default constructor.
     *
     *  Constructs an abstract location that does not refer to any location.  The @ref isEmpty method will return true for such
     *  objects. */
    AbstractLocation();
    virtual ~AbstractLocation();

    /** Copy constructor. */
    AbstractLocation(const AbstractLocation&);

    /** Assignment operator. */
    AbstractLocation& operator=(const AbstractLocation&);

    /** Register referent.
     *
     *  Constructs an abstract location that refers to a register.
     *
     * @{ */
    explicit AbstractLocation(RegisterDescriptor);
    explicit AbstractLocation(RegisterDescriptor, const RegisterDictionaryPtr &regdict);
    /** @} */

    /** Memory referent.
     *
     *  Constructs an abstract location that refers to a memory location. */
    explicit AbstractLocation(const Address&, size_t nBytes = 0);

    /** Parse an abstract location from a string. */
    static AbstractLocation parse(const std::string&);

    virtual bool isValid() const override;
    virtual uint64_t hash() const override;
    virtual std::string toString() const override;
    virtual void print(std::ostream&) const override;
    virtual std::string printableName() const override;
    virtual bool isEqual(const Location&) const override;
    virtual bool operator<(const Location&) const override;
    virtual bool operator<=(const Location&) const override;
    virtual bool operator>(const Location&) const override;
    virtual bool operator>=(const Location&) const override;

    /** Compare two abstract locations.
     *
     *  Returns -1 if this location is less than @p other, 1 if this location is greater than @p other, or zero if this
     *  location is equal to @p other. */
    int compare(const AbstractLocation &other) const;

    /** Checks register reference.
     *
     *  Returns true if and only if this abstract location refers to a register. It is impossible for an abstract location to
     *  refer to both a register and memory. */
    bool isRegister() const;

    /** Checks memory reference.
     *
     *  Returns true if and only if this abstract location refers to memory.  It is impossible for an abstract location to
     *  refer to both a register and memory */
    bool isAddress() const;

    /** Returns register.
     *
     *  Returns the register to which this abstract location refers.  When called for an abstract location for which @ref
     *  isRegister returns false, the return value is an invalid register descriptor (i.e., one for which
     *  RegisterDescriptor::is_valid returns false. */
    RegisterDescriptor getRegister() const;

    /** Returns memory address.
     *
     *  Returns the memory address to which this abstract location refers.  When called for an abstract location for which @ref
     *  isAddress returns false, the return value is a null pointer. */
    const Address getAddress() const;

    /** Returns size of memory location in bytes.
     *
     *  Returns the size of the memory location in bytes if known, otherwise zero.  It is not valid to call this for an
     *  abstract location that points to a register since registers are not always a multiple of the byte size. */
    size_t nBytes() const;

    /** True if two abstract locations could be aliases.
     *
     *  Two abstract locations are aliases if they refer to the exact same register or memory address. By this definition, the
     *  AX and EAX registers in x86 are not aliases because they do not refer to the exact same register (one is 16 bits and
     *  the other is 32 bits, although the low-order 16 bits are aliases).  Likewise, memory addresses are assumed to refer to
     *  8 bit values and two memory addresses are aliases only when they are equal.
     *
     *  Since memory addresses may be symbolic, this function uses an SMT solver to return true if and only if equality of two
     *  addresses is satisfiable. */
    bool mayAlias(const AbstractLocation &other, const SmtSolverPtr &solver = SmtSolverPtr()) const;

    /** True if two abstract locations are certainly aliases.
     *
     *  Two abstract locations are aliases if they refer to the exact same register or memory address. By this definition, the
     *  AX and EAX registers in x86 are not aliases because they do not refer to the exact same register (one is 16 bits and
     *  the other is 32 bits, although the low-order 16 bits are aliases).  Likewise, memory addresses are assumed to refer to
     *  8 bit values and two memory addresses are aliases only when they are equal.
     *
     *  Since memory addresses may be symbolic, this function uses an SMT solver to return true if and only if the inequality
     *  of two addresses is unsatisfiable. */
    bool mustAlias(const AbstractLocation &other, const SmtSolverPtr &solver = SmtSolverPtr()) const;

    /** Print an abstract location.
     *
     *  The optional register dictionary is used for register names, the optional formatter is used for memory address
     *  expressions.
     *
     *  @{ */
    void print(std::ostream &out, const RegisterDictionaryPtr &regdict) const;
    void print(std::ostream &out, InstructionSemantics::BaseSemantics::Formatter &fmt) const;
    void print(std::ostream &out, const RegisterDictionaryPtr &regdict, InstructionSemantics::BaseSemantics::Formatter &fmt) const;
    /** @} */
};

} // namespace
} // namespace

#endif
#endif
