#include "sage3basic.h"
#include "IntervalSemantics2.h"

namespace rose {
namespace BinaryAnalysis {
namespace InstructionSemantics2 {
namespace IntervalSemantics {

std::ostream&
operator<<(std::ostream &o, const Interval &x)
{
    x.print(o);
    return o;
}

/*******************************************************************************************************************************
 *                                      Interval
 *******************************************************************************************************************************/

std::string
Interval::to_string(uint64_t n)
{
    const char *fmt = NULL;
    if (n<=9) {
        fmt = "%"PRIu64;
    } else if (n<=0xff) {
        fmt = "0x%02"PRIx64;
    } else if (n<=0xffff) {
        fmt = "0x%04"PRIx64;
    } else {
        fmt = "0x%08"PRIx64;
    }
    char buf[64];
    snprintf(buf, sizeof buf, fmt, n);
    return buf;
}

void
Interval::print(std::ostream &o) const
{
    if (empty()) {
        o <<"<empty>";
    } else if (first()==last()) {
        o <<to_string(first());
    } else if (0==first() && 0xff==last()) {
        o <<"byte";
    } else if (0==first() && 0xffff==last()) {
        o <<"word";
    } else if (0==first() && 0xffffffffull==last()) {
        o <<"doubleword";
    } else if (0==first() && 0xffffffffffffffffull==last()) {
        o <<"quadword";
    } else {
        o <<to_string(first()) <<".." <<to_string(last());
    }
}



/*******************************************************************************************************************************
 *                                      Semantic value
 *******************************************************************************************************************************/

Sawyer::Optional<BaseSemantics::SValuePtr>
SValue::createOptionalMerge(const BaseSemantics::SValuePtr &other_, const BaseSemantics::MergerPtr&, SMTSolver*) const {
    // There's no official way to represent BOTTOM
    throw BaseSemantics::NotImplemented("SValue merging for IntervalSemantics is not supported", NULL);
}

// class method
SValuePtr
SValue::instance_from_bits(size_t nbits, uint64_t possible_bits)
{
    static const uint64_t max_complexity = 50; // arbitrary
    Intervals retval;
    possible_bits &= IntegerOps::genMask<uint64_t>(nbits);

    size_t nset = 0, lobit=nbits, hibit=0;
    for (size_t i=0; i<nbits; ++i) {
        if (0 != (possible_bits & IntegerOps::shl1<uint64_t>(i))) {
            ++nset;
            lobit = std::min(lobit, i);
            hibit = std::max(hibit, i);
        }
    }

    if (possible_bits == IntegerOps::genMask<uint64_t>(nset)) {
        // The easy case: all possible bits are grouped together at the low end.
        retval.insert(Interval::inin(0, possible_bits));
    } else {
        // Low-order bit of result must be clear, so the rangemap will have 2^nset ranges
        uint64_t nranges = IntegerOps::shl1<uint64_t>(nset); // 2^nset
        if (nranges>max_complexity) {
            uint64_t hi = IntegerOps::genMask<uint64_t>(hibit+1) ^ IntegerOps::genMask<uint64_t>(lobit);
            retval.insert(Interval::inin(0, hi));
        } else {
            for (uint64_t i=0; i<nranges; ++i) {
                uint64_t lo=0, tmp=i;
                for (uint64_t j=0; j<nbits; ++j) {
                    uint64_t bit = IntegerOps::shl1<uint64_t>(j);
                    if (0 != (possible_bits & bit)) {
                        lo |= (tmp & 1) << j;
                        tmp = tmp >> 1;
                    }
                }
                retval.insert(Interval(lo));
            }
        }
    }
    return SValue::instance(nbits, retval);
}

bool
SValue::may_equal(const BaseSemantics::SValuePtr &other_, SMTSolver *solver) const
{
    SValuePtr other = SValue::promote(other_);
    if (must_equal(other))      // this is faster
        return true;
    return get_intervals().contains(other->get_intervals());
}

bool
SValue::must_equal(const BaseSemantics::SValuePtr &other, SMTSolver *solver) const
{
    return is_number() && other->is_number() && get_number()==other->get_number();
}

uint64_t
SValue::possible_bits() const
{
    uint64_t bits=0;
    for (Intervals::const_iterator ii=p_intervals.begin(); ii!=p_intervals.end(); ++ii) {
        uint64_t lo=ii->first.first(), hi=ii->first.last();
        bits |= lo | hi;
        for (uint64_t bitno=0; bitno<32; ++bitno) {
            uint64_t bit = IntegerOps::shl1<uint64_t>(bitno);
            if (0 == (bits & bit)) {
                uint64_t base = lo & ~IntegerOps::genMask<uint64_t>(bitno);
                if (ii->first.contains(Interval(base+bit)))
                    bits |= bit; 
            }
        }
    }
    return bits;
}



/*******************************************************************************************************************************
 *                                      Memory state
 *******************************************************************************************************************************/

BaseSemantics::SValuePtr
MemoryState::readMemory(const BaseSemantics::SValuePtr &addr, const BaseSemantics::SValuePtr &dflt,
                        BaseSemantics::RiscOperators *addrOps, BaseSemantics::RiscOperators *valOps)
{
    ASSERT_not_implemented("[Robb Matzke 2013-03-14]");
    BaseSemantics::SValuePtr retval;
    return retval;
}

void
MemoryState::writeMemory(const BaseSemantics::SValuePtr &addr, const BaseSemantics::SValuePtr &value,
                         BaseSemantics::RiscOperators *addrOps, BaseSemantics::RiscOperators *valOps)
{
    ASSERT_not_implemented("[Robb Matzke 2013-03-14]");
}

/*******************************************************************************************************************************
 *                                      RISC operators
 *******************************************************************************************************************************/

BaseSemantics::SValuePtr
RiscOperators::and_(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    ASSERT_require(a->get_width()==b->get_width());
    uint64_t ak = a->is_number() ? a->get_number() : 0;
    uint64_t bk = b->is_number() ? b->get_number() : 0;
    uint64_t au = a->is_number() ? 0 : a->possible_bits();
    uint64_t bu = b->is_number() ? 0 : b->possible_bits();
    uint64_t r = ak & bk & au & bu;
    if (au || bu)
        return svalue_from_bits(a->get_width(), r);
    return number_(a->get_width(), r);
}

BaseSemantics::SValuePtr
RiscOperators::or_(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    ASSERT_require(a->get_width()==b->get_width());
    if (a->is_number() && b->is_number()) {
        uint64_t result = a->get_number() | b->get_number();
        return number_(a->get_width(), result);
    }
    uint64_t abits = a->possible_bits(), bbits = b->possible_bits();
    uint64_t rbits = abits | bbits; // bits that could be set in the result
    return svalue_from_bits(a->get_width(), rbits);
}

BaseSemantics::SValuePtr
RiscOperators::xor_(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    ASSERT_require(a->get_width()==b->get_width());
    if (a->is_number() && b->is_number())
        return number_(a->get_width(), a->get_number() ^ b->get_number());
    uint64_t abits = a->possible_bits(), bbits = b->possible_bits();
    uint64_t rbits = abits | bbits; // yes, OR, not XOR
    return svalue_from_bits(a->get_width(), rbits);
}

BaseSemantics::SValuePtr
RiscOperators::invert(const BaseSemantics::SValuePtr &a_)
{
    SValuePtr a = SValue::promote(a_);
    Intervals result;
    uint64_t mask = IntegerOps::genMask<uint64_t>(a->get_width());
    for (Intervals::const_iterator ai=a->get_intervals().begin(); ai!=a->get_intervals().end(); ++ai) {
        const Interval &iv = ai->first;
        uint64_t lo = ~iv.last() & mask;
        uint64_t hi = ~iv.first() & mask;
        result.insert(Interval::inin(lo, hi));
    }
    return svalue_from_intervals(a->get_width(), result);
}

BaseSemantics::SValuePtr
RiscOperators::extract(const BaseSemantics::SValuePtr &a_, size_t begin_bit, size_t end_bit)
{
    using namespace IntegerOps;
    SValuePtr a = SValue::promote(a_);
    ASSERT_require(end_bit<=a->get_width());
    ASSERT_require(begin_bit<end_bit);
    Intervals result;
    uint64_t discard_mask = ~genMask<uint64_t>(end_bit); // hi-order bits being discarded
    uint64_t src_mask = genMask<uint64_t>(a->get_width()); // significant bits in the source operand
    uint64_t dst_mask = genMask<uint64_t>(end_bit-begin_bit); // significant bits in the result
    for (Intervals::const_iterator ai=a->get_intervals().begin(); ai!=a->get_intervals().end(); ++ai) {
        const Interval &iv = ai->first;
        uint64_t d1 = shiftRightLogical2(iv.first() & discard_mask, end_bit, a->get_width()); // discarded part, lo
        uint64_t d2 = shiftRightLogical2(iv.last()  & discard_mask, end_bit, a->get_width()); // discarded part, hi
        uint64_t k1 = shiftRightLogical2(iv.first() & src_mask, begin_bit, a->get_width()) & dst_mask; // keep part, lo
        uint64_t k2 = shiftRightLogical2(iv.last()  & src_mask, begin_bit, a->get_width()) & dst_mask; // keep part, hi
        if (d1==d2) {                   // no overflow in the kept bits
            ASSERT_require(k1<=k2);
            result.insert(Interval::inin(k1, k2));
        } else if (d1+1<d2 || k1<k2) {  // complete overflow
            result.insert(Interval::inin(0, dst_mask));
            break;
        } else {                        // partial overflow
            result.insert(Interval::inin(0, k2));
            result.insert(Interval::inin(k1, dst_mask));
        }
    }
    return svalue_from_intervals(end_bit-begin_bit, result);
}

BaseSemantics::SValuePtr
RiscOperators::concat(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    size_t retsize = a->get_width() + b->get_width();
    Intervals result;
    uint64_t mask_a = IntegerOps::genMask<uint64_t>(a->get_width());
    uint64_t mask_b = IntegerOps::genMask<uint64_t>(b->get_width());
    for (Intervals::const_iterator ai=a->get_intervals().begin(); ai!=a->get_intervals().end(); ++ai) {
        for (Intervals::const_iterator bi=b->get_intervals().begin(); bi!=b->get_intervals().end(); ++bi) {
            uint64_t lo = (IntegerOps::shiftLeft2(bi->first.first() & mask_b, retsize, a->get_width()) |
                           (ai->first.first() & mask_a));
            uint64_t hi = (IntegerOps::shiftLeft2(bi->first.last() & mask_b, retsize, a->get_width()) |
                           (ai->first.last() & mask_a));
            result.insert(Interval::inin(lo, hi));
        }
    }
    return svalue_from_intervals(retsize, result);
}

BaseSemantics::SValuePtr
RiscOperators::leastSignificantSetBit(const BaseSemantics::SValuePtr &a_)
{
    SValuePtr a = SValue::promote(a_);
    size_t nbits = a->get_width();
    if (a->is_number()) {
        uint64_t av = a->get_number();
        if (av) {
            for (size_t i=0; i<nbits; ++i) {
                if (av & IntegerOps::shl1<uint64_t>(i))
                    return number_(nbits, i);
            }
        }
        return number_(nbits, 0);
    }

    uint64_t abits = a->possible_bits();
    ASSERT_require(abits!=0); // or else the value would be known to be zero and handled above
    uint64_t lo=nbits, hi=0;
    for (size_t i=0; i<nbits; ++i) {
        if (abits & IntegerOps::shl1<uint64_t>(i)) {
            lo = std::min(lo, (uint64_t)i);
            hi = std::max(hi, (uint64_t)i);
        }
    }
    Intervals result;
    result.insert(Interval(0));
    result.insert(Interval::inin(lo, hi));
    return svalue_from_intervals(nbits, result);
}

BaseSemantics::SValuePtr
RiscOperators::mostSignificantSetBit(const BaseSemantics::SValuePtr &a_)
{
    SValuePtr a = SValue::promote(a_);
    size_t nbits = a->get_width();
    if (a->is_number()) {
        uint64_t av = a->get_number();
        if (av) {
            for (size_t i=nbits; i>0; --i) {
                if (av && IntegerOps::shl1<uint64_t>((i-1)))
                    return number_(nbits, i-1);
            }
        }
        return number_(nbits, 0);
    }

    uint64_t abits = a->possible_bits();
    ASSERT_require(abits!=0); // or else the value would be known to be zero and handled above
    uint64_t lo=nbits, hi=0;
    for (size_t i=nbits; i>0; --i) {
        if (abits & IntegerOps::shl1<uint64_t>(i-1)) {
            lo = std::min(lo, (uint64_t)i-1);
            hi = std::max(hi, (uint64_t)i-1);
        }
    }
    Intervals result;
    result.insert(Interval(0));
    result.insert(Interval::inin(lo, hi));
    return svalue_from_intervals(nbits, result);
}

BaseSemantics::SValuePtr
RiscOperators::rotateLeft(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    size_t nbitsa = a->get_width();
    if (a->is_number() && b->is_number()) {
        uint64_t bn = b->get_number();
        uint64_t result = ((a->get_number() << bn) & IntegerOps::genMask<uint64_t>(nbitsa)) |
                          ((a->get_number() >> (nbitsa-bn)) & IntegerOps::genMask<uint64_t>(bn));
        return number_(nbitsa, result);
    }
    uint64_t abits = a->possible_bits();
    uint64_t rbits = 0, done = IntegerOps::genMask<uint64_t>(nbitsa);
    for (uint64_t i=0; i<(uint64_t)nbitsa && rbits!=done; ++i) {
        if (b->get_intervals().contains(Interval(i))) {
            rbits |= ((abits << i) & IntegerOps::genMask<uint64_t>(nbitsa)) |
                     ((abits >> (nbitsa-i)) & IntegerOps::genMask<uint64_t>(i));
        }
    }
    return svalue_from_bits(nbitsa, rbits);
}

BaseSemantics::SValuePtr
RiscOperators::rotateRight(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    size_t nbitsa = a->get_width();
    if (a->is_number() && b->is_number()) {
        uint64_t bn = b->get_number();
        uint64_t result = ((a->get_number() >> bn) & IntegerOps::genMask<uint64_t>(nbitsa-bn)) |
                          ((a->get_number() << (nbitsa-bn)) & IntegerOps::genMask<uint64_t>(nbitsa));
        return number_(nbitsa, result);
    }
    uint64_t abits = a->possible_bits();
    uint64_t rbits = 0, done = IntegerOps::genMask<uint64_t>(nbitsa);
    for (uint64_t i=0; i<(uint64_t)nbitsa && rbits!=done; ++i) {
        if (b->get_intervals().contains(Interval(i))) {
            rbits |= ((abits >> i) & IntegerOps::genMask<uint64_t>(nbitsa-i)) |
                     ((abits << (nbitsa-i)) & IntegerOps::genMask<uint64_t>(nbitsa));
        }
    }
    return svalue_from_bits(nbitsa, rbits);

}

BaseSemantics::SValuePtr
RiscOperators::shiftLeft(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    size_t nbitsa = a->get_width();
    if (a->is_number() && b->is_number()) {
        uint64_t result = b->get_number()<nbitsa ? a->get_number() << b->get_number() : (uint64_t)0;
        return number_(nbitsa, result);
    }
    uint64_t abits = a->possible_bits();
    uint64_t rbits = 0, done = IntegerOps::genMask<uint64_t>(nbitsa);
    for (uint64_t i=0; i<(uint64_t)nbitsa && rbits!=done; ++i) {
        if (b->get_intervals().contains(Interval(i)))
            rbits |= (abits << i) & IntegerOps::genMask<uint64_t>(nbitsa);
    }
    return svalue_from_bits(nbitsa, rbits);
}

BaseSemantics::SValuePtr
RiscOperators::shiftRight(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    size_t nbitsa = a->get_width();
    if (b->is_number()) {
        uint64_t bn = b->get_number();
        if (bn >= nbitsa)
            return number_(nbitsa, 0);
        if (a->is_number())
            return number_(nbitsa, a->get_number() >> bn);
        Intervals result;
        for (Intervals::const_iterator ai=a->get_intervals().begin(); ai!=a->get_intervals().end(); ++ai) {
            uint64_t lo = ai->first.first() >> bn;
            uint64_t hi = ai->first.last() >> bn;
            result.insert(Interval::inin(lo, hi));
        }
        return svalue_from_intervals(nbitsa, result);
    }
    Intervals result;
    for (Intervals::const_iterator ai=a->get_intervals().begin(); ai!=a->get_intervals().end(); ++ai) {
        for (uint64_t i=0; i<(uint64_t)nbitsa; ++i) {
            if (b->get_intervals().contains(Interval(i))) {
                uint64_t lo = ai->first.first() >> i;
                uint64_t hi = ai->first.first() >> i;
                result.insert(Interval::inin(lo, hi));
            }
        }
    }
    return svalue_from_intervals(nbitsa, result);
}

BaseSemantics::SValuePtr
RiscOperators::shiftRightArithmetic(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    size_t nbitsa = a->get_width();
    if (a->is_number() && b->is_number()) {
        uint64_t bn = b->get_number();
        uint64_t result = a->get_number() >> bn;
        if (IntegerOps::signBit2(a->get_number(), nbitsa))
            result |= IntegerOps::genMask<uint64_t>(nbitsa) ^ IntegerOps::genMask<uint64_t>(nbitsa-bn);
        return number_(nbitsa, result);
    }
    uint64_t abits = a->possible_bits();
    uint64_t rbits = 0, done = IntegerOps::genMask<uint64_t>(nbitsa);
    for (uint64_t i=0; i<(uint64_t)nbitsa && rbits!=done; ++i) {
        if (b->get_intervals().contains(Interval(i))) {
            rbits |= ((abits >> i) & IntegerOps::genMask<uint64_t>(nbitsa-i)) |
                     (IntegerOps::signBit2(abits, nbitsa) ?
                      done ^ IntegerOps::genMask<uint64_t>(nbitsa-i) : (uint64_t)0);
        }
    }
    return svalue_from_bits(nbitsa, rbits);
}

BaseSemantics::SValuePtr
RiscOperators::equalToZero(const BaseSemantics::SValuePtr &a_)
{
    SValuePtr a = SValue::promote(a_);
    if (a->is_number())
        return 0==a->get_number() ? boolean_(true) : boolean_(false);
    if (!a->get_intervals().contains(Interval(0)))
        return boolean_(false);
    return undefined_(1);
}

BaseSemantics::SValuePtr
RiscOperators::ite(const BaseSemantics::SValuePtr &cond_, const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr cond = SValue::promote(cond_);
    ASSERT_require(1==cond_->get_width());
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    ASSERT_require(a->get_width()==b->get_width());
    if (cond->is_number())
        return cond->get_number() ? a->copy() : b->copy();
    Intervals result = a->get_intervals();
    result.insert_ranges(b->get_intervals());
    return svalue_from_intervals(a->get_width(), result);
}

BaseSemantics::SValuePtr
RiscOperators::unsignedExtend(const BaseSemantics::SValuePtr &a_, size_t new_width)
{
    SValuePtr a = SValue::promote(a_);

    if (a->get_width() == new_width)
        return a->copy();

    if (new_width < a->get_width()) {
        uint64_t lo = IntegerOps::shl1<uint64_t>(new_width);
        uint64_t hi = IntegerOps::genMask<uint64_t>(a->get_width());
        Intervals retval = a->get_intervals();
        retval.erase(Interval::inin(lo, hi));
        return svalue_from_intervals(new_width, retval);
    }

    return svalue_from_intervals(new_width, a->get_intervals());
}

BaseSemantics::SValuePtr
RiscOperators::signExtend(const BaseSemantics::SValuePtr &a_, size_t new_width)
{
    SValuePtr a = SValue::promote(a_);
    uint64_t old_signbit = IntegerOps::shl1<uint64_t>(a->get_width()-1);
    uint64_t new_signbit = IntegerOps::shl1<uint64_t>(new_width-1);
    Intervals result;
    for (Intervals::const_iterator ai=a->get_intervals().begin(); ai!=a->get_intervals().end(); ++ai) {
        uint64_t lo = IntegerOps::signExtend2(ai->first.first(), a->get_width(), new_width);
        uint64_t hi = IntegerOps::signExtend2(ai->first.last(), a->get_width(), new_width);
        if (0==(lo & new_signbit) && 0!=(hi & new_signbit)) {
            result.insert(Interval::inin(lo, IntegerOps::genMask<uint64_t>(a->get_width()-1)));
            result.insert(Interval::inin(old_signbit, hi));
        } else {
            result.insert(Interval::inin(lo, hi));
        }
    }
    return svalue_from_intervals(new_width, result);
}

BaseSemantics::SValuePtr
RiscOperators::add(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    ASSERT_require(a->get_width()==b->get_width());
    size_t nbits = a->get_width();
    const Intervals &aints=a->get_intervals(), &bints=b->get_intervals();
    Intervals result;
    for (Intervals::const_iterator ai=aints.begin(); ai!=aints.end(); ++ai) {
        for (Intervals::const_iterator bi=bints.begin(); bi!=bints.end(); ++bi) {
            uint64_t lo = (ai->first.first() + bi->first.first()) & IntegerOps::genMask<uint64_t>(nbits);
            uint64_t hi = (ai->first.last()  + bi->first.last())  & IntegerOps::genMask<uint64_t>(nbits);
            if (lo < ai->first.first() || lo < bi->first.first()) {
                // lo and hi both overflow
                result.insert(Interval::inin(lo, hi));
            } else if (hi < ai->first.last() || hi < bi->first.last()) {
                // hi overflows, but not lo
                result.insert(Interval::inin(lo, IntegerOps::genMask<uint64_t>(nbits)));
                result.insert(Interval::inin(0, hi));
            } else {
                // no overflow
                result.insert(Interval::inin(lo, hi));
            }
        }
    }
    return svalue_from_intervals(nbits, result);
}

BaseSemantics::SValuePtr
RiscOperators::addWithCarries(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_,
                              const BaseSemantics::SValuePtr &c_, BaseSemantics::SValuePtr &carry_out/*out*/)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    ASSERT_require(a->get_width()==b->get_width());
    size_t nbits = a->get_width();
    SValuePtr c = SValue::promote(c_);
    ASSERT_require(c->get_width()==1);
    SValuePtr wide_carry = SValue::promote(unsignedExtend(c, nbits));
    SValuePtr retval = SValue::promote(add(add(a, b), wide_carry));

