#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/InstructionSemantics/IntervalSemantics.h>

#include <Rose/As.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/Merger.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/RegisterStateGeneric.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/State.h>
#include <Rose/BinaryAnalysis/RegisterDictionary.h>
#include <integerOps.h>                                 // rose

#include <Sawyer/BitVector.h>
#include <boost/lexical_cast.hpp>

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics {
namespace IntervalSemantics {

static const size_t maxComplexity = 50;                 // arbitrary max intervals in IntervalSet

/*******************************************************************************************************************************
 *                                      Semantic value
 *******************************************************************************************************************************/

SValue::SValue(size_t nbits)
    : BaseSemantics::SValue(nbits), isBottom_(false) {
    intervals_.insert(Interval::hull(0, IntegerOps::genMask<uint64_t>(nbits)));
}

SValue::SValue(size_t nbits, uint64_t number)
    : BaseSemantics::SValue(nbits), isBottom_(false) {
    number &= IntegerOps::genMask<uint64_t>(nbits);
    intervals_.insert(number);
}

SValue::SValue(size_t nbits, uint64_t v1, uint64_t v2):
    BaseSemantics::SValue(nbits), isBottom_(false) {
    v1 &= IntegerOps::genMask<uint64_t>(nbits);
    v2 &= IntegerOps::genMask<uint64_t>(nbits);
    ASSERT_require(v1<=v2);
    intervals_.insert(Interval::hull(v1, v2));
}

SValue::SValue(size_t nbits, const Intervals &intervals):
    BaseSemantics::SValue(nbits), isBottom_(false) {
    ASSERT_require(!intervals.isEmpty());
    ASSERT_require((intervals.greatest() <= IntegerOps::genMask<uint64_t>(nbits)));
    intervals_ = intervals;
}

SValue::Ptr
SValue::instance() {
    return SValuePtr(new SValue(1));
}

SValue::Ptr
SValue::instance_bottom(size_t nbits) {
    SValue *self = new SValue(nbits);
    self->isBottom_ = true;
    return SValuePtr(self);
}

SValue::Ptr
SValue::instance_undefined(size_t nbits) {
    return SValuePtr(new SValue(nbits));
}

SValue::Ptr
SValue::instance_unspecified(size_t nbits) {
    return SValuePtr(new SValue(nbits));
}

SValue::Ptr
SValue::instance_integer(size_t nbits, uint64_t number) {
    return SValuePtr(new SValue(nbits, number));
}

SValue::Ptr
SValue::instance_intervals(size_t nbits, const Intervals &intervals) {
    return SValuePtr(new SValue(nbits, intervals));
}

SValue::Ptr
SValue::instance_hull(size_t nbits, uint64_t v1, uint64_t v2) {
    return SValuePtr(new SValue(nbits, v1, v2));
}

SValue::Ptr
SValue::instance_copy(const SValuePtr &other) {
    return SValuePtr(new SValue(*other));
}

SValue::Ptr
SValue::promote(const BaseSemantics::SValue::Ptr &v) {
    SValuePtr retval = as<SValue>(v);
    ASSERT_not_null(retval);
    return retval;
}

BaseSemantics::SValue::Ptr
SValue::bottom_(size_t nbits) const {
    return instance_bottom(nbits);
}

BaseSemantics::SValue::Ptr
SValue::undefined_(size_t nbits) const {
    return instance_undefined(nbits);
}

BaseSemantics::SValue::Ptr
SValue::unspecified_(size_t nbits) const {
    return instance_unspecified(nbits);
}

BaseSemantics::SValue::Ptr
SValue::number_(size_t nbits, uint64_t number) const {
    return instance_integer(nbits, number);
}

BaseSemantics::SValue::Ptr
SValue::copy(size_t new_width) const {
    SValuePtr retval(new SValue(*this));
    if (new_width!=0 && new_width!=retval->nBits())
        retval->set_width(new_width);
    return retval;
}

Sawyer::Optional<BaseSemantics::SValue::Ptr>
SValue::createOptionalMerge(const BaseSemantics::SValue::Ptr &other_, const BaseSemantics::Merger::Ptr&,
                            const SmtSolverPtr&) const {
    SValue::Ptr other = SValue::promote(other_);
    ASSERT_require(nBits() == other->nBits());
    BaseSemantics::SValue::Ptr retval;

    if (isBottom())
        return Sawyer::Nothing();                       // no change
    if (other->isBottom())
        return bottom_(nBits());

    Intervals newIntervals = intervals_;
    for (const Interval &interval: other->intervals_.intervals())
        newIntervals.insert(interval);

    if (newIntervals == other->intervals_)
        return Sawyer::Nothing();                       // no change

    if (newIntervals.nIntervals() > maxComplexity) {
        retval = instance_hull(nBits(), newIntervals.least(), newIntervals.greatest());
        return retval;
    }

    retval = instance_intervals(nBits(), newIntervals);
    return retval;
}

// class method
SValue::Ptr
SValue::instance_from_bits(size_t nbits, uint64_t possible_bits)
{
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
        retval.insert(Interval::hull(0, possible_bits));
    } else {
        // Low-order bit of result must be clear, so the rangemap will have 2^nset ranges
        uint64_t nranges = IntegerOps::shl1<uint64_t>(nset); // 2^nset
        if (nranges>maxComplexity) {
            uint64_t hi = IntegerOps::genMask<uint64_t>(hibit+1) ^ IntegerOps::genMask<uint64_t>(lobit);
            retval.insert(Interval::hull(0, hi));
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
                retval.insert(lo);
            }
        }
    }
    return SValue::instance_intervals(nbits, retval);
}

