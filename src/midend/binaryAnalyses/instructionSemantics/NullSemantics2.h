#ifndef Rose_NullSemantics_H
#define Rose_NullSemantics_H

#include "x86InstructionSemantics.h"
#include "BaseSemantics2.h"

namespace BinaryAnalysis { // documented elsewhere
namespace InstructionSemantics { // documented elsewhere
        

/** Semantic domain that does nothing, but is well documented.
 *
 *  This semantic domain is useful for testing, as a no-op in a multi-policy, as documentation for the instruction
 *  semantics API (the RISC operations), or for debugging. */
namespace NullSemantics {

/*******************************************************************************************************************************
 *                                      ValueType
 *******************************************************************************************************************************/

/** Smart pointer to a SValue.  SValue objects are reference counted through boost::shared_ptr smart pointers. The
 *  underlying object should never be explicitly deleted. */
typedef boost::shared_ptr<class SValue> SValuePtr;

/** Values in the NullSemantics domain.  Values are essentially void. */
class SValue: public BaseSemantics::SValue {
protected:
    // protected because these are reference counted. See base class.
    explicit SValue(size_t nBits): BaseSemantics::SValue(nBits) {}

public:
    /** Constructor. */
    static SValuePtr instance() {
        return SValuePtr(new SValue(1));
    }

    /** Promote a base value to a NullSemantics value.  The value @p v must have a NullSemantics::SValue dynamic type. */
    static SValuePtr promote(const BaseSemantics::SValuePtr &v) {
        SValuePtr retval = boost::dynamic_pointer_cast<SValue>(v);
        assert(retval!=NULL);
        return retval;
    }

    // Virtual constructors inherited from base class
    virtual BaseSemantics::SValuePtr undefined_(size_t nBits) const /*override*/ {
        return SValuePtr(new SValue(nBits));
    }
    virtual BaseSemantics::SValuePtr number_(size_t nBits, uint64_t number) const /*override*/ {
        return SValuePtr(new SValue(nBits)); // the number is not important in this semantic domain
    }
    virtual BaseSemantics::SValuePtr copy_(size_t new_width=0) const /*override*/ {
        SValuePtr retval(new SValue(*this));
        if (new_width!=0 && new_width!=retval->get_width())
            retval->set_width(new_width);
        return retval;
    }

    virtual bool is_number() const { return false; }
    virtual uint64_t get_number() const { assert(!"not a number"); abort(); }

    virtual bool may_equal(const BaseSemantics::SValuePtr &other, SMTSolver *solver=NULL) const /*override*/ {
        return true;
    }

    virtual bool must_equal(const BaseSemantics::SValuePtr &other, SMTSolver *solver=NULL) const /*override*/ {
        return this == other.get(); // must be equal if they're both the same object
    }

    virtual void print(std::ostream &o, BaseSemantics::PrintHelper *helper=NULL) const /*override*/ {
        o <<"VOID";
    }
};

/*******************************************************************************************************************************
 *                                      RISC Operators
 *******************************************************************************************************************************/

typedef boost::shared_ptr<class RiscOperators> RiscOperatorsPtr;

/** NullSemantics operators always return a new undefined value.  They do, however, check certain preconditions. */
class RiscOperators: public BaseSemantics::RiscOperators {
protected:
    // Same constructors as for the base class
    explicit RiscOperators(const BaseSemantics::SValuePtr &protoval): BaseSemantics::RiscOperators(protoval) {}
    explicit RiscOperators(const BaseSemantics::StatePtr &state): BaseSemantics::RiscOperators(state) {}

public:
    /** Constructor. See the virtual constructor, create(), for details. */
    static RiscOperatorsPtr instance(const BaseSemantics::SValuePtr &protoval) {
        return RiscOperatorsPtr(new RiscOperators(protoval));
    }

    /** Constructor. See the virtual constructor, create(), for details. */
    static RiscOperatorsPtr instance(const BaseSemantics::StatePtr &state) {
        return RiscOperatorsPtr(new RiscOperators(state));
    }

    virtual BaseSemantics::RiscOperatorsPtr create(const BaseSemantics::SValuePtr &protoval) const /*override*/ {
        return instance(protoval);
    }

    virtual BaseSemantics::RiscOperatorsPtr create(const BaseSemantics::StatePtr &state) const /*override*/ {
        return instance(state);
    }

public:
    virtual BaseSemantics::SValuePtr and_(const BaseSemantics::SValuePtr &a_,
                                          const BaseSemantics::SValuePtr &b_) /*override*/ {
        SValuePtr a = SValue::promote(a_);
        SValuePtr b = SValue::promote(b_);
        assert(a->get_width()==b->get_width());
        return undefined_(a->get_width());
    }
    