    uint64_t known1 = a->is_number() ? a->get_number() : 0;
    uint64_t known2 = b->is_number() ? b->get_number() : 0;
    uint64_t known3 = c->is_number() ? c->get_number() : 0;
    uint64_t unkwn1 = a->is_number() ? 0 : a->possible_bits();
    uint64_t unkwn2 = b->is_number() ? 0 : b->possible_bits();
    uint64_t unkwn3 = c->is_number() ? 0 : c->possible_bits();
    enum Carry { C_FALSE, C_TRUE, C_UNKNOWN };
    Carry cin = C_FALSE; // carry propagated across loop iterations
    uint64_t known_co=0, unkwn_co=0; // known or possible carry-out bits
    for (size_t i=0; i<nbits; ++i, known1>>=1, known2>>=1, known3>>=1, unkwn1>>=1, unkwn2>>=1, unkwn3>>=1) {
        int known_sum = (known1 & 1) + (known2 & 1) + (known3 & 1) + (C_TRUE==cin ? 1 : 0);
        int unkwn_sum = (unkwn1 & 1) + (unkwn2 & 1) + (unkwn3 & 1) + (C_UNKNOWN==cin ? 1 : 0);
        if (known_sum>1) {
            known_co |= IntegerOps::shl1<uint64_t>(i);
            cin = C_TRUE;
        } else if (known_sum + unkwn_sum > 1) {
            unkwn_co |= IntegerOps::shl1<uint64_t>(i);
            cin = C_UNKNOWN;
        }
    }
    if (unkwn_co) {
        carry_out = svalue_from_bits(nbits, known_co | unkwn_co);
    } else {
        carry_out = number_(nbits, known_co);
    }
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::negate(const BaseSemantics::SValuePtr &a_)
{
    SValuePtr a = SValue::promote(a_);
    size_t nbits = a->get_width();
    Intervals result;
    uint64_t mask = IntegerOps::genMask<uint64_t>(nbits);
    for (Intervals::const_iterator ai=a->get_intervals().begin(); ai!=a->get_intervals().end(); ++ai) {
        const Interval &iv = ai->first;
        uint64_t lo = -iv.last() & mask;
        uint64_t hi = -iv.first() & mask;
        if (0==hi) {
            ASSERT_require(0==iv.first());
            result.insert(Interval(0));
            if (0!=lo) {
                ASSERT_require(0!=iv.last());
                result.insert(Interval::inin(lo, IntegerOps::genMask<uint64_t>(nbits)));
            }
        } else {
            ASSERT_require(lo<=hi);
            result.insert(Interval::inin(lo, hi));
        }
    }
    return svalue_from_intervals(nbits, result);
}

BaseSemantics::SValuePtr
RiscOperators::signedDivide(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    size_t nbitsa = a->get_width();
    size_t nbitsb = b->get_width();
    if (!IntegerOps::signBit2(a->possible_bits(), nbitsa) && !IntegerOps::signBit2(b->possible_bits(), nbitsb))
        return unsignedDivide(a, b);
    return undefined_(nbitsa); // FIXME, we can do better
}

BaseSemantics::SValuePtr
RiscOperators::signedModulo(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    size_t nbitsa = a->get_width();
    size_t nbitsb = b->get_width();
    if (!IntegerOps::signBit2(a->possible_bits(), nbitsa) && !IntegerOps::signBit2(b->possible_bits(), nbitsb))
        return unsignedModulo(a, b);
    return undefined_(nbitsb); // FIXME, we can do better
}

BaseSemantics::SValuePtr
RiscOperators::signedMultiply(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    size_t nbitsa = a->get_width();
    size_t nbitsb = b->get_width();
    if (!IntegerOps::signBit2(a->possible_bits(), nbitsa) && !IntegerOps::signBit2(b->possible_bits(), nbitsb))
        return unsignedMultiply(a, b);
    return undefined_(nbitsa+nbitsb); // FIXME, we can do better
}

BaseSemantics::SValuePtr
RiscOperators::unsignedDivide(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    size_t nbitsa = a->get_width();
    Intervals result;
    for (Intervals::const_iterator ai=a->get_intervals().begin(); ai!=a->get_intervals().end(); ++ai) {
        for (Intervals::const_iterator bi=b->get_intervals().begin(); bi!=b->get_intervals().end(); ++bi) {
            uint64_t lo = ai->first.first() / std::max(bi->first.last(),  (uint64_t)1);
            uint64_t hi = ai->first.last()  / std::max(bi->first.first(), (uint64_t)1);
            ASSERT_require((lo<=IntegerOps::genMask<uint64_t>(nbitsa)));
            ASSERT_require((hi<=IntegerOps::genMask<uint64_t>(nbitsa)));
            ASSERT_require(lo<=hi);
            result.insert(Interval::inin(lo, hi));
        }
    }
    return svalue_from_intervals(nbitsa, result);
}

BaseSemantics::SValuePtr
RiscOperators::unsignedModulo(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    size_t nbitsa = a->get_width();
    size_t nbitsb = b->get_width();
    if (b->is_number()) {
        // If B is a power of two then mask away the high bits of A
        uint64_t bn = b->get_number();
        uint64_t limit = IntegerOps::genMask<uint64_t>(nbitsa);
        for (size_t i=0; i<nbitsb; ++i) {
            uint64_t twopow = IntegerOps::shl1<uint64_t>(i);
            if (bn==twopow) {
                Intervals result = a->get_intervals();
                result.erase(Interval::inin(twopow, limit));
                return svalue_from_intervals(nbitsb, result);
            }
        }
    }
    return undefined_(nbitsb); // FIXME: can we do better?
}

BaseSemantics::SValuePtr
RiscOperators::unsignedMultiply(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    Intervals result;
    for (Intervals::const_iterator ai=a->get_intervals().begin(); ai!=a->get_intervals().end(); ++ai) {
        for (Intervals::const_iterator bi=b->get_intervals().begin(); bi!=b->get_intervals().end(); ++bi) {
            uint64_t lo = ai->first.first() * bi->first.first();
            uint64_t hi = ai->first.last()  * bi->first.last();
            ASSERT_require(lo<=hi);
            result.insert(Interval::inin(lo, hi));
        }
    }
    return svalue_from_intervals(a->get_width()+b->get_width(), result);
}

BaseSemantics::SValuePtr
RiscOperators::readMemory(const RegisterDescriptor &segreg,
                          const BaseSemantics::SValuePtr &address,
                          const BaseSemantics::SValuePtr &dflt,
                          const BaseSemantics::SValuePtr &condition)
{
    return dflt->copy(); // FIXME
}

void
RiscOperators::writeMemory(const RegisterDescriptor &segreg,
                           const BaseSemantics::SValuePtr &address,
                           const BaseSemantics::SValuePtr &value,
                           const BaseSemantics::SValuePtr &condition) {
    // FIXME
}

} // namespace
} // namespace
} // namespace
} // namespace
