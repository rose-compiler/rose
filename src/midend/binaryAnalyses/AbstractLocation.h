#ifndef Rose_BinaryAnalysis_AbstractLocation_H
#define Rose_BinaryAnalysis_AbstractLocation_H

namespace rose {
namespace BinaryAnalysis {

/** Abstract location.
 *
 *  An abstract location represents either a register name or memory address. Registers are represented by RegisterDescriptor
 *  and addresses are represented by semantic values (subclasses of BaseSemantics::SValue) specified by the template argument,
 *  which must be a smart pointer to such an object.
 *
 *  For instance, to represent abstract locations where memory addresses have symbolic values, one uses:
 *
 *  @code
 *   using namespace rose::BinaryAnalysis;
 *   typedef AbstractLocation<InstructionSemantics::SymbolicSemantics::SValuePtr> ALoc;
 *   ALoc aloc1(REG_EAX); // REG_EAX is a RegisterDescriptor
 *   ALoc aloc2(addr); // addr is an SValuePtr for the symbolic semantics domain
 *  @endcode
 *
 *  Abstract locations are immutable objects. */
template<class A>
class AbstractLocation {
public:
    typedef A Address;                                  /**< Type of memory address. */
private:
    RegisterDescriptor reg_;
    Address addr_;
public:
    /** Default constructor.
     *
     *  Constructs an abstract location that does not refer to any location.  The @ref isValid method will return true for such
     *  objects. */
    AbstractLocation() {}

    /** Copy constructor. */
    AbstractLocation(const AbstractLocation &other): reg_(other.reg_), addr_(other.addr_) {}

    /** Register referent.
     *
     *  Constructs an abstract location that refers to a register. */
    AbstractLocation(const RegisterDescriptor &reg): reg_(reg) {}

    /** Memory referent.
     *
     *  Constructs an abstract location that refers to a memory location. */
    AbstractLocation(const Address &addr): addr_(addr) {}

    /** Assignment operator. */
    AbstractLocation& operator=(const AbstractLocation &other) {
        reg_ = other.reg_;
        addr_ = other.addr_;
        return *this;
    }

    /** Validity checker.
     *
     *  Returns true if this abstract location refers to either a register or a memory location.  Default constructed abstract
     *  locations refer to neither, as do locations that have both an invalid register descriptor and a null memory address. */
    bool isValid() const { return isRegister() || isMemory(); }

    /** Checks register reference.
     *
     *  Returns true if and only if this abstract location refers to a register. It is impossible for an abstract location to
     *  refer to both a register and memory. */
    bool isRegister() const { return reg_.is_valid(); }

    /** Checks memory reference.
     *
     *  Returns true if and only if this abstract location refers to memory.  It is impossible for an abstract location to
     *  refer to both a register and memory */
    bool isMemory() const { return addr_!=NULL; }

    /** Returns register.
     *
     *  Returns the register to which this abstract location refers.  When called for an abstract location for which @ref
     *  isRegister returns false, the return value is an invalid register descriptor (i.e., one for which
     *  RegisterDescriptor::is_valid returns false. */
    const RegisterDescriptor& getRegister() const { return reg_; }

    /** Returns memory address.
     *
     *  Returns the memory to which this abstract location refers.  When called for an abstract location for which @ref
     *  isMemory returns false, the return value is a null pointer. */
    const Address getMemory() const { return addr_; }

    /** True if two abstract locations could be aliases.
     *
     *  Two abstract locations are aliases if they refer to the exact same register or memory address. By this definition, the
     *  AX and EAX registers in x86 are not aliases because they do not refer to the exact same register (one is 16 bits and
     *  the other is 32 bits, although the low-order 16 bits are aliases).  Likewise, memory addresses are assumed to refer to
     *  8 bit values and two memory addresses are aliases only when they are equal.
     *
     *  Since memory addresses may be symbolic, this function uses an SMT solver to return true if and only if equality of two
     *  addresses is satisfiable. */
    bool mayAlias(const AbstractLocation &other, SMTSolver *solver=NULL) const {
        if (isRegister() && other.isRegister()) {
            return reg_ == other.reg_;
        } else if (isMemory() && other.isMemory()) {
            return addr_->may_equal(other.addr_, solver);
        } else {
            return false;
        }
    }

    /** True if two abstract locations are certainly aliases.
     *
     *  Two abstract locations are aliases if they refer to the exact same register or memory address. By this definition, the
     *  AX and EAX registers in x86 are not aliases because they do not refer to the exact same register (one is 16 bits and
     *  the other is 32 bits, although the low-order 16 bits are aliases).  Likewise, memory addresses are assumed to refer to
     *  8 bit values and two memory addresses are aliases only when they are equal.
     *
     *  Since memory addresses may be symbolic, this function uses an SMT solver to return true if and only if the inequality
     *  of two addresses is unsatisfiable. */
    bool mustAlias(const AbstractLocation &other, SMTSolver *solver=NULL) const {
        if (isRegister() && other.isRegister()) {
            return reg_ == other.reg_;
        } else if (isMemory() && other.isMemory()) {
            return addr_->must_equal(other.addr_, solver);
        } else {
            return false;
        }
    }
};

} // namespace
} // namespace

#endif