    virtual BaseSemantics::SValuePtr or_(const BaseSemantics::SValuePtr &a_,
                                         const BaseSemantics::SValuePtr &b_) /*override*/ {
        SValuePtr a = SValue::promote(a_);
        SValuePtr b = SValue::promote(b_);
        assert(a->get_width()==b->get_width());
        return undefined_(a->get_width());
    }

    virtual BaseSemantics::SValuePtr xor_(const BaseSemantics::SValuePtr &a_,
                                          const BaseSemantics::SValuePtr &b_) /*override*/ {
        SValuePtr a = SValue::promote(a_);
        SValuePtr b = SValue::promote(b_);
        assert(a->get_width()==b->get_width());
        return undefined_(a->get_width());
    }

    virtual BaseSemantics::SValuePtr invert(const BaseSemantics::SValuePtr &a_) /*override*/ {
        SValuePtr a = SValue::promote(a_);
        return undefined_(a->get_width());
    }

    virtual BaseSemantics::SValuePtr extract(const BaseSemantics::SValuePtr &a_,
                                             size_t begin_bit, size_t end_bit) /*override*/ {
        SValuePtr a = SValue::promote(a_);
        assert(end_bit<=a->get_width());
        assert(begin_bit<end_bit);
        return undefined_(end_bit-begin_bit);
    }

    virtual BaseSemantics::SValuePtr concat(const BaseSemantics::SValuePtr &a_,
                                            const BaseSemantics::SValuePtr &b_) /*override*/ {
        SValuePtr a = SValue::promote(a_);
        SValuePtr b = SValue::promote(b_);
        return undefined_(a->get_width() + b->get_width());
    }

    virtual BaseSemantics::SValuePtr leastSignificantSetBit(const BaseSemantics::SValuePtr &a_) /*override*/ {
        SValuePtr a = SValue::promote(a_);
        return undefined_(a->get_width());
    }

    virtual BaseSemantics::SValuePtr mostSignificantSetBit(const BaseSemantics::SValuePtr &a_) /*override*/ {
        SValuePtr a = SValue::promote(a_);
        return undefined_(a->get_width());
    }

    virtual BaseSemantics::SValuePtr rotateLeft(const BaseSemantics::SValuePtr &a_,
                                                const BaseSemantics::SValuePtr &sa_) /*override*/ {
        SValuePtr a = SValue::promote(a_);
        SValue::promote(sa_);
        return undefined_(a->get_width());
    }

    virtual BaseSemantics::SValuePtr rotateRight(const BaseSemantics::SValuePtr &a_,
                                                 const BaseSemantics::SValuePtr &sa_) /*override*/ {
        SValuePtr a = SValue::promote(a_);
        SValuePtr sa = SValue::promote(sa_);
        return undefined_(a->get_width());
    }

    virtual BaseSemantics::SValuePtr shiftLeft(const BaseSemantics::SValuePtr &a_,
                                               const BaseSemantics::SValuePtr &sa_) /*override*/ {
        SValuePtr a = SValue::promote(a_);
        SValuePtr sa = SValue::promote(sa_);
        return undefined_(a->get_width());
    }

    virtual BaseSemantics::SValuePtr shiftRight(const BaseSemantics::SValuePtr &a_,
                                                const BaseSemantics::SValuePtr &sa_) /*override*/ {
        SValuePtr a = SValue::promote(a_);
        SValuePtr sa = SValue::promote(sa_);
        return undefined_(a->get_width());
    }

    virtual BaseSemantics::SValuePtr shiftRightArithmetic(const BaseSemantics::SValuePtr &a_,
                                                          const BaseSemantics::SValuePtr &sa_) /*override*/ {
        SValuePtr a = SValue::promote(a_);
        SValuePtr sa = SValue::promote(sa_);
        return undefined_(a->get_width());
    }

    virtual BaseSemantics::SValuePtr equalToZero(const BaseSemantics::SValuePtr &a_) /*override*/ {
        SValue::promote(a_);
        return undefined_(1);
    }

    virtual BaseSemantics::SValuePtr ite(const BaseSemantics::SValuePtr &sel_,
                                         const BaseSemantics::SValuePtr &a_,
                                         const BaseSemantics::SValuePtr &b_) /*override*/ {
        SValuePtr sel = SValue::promote(sel_);
        SValuePtr a = SValue::promote(a_);
        SValuePtr b = SValue::promote(b_);
        assert(1==sel->get_width());
        assert(a->get_width()==b->get_width());
        return undefined_(a->get_width());
    }

