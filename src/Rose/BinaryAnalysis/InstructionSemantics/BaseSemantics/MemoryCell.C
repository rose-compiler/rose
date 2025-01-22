#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/MemoryCell.h>

#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/Formatter.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/RiscOperators.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/SValue.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/Utility.h>
#include <Rose/StringUtility/NumberToString.h>

#include <boost/enable_shared_from_this.hpp>

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics {
namespace BaseSemantics {

MemoryCell::MemoryCell() {}

MemoryCell::MemoryCell(const SValue::Ptr &address, const SValue::Ptr &value)
    : address_(address), value_(value) {
    ASSERT_not_null(address);
    ASSERT_not_null(value);
}

MemoryCell::MemoryCell(const MemoryCell &other)
    : boost::enable_shared_from_this<MemoryCell>(other) {
    address_ = other.address_->copy();
    value_ = other.value_->copy();
    writers_ = other.writers_;
    ioProperties_ = other.ioProperties_;
    position_ = other.position_;
}

MemoryCell::~MemoryCell() {}

SValue::Ptr
MemoryCell::address() const {
    return address_;
}

void
MemoryCell::address(const SValue::Ptr &addr) {
    ASSERT_not_null(addr);
    address_ = addr;
}

SValue::Ptr
MemoryCell::value() const {
    return value_;
}

void
MemoryCell::value(const SValue::Ptr &v) {
    ASSERT_not_null(v);
    value_ = v;
}

bool
MemoryCell::NonWrittenCells::operator()(const MemoryCell::Ptr &cell) {
    return cell->getWriters().isEmpty();
}

bool
MemoryCell::mayAlias(const MemoryCell::Ptr &other, RiscOperators *addrOps) const
{
    // Check for the easy case:  two one-byte cells may alias one another if their addresses may be equal.
    if (8==value_->nBits() && 8==other->value()->nBits())
        return address_->mayEqual(other->address(), addrOps->solver());

    size_t addr_nbits = address_->nBits();
    ASSERT_require(other->address()->nBits()==addr_nbits);

    ASSERT_require(value_->nBits() % 8 == 0);       // memory is byte addressable, so values must be multiples of a byte
    SValue::Ptr lo1 = address_;
    SValue::Ptr hi1 = addrOps->add(lo1, addrOps->number_(lo1->nBits(), value_->nBits() / 8));

    ASSERT_require(other->value()->nBits() % 8 == 0);
    SValue::Ptr lo2 = other->address();
    SValue::Ptr hi2 = addrOps->add(lo2, addrOps->number_(lo2->nBits(), other->value()->nBits() / 8));

    // Two cells may_alias iff we can prove that they are not disjoint.  The two cells are disjoint iff lo2 >= hi1 or lo1 >=
    // hi2. Two things complicate this: first, the values might not be known quantities, depending on the semantic domain.
    // Second, the RiscOperators does not define a greater-than-or-equal operation, so we need to write it in terms of a
    // subtraction. See x86 CMP and JG instructions for examples. ("sf" is sign flag, "of" is overflow flag.)
    SValue::Ptr carries;
    SValue::Ptr diff = addrOps->addWithCarries(lo2, addrOps->invert(hi1), addrOps->boolean_(true), carries/*out*/);
    SValue::Ptr sf = addrOps->extract(diff, addr_nbits-1, addr_nbits);
    SValue::Ptr of = addrOps->xor_(addrOps->extract(carries, addr_nbits-1, addr_nbits),
                                 addrOps->extract(carries, addr_nbits-2, addr_nbits-1));
    SValue::Ptr cond1 = addrOps->invert(addrOps->xor_(sf, of));
    diff = addrOps->addWithCarries(lo1, addrOps->invert(hi2), addrOps->boolean_(true), carries/*out*/);
    sf = addrOps->extract(diff, addr_nbits-1, addr_nbits);
    of = addrOps->xor_(addrOps->extract(carries, addr_nbits-1, addr_nbits),
                       addrOps->extract(carries, addr_nbits-2, addr_nbits-1));
    SValue::Ptr cond2 = addrOps->invert(addrOps->xor_(sf, of));
    SValue::Ptr disjoint = addrOps->or_(cond1, cond2);
    return !disjoint->isTrue();
}

bool
MemoryCell::mustAlias(const MemoryCell::Ptr &other, RiscOperators *addrOps) const
{
    // Check the easy case: two one-byte cells must alias one another if their address must be equal.
    if (8==value_->nBits() && 8==other->value()->nBits())
        return address_->mustEqual(other->address(), addrOps->solver());

    size_t addr_nbits = address_->nBits();
    ASSERT_require(other->address()->nBits()==addr_nbits);

    ASSERT_require(value_->nBits() % 8 == 0);
    SValue::Ptr lo1 = address_;
    SValue::Ptr hi1 = addrOps->add(lo1, addrOps->number_(lo1->nBits(), value_->nBits() / 8));

    ASSERT_require(other->value()->nBits() % 8 == 0);
    SValue::Ptr lo2 = other->address();
    SValue::Ptr hi2 = addrOps->add(lo2, addrOps->number_(lo2->nBits(), other->value()->nBits() / 8));

    // Two cells must_alias iff hi2 >= lo1 and hi1 >= lo2. Two things complicate this: first, the values might not be known
    // quantities, depending on the semantic domain.  Second, the RiscOperators does not define a greater-than-or-equal
    // operation, so we need to write it in terms of a subtraction. See x86 CMP and JG instructions for examples. ("sf" is sign
    // flag, "of" is overflow flag.)
    SValue::Ptr carries;
    SValue::Ptr diff = addrOps->addWithCarries(hi2, addrOps->invert(lo1), addrOps->boolean_(true), carries/*out*/);
    SValue::Ptr sf = addrOps->extract(diff, addr_nbits-1, addr_nbits);
    SValue::Ptr of = addrOps->xor_(addrOps->extract(carries, addr_nbits-1, addr_nbits),
                                 addrOps->extract(carries, addr_nbits-2, addr_nbits-1));
    SValue::Ptr cond1 = addrOps->invert(addrOps->xor_(sf, of));
    diff = addrOps->addWithCarries(hi1, addrOps->invert(lo2), addrOps->boolean_(true), carries/*out*/);
    sf = addrOps->extract(diff, addr_nbits-1, addr_nbits);
    of = addrOps->xor_(addrOps->extract(carries, addr_nbits-1, addr_nbits),
                       addrOps->extract(carries, addr_nbits-2, addr_nbits-1));
    SValue::Ptr cond2 = addrOps->invert(addrOps->xor_(sf, of));
    SValue::Ptr overlap = addrOps->and_(cond1, cond2);
    return overlap->isTrue();
}

void
MemoryCell::hash(Combinatorics::Hasher &hasher) const {
    ASSERT_not_null(address_);
    address_->hash(hasher);
    ASSERT_not_null(value_);
    value_->hash(hasher);
}

void
MemoryCell::print(std::ostream &stream) const {
    Formatter fmt;
    print(stream, fmt);
}

void
MemoryCell::print(std::ostream &stream, Formatter &fmt) const
{
    stream <<"addr=" <<(*address_+fmt);

    if (fmt.get_show_latest_writers()) {
        const AddressSet &writers = getWriters();
        if (writers.isEmpty()) {
            // nothing to show
        } else if (writers.size() == 1) {
            stream <<" writer=" <<StringUtility::addrToString(*writers.values().begin());
        } else {
            stream <<" writers=[";
            for (AddressSet::ConstIterator iter=writers.values().begin(); iter!=writers.values().end(); ++iter) {
                stream <<(iter==writers.values().begin() ? "" : ", ")
                       <<StringUtility::addrToString(*iter);
            }
            stream <<"]";
        }
    }

    // FIXME[Robb P. Matzke 2015-08-12]: This doesn't take into account all combinations of properties -- just a few of the
    // more common ones.
    if (fmt.get_show_properties()) {
        if (ioProperties_.exists(IO_READ_BEFORE_WRITE)) {
            stream <<" read-before-write";
        } else if (ioProperties_.exists(IO_WRITE) && ioProperties_.exists(IO_READ)) {
            // nothing
        } else if (ioProperties_.exists(IO_READ)) {
            stream <<" read-only";
        } else if (ioProperties_.exists(IO_WRITE)) {
            stream <<" write-only";
        }
    }

    stream <<" value=" <<(*value_+fmt);
}

MemoryCell::WithFormatter
MemoryCell::with_format(Formatter &fmt) {
    return WithFormatter(shared_from_this(), fmt);
}

MemoryCell::WithFormatter
MemoryCell::operator+(Formatter &fmt) {
    return with_format(fmt);
}

MemoryCell::WithFormatter
MemoryCell::operator+(const std::string &linePrefix) {
    static Formatter fmt;
    fmt.set_line_prefix(linePrefix);
    return with_format(fmt);
}

void
MemoryCell::setWriter(rose_addr_t writerVa) {
    eraseWriters();
    writers_.insert(writerVa);
}

std::ostream& operator<<(std::ostream &o, const MemoryCell &x) {
    x.print(o);
    return o;
}

std::ostream& operator<<(std::ostream &o, const MemoryCell::WithFormatter &x) {
    x.print(o);
    return o;
}

} // namespace
} // namespace
} // namespace
} // namespace

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics::MemoryCell);
#endif

#endif
