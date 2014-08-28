#ifndef Rose_BinaryAnalysis_AbstractLocation_H
#define Rose_BinaryAnalysis_AbstractLocation_H

#include "BaseSemantics2.h"

namespace rose {
namespace BinaryAnalysis {

/** Abstract location.
 *
 *  An abstract location represents either a register name or memory address. Registers are represented by RegisterDescriptor
 *  and addresses are represented by semantic values (subclasses of BaseSemantics::SValue).
 *
 *  For instance, to represent abstract locations where memory addresses have symbolic values, one uses:
 *
 *  @code
 *   using namespace rose::BinaryAnalysis;
 *   AbstractLocation aloc1(REG_EAX); // REG_EAX is a RegisterDescriptor
 *   AbstractLocation aloc2(addr); // addr is an SValuePtr for the symbolic semantics domain
 *  @endcode
 *
 *  Abstract locations are immutable objects. */
class AbstractLocation {
public:
    typedef InstructionSemantics2::BaseSemantics::SValuePtr Address; /**< Type of memory address. */

private:
    RegisterDescriptor reg_;
    Address addr_;
    const RegisterDictionary *regdict_;

public:
    /** Default constructor.
     *
     *  Constructs an abstract location that does not refer to any location.  The @ref isValid method will return true for such
     *  objects. */
    AbstractLocation(): regdict_(NULL) {}

    /** Copy constructor. */
    AbstractLocation(const AbstractLocation &other): reg_(other.reg_), addr_(other.addr_), regdict_(other.regdict_) {}

    /** Register referent.
     *
     *  Constructs an abstract location that refers to a register. */
    explicit AbstractLocation(const RegisterDescriptor &reg, const RegisterDictionary *regdict=NULL)
        : reg_(reg), regdict_(regdict) {}

    /** Memory referent.
     *
     *  Constructs an abstract location that refers to a memory location. */
    explicit AbstractLocation(const Address &addr): addr_(addr), regdict_(NULL) {}

    /** Assignment operator. */
    AbstractLocation& operator=(const AbstractLocation &other) {
        reg_ = other.reg_;
        addr_ = other.addr_;
        regdict_ = other.regdict_;
        return *this;
    }

    /** Validity checker.
     *
     *  Returns true if this abstract location refers to either a register or a memory location.  Default constructed abstract
     *  locations refer to neither, as do locations that have both an invalid register descriptor and a null memory address. */
    bool isValid() const { return isRegister() || isAddress(); }

    /** Checks register reference.
     *
     *  Returns true if and only if this abstract location refers to a register. It is impossible for an abstract location to
     *  refer to both a register and memory. */
    bool isRegister() const { return reg_.is_valid(); }

    /** Checks memory reference.
     *
     *  Returns true if and only if this abstract location refers to memory.  It is impossible for an abstract location to
     *  refer to both a register and memory */
    bool isAddress() const { return addr_!=NULL; }

    /** Returns register.
     *
     *  Returns the register to which this abstract location refers.  When called for an abstract location for which @ref
     *  isRegister returns false, the return value is an invalid register descriptor (i.e., one for which
     *  RegisterDescriptor::is_valid returns false. */
    const RegisterDescriptor& getRegister() const { return reg_; }

    /** Returns memory address.
     *
     *  Returns the memory address to which this abstract location refers.  When called for an abstract location for which @ref
     *  isAddress returns false, the return value is a null pointer. */
    const Address getAddress() const { return addr_; }

    /** True if two abstract locations could be aliases.
     *
     *  Two abstract locations are aliases if they refer to the exact same register or memory address. By this definition, the
     *  AX and EAX registers in x86 are not aliases because they do not refer to the exact same register (one is 16 bits and
     *  the other is 32 bits, although the low-order 16 bits are aliases).  Likewise, memory addresses are assumed to refer to
     *  8 bit values and two memory addresses are aliases only when they are equal.
     *
     *  Since memory addresses may be symbolic, this function uses an SMT solver to return true if and only if equality of two
     *  addresses is satisfiable. */
    bool mayAlias(const AbstractLocation &other, SMTSolver *solver=NULL) const;

    /** True if two abstract locations are certainly aliases.
     *
     *  Two abstract locations are aliases if they refer to the exact same register or memory address. By this definition, the
     *  AX and EAX registers in x86 are not aliases because they do not refer to the exact same register (one is 16 bits and
     *  the other is 32 bits, although the low-order 16 bits are aliases).  Likewise, memory addresses are assumed to refer to
     *  8 bit values and two memory addresses are aliases only when they are equal.
     *
     *  Since memory addresses may be symbolic, this function uses an SMT solver to return true if and only if the inequality
     *  of two addresses is unsatisfiable. */
    bool mustAlias(const AbstractLocation &other, SMTSolver *solver=NULL) const;

    /** Print an abstract location.
     *
     *  The optional register dictionary is used for register names, the optional formatter is used for memory address
     *  expressions.
     *
     *  @{ */
    void print(std::ostream &out, const RegisterDictionary *regdict=NULL) const {
        InstructionSemantics2::BaseSemantics::Formatter fmt;
        print(out, regdict, fmt);
    }
    void print(std::ostream &out, InstructionSemantics2::BaseSemantics::Formatter &fmt) const {
        print(out, NULL, fmt);
    }
    void print(std::ostream &out, const RegisterDictionary *regdict, InstructionSemantics2::BaseSemantics::Formatter &fmt) const;
    /** @} */
};

std::ostream& operator<<(std::ostream&, const AbstractLocation&);

} // namespace
} // namespace

#endif
