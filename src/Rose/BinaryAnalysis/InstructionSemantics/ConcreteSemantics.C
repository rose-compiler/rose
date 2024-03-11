#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"
#include <Rose/BinaryAnalysis/InstructionSemantics/ConcreteSemantics.h>

#include <Rose/BinaryAnalysis/Hexdump.h>

#include <rose_isnan.h>
#include "integerOps.h"

#include "SageBuilderAsm.h"
#include <Sawyer/BitVectorSupport.h>

using namespace Sawyer::Container;
typedef Sawyer::Container::BitVector::BitRange BitRange;

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics {
namespace ConcreteSemantics {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      SValue
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Sawyer::Optional<BaseSemantics::SValue::Ptr>
SValue::createOptionalMerge(const BaseSemantics::SValue::Ptr &/*other*/, const BaseSemantics::Merger::Ptr&,
                            const SmtSolverPtr&) const {
    // There's no official way to represent BOTTOM
    throw BaseSemantics::NotImplemented("SValue merging for ConcreteSemantics is not supported", NULL);
}

void
SValue::bits(const Sawyer::Container::BitVector &newBits) {
    ASSERT_require(newBits.size() == bits_.size());
    ASSERT_require(newBits.size() == nBits());
    bits_ = newBits;
}

void
SValue::hash(Combinatorics::Hasher &hasher) const {
    hasher.insert(nBits()); // so 0x00ff is not the same as 0xff, for instance (think signed interpretation)
    for (size_t i = 0; i < bits_.size(); i += 64) {
        size_t n = std::min(bits_.size() - i, size_t(64));
        auto where = Sawyer::Container::BitVector::BitRange::baseSize(i, n);
        uint64_t value = bits_.toInteger(where);
        hasher.insert(value);
    }
}

bool
SValue::may_equal(const BaseSemantics::SValue::Ptr &other, const SmtSolverPtr&) const {
    return 0 == bits_.compare(SValue::promote(other)->bits());
}

bool
SValue::must_equal(const BaseSemantics::SValue::Ptr &other, const SmtSolverPtr&) const {
    return 0 == bits_.compare(SValue::promote(other)->bits());
}

void
SValue::set_width(size_t newWidth) {
    ASSERT_require(newWidth > 0);
    if (newWidth != nBits()) {
        bits_.resize(newWidth);
        BaseSemantics::SValue::set_width(newWidth);
        ASSERT_require(bits_.size() == nBits());
    }
}

uint64_t
SValue::get_number() const {
    return bits_.toInteger();
}

void
SValue::print(std::ostream &out, BaseSemantics::Formatter&) const {
    if (nBits() <= 64) {
        out <<StringUtility::toHex2(bits_.toInteger(), nBits());
    } else {
        out <<"0x" << bits_.toHex();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      MemoryState
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
MemoryState::pageSize(rose_addr_t nBytes) {
    ASSERT_require(map_ == NULL || map_->isEmpty());
    pageSize_ = std::max(nBytes, (rose_addr_t)1);
}

void
MemoryState::allocatePage(rose_addr_t va) {
    if (!map_)
        map_ = MemoryMap::instance();
    rose_addr_t pageVa = alignDown(va, pageSize_);
    unsigned acc = MemoryMap::READABLE | MemoryMap::WRITABLE;
    map_->insert(AddressInterval::hull(pageVa, pageVa+pageSize_-1),
                 MemoryMap::Segment(MemoryMap::AllocatingBuffer::instance(pageSize_),
                                    0, acc, "ConcreteSemantics demand allocated"));
}

void
MemoryState::memoryMap(const MemoryMap::Ptr &map, Sawyer::Optional<unsigned> padAccess) {
    map_ = map;
    if (!map)
        return;

    rose_addr_t va = 0;
    while (map_->atOrAfter(va).next().assignTo(va)) {
        rose_addr_t pageVa = alignDown(va, pageSize_);

        // Mapped area must begin at a page boundary.
        if (pageVa < va) {
            unsigned acc = padAccess ? *padAccess : map_->get(va).accessibility();
            map_->insert(AddressInterval::hull(pageVa, va-1),
                         MemoryMap::Segment(MemoryMap::AllocatingBuffer::instance(va-pageVa), 0, acc, "padding"));
        }

        // Mapped area must end at the last byte before a page boundary.
        if (AddressInterval unused = map_->unmapped(va)) {
            va = unused.least();
            rose_addr_t nextPageVa = alignUp(va, pageSize_);
            if (nextPageVa > va) {
                unsigned acc = padAccess ? *padAccess : map_->get(va-1).accessibility();
                map_->insert(AddressInterval::hull(va, nextPageVa-1),
                             MemoryMap::Segment(MemoryMap::AllocatingBuffer::instance(nextPageVa-va), 0, acc, "padding"));
                va = nextPageVa - 1;
            }
        } else {
            break;
        }
        ++va;
    }
}

BaseSemantics::SValue::Ptr
MemoryState::readOrPeekMemory(const BaseSemantics::SValue::Ptr &addr_, const BaseSemantics::SValue::Ptr &dflt_,
                              BaseSemantics::RiscOperators */*addrOps*/, BaseSemantics::RiscOperators */*valOps*/,
                              bool allowSideEffects) {
    ASSERT_require2(8==dflt_->nBits(), "ConcreteSemantics::MemoryState requires memory cells contain 8-bit data");
    rose_addr_t addr = addr_->toUnsigned().get();
    uint8_t dflt = dflt_->toUnsigned().get();
    if (!map_ || !map_->at(addr).exists()) {
        if (allowSideEffects) {
            allocatePage(addr);
            map_->at(addr).limit(1).write(&dflt);
        }
        return dflt_;
    }
    map_->at(addr).limit(1).read(&dflt);
    return dflt_->number_(8, dflt);
}

BaseSemantics::SValue::Ptr
MemoryState::readMemory(const BaseSemantics::SValue::Ptr &addr, const BaseSemantics::SValue::Ptr &dflt,
                        BaseSemantics::RiscOperators *addrOps, BaseSemantics::RiscOperators *valOps) {
    return readOrPeekMemory(addr, dflt, addrOps, valOps, true /*allow side effects*/);
}

BaseSemantics::SValue::Ptr
MemoryState::peekMemory(const BaseSemantics::SValue::Ptr &addr, const BaseSemantics::SValue::Ptr &dflt,
                        BaseSemantics::RiscOperators *addrOps, BaseSemantics::RiscOperators *valOps) {
    return readOrPeekMemory(addr, dflt, addrOps, valOps, false /*no side effects allowed*/);
}

void
MemoryState::writeMemory(const BaseSemantics::SValue::Ptr &addr_, const BaseSemantics::SValue::Ptr &value_,
                         BaseSemantics::RiscOperators */*addrOps*/, BaseSemantics::RiscOperators */*valOps*/) {
    ASSERT_require2(8==value_->nBits(), "ConcreteSemantics::MemoryState requires memory cells contain 8-bit data");
    rose_addr_t addr = addr_->toUnsigned().get();
    uint8_t value = value_->toUnsigned().get();
    if (!map_ || !map_->at(addr).exists())
        allocatePage(addr);
    map_->at(addr).limit(1).write(&value);
}

bool
MemoryState::merge(const BaseSemantics::MemoryState::Ptr &/*other*/, BaseSemantics::RiscOperators */*addrOps*/,
                   BaseSemantics::RiscOperators */*valOps*/) {
    throw BaseSemantics::NotImplemented("MemoryState merging for ConcreteSemantics is not supported", NULL);
}

void
MemoryState::hash(Combinatorics::Hasher &hasher, BaseSemantics::RiscOperators*/*addrOps*/,
                  BaseSemantics::RiscOperators*/*valOps*/) const {
    if (map_)
        map_->hash(hasher);
}

void
MemoryState::print(std::ostream &out, Formatter &fmt) const {
    if (!map_) {
        out <<fmt.get_line_prefix() <<"no memory map\n";
    } else {
        map_->dump(out, fmt.get_line_prefix());
        rose_addr_t pageVa = 0;
        uint8_t* page = new uint8_t[pageSize_];
        while (map_->atOrAfter(pageVa).next().assignTo(pageVa)) {
            size_t nread = map_->at(pageVa).limit(pageSize_).read(page).size();
            ASSERT_always_require(nread == pageSize_);
            HexdumpFormat hdFmt;
            hdFmt.prefix = fmt.get_line_prefix();
            out <<fmt.get_line_prefix();
            hexdump(out, pageVa, (const unsigned char*)page, pageSize_, hdFmt);
            out <<"\n";
            if (pageVa + (pageSize_-1) == map_->hull().greatest())
                break;
            pageVa += pageSize_;
        }
        delete [] page;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      RiscOperators
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RiscOperators::RiscOperators(const BaseSemantics::SValue::Ptr &protoval, const SmtSolver::Ptr &solver)
    : BaseSemantics::RiscOperators(protoval, solver) {
    name("Concrete");
    (void) SValue::promote(protoval); // make sure its dynamic type is a ConcreteSemantics::SValue
}

RiscOperators::RiscOperators(const BaseSemantics::State::Ptr &state, const SmtSolver::Ptr &solver)
    : BaseSemantics::RiscOperators(state, solver) {
    name("Concrete");
    (void) SValue::promote(state->protoval());      // values must have ConcreteSemantics::SValue dynamic type
}

RiscOperators::~RiscOperators() {}

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

SValue::Ptr
RiscOperators::svalueNumber(const Sawyer::Container::BitVector &bits) {
    SValue::Ptr retval = SValue::promote(svalueNumber(bits.size(), 0));
    retval->bits(bits);
    return retval;
}

RiscOperators::Ptr
RiscOperators::promote(const BaseSemantics::RiscOperators::Ptr &x) {
    Ptr retval = boost::dynamic_pointer_cast<RiscOperators>(x);
    ASSERT_not_null(retval);
    return retval;
}

void
RiscOperators::interrupt(int /*major*/, int /*minor*/) {
    currentState()->clear();
}

BaseSemantics::SValue::Ptr
RiscOperators::and_(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &b_) {
    BitVector result = SValue::promote(a_)->bits();
    result.bitwiseAnd(SValue::promote(b_)->bits());
    return svalueNumber(result);
}

BaseSemantics::SValue::Ptr
RiscOperators::or_(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &b_) {
    BitVector result = SValue::promote(a_)->bits();
    result.bitwiseOr(SValue::promote(b_)->bits());
    return svalueNumber(result);
}

BaseSemantics::SValue::Ptr
RiscOperators::xor_(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &b_) {
    BitVector result = SValue::promote(a_)->bits();
    result.bitwiseXor(SValue::promote(b_)->bits());
    return svalueNumber(result);
}

BaseSemantics::SValue::Ptr
RiscOperators::invert(const BaseSemantics::SValue::Ptr &a_) {
    BitVector result = SValue::promote(a_)->bits();
    result.invert();
    return svalueNumber(result);
}

BaseSemantics::SValue::Ptr
RiscOperators::extract(const BaseSemantics::SValue::Ptr &a_, size_t begin_bit, size_t end_bit) {
    ASSERT_require(end_bit <= a_->nBits());
    ASSERT_require(begin_bit < end_bit);
    BitVector result(end_bit - begin_bit);
    result.copy(result.hull(), SValue::promote(a_)->bits(), BitRange::hull(begin_bit, end_bit-1));
    return svalueNumber(result);
}

BaseSemantics::SValue::Ptr
RiscOperators::concat(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &b_) {
    size_t resultNBits = a_->nBits() + b_->nBits();
    BitVector result = SValue::promote(a_)->bits();
    result.resize(resultNBits);
    result.copy(BitRange::baseSize(a_->nBits(), b_->nBits()),
                SValue::promote(b_)->bits(), BitRange::baseSize(0, b_->nBits()));
    return svalueNumber(result);
}

BaseSemantics::SValue::Ptr
RiscOperators::leastSignificantSetBit(const BaseSemantics::SValue::Ptr &a_) {
    uint64_t count = SValue::promote(a_)->bits().leastSignificantSetBit().orElse(0);
    return svalueNumber(a_->nBits(), count);
}

BaseSemantics::SValue::Ptr
RiscOperators::mostSignificantSetBit(const BaseSemantics::SValue::Ptr &a_) {
    uint64_t count = SValue::promote(a_)->bits().mostSignificantSetBit().orElse(0);
    return svalueNumber(a_->nBits(), count);
}

BaseSemantics::SValue::Ptr
RiscOperators::rotateLeft(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &sa_) {
    BitVector result = SValue::promote(a_)->bits();
    result.rotateLeft(sa_->toUnsigned().get());
    return svalueNumber(result);
}

BaseSemantics::SValue::Ptr
RiscOperators::rotateRight(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &sa_) {
    BitVector result = SValue::promote(a_)->bits();
    result.rotateRight(sa_->toUnsigned().get());
    return svalueNumber(result);
}

BaseSemantics::SValue::Ptr
RiscOperators::shiftLeft(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &sa_) {
    BitVector result = SValue::promote(a_)->bits();
    result.shiftLeft(sa_->toUnsigned().get());
    return svalueNumber(result);
}

BaseSemantics::SValue::Ptr
RiscOperators::shiftRight(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &sa_) {
    BitVector result = SValue::promote(a_)->bits();
    result.shiftRight(sa_->toUnsigned().get());
    return svalueNumber(result);
}

BaseSemantics::SValue::Ptr
RiscOperators::shiftRightArithmetic(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &sa_) {
    BitVector result = SValue::promote(a_)->bits();
    result.shiftRightArithmetic(sa_->toUnsigned().get());
    return svalueNumber(result);
}

BaseSemantics::SValue::Ptr
RiscOperators::equalToZero(const BaseSemantics::SValue::Ptr &a_) {
    return svalueBoolean(SValue::promote(a_)->bits().isEqualToZero());
}

BaseSemantics::SValue::Ptr
RiscOperators::iteWithStatus(const BaseSemantics::SValue::Ptr &sel_,
                             const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &b_,
                             BaseSemantics::RiscOperators::IteStatus &status) {
    ASSERT_require(sel_->nBits() == 1);
    if (sel_->toUnsigned().get()) {
        status = IteStatus::A;
        return a_->copy();
    } else {
        status = IteStatus::B;
        return b_->copy();
    }
}

BaseSemantics::SValue::Ptr
RiscOperators::unsignedExtend(const BaseSemantics::SValue::Ptr &a_, size_t new_width) {
    BitVector result = SValue::promote(a_)->bits();
    result.resize(new_width);
    return svalueNumber(result);
}

BaseSemantics::SValue::Ptr
RiscOperators::signExtend(const BaseSemantics::SValue::Ptr &a_, size_t new_width) {
    BitVector result(new_width);
    result.signExtend(SValue::promote(a_)->bits());
    return svalueNumber(result);
}

BaseSemantics::SValue::Ptr
RiscOperators::add(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &b_) {
    BitVector result = SValue::promote(a_)->bits();
    result.add(SValue::promote(b_)->bits());
    return svalueNumber(result);
}

BaseSemantics::SValue::Ptr
RiscOperators::addWithCarries(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &b_,
                              const BaseSemantics::SValue::Ptr &c_, BaseSemantics::SValue::Ptr &carry_out/*out*/) {
    size_t nbits = a_->nBits();

    // Values extended by one bit
    BitVector   ae = SValue::promote(a_)->bits();   ae.resize(nbits+1);
    BitVector   be = SValue::promote(b_)->bits();   be.resize(nbits+1);
    BitVector   ce = SValue::promote(c_)->bits();   ce.resize(nbits+1);

    // Extended sum
    BitVector se = ae;
    se.add(be);
    se.add(ce);

    // Carry out
    BitVector co = ae;
    co.bitwiseXor(be);
    co.bitwiseXor(se);
    co.shiftRight(1);
    co.resize(nbits);
    carry_out = svalueNumber(co);

    se.resize(nbits);
    return svalueNumber(se);
}

BaseSemantics::SValue::Ptr
RiscOperators::negate(const BaseSemantics::SValue::Ptr &a_) {
    BitVector result = SValue::promote(a_)->bits();
    result.negate();
    return svalueNumber(result);
}

BaseSemantics::SValue::Ptr
RiscOperators::signedDivide(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &b_) {
    // FIXME[Robb P. Matzke 2015-03-31]: BitVector doesn't have a divide method
    if (a_->nBits() > 64 || b_->nBits() > 64) {
        throw BaseSemantics::NotImplemented("signedDivide x[" + StringUtility::addrToString(a_->nBits()) +
                                            "] / y[" + StringUtility::addrToString(b_->nBits()) +
                                            "] is not implemented", currentInstruction());
    }
    int64_t a = IntegerOps::signExtend2(a_->toUnsigned().get(), a_->nBits(), 64);
    int64_t b = IntegerOps::signExtend2(b_->toUnsigned().get(), b_->nBits(), 64);
    if (0==b)
        throw BaseSemantics::Exception("division by zero", currentInstruction());
    return svalueNumber(a_->nBits(), a/b);
}

BaseSemantics::SValue::Ptr
RiscOperators::signedModulo(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &b_) {
    // FIXME[Robb P. Matzke 2015-03-31]: BitVector doesn't have a modulo method
    if (a_->nBits() > 64 || b_->nBits() > 64) {
        throw BaseSemantics::NotImplemented("signedModulo x[" + StringUtility::addrToString(a_->nBits()) +
                                            "] % y[" + StringUtility::addrToString(b_->nBits()) +
                                            "] is not implemented", currentInstruction());
    }
    int64_t a = IntegerOps::signExtend2(a_->toUnsigned().get(), a_->nBits(), 64);
    int64_t b = IntegerOps::signExtend2(b_->toUnsigned().get(), b_->nBits(), 64);
    if (0==b)
        throw BaseSemantics::Exception("division by zero", currentInstruction());
    return svalueNumber(b_->nBits(), a % b);
}

BaseSemantics::SValue::Ptr
RiscOperators::signedMultiply(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &b_) {
    // FIXME[Robb P. Matzke 2015-03-31]: BitVector doesn't have a multiply method
    ASSERT_require2(a_->nBits() <= 64, "not implemented");
    ASSERT_require2(b_->nBits() <= 64, "not implemented");

    if (a_->nBits() == 64 && b_->nBits() == 64) {
        // Do long multiplication with 32 bits at a time in 64-bit variables (32 bits * 32 bits = 64 bits)
        // FIXME[Robb P. Matzke 2015-03-23]: use arbitrary width vector multiply in Sawyer when it's available.
        uint64_t a0 = a_->toUnsigned().get() & 0xffffffff;
        uint64_t a1 = a_->toUnsigned().get() >> 32;
        uint64_t b0 = b_->toUnsigned().get() & 0xffffffff;
        uint64_t b1 = b_->toUnsigned().get() >> 32;
        uint64_t c0 = a0 * b0;
        uint64_t c1 = (a0 * b1) + (a1 * b0) + (c0 >> 32);
        uint64_t c2 = (a1 * b1) + (c1 >> 32);
        c0 &= 0xffffffff;
        c1 &= 0xffffffff;
        //c2: use all 64 bits

        BitVector product(128);
        product.fromInteger(BitRange::baseSize( 0, 64), (c1 << 32) | c0);
        product.fromInteger(BitRange::baseSize(64, 64), c2);
        return svalueNumber(product);
    } else if (a_->nBits() + b_->nBits() > 64) {
        throw BaseSemantics::NotImplemented("signedMultiply x[" + StringUtility::addrToString(a_->nBits()) +
                                            "] * y[" + StringUtility::addrToString(b_->nBits()) +
                                            "] is not implemented", currentInstruction());
    } else {
        ASSERT_require2(a_->nBits() + b_->nBits() <= 64, "not implemented yet");
        int64_t a = IntegerOps::signExtend2(a_->toUnsigned().get(), a_->nBits(), 64);
        int64_t b = IntegerOps::signExtend2(b_->toUnsigned().get(), b_->nBits(), 64);
        return svalueNumber(a_->nBits() + b_->nBits(), a * b);
    }
}

BaseSemantics::SValue::Ptr
RiscOperators::unsignedDivide(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &b_) {
    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr b = SValue::promote(b_);

    // This is a common case on 64-bit architectures
    // FIXME[Robb P. Matzke 2015-06-24]: this will probably only compile with GCC >4.x on a 64-bit machine. The real solution
    // would be to either use a multi-precision library (sort of overkill) or implement division in Sawyer's BitVector class.
#ifdef __x86_64
    if (a->nBits() == 128 && b->nBits() == 64) {
        __uint128_t numerator = a->bits().toInteger(BitRange::baseSize(64, 64));
        numerator <<= 64;
        numerator |= a->bits().toInteger(BitRange::baseSize(0, 64));
        uint64_t denominator = b->bits().toInteger();
        if (0 == denominator)
            throw BaseSemantics::Exception("division by zero", currentInstruction());
        __uint128_t ratio = numerator / denominator;
        uint64_t ratio_lo = ratio;
        uint64_t ratio_hi = ratio >> 64;
        BitVector resultBits(128);
        resultBits.fromInteger(BitRange::baseSize(0, 64), ratio_lo);
        resultBits.fromInteger(BitRange::baseSize(64, 64), ratio_hi);
        return svalueNumber(resultBits);
    }
#endif

    // FIXME[Robb P. Matzke 2015-03-31]: BitVector doesn't have a divide method
    if (a->nBits() > 64 || b->nBits() > 64) {
        throw BaseSemantics::NotImplemented("unsignedDivide x[" + StringUtility::addrToString(a->nBits()) +
                                            "] / y[" + StringUtility::addrToString(b->nBits()) +
                                            "] is not implemented", currentInstruction());
    }

    uint64_t an = a->toUnsigned().get();
    uint64_t bn = b->toUnsigned().get();
    if (0==bn)
        throw BaseSemantics::Exception("division by zero", currentInstruction());
    return svalueNumber(a->nBits(), an/bn);
}

BaseSemantics::SValue::Ptr
RiscOperators::unsignedModulo(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &b_) {
    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr b = SValue::promote(b_);

    // This is a common case on 64-bit architectures
    // FIXME[Robb P. Matzke 2015-06-24]: this will probably only compile with GCC >4.x on a 64-bit machine. The real solution
    // would be to either use a multi-precision library (sort of overkill) or implement division in Sawyer's BitVector class.
#ifdef __x86_64
    if (a->nBits() == 128 && b->nBits() == 64) {
        __uint128_t numerator = a->bits().toInteger(BitRange::baseSize(64, 64));
        numerator <<= 64;
        numerator |= a->bits().toInteger(BitRange::baseSize(0, 64));
        uint64_t denominator = b->bits().toInteger();
        if (0 == denominator)
            throw BaseSemantics::Exception("division by zero", currentInstruction());
        uint64_t remainder = numerator % denominator;
        return svalueNumber(64, remainder);
    }
#endif

    // FIXME[Robb P. Matzke 2015-03-31]: BitVector doesn't have a modulo method
    if (a->nBits() > 64 || b->nBits() > 64) {
        throw BaseSemantics::NotImplemented("unsignedModulo x[" + StringUtility::addrToString(a->nBits()) +
                                            "] % y[" + StringUtility::addrToString(b->nBits()) +
                                            "] is not implemented", currentInstruction());
    }
    uint64_t an = a->toUnsigned().get();
    uint64_t bn = b->toUnsigned().get();
    if (0==bn)
        throw BaseSemantics::Exception("division by zero", currentInstruction());
    return svalueNumber(b->nBits(), an % bn);
}

BaseSemantics::SValue::Ptr
RiscOperators::unsignedMultiply(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &b_) {
    // FIXME[Robb P. Matzke 2015-03-31]: BitVector doesn't have a multiply method
    ASSERT_require2(a_->nBits() <= 64, "not implemented");
    ASSERT_require2(b_->nBits() <= 64, "not implemented");

    if (a_->nBits() == 64 && b_->nBits() == 64) {
        // Do long multiplication with 32 bits at a time in 64-bit variables (32 bits * 32 bits = 64 bits)
        // FIXME[Robb P. Matzke 2015-03-23]: use arbitrary width vector multiply in Sawyer when it's available.
        uint64_t a0 = a_->toUnsigned().get() & 0xffffffff;
        uint64_t a1 = a_->toUnsigned().get() >> 32;
        uint64_t b0 = b_->toUnsigned().get() & 0xffffffff;
        uint64_t b1 = b_->toUnsigned().get() >> 32;
        uint64_t c0 = a0 * b0;
        uint64_t c1 = (a0 * b1) + (a1 * b0) + (c0 >> 32);
        uint64_t c2 = (a1 * b1) + (c1 >> 32);
        c0 &= 0xffffffff;
        c1 &= 0xffffffff;
        //c2: use all 64 bits

        BitVector product(128);
        product.fromInteger(BitRange::baseSize( 0, 64), (c1 << 32) | c0);
        product.fromInteger(BitRange::baseSize(64, 64), c2);
        return svalueNumber(product);
    } else if (a_->nBits() + b_->nBits() > 64) {
        throw BaseSemantics::NotImplemented("unsignedMultiply x[" + StringUtility::addrToString(a_->nBits()) +
                                            "] * y[" + StringUtility::addrToString(b_->nBits()) +
                                            "] is not implemented", currentInstruction());
    } else {
        ASSERT_require2(a_->nBits() + b_->nBits() <= 64, "not implemented yet");
        uint64_t a = a_->toUnsigned().get();
        uint64_t b = b_->toUnsigned().get();
        return svalueNumber(a_->nBits() + b_->nBits(), a * b);
    }
}

BaseSemantics::SValue::Ptr
RiscOperators::readOrPeekMemory(RegisterDescriptor segreg, const BaseSemantics::SValue::Ptr &address,
                                const BaseSemantics::SValue::Ptr &dflt, bool allowSideEffects) {
    size_t nbits = dflt->nBits();
    ASSERT_require(0 == nbits % 8);

    // Offset the address by the value of the segment register.
    BaseSemantics::SValue::Ptr adjustedVa;
    if (segreg.isEmpty()) {
        adjustedVa = address;
    } else {
        BaseSemantics::SValue::Ptr segregValue;
        if (allowSideEffects) {
            segregValue = readRegister(segreg, undefined_(segreg.nBits()));
        } else {
            segregValue = peekRegister(segreg, undefined_(segreg.nBits()));
        }
        adjustedVa = add(address, signExtend(segregValue, address->nBits()));
    }

    // Read the bytes and concatenate them together.
    BaseSemantics::SValue::Ptr retval;
    size_t nbytes = nbits/8;
    BaseSemantics::MemoryState::Ptr mem = currentState()->memoryState();
    for (size_t bytenum=0; bytenum<nbits/8; ++bytenum) {
        size_t byteOffset = ByteOrder::ORDER_MSB==mem->get_byteOrder() ? nbytes-(bytenum+1) : bytenum;
        BaseSemantics::SValue::Ptr byte_dflt = extract(dflt, 8*byteOffset, 8*byteOffset+8);
        BaseSemantics::SValue::Ptr byte_addr = add(adjustedVa, number_(adjustedVa->nBits(), bytenum));

        // Use the lazily updated initial memory state if there is one.
        if (initialState()) {
            if (allowSideEffects) {
                byte_dflt = initialState()->readMemory(byte_addr, byte_dflt, this, this);
            } else {
                byte_dflt = initialState()->peekMemory(byte_addr, byte_dflt, this, this);
            }
        }
        
        // Read the current memory state
        SValue::Ptr byte_value;
        if (allowSideEffects) {
            byte_value = SValue::promote(currentState()->readMemory(byte_addr, byte_dflt, this, this));
        } else {
            byte_value = SValue::promote(currentState()->peekMemory(byte_addr, byte_dflt, this, this));
        }
        if (0==bytenum) {
            retval = byte_value;
        } else if (ByteOrder::ORDER_MSB==mem->get_byteOrder()) {
            retval = concat(byte_value, retval);
        } else if (ByteOrder::ORDER_LSB==mem->get_byteOrder()) {
            retval = concat(retval, byte_value);
        } else {
            // See BaseSemantics::MemoryState::set_byteOrder
            throw BaseSemantics::Exception("multi-byte read with memory having unspecified byte order", currentInstruction());
        }
    }

    ASSERT_require(retval!=NULL && retval->nBits()==nbits);
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::readMemory(RegisterDescriptor segreg, const BaseSemantics::SValue::Ptr &address,
                          const BaseSemantics::SValue::Ptr &dflt, const BaseSemantics::SValue::Ptr &cond) {
    ASSERT_require(1==cond->nBits()); // FIXME: condition is not used
    if (cond->isFalse())
        return dflt;
    return readOrPeekMemory(segreg, address, dflt, true /*allow side effects*/);
}

BaseSemantics::SValue::Ptr
RiscOperators::peekMemory(RegisterDescriptor segreg, const BaseSemantics::SValue::Ptr &address,
                          const BaseSemantics::SValue::Ptr &dflt) {
    return readOrPeekMemory(segreg, address, dflt, false /*no side effects allowed*/);
}

void
RiscOperators::writeMemory(RegisterDescriptor segreg, const BaseSemantics::SValue::Ptr &address,
                           const BaseSemantics::SValue::Ptr &value_, const BaseSemantics::SValue::Ptr &cond) {
    ASSERT_require(1==cond->nBits()); // FIXME: condition is not used
    if (cond->isFalse())
        return;

    // Offset the address by the value of the segment register.
    BaseSemantics::SValue::Ptr adjustedVa;
    if (segreg.isEmpty()) {
        adjustedVa = address;
    } else {
        BaseSemantics::SValue::Ptr segregValue = readRegister(segreg, undefined_(segreg.nBits()));
        adjustedVa = add(address, signExtend(segregValue, address->nBits()));
    }

    SValue::Ptr value = SValue::promote(value_->copy());
    size_t nbits = value->nBits();
    ASSERT_require(0 == nbits % 8);
    size_t nbytes = nbits/8;
    BaseSemantics::MemoryState::Ptr mem = currentState()->memoryState();
    for (size_t bytenum=0; bytenum<nbytes; ++bytenum) {
        size_t byteOffset = 0;
        if (1 == nbytes) {
            // void
        } else if (ByteOrder::ORDER_MSB==mem->get_byteOrder()) {
            byteOffset = nbytes-(bytenum+1);
        } else if (ByteOrder::ORDER_LSB==mem->get_byteOrder()) {
            byteOffset = bytenum;
        } else {
            // See BaseSemantics::MemoryState::set_byteOrder
            throw BaseSemantics::Exception("multi-byte write with memory having unspecified byte order", currentInstruction());
        }

        BaseSemantics::SValue::Ptr byte_value = extract(value, 8*byteOffset, 8*byteOffset+8);
        BaseSemantics::SValue::Ptr byte_addr = add(adjustedVa, number_(adjustedVa->nBits(), bytenum));
        currentState()->writeMemory(byte_addr, byte_value, this, this);
    }
}

double
RiscOperators::exprToDouble(const BaseSemantics::SValue::Ptr &a, SgAsmFloatType *aType) {
    ASSERT_require(a->isConcrete());
    if (aType == SageBuilderAsm::buildIeee754Binary64()) {
        ASSERT_require(sizeof(double) == sizeof(int64_t));
        union {
            double fp;
            int64_t i;
        } u;
        u.i = a->toUnsigned().get();
        return u.fp;
    } else if (aType == SageBuilderAsm::buildIeee754Binary32()) {
        ASSERT_require(sizeof(float) == sizeof(int32_t));
        union {
            float fp;
            int32_t i;
        } u;
        u.i = a->toUnsigned().get();
        return u.fp;
    } else {
        throw BaseSemantics::NotImplemented("exprToDouble type not supported", currentInstruction());
    }
}

BaseSemantics::SValue::Ptr
RiscOperators::doubleToExpr(double d, SgAsmFloatType *retType) {
    if (retType == SageBuilderAsm::buildIeee754Binary64()) {
        ASSERT_require(sizeof(double) == sizeof(int64_t));
        union {
            double fp;
            int64_t i;
        } u;
        u.fp = d;
        return svalueNumber(64, u.i);
    } else if (retType == SageBuilderAsm::buildIeee754Binary32()) {
        ASSERT_require(sizeof(float) == sizeof(int32_t));
        union {
            float fp;
            int32_t i;
        } u;
        u.fp = d;
        return svalueNumber(32, u.i);
    } else {
        throw BaseSemantics::NotImplemented("doubleToExpr type not supported", currentInstruction());
    }
}
    
BaseSemantics::SValue::Ptr
RiscOperators::fpFromInteger(const BaseSemantics::SValue::Ptr &intValue, SgAsmFloatType *retType) {
    ASSERT_require(intValue->isConcrete());
    return doubleToExpr((double)intValue->toUnsigned().get(), retType);
}

BaseSemantics::SValue::Ptr
RiscOperators::fpToInteger(const BaseSemantics::SValue::Ptr &a, SgAsmFloatType *aType, const BaseSemantics::SValue::Ptr &dflt) {
    double ad = exprToDouble(a, aType);
    size_t nBits = dflt->nBits();
    double minInt = -pow(2.0, nBits-1);
    double maxInt = pow(2.0, nBits-1);
    if (rose_isnan(ad) || ad < minInt || ad >= maxInt)
        return dflt;
    int64_t ai = ad;
    return number_(nBits, ai);
}

BaseSemantics::SValue::Ptr
RiscOperators::fpAdd(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &b, SgAsmFloatType *fpType) {
    double ad = exprToDouble(a, fpType);
    double bd = exprToDouble(b, fpType);
    double result = ad + bd;
    return doubleToExpr(result, fpType);
}

BaseSemantics::SValue::Ptr
RiscOperators::fpSubtract(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &b, SgAsmFloatType *fpType) {
    double ad = exprToDouble(a, fpType);
    double bd = exprToDouble(b, fpType);
    double result = ad - bd;
    return doubleToExpr(result, fpType);
}

BaseSemantics::SValue::Ptr
RiscOperators::fpMultiply(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &b, SgAsmFloatType *fpType) {
    double ad = exprToDouble(a, fpType);
    double bd = exprToDouble(b, fpType);
    double result = ad * bd;
    return doubleToExpr(result, fpType);
}

BaseSemantics::SValue::Ptr
RiscOperators::fpRoundTowardZero(const BaseSemantics::SValue::Ptr &a, SgAsmFloatType *fpType) {
    double ad = exprToDouble(a, fpType);
    double result = trunc(ad);
    return doubleToExpr(result, fpType);
}

} // namespace
} // namespace
} // namespace
} // namespace

#endif