SValue::Ptr
SValue::create(size_t nbits, uint64_t v1, uint64_t v2) {
    return instance_hull(nbits, v1, v2);
}

SValue::Ptr
SValue::create(size_t nbits, const Intervals &intervals) {
    return instance_intervals(nbits, intervals);
}

SValue::Ptr
SValue::create_from_bits(size_t nbits, uint64_t possible_bits) {
    return instance_from_bits(nbits, possible_bits);
}

bool
SValue::may_equal(const BaseSemantics::SValue::Ptr &other_, const SmtSolverPtr&) const
{
    SValue::Ptr other = SValue::promote(other_);
    if (nBits() != other->nBits())
        return false;
    if (isBottom() || other->isBottom())
        return true;
    if (mustEqual(other))      // this is faster
        return true;
    return get_intervals().contains(other->get_intervals());
}

bool
SValue::must_equal(const BaseSemantics::SValue::Ptr &other_, const SmtSolverPtr&) const
{
    SValue::Ptr other = SValue::promote(other_);
    if (nBits() != other->nBits())
        return false;
    if (isBottom() || other->isBottom())
        return false;
    auto thisU = this->toUnsigned();
    auto otherU = other->toUnsigned();
    return thisU && otherU && *thisU == *otherU;
}

uint64_t
SValue::possible_bits() const
{
    uint64_t bits = 0;
    for (const Interval &interval: intervals_.intervals()) {
        uint64_t lo = interval.least(), hi = interval.greatest();
        bits |= lo | hi;
        for (uint64_t bitno=0; bitno<nBits(); ++bitno) {
            uint64_t bit = IntegerOps::shl1<uint64_t>(bitno);
            if (0 == (bits & bit)) {
                uint64_t base = lo & ~IntegerOps::genMask<uint64_t>(bitno);
                if (interval.contains(base+bit))
                    bits |= bit; 
            }
        }
    }
    return bits;
}

void
SValue::hash(Combinatorics::Hasher &hasher) const {
    hasher.insert(nBits());
    for (const Interval &interval: intervals_.intervals()) {
        ASSERT_forbid(interval.isEmpty());
        hasher.insert(interval.least());
        hasher.insert(interval.greatest());
    }
}

bool
SValue::isBottom() const {
    return isBottom_;
}

static std::string
toString(uint64_t n, size_t nbits) {
    if (n <= 9) {
        return boost::lexical_cast<std::string>(n);
    } else {
        Sawyer::Container::BitVector bv(nbits);
        bv.fromInteger(n);
        return "0x" + bv.toHex();
    }
}

void
SValue::print(std::ostream &output, BaseSemantics::Formatter&) const {
    Intervals::Scalar maxValue = IntegerOps::genMask<Intervals::Scalar>(nBits());
    if (isBottom()) {
        output <<"bottom";
    } else if (intervals_.nIntervals() == 1 && intervals_.hull() == Interval::hull(0, maxValue)) {
        output <<"any";
    } else {
        output <<"{";
        for (const Interval &interval: intervals_.intervals()) {
            if (interval.least() != intervals_.hull().least())
                output <<", ";
            output <<IntervalSemantics::toString(interval.least(), nBits());
            if (!interval.isSingleton())
                output <<".." <<IntervalSemantics::toString(interval.greatest(), nBits());
        }
        output <<"}";
    }
    output <<"[" <<nBits() <<"]";
}

bool
SValue::is_number() const {
    return 1 == intervals_.size();
}

uint64_t
SValue::get_number() const {
    ASSERT_require(1==intervals_.size());
    return intervals_.least();
}

const Intervals&
SValue::get_intervals() const {
    return intervals_;
}

void
SValue::set_intervals(const Intervals &intervals) {
    intervals_ = intervals;
}

/*******************************************************************************************************************************
 *                                      Memory state
 *******************************************************************************************************************************/

