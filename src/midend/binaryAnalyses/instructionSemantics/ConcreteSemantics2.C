#include "sage3basic.h"
#include "ConcreteSemantics2.h"
#include "integerOps.h"
#include <sawyer/BitVectorSupport.h>

using namespace Sawyer::Container;
using Sawyer::Container::BitVectorSupport::BitRange;

namespace rose {
namespace BinaryAnalysis {
namespace InstructionSemantics2 {
namespace ConcreteSemantics {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      SValue
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool
SValue::may_equal(const BaseSemantics::SValuePtr &other, SMTSolver*) const {
    const uint64_t *a = this->bits();
    const uint64_t *b = SValue::promote(other)->bits();
    return a[0] == b[0] && a[1] == b[1];
}

bool
SValue::must_equal(const BaseSemantics::SValuePtr &other, SMTSolver*) const {
    const uint64_t *a = this->bits();
    const uint64_t *b = SValue::promote(other)->bits();
    return a[0] == b[0] && a[1] == b[1];
}

void
SValue::set_width(size_t nbits) {
    ASSERT_require(nbits > 0);
    if (nbits != get_width()) {
        ASSERT_require(nbits <= maxNBits_);
        if (nbits < get_width())
            BitVectorSupport::clear<uint64_t>(bits_, BitRange::hull(nbits, maxNBits_-1));
        BaseSemantics::SValue::set_width(nbits);
    }
}

uint64_t
SValue::get_number() const {
    ASSERT_require(get_width() <= 64);
    return bits_[0];
}

void
SValue::print(std::ostream &out, BaseSemantics::Formatter&) const {
    if (get_width() <= 64) {
        out <<StringUtility::toHex2(bits_[0], get_width());
    } else {
        out << BitVectorSupport::toHex<uint64_t>(bits_, BitRange::baseSize(0, get_width()));
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      MemoryState
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
MemoryState::pageSize(rose_addr_t nBytes) {
    ASSERT_require(map_.isEmpty());
    pageSize_ = std::max(nBytes, (rose_addr_t)1);
}

void
MemoryState::allocatePage(rose_addr_t va) {
    rose_addr_t pageVa = alignDown(va, pageSize_);
    map_.insert(AddressInterval::baseSize(pageVa, pageSize_),
                MemoryMap::Segment(MemoryMap::AllocatingBuffer::instance(pageSize_)));
}

void
MemoryState::memoryMap(const MemoryMap &map, Sawyer::Optional<unsigned> padAccess) {
    map_ = map;
    rose_addr_t va = 0;
    while (map_.atOrAfter(va).next().assignTo(va)) {
        rose_addr_t pageVa = alignDown(va, pageSize_);

        // Mapped area must begin at a page boundary.
        if (pageVa < va) {
            unsigned acc = padAccess ? *padAccess : map_.get(va).accessibility();
            map_.insert(AddressInterval::hull(pageVa, va-1),
                        MemoryMap::Segment(MemoryMap::AllocatingBuffer::instance(va-pageVa), 0, acc, "padding"));
        }

        // Mapped area must end at the last byte before a page boundary.
        if (AddressInterval unused = map_.unmapped(va)) {
            va = unused.least();
            rose_addr_t nextPageVa = alignUp(va, pageSize_);
            if (nextPageVa > va) {
                unsigned acc = padAccess ? *padAccess : map_.get(va-1).accessibility();
                map_.insert(AddressInterval::hull(va, nextPageVa-1),
                            MemoryMap::Segment(MemoryMap::AllocatingBuffer::instance(nextPageVa-va), 0, acc, "padding"));
                va = nextPageVa - 1;
            }
        } else {
            break;
        }
        ++va;
    }
}

BaseSemantics::SValuePtr
MemoryState::readMemory(const BaseSemantics::SValuePtr &addr_, const BaseSemantics::SValuePtr &dflt_,
                        BaseSemantics::RiscOperators *addrOps, BaseSemantics::RiscOperators *valOps) {
    ASSERT_require2(8==dflt_->get_width(), "ConcreteSemantics::MemoryState requires memory cells contain 8-bit data");
    rose_addr_t addr = SValue::promote(addr_)->get_number();
    uint8_t dflt = SValue::promote(dflt_)->get_number();
    if (!map_.at(addr).exists()) {
        allocatePage(addr);
        map_.at(addr).limit(1).write(&dflt);
        return dflt_;
    }
    map_.at(addr).limit(1).read(&dflt);
    return dflt_->number_(8, dflt);
}

void
MemoryState::writeMemory(const BaseSemantics::SValuePtr &addr_, const BaseSemantics::SValuePtr &value_,
                         BaseSemantics::RiscOperators *addrOps, BaseSemantics::RiscOperators *valOps) {
    ASSERT_require2(8==value_->get_width(), "ConcreteSemantics::MemoryState requires memory cells contain 8-bit data");
    rose_addr_t addr = SValue::promote(addr_)->get_number();
    uint8_t value = SValue::promote(value_)->get_number();
    if (!map_.at(addr).exists())
        allocatePage(addr);
    map_.at(addr).limit(1).write(&value);
}

void
MemoryState::print(std::ostream &out, Formatter&) const {
    map_.dump(out);
    rose_addr_t pageVa = 0;
    while (map_.atOrAfter(pageVa).next().assignTo(pageVa)) {
        uint8_t page[pageSize_];
        size_t nread = map_.at(pageVa).limit(pageSize_).read(page).size();
        ASSERT_require(nread == pageSize_);
        HexdumpFormat fmt;
        SgAsmExecutableFileFormat::hexdump(out, pageVa, (const unsigned char*)page, pageSize_, fmt);
        out <<"\n";
        if (pageVa + (pageSize_-1) == map_.hull().greatest())
            break;
        pageVa += pageSize_;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      RiscOperators
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RiscOperators::interrupt(int majr, int minr) {
    get_state()->clear();
}

BaseSemantics::SValuePtr
RiscOperators::and_(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_) {
    SValuePtr retval = SValue::promote(a_->copy());
    uint64_t *r = retval->bits();
    const uint64_t *b = SValue::promote(b_)->bits();
    r[0] &= b[0];
    r[1] &= b[1];
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::or_(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_) {
    SValuePtr retval = SValue::promote(a_->copy());
    uint64_t *r = retval->bits();
    const uint64_t *b = SValue::promote(b_)->bits();
    r[0] |= b[0];
    r[1] |= b[1];
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::xor_(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_) {
    SValuePtr retval = SValue::promote(a_->copy());
    uint64_t *r = retval->bits();
    const uint64_t *b = SValue::promote(b_)->bits();
    r[0] ^= b[0];
    r[1] ^= b[1];
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::invert(const BaseSemantics::SValuePtr &a_) {
    SValuePtr retval = SValue::promote(a_->copy());
    BitVectorSupport::invert<uint64_t>(retval->bits(), BitRange::baseSize(0, retval->get_width()));
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::extract(const BaseSemantics::SValuePtr &a_, size_t begin_bit, size_t end_bit) {
    ASSERT_require(end_bit <= a_->get_width());
    ASSERT_require(begin_bit < end_bit);
    SValuePtr retval = svalue_zero(end_bit-begin_bit);
    uint64_t *r = retval->bits();
    const uint64_t *a = SValue::promote(a_)->bits();
    BitVectorSupport::copy<uint64_t>(a, BitRange::hull(begin_bit, end_bit-1), r, BitRange::baseSize(0, end_bit-begin_bit));
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::concat(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_) {
    size_t resultNBits = a_->get_width() + b_->get_width();
    ASSERT_require(resultNBits <= SValue::maxNBits_);
    SValuePtr retval = svalue_zero(a_->get_width() + b_->get_width());
    uint64_t *r = retval->bits();
    const uint64_t *a = SValue::promote(a_)->bits();
    BitVectorSupport::copy<uint64_t>(a, BitRange::baseSize(0, a_->get_width()),
                                     r, BitRange::baseSize(0, a_->get_width()));
    const uint64_t *b = SValue::promote(b_)->bits();
    BitVectorSupport::copy<uint64_t>(b, BitRange::baseSize(0, b_->get_width()),
                                     r, BitRange::baseSize(a_->get_width(), b_->get_width()));
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::leastSignificantSetBit(const BaseSemantics::SValuePtr &a_) {
    SValuePtr retval = svalue_zero(a_->get_width());
    const uint64_t *a = SValue::promote(a_)->bits();
    retval->bits()[0] = BitVectorSupport::leastSignificantSetBit<uint64_t>(a, BitRange::baseSize(0, a_->get_width())).orElse(0);
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::mostSignificantSetBit(const BaseSemantics::SValuePtr &a_) {
    SValuePtr retval = svalue_zero(a_->get_width());
    const uint64_t *a = SValue::promote(a_)->bits();
    retval->bits()[0] = BitVectorSupport::mostSignificantSetBit<uint64_t>(a, BitRange::baseSize(0, a_->get_width())).orElse(0);
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::rotateLeft(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &sa_) {
    SValuePtr retval = SValue::promote(a_->copy());
    BitVectorSupport::rotateLeft<uint64_t>(retval->bits(), BitRange::baseSize(0, retval->get_width()), sa_->get_number());
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::rotateRight(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &sa_) {
    SValuePtr retval = SValue::promote(a_->copy());
    BitVectorSupport::rotateRight<uint64_t>(retval->bits(), BitRange::baseSize(0, retval->get_width()), sa_->get_number());
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::shiftLeft(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &sa_) {
    SValuePtr retval = SValue::promote(a_->copy());
    BitVectorSupport::shiftLeft<uint64_t>(retval->bits(), BitRange::baseSize(0, retval->get_width()), sa_->get_number());
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::shiftRight(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &sa_) {
    SValuePtr retval = SValue::promote(a_->copy());
    BitVectorSupport::shiftRight<uint64_t>(retval->bits(), BitRange::baseSize(0, retval->get_width()), sa_->get_number());
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::shiftRightArithmetic(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &sa_) {
    SValuePtr retval = SValue::promote(a_->copy());
    BitVectorSupport::shiftRightArithmetic<uint64_t>(retval->bits(), BitRange::baseSize(0, retval->get_width()),
                                                     sa_->get_number());
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::equalToZero(const BaseSemantics::SValuePtr &a_) {
    const uint64_t *a = SValue::promote(a_)->bits();
    bool result = a[0]==0 && a[1]==0;
    return svalue_boolean(result);
}

BaseSemantics::SValuePtr
RiscOperators::ite(const BaseSemantics::SValuePtr &sel_, const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_) {
    if (equalToZero(sel_)) {
        return b_->copy();
    } else {
        return a_->copy();
    }
}

BaseSemantics::SValuePtr
RiscOperators::unsignedExtend(const BaseSemantics::SValuePtr &a_, size_t new_width) {
    BaseSemantics::SValuePtr retval = a_->copy();
    retval->set_width(new_width);
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::signExtend(const BaseSemantics::SValuePtr &a_, size_t new_width) {
    ASSERT_require(new_width > 0);
    ASSERT_require(new_width <= SValue::maxNBits_);
    SValuePtr retval = svalue_zero(new_width);
    uint64_t *r = retval->bits();
    const uint64_t *a = SValue::promote(a_)->bits();
    BitVectorSupport::signExtend<uint64_t>(a, BitRange::baseSize(0, a_->get_width()), r, BitRange::baseSize(0, new_width));
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::add(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_) {
    SValuePtr retval = SValue::promote(a_->copy());
    uint64_t *r = retval->bits();
    const uint64_t *b = SValue::promote(b_)->bits();
    BitVectorSupport::add<uint64_t>(b, BitRange::baseSize(0, b_->get_width()), r, BitRange::baseSize(0, retval->get_width()));
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::addWithCarries(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_,
                              const BaseSemantics::SValuePtr &c_, BaseSemantics::SValuePtr &carry_out/*out*/) {
    ASSERT_require(a_->get_width()==b_->get_width());
    ASSERT_require(c_->get_width()==1);
    const uint64_t *a = SValue::promote(a_)->bits();
    const uint64_t *b = SValue::promote(b_)->bits();
    const uint64_t *c = SValue::promote(c_)->bits();
    uint64_t a3[3], b3[3], c3[3], s3[3], x3[3];
    a3[0] = a[0]; a3[1] = a[1]; a3[2] = 0;
    b3[0] = b[0]; b3[1] = b[1]; b3[2] = 0;
    c3[0] = c[0]; c3[1] = c[1]; c3[2] = 0;
    s3[0] = a[0]; s3[1] = a[1]; s3[2] = 0;              // a + b + c
    x3[0] = a[0]; x3[1] = a[1]; x3[2] = 0;              // a ^ b ^ c
    BitRange arange = BitRange::baseSize(0, a_->get_width());
    BitRange erange = BitRange::baseSize(0, a_->get_width()+1);
    BitVectorSupport::add<uint64_t>(b3, erange, s3, erange);
    BitVectorSupport::add<uint64_t>(c3, erange, s3, erange);
    BitVectorSupport::bitwiseXor<uint64_t>(b3, erange, x3, erange);
    BitVectorSupport::bitwiseXor<uint64_t>(s3, erange, x3, erange);

    SValuePtr co = svalue_zero(a_->get_width());
    BitVectorSupport::copy<uint64_t>(x3, BitRange::baseSize(1, a_->get_width()), co->bits(), arange);
    carry_out = co;

    SValuePtr retval = svalue_zero(a_->get_width());
    BitVectorSupport::copy<uint64_t>(s3, arange, retval->bits(), arange);
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::negate(const BaseSemantics::SValuePtr &a_) {
    SValuePtr retval = SValue::promote(a_->copy());
    uint64_t *r = retval->bits();
    BitVectorSupport::negate<uint64_t>(r, BitRange::baseSize(0, retval->get_width()));
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::signedDivide(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_) {
    ASSERT_require2(a_->get_width() <= 64, "not implemented");
    ASSERT_require2(b_->get_width() <= 64, "not implemented");
    int64_t a = IntegerOps::signExtend2(a_->get_number(), a_->get_width(), 64);
    int64_t b = IntegerOps::signExtend2(b_->get_number(), b_->get_width(), 64);
    if (0==b)
        throw BaseSemantics::Exception("division by zero", get_insn());
    return svalue_number(a_->get_width(), a/b);
}

BaseSemantics::SValuePtr
RiscOperators::signedModulo(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_) {
    ASSERT_require2(a_->get_width() <= 64, "not implemented");
    ASSERT_require2(b_->get_width() <= 64, "not implemented");
    int64_t a = IntegerOps::signExtend2(a_->get_number(), a_->get_width(), 64);
    int64_t b = IntegerOps::signExtend2(b_->get_number(), b_->get_width(), 64);
    if (0==b)
        throw BaseSemantics::Exception("division by zero", get_insn());
    return svalue_number(b_->get_width(), a % b);
}

BaseSemantics::SValuePtr
RiscOperators::signedMultiply(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_) {
    ASSERT_require2(a_->get_width() <= 64, "not implemented");
    ASSERT_require2(b_->get_width() <= 64, "not implemented");
    ASSERT_require2(a_->get_width() + b_->get_width() <= 64, "not implemented");
    int64_t a = IntegerOps::signExtend2(a_->get_number(), a_->get_width(), 64);
    int64_t b = IntegerOps::signExtend2(b_->get_number(), b_->get_width(), 64);
    return svalue_number(a_->get_width() + b_->get_width(), a * b);
}

BaseSemantics::SValuePtr
RiscOperators::unsignedDivide(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_) {
    ASSERT_require2(a_->get_width() <= 64, "not implemented");
    ASSERT_require2(b_->get_width() <= 64, "not implemented");
    uint64_t a = a_->get_number();
    uint64_t b = b_->get_number();
    if (0==b)
        throw BaseSemantics::Exception("division by zero", get_insn());
    return svalue_number(a_->get_width(), a/b);
}

BaseSemantics::SValuePtr
RiscOperators::unsignedModulo(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_) {
    ASSERT_require2(a_->get_width() <= 64, "not implemented");
    ASSERT_require2(b_->get_width() <= 64, "not implemented");
    uint64_t a = a_->get_number();
    uint64_t b = b_->get_number();
    if (0==b)
        throw BaseSemantics::Exception("division by zero", get_insn());
    return svalue_number(b_->get_width(), a % b);
}

BaseSemantics::SValuePtr
RiscOperators::unsignedMultiply(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_) {
    ASSERT_require2(a_->get_width() <= 64, "not implemented");
    ASSERT_require2(b_->get_width() <= 64, "not implemented");
    ASSERT_require2(a_->get_width() + b_->get_width() <= 64, "not implemented");
    uint64_t a = a_->get_number();
    uint64_t b = b_->get_number();
    return svalue_number(a_->get_width() + b_->get_width(), a * b);
}

BaseSemantics::SValuePtr
RiscOperators::readMemory(const RegisterDescriptor &segreg, const BaseSemantics::SValuePtr &address,
                          const BaseSemantics::SValuePtr &dflt, const BaseSemantics::SValuePtr &cond) {
    size_t nbits = dflt->get_width();
    ASSERT_require(0 == nbits % 8);
    ASSERT_require(1==cond->get_width()); // FIXME: condition is not used

    // Read the bytes and concatenate them together.
    SValuePtr retval;
    size_t nbytes = nbits/8;
    BaseSemantics::MemoryStatePtr mem = get_state()->get_memory_state();
    for (size_t bytenum=0; bytenum<nbits/8; ++bytenum) {
        size_t byteOffset = ByteOrder::ORDER_MSB==mem->get_byteOrder() ? nbytes-(bytenum+1) : bytenum;
        BaseSemantics::SValuePtr byte_dflt = extract(dflt, 8*byteOffset, 8*byteOffset+8);
        BaseSemantics::SValuePtr byte_addr = add(address, number_(address->get_width(), bytenum));
        SValuePtr byte_value = SValue::promote(state->readMemory(byte_addr, byte_dflt, this, this));
        if (0==bytenum) {
            retval = byte_value;
        } else if (ByteOrder::ORDER_MSB==mem->get_byteOrder()) {
            retval = SValue::promote(concat(byte_value, retval));
        } else {
            retval = SValue::promote(concat(retval, byte_value));
        }
    }

    ASSERT_require(retval!=NULL && retval->get_width()==nbits);
    return retval;
}

void
RiscOperators::writeMemory(const RegisterDescriptor &segreg, const BaseSemantics::SValuePtr &address,
                           const BaseSemantics::SValuePtr &value_, const BaseSemantics::SValuePtr &cond) {
    SValuePtr value = SValue::promote(value_->copy());
    size_t nbits = value->get_width();
    ASSERT_require(0 == nbits % 8);
    ASSERT_require(1==cond->get_width()); // FIXME: condition is not used
    size_t nbytes = nbits/8;
    BaseSemantics::MemoryStatePtr mem = get_state()->get_memory_state();
    for (size_t bytenum=0; bytenum<nbytes; ++bytenum) {
        size_t byteOffset = ByteOrder::ORDER_MSB==mem->get_byteOrder() ? nbytes-(bytenum+1) : bytenum;
        BaseSemantics::SValuePtr byte_value = extract(value, 8*byteOffset, 8*byteOffset+8);
        BaseSemantics::SValuePtr byte_addr = add(address, number_(address->get_width(), bytenum));
        state->writeMemory(byte_addr, byte_value, this, this);
    }
}

} // namespace
} // namespace
} // namespace
} // namespace
