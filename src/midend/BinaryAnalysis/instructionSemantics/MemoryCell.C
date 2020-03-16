#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include <sage3basic.h>
#include <MemoryCell.h>

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics2 {
namespace BaseSemantics {

bool
MemoryCell::NonWrittenCells::operator()(const MemoryCellPtr &cell) const {
    return cell->getWriters().isEmpty();
}

bool
MemoryCell::may_alias(const MemoryCellPtr &other, RiscOperators *addrOps) const
{
    // Check for the easy case:  two one-byte cells may alias one another if their addresses may be equal.
    if (8==value_->get_width() && 8==other->get_value()->get_width())
        return address_->may_equal(other->get_address(), addrOps->solver());

    size_t addr_nbits = address_->get_width();
    ASSERT_require(other->get_address()->get_width()==addr_nbits);

    ASSERT_require(value_->get_width() % 8 == 0);       // memory is byte addressable, so values must be multiples of a byte
    SValuePtr lo1 = address_;
    SValuePtr hi1 = addrOps->add(lo1, addrOps->number_(lo1->get_width(), value_->get_width() / 8));

    ASSERT_require(other->get_value()->get_width() % 8 == 0);
    SValuePtr lo2 = other->get_address();
    SValuePtr hi2 = addrOps->add(lo2, addrOps->number_(lo2->get_width(), other->get_value()->get_width() / 8));

    // Two cells may_alias iff we can prove that they are not disjoint.  The two cells are disjoint iff lo2 >= hi1 or lo1 >=
    // hi2. Two things complicate this: first, the values might not be known quantities, depending on the semantic domain.
    // Second, the RiscOperators does not define a greater-than-or-equal operation, so we need to write it in terms of a
    // subtraction. See x86 CMP and JG instructions for examples. ("sf" is sign flag, "of" is overflow flag.)
    SValuePtr carries;
    SValuePtr diff = addrOps->addWithCarries(lo2, addrOps->invert(hi1), addrOps->boolean_(true), carries/*out*/);
    SValuePtr sf = addrOps->extract(diff, addr_nbits-1, addr_nbits);
    SValuePtr of = addrOps->xor_(addrOps->extract(carries, addr_nbits-1, addr_nbits),
                                 addrOps->extract(carries, addr_nbits-2, addr_nbits-1));
    SValuePtr cond1 = addrOps->invert(addrOps->xor_(sf, of));
    diff = addrOps->addWithCarries(lo1, addrOps->invert(hi2), addrOps->boolean_(true), carries/*out*/);
    sf = addrOps->extract(diff, addr_nbits-1, addr_nbits);
    of = addrOps->xor_(addrOps->extract(carries, addr_nbits-1, addr_nbits),
                       addrOps->extract(carries, addr_nbits-2, addr_nbits-1));
    SValuePtr cond2 = addrOps->invert(addrOps->xor_(sf, of));
    SValuePtr disjoint = addrOps->or_(cond1, cond2);
    if (disjoint->is_number() && disjoint->get_number()!=0)
        return false;
    return true;
}

bool
MemoryCell::must_alias(const MemoryCellPtr &other, RiscOperators *addrOps) const
{
    // Check the easy case: two one-byte cells must alias one another if their address must be equal.
    if (8==value_->get_width() && 8==other->get_value()->get_width())
        return address_->must_equal(other->get_address(), addrOps->solver());

    size_t addr_nbits = address_->get_width();
    ASSERT_require(other->get_address()->get_width()==addr_nbits);

    ASSERT_require(value_->get_width() % 8 == 0);
    SValuePtr lo1 = address_;
    SValuePtr hi1 = addrOps->add(lo1, addrOps->number_(lo1->get_width(), value_->get_width() / 8));

    ASSERT_require(other->get_value()->get_width() % 8 == 0);
    SValuePtr lo2 = other->get_address();
    SValuePtr hi2 = addrOps->add(lo2, addrOps->number_(lo2->get_width(), other->get_value()->get_width() / 8));

    // Two cells must_alias iff hi2 >= lo1 and hi1 >= lo2. Two things complicate this: first, the values might not be known
    // quantities, depending on the semantic domain.  Second, the RiscOperators does not define a greater-than-or-equal
    // operation, so we need to write it in terms of a subtraction. See x86 CMP and JG instructions for examples. ("sf" is sign
    // flag, "of" is overflow flag.)
    SValuePtr carries;
    SValuePtr diff = addrOps->addWithCarries(hi2, addrOps->invert(lo1), addrOps->boolean_(true), carries/*out*/);
    SValuePtr sf = addrOps->extract(diff, addr_nbits-1, addr_nbits);
    SValuePtr of = addrOps->xor_(addrOps->extract(carries, addr_nbits-1, addr_nbits),
                                 addrOps->extract(carries, addr_nbits-2, addr_nbits-1));
    SValuePtr cond1 = addrOps->invert(addrOps->xor_(sf, of));
    diff = addrOps->addWithCarries(hi1, addrOps->invert(lo2), addrOps->boolean_(true), carries/*out*/);
    sf = addrOps->extract(diff, addr_nbits-1, addr_nbits);
    of = addrOps->xor_(addrOps->extract(carries, addr_nbits-1, addr_nbits),
                       addrOps->extract(carries, addr_nbits-2, addr_nbits-1));
    SValuePtr cond2 = addrOps->invert(addrOps->xor_(sf, of));
    SValuePtr overlap = addrOps->and_(cond1, cond2);
    if (overlap->is_number() && overlap->get_number()!=0)
        return true;
    return false;
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

#endif