MemoryState::MemoryState(const BaseSemantics::MemoryCell::Ptr &protocell)
    : BaseSemantics::MemoryCellList(protocell) {}

MemoryState::MemoryState(const BaseSemantics::SValue::Ptr &addrProtoval, const BaseSemantics::SValue::Ptr &valProtoval)
    : BaseSemantics::MemoryCellList(addrProtoval, valProtoval) {}

MemoryState::MemoryState(const MemoryState &other)
    : BaseSemantics::MemoryCellList(other) {}

MemoryState::Ptr
MemoryState::instance(const BaseSemantics::MemoryCell::Ptr &protocell) {
    return MemoryState::Ptr(new MemoryState(protocell));
}

MemoryState::Ptr
MemoryState::instance(const BaseSemantics::SValue::Ptr &addrProtoval, const BaseSemantics::SValue::Ptr &valProtoval) {
    return MemoryStatePtr(new MemoryState(addrProtoval, valProtoval));
}

BaseSemantics::MemoryState::Ptr
MemoryState::create(const BaseSemantics::MemoryCell::Ptr &protocell) const {
    return instance(protocell);
}

BaseSemantics::MemoryState::Ptr
MemoryState::create(const BaseSemantics::SValue::Ptr &addrProtoval, const BaseSemantics::SValue::Ptr &valProtoval) const {
    return instance(addrProtoval, valProtoval);
}

BaseSemantics::AddressSpace::Ptr
MemoryState::clone() const {
    return MemoryState::Ptr(new MemoryState(*this));
}

BaseSemantics::SValue::Ptr
MemoryState::readMemory(const BaseSemantics::SValue::Ptr &/*addr*/, const BaseSemantics::SValue::Ptr &/*dflt*/,
                        BaseSemantics::RiscOperators */*addrOps*/, BaseSemantics::RiscOperators */*valOps*/)
{
    ASSERT_not_implemented("[Robb Matzke 2013-03-14]");
    BaseSemantics::SValue::Ptr retval;
    return retval;
}

BaseSemantics::SValue::Ptr
MemoryState::peekMemory(const BaseSemantics::SValue::Ptr &/*addr*/, const BaseSemantics::SValue::Ptr &/*dflt*/,
                        BaseSemantics::RiscOperators */*addrOps*/, BaseSemantics::RiscOperators */*valOps*/)
{
    ASSERT_not_implemented("[Robb Matzke 2018-01-17]");
    BaseSemantics::SValue::Ptr retval;
    return retval;
}

void
MemoryState::writeMemory(const BaseSemantics::SValue::Ptr &/*addr*/, const BaseSemantics::SValue::Ptr &/*value*/,
                         BaseSemantics::RiscOperators */*addrOps*/, BaseSemantics::RiscOperators */*valOps*/)
{
    ASSERT_not_implemented("[Robb Matzke 2013-03-14]");
}

/*******************************************************************************************************************************
 *                                      RISC operators
 *******************************************************************************************************************************/

RiscOperators::RiscOperators(const BaseSemantics::SValue::Ptr &protoval, const SmtSolverPtr &solver)
    : BaseSemantics::RiscOperators(protoval, solver) {
    name("Interval");
    (void) SValue::promote(protoval); // make sure its dynamic type is an IntervalSemantics::SValue or subclass thereof
}

RiscOperators::RiscOperators(const BaseSemantics::State::Ptr &state, const SmtSolver::Ptr &solver)
    : BaseSemantics::RiscOperators(state, solver) {
    name("Interval");
    (void) SValue::promote(state->protoval());      // dynamic type must be IntervalSemantics::SValue or subclass thereof
}

RiscOperators::Ptr
RiscOperators::instanceFromRegisters(const RegisterDictionary::Ptr &regdict, const SmtSolver::Ptr &solver) {
    BaseSemantics::SValue::Ptr protoval = SValue::instance();
    BaseSemantics::RegisterState::Ptr registers = RegisterState::instance(protoval, regdict);
    BaseSemantics::MemoryState::Ptr memory = MemoryState::instance(protoval, protoval);
    BaseSemantics::State::Ptr state = State::instance(registers, memory);
    return Ptr(new RiscOperators(state, solver));
}

RiscOperators::Ptr
RiscOperators::instanceFromProtoval(const BaseSemantics::SValue::Ptr &protoval, const SmtSolver::Ptr &solver) {
    return Ptr(new RiscOperators(protoval, solver));
}

RiscOperators::Ptr
RiscOperators::instanceFromState(const BaseSemantics::State::Ptr &state, const SmtSolver::Ptr &solver) {
    return Ptr(new RiscOperators(state, solver));
}

BaseSemantics::RiscOperators::Ptr
RiscOperators::create(const BaseSemantics::SValue::Ptr &protoval, const SmtSolver::Ptr &solver) const {
    return instanceFromProtoval(protoval, solver);
}