    virtual BaseSemantics::SValuePtr signExtend(const BaseSemantics::SValuePtr &a_, size_t new_width) /*override*/ {
        SValuePtr a = SValue::promote(a_);
        if (new_width==a->get_width())
            return a;
        return undefined_(new_width);
    }

    virtual BaseSemantics::SValuePtr add(const BaseSemantics::SValuePtr &a_,
                                         const BaseSemantics::SValuePtr &b_) /*override*/ {
        SValuePtr a = SValue::promote(a_);
        SValuePtr b = SValue::promote(b_);
        assert(a->get_width()==b->get_width());
        return undefined_(a->get_width());
    }

    virtual BaseSemantics::SValuePtr addWithCarries(const BaseSemantics::SValuePtr &a_,
                                                    const BaseSemantics::SValuePtr &b_,
                                                    const BaseSemantics::SValuePtr &c_,
                                                    BaseSemantics::SValuePtr &carry_out/*out*/) /*override*/ {
        SValuePtr a = SValue::promote(a_);
        SValuePtr b = SValue::promote(b_);
        SValuePtr c = SValue::promote(c_);
        assert(a->get_width()==b->get_width() && c->get_width()==1);
        carry_out = undefined_(a->get_width());
        return undefined_(a->get_width());
    }

    virtual BaseSemantics::SValuePtr negate(const BaseSemantics::SValuePtr &a_) /*override*/ {
        SValuePtr a = SValue::promote(a_);
        return undefined_(a->get_width());
    }

    virtual BaseSemantics::SValuePtr signedDivide(const BaseSemantics::SValuePtr &a_,
                                                  const BaseSemantics::SValuePtr &b_) /*override*/ {
        SValuePtr a = SValue::promote(a_);
        SValuePtr b = SValue::promote(b_);
        return undefined_(a->get_width());
    }

    virtual BaseSemantics::SValuePtr signedModulo(const BaseSemantics::SValuePtr &a_,
                                                  const BaseSemantics::SValuePtr &b_) /*override*/ {
        SValuePtr a = SValue::promote(a_);
        SValuePtr b = SValue::promote(b_);
        return undefined_(b->get_width());
    }

    virtual BaseSemantics::SValuePtr signedMultiply(const BaseSemantics::SValuePtr &a_,
                                                    const BaseSemantics::SValuePtr &b_) /*override*/ {
        SValuePtr a = SValue::promote(a_);
        SValuePtr b = SValue::promote(b_);
        size_t retwidth = a->get_width() + b->get_width();
        return undefined_(retwidth);
    }

    virtual BaseSemantics::SValuePtr unsignedDivide(const BaseSemantics::SValuePtr &a_,
                                                    const BaseSemantics::SValuePtr &b_) /*override*/ {
        SValuePtr a = SValue::promote(a_);
        SValuePtr b = SValue::promote(b_);
        return undefined_(a->get_width());
    }

    virtual BaseSemantics::SValuePtr unsignedModulo(const BaseSemantics::SValuePtr &a_,
                                                    const BaseSemantics::SValuePtr &b_) /*override*/ {
        SValuePtr a = SValue::promote(a_);
        SValuePtr b = SValue::promote(b_);
        return undefined_(b->get_width());
    }

    virtual BaseSemantics::SValuePtr unsignedMultiply(const BaseSemantics::SValuePtr &a_,
                                                      const BaseSemantics::SValuePtr &b_) /*override*/ {
        SValuePtr a = SValue::promote(a_);
        SValuePtr b = SValue::promote(b_);
        size_t retwidth = a->get_width() + b->get_width();
        return undefined_(retwidth);
    }

    virtual BaseSemantics::SValuePtr readMemory(X86SegmentRegister sg,
                                                const BaseSemantics::SValuePtr &addr,
                                                const BaseSemantics::SValuePtr &cond,
                                                size_t nbits) /*override*/ {
        assert(get_state()!=NULL);
        assert(8==nbits);
        (void) SValue::promote(addr);
        (void) SValue::promote(cond);
        return undefined_(8);
    }

    virtual void writeMemory(X86SegmentRegister sg,
                             const BaseSemantics::SValuePtr &addr,
                             const BaseSemantics::SValuePtr &data,
                             const BaseSemantics::SValuePtr &cond) /*override*/ {
        assert(get_state()!=NULL);
        (void) SValue::promote(addr);
        (void) SValue::promote(data);
        (void) SValue::promote(cond);
    }
};

} /*namespace*/
} /*namespace*/
} /*namespace*/

#endif