BaseSemantics::RiscOperators::Ptr
RiscOperators::create(const BaseSemantics::State::Ptr &state, const SmtSolver::Ptr &solver) const {
    return instanceFromState(state, solver);
}

RiscOperators::Ptr
RiscOperators::promote(const BaseSemantics::RiscOperators::Ptr &x) {
    Ptr retval = as<RiscOperators>(x);
    ASSERT_not_null(retval);
    return retval;
}

SValue::Ptr
RiscOperators::svalue_from_bits(size_t nbits, uint64_t possible_bits) {
    return SValue::promote(protoval())->create_from_bits(nbits, possible_bits);
}

SValue::Ptr
RiscOperators::svalue_from_intervals(size_t nbits, const Intervals &intervals) {
    return SValue::promote(protoval())->create(nbits, intervals);
}

BaseSemantics::SValue::Ptr
RiscOperators::and_(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &b_)
{
    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr b = SValue::promote(b_);
    ASSERT_require(a->nBits()==b->nBits());
    if (a->isBottom() || b->isBottom())
        return bottom_(a->nBits());
    uint64_t ak = a->toUnsigned().orElse(0);
    uint64_t bk = b->toUnsigned().orElse(0);
    uint64_t au = a->isConcrete() ? 0 : a->possible_bits();
    uint64_t bu = b->isConcrete() ? 0 : b->possible_bits();
    uint64_t r = ak & bk & au & bu;
    if (au || bu)
        return svalue_from_bits(a->nBits(), r);
    return number_(a->nBits(), r);
}

BaseSemantics::SValue::Ptr
RiscOperators::or_(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &b_)
{
    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr b = SValue::promote(b_);
    ASSERT_require(a->nBits()==b->nBits());
    if (a->isBottom() || b->isBottom())
        return bottom_(a->nBits());
    auto aNum = a->toUnsigned();
    auto bNum = b->toUnsigned();
    if (aNum && bNum) {
        uint64_t result = *aNum | *bNum;
        return number_(a->nBits(), result);
    }
    uint64_t abits = a->possible_bits(), bbits = b->possible_bits();
    uint64_t rbits = abits | bbits; // bits that could be set in the result
    return svalue_from_bits(a->nBits(), rbits);
}

BaseSemantics::SValue::Ptr
RiscOperators::xor_(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &b_)
{
    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr b = SValue::promote(b_);
    ASSERT_require(a->nBits()==b->nBits());
    if (a->isBottom() || b->isBottom())
        return bottom_(a->nBits());
    auto aNum = a->toUnsigned();
    auto bNum = b->toUnsigned();
    if (aNum && bNum)
        return number_(a->nBits(), *aNum ^ *bNum);
    uint64_t abits = a->possible_bits(), bbits = b->possible_bits();
    uint64_t rbits = abits | bbits; // yes, OR, not XOR
    return svalue_from_bits(a->nBits(), rbits);
}

BaseSemantics::SValue::Ptr
RiscOperators::invert(const BaseSemantics::SValue::Ptr &a_)
{
    SValue::Ptr a = SValue::promote(a_);
    if (a->isBottom())
        return bottom_(a->nBits());
    Intervals result;
    uint64_t mask = IntegerOps::genMask<uint64_t>(a->nBits());
    for (const Interval &interval: a->get_intervals().intervals()) {
        uint64_t lo = ~interval.greatest() & mask;
        uint64_t hi = ~interval.least() & mask;
        result.insert(Interval::hull(lo, hi));
    }
    return svalue_from_intervals(a->nBits(), result);
}

BaseSemantics::SValue::Ptr
RiscOperators::extract(const BaseSemantics::SValue::Ptr &a_, size_t begin_bit, size_t end_bit)
{
    using namespace IntegerOps;
    SValue::Ptr a = SValue::promote(a_);
    ASSERT_require(end_bit<=a->nBits());
    ASSERT_require(begin_bit<end_bit);
    if (a->isBottom())
        return bottom_(a->nBits());
    Intervals result;
    uint64_t discard_mask = ~genMask<uint64_t>(end_bit); // hi-order bits being discarded
    uint64_t src_mask = genMask<uint64_t>(a->nBits()); // significant bits in the source operand
    uint64_t dst_mask = genMask<uint64_t>(end_bit-begin_bit); // significant bits in the result
    for (const Interval &iv: a->get_intervals().intervals()) {
        uint64_t d1 = shiftRightLogical2(iv.least()     & discard_mask, end_bit, a->nBits()); // discarded part, lo
        uint64_t d2 = shiftRightLogical2(iv.greatest()  & discard_mask, end_bit, a->nBits()); // discarded part, hi
        uint64_t k1 = shiftRightLogical2(iv.least()     & src_mask, begin_bit, a->nBits()) & dst_mask; // keep part, lo
        uint64_t k2 = shiftRightLogical2(iv.greatest()  & src_mask, begin_bit, a->nBits()) & dst_mask; // keep part, hi
        if (d1==d2) {                   // no overflow in the kept bits
            ASSERT_require(k1<=k2);
            result.insert(Interval::hull(k1, k2));
        } else if (d1+1<d2 || k1<k2) {  // complete overflow
            result.insert(Interval::hull(0, dst_mask));
            break;
        } else {                        // partial overflow
            result.insert(Interval::hull(0, k2));
            result.insert(Interval::hull(k1, dst_mask));
        }
    }
    return svalue_from_intervals(end_bit-begin_bit, result);
}

BaseSemantics::SValue::Ptr
RiscOperators::concat(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &b_)
{
    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr b = SValue::promote(b_);
    size_t retsize = a->nBits() + b->nBits();
    if (a->isBottom() || b->isBottom())
        return bottom_(a->nBits() + b->nBits());
    Intervals result;
    uint64_t mask_a = IntegerOps::genMask<uint64_t>(a->nBits());
    uint64_t mask_b = IntegerOps::genMask<uint64_t>(b->nBits());
    for (const Interval &av: a->get_intervals().intervals()) {
        for (const Interval &bv: b->get_intervals().intervals()) {
            uint64_t lo = (IntegerOps::shiftLeft2(bv.least() & mask_b, retsize, a->nBits()) |
                           (av.least() & mask_a));
            uint64_t hi = (IntegerOps::shiftLeft2(bv.greatest() & mask_b, retsize, a->nBits()) |
                           (av.greatest() & mask_a));
            result.insert(Interval::hull(lo, hi));
        }
    }
    return svalue_from_intervals(retsize, result);
}

BaseSemantics::SValue::Ptr
RiscOperators::leastSignificantSetBit(const BaseSemantics::SValue::Ptr &a_)
{
    SValue::Ptr a = SValue::promote(a_);
    size_t nbits = a->nBits();
    if (a->isBottom())
        return bottom_(nbits);
    if (auto av = a->toUnsigned()) {
        if (*av) {
            for (size_t i=0; i<nbits; ++i) {
                if (*av & IntegerOps::shl1<uint64_t>(i))
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
    result.insert(Interval::hull(lo, hi));
    return svalue_from_intervals(nbits, result);
}

BaseSemantics::SValue::Ptr
RiscOperators::mostSignificantSetBit(const BaseSemantics::SValue::Ptr &a_)
{
    SValue::Ptr a = SValue::promote(a_);
    size_t nbits = a->nBits();
    if (a->isBottom())
        return bottom_(nbits);
    if (auto av = a->toUnsigned()) {
        if (*av) {
            for (size_t i=nbits; i>0; --i) {
                if (*av && IntegerOps::shl1<uint64_t>((i-1)))
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
    result.insert(Interval::hull(lo, hi));
    return svalue_from_intervals(nbits, result);
}

BaseSemantics::SValue::Ptr
RiscOperators::rotateLeft(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &b_)
{
    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr b = SValue::promote(b_);
    size_t nbitsa = a->nBits();
    if (a->isBottom() || b->isBottom())
        return bottom_(nbitsa);
    auto aNum = a->toUnsigned();
    auto bNum = b->toUnsigned();
    if (aNum && bNum) {
        uint64_t result = ((*aNum << *bNum) & IntegerOps::genMask<uint64_t>(nbitsa)) |
                          ((*aNum >> (nbitsa - *bNum)) & IntegerOps::genMask<uint64_t>(*bNum));
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

BaseSemantics::SValue::Ptr
RiscOperators::rotateRight(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &b_)
{
    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr b = SValue::promote(b_);
    size_t nbitsa = a->nBits();
    if (a->isBottom() || b->isBottom())
        return bottom_(nbitsa);
    auto aNum = a->toUnsigned();
    auto bNum = b->toUnsigned();
    if (aNum && bNum) {
        uint64_t result = ((*aNum >> *bNum) & IntegerOps::genMask<uint64_t>(nbitsa - *bNum)) |
                          ((*aNum << (nbitsa - *bNum)) & IntegerOps::genMask<uint64_t>(nbitsa));
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

BaseSemantics::SValue::Ptr
RiscOperators::shiftLeft(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &b_)
{
    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr b = SValue::promote(b_);
    size_t nbitsa = a->nBits();
    if (a->isBottom() || b->isBottom())
        return bottom_(nbitsa);
    auto aNum = a->toUnsigned();
    auto bNum = b->toUnsigned();
    if (aNum && bNum) {
        uint64_t result = *bNum < nbitsa ? *aNum << *bNum : (uint64_t)0;
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

BaseSemantics::SValue::Ptr
RiscOperators::shiftRight(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &b_)
{
    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr b = SValue::promote(b_);
    size_t nbitsa = a->nBits();
    if (a->isBottom() || b->isBottom())
        return bottom_(nbitsa);
    if (auto bNum = b->toUnsigned()) {
        if (*bNum >= nbitsa)
            return number_(nbitsa, 0);
        if (auto aNum = a->toUnsigned())
            return number_(nbitsa, *aNum >> *bNum);
        Intervals result;
        for (const Interval &av: a->get_intervals().intervals()) {
            uint64_t lo = av.least() >> *bNum;
            uint64_t hi = av.greatest() >> *bNum;
            result.insert(Interval::hull(lo, hi));
        }
        return svalue_from_intervals(nbitsa, result);
    }
    Intervals result;
    for (const Interval &av: a->get_intervals().intervals()) {
        for (uint64_t i=0; i<(uint64_t)nbitsa; ++i) {
            if (b->get_intervals().contains(i)) {
                uint64_t lo = av.least() >> i;
                uint64_t hi = av.greatest() >> i;
                result.insert(Interval::hull(lo, hi));
            }
        }
    }
    return svalue_from_intervals(nbitsa, result);
}

BaseSemantics::SValue::Ptr
RiscOperators::shiftRightArithmetic(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &b_)
{
    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr b = SValue::promote(b_);
    size_t nbitsa = a->nBits();
    if (a->isBottom() || b->isBottom())
        return bottom_(nbitsa);
    auto aNum = a->toUnsigned();
    auto bNum = b->toUnsigned();
    if (aNum && bNum) {
        uint64_t result = *aNum >> *bNum;
        if (IntegerOps::signBit2(*aNum, nbitsa))
            result |= IntegerOps::genMask<uint64_t>(nbitsa) ^ IntegerOps::genMask<uint64_t>(nbitsa - *bNum);
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

BaseSemantics::SValue::Ptr
RiscOperators::equalToZero(const BaseSemantics::SValue::Ptr &a_)
{
    SValue::Ptr a = SValue::promote(a_);
    if (a->isBottom())
        return bottom_(1);
    if (auto aNum = a->toUnsigned())
        return boolean_(0 == *aNum);
    if (!a->get_intervals().contains(Interval(0)))
        return boolean_(false);
    return undefined_(1);
}

BaseSemantics::SValue::Ptr
RiscOperators::iteWithStatus(const BaseSemantics::SValue::Ptr &cond_, const BaseSemantics::SValue::Ptr &a_,
                             const BaseSemantics::SValue::Ptr &b_, IteStatus &status) {
    SValue::Ptr cond = SValue::promote(cond_);
    ASSERT_require(1 == cond_->nBits());
    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr b = SValue::promote(b_);
    ASSERT_require(a->nBits() == b->nBits());
    if (cond->isBottom()) {
        if (a->mustEqual(b)) {
            status = IteStatus::NEITHER;
            return a->copy();
        } else {
            status = IteStatus::BOTH;
            return bottom_(a->nBits());
        }
    }
    if (auto condVal = cond->toUnsigned()) {
        if (*condVal) {
            status = IteStatus::A;
            return a->copy();
        } else {
            status = IteStatus::B;
            return b->copy();
        }
    }
    status = IteStatus::BOTH;
    Intervals result = a->get_intervals();
    result.insertMultiple(b->get_intervals());
    return svalue_from_intervals(a->nBits(), result);
}

BaseSemantics::SValue::Ptr
RiscOperators::unsignedExtend(const BaseSemantics::SValue::Ptr &a_, size_t new_width)
{
    SValue::Ptr a = SValue::promote(a_);

    if (a->nBits() == new_width)
        return a->copy();

    if (a->isBottom())
        return bottom_(a->nBits());

    if (new_width < a->nBits()) {
        uint64_t lo = IntegerOps::shl1<uint64_t>(new_width);
        uint64_t hi = IntegerOps::genMask<uint64_t>(a->nBits());
        Intervals retval = a->get_intervals();
        retval.erase(Interval::hull(lo, hi));
        return svalue_from_intervals(new_width, retval);
    }

    return svalue_from_intervals(new_width, a->get_intervals());
}

BaseSemantics::SValue::Ptr
RiscOperators::signExtend(const BaseSemantics::SValue::Ptr &a_, size_t new_width)
{
    SValue::Ptr a = SValue::promote(a_);
    if (a->nBits() == new_width)
        return a->copy();
    if (a->isBottom())
        return bottom_(new_width);

    uint64_t old_signbit = IntegerOps::shl1<uint64_t>(a->nBits()-1);
    uint64_t new_signbit = IntegerOps::shl1<uint64_t>(new_width-1);
    Intervals result;
    for (const Interval &av: a->get_intervals().intervals()) {
        uint64_t lo = IntegerOps::signExtend2(av.least(), a->nBits(), new_width);
        uint64_t hi = IntegerOps::signExtend2(av.greatest(), a->nBits(), new_width);
        if (0==(lo & new_signbit) && 0!=(hi & new_signbit)) {
            result.insert(Interval::hull(lo, IntegerOps::genMask<uint64_t>(a->nBits()-1)));
            result.insert(Interval::hull(old_signbit, hi));
        } else {
            result.insert(Interval::hull(lo, hi));
        }
    }
    return svalue_from_intervals(new_width, result);
}

BaseSemantics::SValue::Ptr
RiscOperators::add(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &b_)
{
    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr b = SValue::promote(b_);
    ASSERT_require(a->nBits()==b->nBits());
    size_t nbits = a->nBits();
    if (a->isBottom() || b->isBottom())
        return bottom_(nbits);

    const Intervals &aints=a->get_intervals(), &bints=b->get_intervals();
    Intervals result;
    for (const Interval &av: aints.intervals()) {
        for (const Interval &bv: bints.intervals()) {
            uint64_t lo = (av.least() + bv.least()) & IntegerOps::genMask<uint64_t>(nbits);
            uint64_t hi = (av.greatest()  + bv.greatest())  & IntegerOps::genMask<uint64_t>(nbits);
            if (lo < av.least() || lo < bv.least()) {
                // lo and hi both overflow
                result.insert(Interval::hull(lo, hi));
            } else if (hi < av.greatest() || hi < bv.greatest()) {
                // hi overflows, but not lo
                result.insert(Interval::hull(lo, IntegerOps::genMask<uint64_t>(nbits)));
                result.insert(Interval::hull(0, hi));
            } else {
                // no overflow
                result.insert(Interval::hull(lo, hi));
            }
        }
    }
    return svalue_from_intervals(nbits, result);
}

BaseSemantics::SValue::Ptr
RiscOperators::addWithCarries(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &b_,
                              const BaseSemantics::SValue::Ptr &c_, BaseSemantics::SValue::Ptr &carry_out/*out*/)
{
    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr b = SValue::promote(b_);
    ASSERT_require(a->nBits()==b->nBits());
    size_t nbits = a->nBits();
    SValue::Ptr c = SValue::promote(c_);
    ASSERT_require(c->nBits()==1);
    if (a->isBottom() || b->isBottom() || c->isBottom())
        return bottom_(nbits);

    SValue::Ptr wide_carry = SValue::promote(unsignedExtend(c, nbits));
    SValue::Ptr retval = SValue::promote(add(add(a, b), wide_carry));

    uint64_t known1 = a->toUnsigned().orElse(0);
    uint64_t known2 = b->toUnsigned().orElse(0);
    uint64_t known3 = c->toUnsigned().orElse(0);
    uint64_t unkwn1 = a->toUnsigned() ? 0 : a->possible_bits();
    uint64_t unkwn2 = b->toUnsigned() ? 0 : b->possible_bits();
    uint64_t unkwn3 = c->toUnsigned() ? 0 : c->possible_bits();
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

BaseSemantics::SValue::Ptr
RiscOperators::negate(const BaseSemantics::SValue::Ptr &a_)
{
    SValue::Ptr a = SValue::promote(a_);
    size_t nbits = a->nBits();
    if (a->isBottom())
        return bottom_(nbits);
    Intervals result;
    uint64_t mask = IntegerOps::genMask<uint64_t>(nbits);
    for (const Interval &iv: a->get_intervals().intervals()) {
        uint64_t lo = -iv.greatest() & mask;
        uint64_t hi = -iv.least() & mask;
        if (0==hi) {
            ASSERT_require(0==iv.least());
            result.insert(0);
            if (0!=lo) {
                ASSERT_require(0!=iv.greatest());
                result.insert(Interval::hull(lo, IntegerOps::genMask<uint64_t>(nbits)));
            }
        } else {
            ASSERT_require(lo<=hi);
            result.insert(Interval::hull(lo, hi));
        }
    }
    return svalue_from_intervals(nbits, result);
}

BaseSemantics::SValue::Ptr
RiscOperators::signedDivide(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &b_)
{
    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr b = SValue::promote(b_);
    size_t nbitsa = a->nBits();
    size_t nbitsb = b->nBits();
    if (a->isBottom() || b->isBottom())
        return bottom_(nbitsa);
    if (!IntegerOps::signBit2(a->possible_bits(), nbitsa) && !IntegerOps::signBit2(b->possible_bits(), nbitsb))
        return unsignedDivide(a, b);
    return undefined_(nbitsa); // FIXME, we can do better
}

BaseSemantics::SValue::Ptr
RiscOperators::signedModulo(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &b_)
{
    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr b = SValue::promote(b_);
    size_t nbitsa = a->nBits();
    size_t nbitsb = b->nBits();
    if (a->isBottom() || b->isBottom())
        return bottom_(nbitsb);
    if (!IntegerOps::signBit2(a->possible_bits(), nbitsa) && !IntegerOps::signBit2(b->possible_bits(), nbitsb))
        return unsignedModulo(a, b);
    return undefined_(nbitsb); // FIXME, we can do better
}

BaseSemantics::SValue::Ptr
RiscOperators::signedMultiply(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &b_)
{
    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr b = SValue::promote(b_);
    size_t nbitsa = a->nBits();
    size_t nbitsb = b->nBits();
    if (a->isBottom() || b->isBottom())
        return bottom_(nbitsa+nbitsb);
    if (!IntegerOps::signBit2(a->possible_bits(), nbitsa) && !IntegerOps::signBit2(b->possible_bits(), nbitsb))
        return unsignedMultiply(a, b);
    return undefined_(nbitsa+nbitsb); // FIXME, we can do better
}

BaseSemantics::SValue::Ptr
RiscOperators::unsignedDivide(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &b_)
{
    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr b = SValue::promote(b_);
    size_t nbitsa = a->nBits();
    if (a->isBottom() || b->isBottom())
        return bottom_(nbitsa);
    Intervals result;
    for (const Interval &av: a->get_intervals().intervals()) {
        for (const Interval &bv: b->get_intervals().intervals()) {
            uint64_t lo = av.least() / std::max(bv.greatest(),  (uint64_t)1);
            uint64_t hi = av.greatest()  / std::max(bv.least(), (uint64_t)1);
            ASSERT_require((lo<=IntegerOps::genMask<uint64_t>(nbitsa)));
            ASSERT_require((hi<=IntegerOps::genMask<uint64_t>(nbitsa)));
            ASSERT_require(lo<=hi);
            result.insert(Interval::hull(lo, hi));
        }
    }
    return svalue_from_intervals(nbitsa, result);
}

BaseSemantics::SValue::Ptr
RiscOperators::unsignedModulo(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &b_)
{
    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr b = SValue::promote(b_);
    size_t nbitsa = a->nBits();
    size_t nbitsb = b->nBits();
    if (a->isBottom() || b->isBottom())
        return bottom_(nbitsb);
    if (auto bNum = b->toUnsigned()) {
        // If B is a power of two then mask away the high bits of A
        uint64_t limit = IntegerOps::genMask<uint64_t>(nbitsa);
        for (size_t i=0; i<nbitsb; ++i) {
            uint64_t twopow = IntegerOps::shl1<uint64_t>(i);
            if (*bNum == twopow) {
                Intervals result = a->get_intervals();
                result.erase(Interval::hull(twopow, limit));
                return svalue_from_intervals(nbitsb, result);
            }
        }
    }
    return undefined_(nbitsb); // FIXME: can we do better?
}

BaseSemantics::SValue::Ptr
RiscOperators::unsignedMultiply(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &b_)
{
    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr b = SValue::promote(b_);
    if (a->isBottom() || b->isBottom())
        return bottom_(a->nBits() + b->nBits());
    Intervals result;
    for (const Interval &av: a->get_intervals().intervals()) {
        for (const Interval &bv: b->get_intervals().intervals()) {
            uint64_t lo = av.least() * bv.least();
            uint64_t hi = av.greatest()  * bv.greatest();
            ASSERT_require(lo<=hi);
            result.insert(Interval::hull(lo, hi));
        }
    }
    return svalue_from_intervals(a->nBits()+b->nBits(), result);
}

BaseSemantics::SValue::Ptr
RiscOperators::readMemory(RegisterDescriptor /*segreg*/,
                          const BaseSemantics::SValue::Ptr &/*address*/,
                          const BaseSemantics::SValue::Ptr &dflt,
                          const BaseSemantics::SValue::Ptr &/*condition*/)
{
    return dflt->copy(); // FIXME
}

BaseSemantics::SValue::Ptr
RiscOperators::peekMemory(RegisterDescriptor /*segreg*/,
                          const BaseSemantics::SValue::Ptr &/*address*/,
                          const BaseSemantics::SValue::Ptr &dflt)
{
    return dflt->copy(); // FIXME[Robb Matzke 2018-01-17]
}

void
RiscOperators::writeMemory(RegisterDescriptor /*segreg*/,
                           const BaseSemantics::SValue::Ptr &/*address*/,
                           const BaseSemantics::SValue::Ptr &/*value*/,
                           const BaseSemantics::SValue::Ptr &/*condition*/) {
    // FIXME
}

} // namespace
} // namespace
} // namespace
} // namespace

#endif
