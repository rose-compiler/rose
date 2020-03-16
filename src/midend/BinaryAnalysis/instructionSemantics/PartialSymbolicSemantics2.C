#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include "sage3basic.h"
#include "PartialSymbolicSemantics2.h"

#include "CommandLine.h"

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics2 {
namespace PartialSymbolicSemantics {

uint64_t name_counter;

uint64_t
Formatter::rename(uint64_t orig_name)
{
    if (0==orig_name)
        return orig_name;
    std::pair<Map::iterator, bool> inserted = renames.insert(std::make_pair(orig_name, next_name));
    if (!inserted.second)
        return orig_name;
    return next_name++;
}

/*******************************************************************************************************************************
 *                                      SValue
 *******************************************************************************************************************************/

Sawyer::Optional<BaseSemantics::SValuePtr>
SValue::createOptionalMerge(const BaseSemantics::SValuePtr &other_, const BaseSemantics::MergerPtr &merger,
                            const SmtSolverPtr &solver) const {
    if (must_equal(other_, solver))
        return Sawyer::Nothing();
    return bottom_(get_width());
}

bool
SValue::may_equal(const BaseSemantics::SValuePtr &other_, const SmtSolverPtr &solver) const 
{
    SValuePtr other = promote(other_);
    if (must_equal(other, solver))
        return true;
    return this->name!=0 || other->name!=0;
}

bool
SValue::must_equal(const BaseSemantics::SValuePtr &other_, const SmtSolverPtr &solver) const
{
    SValuePtr other = promote(other_);
    return (this->name==other->name &&
            (!this->name || this->negate==other->negate) &&
            this->offset==other->offset);
}

void
SValue::print(std::ostream &stream, BaseSemantics::Formatter &formatter_) const
{
    FormatRestorer restorer(stream); // restore format flags when we leave this scope
    uint64_t sign_bit = (uint64_t)1 << (get_width()-1); /* e.g., 80000000 */
    uint64_t val_mask = sign_bit - 1;             /* e.g., 7fffffff */
    /*magnitude of negative value*/
    uint64_t negative = get_width()>1 && (offset & sign_bit) ? (~offset & val_mask) + 1 : 0;
    Formatter *formatter = dynamic_cast<Formatter*>(&formatter_);

    if (name!=0) {
        /* This is a named value rather than a constant. */
        uint64_t renamed = formatter ? formatter->rename(name) : name;
        const char *sign = negate ? "-" : "";
        stream <<sign <<"v" <<std::dec <<renamed;
        if (negative) {
            stream <<"-0x" <<std::hex <<negative;
        } else if (offset) {
            stream <<"+0x" <<std::hex <<offset;
        }
    } else {
        /* This is a constant */
        ROSE_ASSERT(!negate);
        stream <<"0x" <<std::hex <<offset;
        if (negative)
            stream <<" (-0x" <<std::hex <<negative <<")";
    }

    stream <<"[" <<std::dec <<get_width() <<"]";
}


/*******************************************************************************************************************************
 *                                      State
 *******************************************************************************************************************************/

void
State::print_diff_registers(std::ostream &o, const StatePtr &other_state, Formatter &fmt) const
{
    ASSERT_not_implemented("[Robb P. Matzke 2014-10-07]");
}

bool
State::equal_registers(const StatePtr &other) const
{
    ASSERT_not_implemented("[Robb P. Matzke 2014-10-07]");
    return false;
}

void
State::discard_popped_memory()
{
    ASSERT_not_implemented("[Robb P. Matzke 2014-10-07]");
}


/*******************************************************************************************************************************
 *                                      RISC Operators
 *******************************************************************************************************************************/

RiscOperatorsPtr
RiscOperators::instance(const RegisterDictionary *regdict)
{
    BaseSemantics::SValuePtr protoval = SValue::instance();
#if defined(__GNUC__)
#if __GNUC__==4 && __GNUC_MINOR__==2
    // GCC 4.2.4 may have problems optimizing the function.  It was originally [2014-07] defined in the header file where an
    // extraneous volatile read from the protoval reference counter defeated the bug.  Later [2014-10] the proval pointer
    // mechanism was changed to use Sawyer::SharedPointer and the volatile read was no longer an option, but adding extra
    // function calls (like 'std::cerr <<ownershipCount(protoval) <<"\n"') defeated the bug.  Eventually [2014-10] this
    // function was moved from the header to the .C file which seems to defeat the bug without the need for volatile reads or
    // extra function calls.
#endif
#endif
    BaseSemantics::RegisterStatePtr registers = RegisterState::instance(protoval, regdict);
    MemoryStatePtr memory = MemoryState::instance(protoval, protoval);
    memory->byteRestricted(false); // because extracting bytes from a word results in new variables for this domain
    BaseSemantics::StatePtr state = State::instance(registers, memory);
    SmtSolverPtr solver = SmtSolver::instance(Rose::CommandLine::genericSwitchArgs.smtSolver);
    RiscOperatorsPtr ops = RiscOperatorsPtr(new RiscOperators(state, solver));
    return ops;
}

void
RiscOperators::interrupt(int majr, int minr)
{
    currentState()->clear();
}

BaseSemantics::SValuePtr
RiscOperators::and_(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    ASSERT_require(a->get_width()==b->get_width());
    if ((!a->name && 0==a->offset) || (!b->name && 0==b->offset))
        return number_(a->get_width(), 0);
    if (a->name || b->name)
        return undefined_(a->get_width());
    return number_(a->get_width(), a->offset & b->offset);
}

BaseSemantics::SValuePtr
RiscOperators::or_(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    ASSERT_require(a->get_width()==b->get_width());
    if (a->must_equal(b))
        return a->copy();
    if (!a->name && !b->name)
        return number_(a->get_width(), a->offset | b->offset);
    if (!a->name && a->offset==IntegerOps::genMask<uint64_t>(a->get_width()))
        return a->copy();
    if (!b->name && b->offset==IntegerOps::genMask<uint64_t>(a->get_width()))
        return b->copy();
    return undefined_(a->get_width());
}

BaseSemantics::SValuePtr
RiscOperators::xor_(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    ASSERT_require(a->get_width()==b->get_width());
    if (!a->name && !b->name)
        return number_(a->get_width(), a->offset ^ b->offset);
    if (a->must_equal(b))
        return number_(a->get_width(), 0);
    if (!b->name) {
        if (0==b->offset)
            return a->copy();
        if (b->offset==IntegerOps::genMask<uint64_t>(a->get_width()))
            return invert(a);
    }
    if (!a->name) {
        if (0==a->offset)
            return b->copy();
        if (a->offset==IntegerOps::genMask<uint64_t>(a->get_width()))
            return invert(b);
    }
    return undefined_(a->get_width());
}

BaseSemantics::SValuePtr
RiscOperators::invert(const BaseSemantics::SValuePtr &a_)
{
    SValuePtr a = SValue::promote(a_);
    if (a->name)
        return a->create(a->get_width(), a->name, ~a->offset, !a->negate);
    return number_(a->get_width(), ~a->offset);
}

BaseSemantics::SValuePtr
RiscOperators::extract(const BaseSemantics::SValuePtr &a_, size_t begin_bit, size_t end_bit)
{
    SValuePtr a = SValue::promote(a_);
    ASSERT_require(end_bit<=a->get_width());
    ASSERT_require(begin_bit<end_bit);
    if (0==begin_bit) {
        if (end_bit==a->get_width())
            return a->copy();
        return a->copy(end_bit);
    }
    if (a->name)
        return undefined_(end_bit-begin_bit);
    return number_(end_bit-begin_bit, (a->offset >> begin_bit) & IntegerOps::genMask<uint64_t>(end_bit-begin_bit));
}

BaseSemantics::SValuePtr
RiscOperators::concat(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    if (a->name || b->name)
        return undefined_(a->get_width() + b->get_width());
    return number_(a->get_width()+b->get_width(), a->offset | (b->offset << a->get_width()));
}

BaseSemantics::SValuePtr
RiscOperators::equalToZero(const BaseSemantics::SValuePtr &a_)
{
    SValuePtr a = SValue::promote(a_);
    if (a->name)
        return undefined_(1);
    return a->offset ? boolean_(false) : boolean_(true);
}

BaseSemantics::SValuePtr
RiscOperators::ite(const BaseSemantics::SValuePtr &sel_,
                   const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr sel = SValue::promote(sel_);
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    ASSERT_require(1==sel->get_width());
    ASSERT_require(a->get_width()==b->get_width());
    if (a->must_equal(b))
        return a->copy();
    if (sel->name)
        return undefined_(a->get_width());
    return sel->offset ? a->copy() : b->copy();
}

BaseSemantics::SValuePtr
RiscOperators::leastSignificantSetBit(const BaseSemantics::SValuePtr &a_)
{
    SValuePtr a = SValue::promote(a_);
    if (a->name)
        return undefined_(a->get_width());
    for (size_t i=0; i<a->get_width(); ++i) {
        if (a->offset & ((uint64_t)1 << i))
            return number_(a->get_width(), i);
    }
    return number_(a->get_width(), 0);
}

BaseSemantics::SValuePtr
RiscOperators::mostSignificantSetBit(const BaseSemantics::SValuePtr &a_)
{
    SValuePtr a = SValue::promote(a_);
    if (a->name)
        return undefined_(a->get_width());
    for (size_t i=a->get_width(); i>0; --i) {
        if (a->offset & ((uint64_t)1 << (i-1)))
            return number_(a->get_width(), i-1);
    }
    return number_(a->get_width(), 0);
}

BaseSemantics::SValuePtr
RiscOperators::rotateLeft(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &sa_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr sa = SValue::promote(sa_);
    if (!a->name && !sa->name)
        return number_(a->get_width(), IntegerOps::rotateLeft2(a->offset, sa->offset, a->get_width()));
    if (!sa->name && 0==sa->offset % a->get_width())
        return a->copy();
    return undefined_(a->get_width());
}

BaseSemantics::SValuePtr
RiscOperators::rotateRight(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &sa_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr sa = SValue::promote(sa_);
    if (!a->name && !sa->name) {
        return number_(a->get_width(), IntegerOps::rotateRight2(a->offset, sa->offset, a->get_width()));
        size_t count = sa->offset % a->get_width();
        uint64_t n = (a->offset >> count) | (a->offset << (a->get_width()-count));
        return number_(a->get_width(), n);
    }
    if (!sa->name && 0==sa->offset % a->get_width())
        return a->copy();
    return undefined_(a->get_width());
}

BaseSemantics::SValuePtr
RiscOperators::shiftLeft(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &sa_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr sa = SValue::promote(sa_);
    if (!a->name && !sa->name)
        return number_(a->get_width(), IntegerOps::shiftLeft2(a->offset, sa->offset, a->get_width()));
    if (!sa->name) {
        if (0==sa->offset)
            return a->copy();
        if (sa->offset>=a->get_width())
            return number_(a->get_width(), 0);
    }
    return undefined_(a->get_width());
}

BaseSemantics::SValuePtr
RiscOperators::shiftRight(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &sa_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr sa = SValue::promote(sa_);
    if (!sa->name) {
        if (sa->offset>a->get_width())
            return number_(a->get_width(), 0);
        if (0==sa->offset)
            return a->copy();
    }
    if (!a->name && !sa->name)
        return number_(a->get_width(), IntegerOps::shiftRightLogical2(a->offset, sa->offset, a->get_width()));
    return undefined_(a->get_width());
}

BaseSemantics::SValuePtr
RiscOperators::shiftRightArithmetic(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &sa_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr sa = SValue::promote(sa_);
    if (!sa->name && 0==sa->offset)
        return a->copy();
    if (!a->name && !sa->name)
        return number_(a->get_width(), IntegerOps::shiftRightArithmetic2(a->offset, sa->offset, a->get_width()));
    return undefined_(a->get_width());
}

BaseSemantics::SValuePtr
RiscOperators::add(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    ASSERT_require(a->get_width()==b->get_width());
    if (a->name==b->name && (!a->name || a->negate!=b->negate)) {
        /* [V1+x] + [-V1+y] = [x+y]  or
         * [x] + [y] = [x+y] */
        return number_(a->get_width(), a->offset + b->offset);
    } else if (!a->name || !b->name) {
        /* [V1+x] + [y] = [V1+x+y]   or
         * [x] + [V2+y] = [V2+x+y]   or
         * [-V1+x] + [y] = [-V1+x+y] or
         * [x] + [-V2+y] = [-V2+x+y] */
        return a->create(a->get_width(), a->name+b->name, a->offset+b->offset, a->negate || b->negate);
    } else {
        return undefined_(a->get_width());
    }
}

BaseSemantics::SValuePtr
RiscOperators::addWithCarries(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_,
                              const BaseSemantics::SValuePtr &c_, BaseSemantics::SValuePtr &carry_out/*out*/)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    SValuePtr c = SValue::promote(c_);
    ASSERT_require(a->get_width()==b->get_width() && c->get_width()==1);
    int n_unknown = (a->name?1:0) + (b->name?1:0) + (c->name?1:0);
    if (n_unknown <= 1) {
        /* At most, one of the operands is an unknown value. See add() for more details. */
        uint64_t sum = a->offset + b->offset + c->offset;
        if (0==n_unknown) {
            carry_out = number_(a->get_width(), (a->offset ^ b->offset ^ sum)>>1);
        } else {
            carry_out = undefined_(a->get_width());
        }
        return a->create(a->get_width(), a->name+b->name+c->name, sum, a->negate||b->negate||c->negate);
    } else if (a->name==b->name && !c->name && a->negate!=b->negate) {
        /* A and B are known or have bases that cancel out, and C is known */
        uint64_t sum = a->offset + b->offset + c->offset;
        carry_out = number_(a->get_width(), (a->offset ^ b->offset ^ sum)>>1);
        return number_(a->get_width(), sum);
    } else {
        carry_out = undefined_(a->get_width());
        return undefined_(a->get_width());
    }
}

BaseSemantics::SValuePtr
RiscOperators::negate(const BaseSemantics::SValuePtr &a_)
{
    SValuePtr a = SValue::promote(a_);
    if (a->name)
        return a->create(a->get_width(), a->name, -a->offset, !a->negate);
    return number_(a->get_width(), -a->offset);
}

BaseSemantics::SValuePtr
RiscOperators::signedDivide(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    if (!b->name) {
        if (0==b->offset)
            throw BaseSemantics::Exception("division by zero", currentInstruction());
        if (!a->name)
            return number_(a->get_width(),
                           (IntegerOps::signExtend2(a->offset, a->get_width(), 64) /
                            IntegerOps::signExtend2(b->offset, b->get_width(), 64)));
        if (1==b->offset)
            return a->copy();
        if (b->offset==IntegerOps::genMask<uint64_t>(b->get_width()))
            return negate(a);
        /*FIXME: also possible to return zero if B is large enough. [RPM 2010-05-18]*/
    }
    return undefined_(a->get_width());
}

BaseSemantics::SValuePtr
RiscOperators::signedModulo(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    if (a->name || b->name)
        return undefined_(b->get_width());
    if (0==b->offset)
        throw BaseSemantics::Exception("division by zero", currentInstruction());
    return number_(b->get_width(),
                   (IntegerOps::signExtend2(a->offset, a->get_width(), 64) %
                    IntegerOps::signExtend2(b->offset, b->get_width(), 64)));
    /* FIXME: More folding possibilities... if 'b' is a power of two then we can return 'a' with the bitsize of 'b'. */
}

BaseSemantics::SValuePtr
RiscOperators::signedMultiply(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    size_t retwidth = a->get_width() + b->get_width();
    if (!a->name && !b->name)
        return number_(retwidth,
                       (IntegerOps::signExtend2(a->offset, a->get_width(), 64) *
                        IntegerOps::signExtend2(b->offset, b->get_width(), 64)));
    if (!b->name) {
        if (0==b->offset)
            return number_(retwidth, 0);
        if (1==b->offset)
            return signExtend(a, retwidth);
        if (b->offset==IntegerOps::genMask<uint64_t>(b->get_width()))
            return signExtend(negate(a), retwidth);
    }
    if (!a->name) {
        if (0==a->offset)
            return number_(retwidth, 0);
        if (1==a->offset)
            return signExtend(b, retwidth);
        if (a->offset==IntegerOps::genMask<uint64_t>(a->get_width()))
            return signExtend(negate(b), retwidth);
    }
    return undefined_(retwidth);
}

BaseSemantics::SValuePtr
RiscOperators::unsignedDivide(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    if (!b->name) {
        if (0==b->offset)
            throw BaseSemantics::Exception("division by zero", currentInstruction());
        if (!a->name)
            return number_(a->get_width(), a->offset / b->offset);
        if (1==b->offset)
            return a->copy();
        /*FIXME: also possible to return zero if B is large enough. [RPM 2010-05-18]*/
    }
    return undefined_(a->get_width());
}

BaseSemantics::SValuePtr
RiscOperators::unsignedModulo(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    if (!b->name) {
        if (0==b->offset)
            throw BaseSemantics::Exception("division by zero", currentInstruction());
        if (!a->name)
            return number_(b->get_width(), a->offset % b->offset);
        /* FIXME: More folding possibilities... if 'b' is a power of two then we can return 'a' with the
         * bitsize of 'b'. */
    }
    SValuePtr a2 = SValue::promote(unsignedExtend(a, 64));
    SValuePtr b2 = SValue::promote(unsignedExtend(b, 64));
    if (a2->must_equal(b2))
        return b->copy();
    return undefined_(b->get_width());
}

BaseSemantics::SValuePtr
RiscOperators::unsignedMultiply(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    size_t retwidth = a->get_width() + b->get_width();
    if (!a->name && !b->name)
        return number_(retwidth, a->offset * b->offset);
    if (!b->name) {
        if (0==b->offset)
            return number_(retwidth, 0);
        if (1==b->offset)
            return unsignedExtend(a, retwidth);
    }
    if (!a->name) {
        if (0==a->offset)
            return number_(retwidth, 0);
        if (1==a->offset)
            return unsignedExtend(b, retwidth);
    }
    return undefined_(retwidth);
}

BaseSemantics::SValuePtr
RiscOperators::signExtend(const BaseSemantics::SValuePtr &a_, size_t new_width)
{
    SValuePtr a = SValue::promote(a_);
    if (new_width==a->get_width())
        return a->copy();
    if (a->name)
        return undefined_(new_width);
    return number_(new_width, IntegerOps::signExtend2(a->offset, a->get_width(), new_width));
}

void
RiscOperators::writeMemory(RegisterDescriptor segreg,
                           const BaseSemantics::SValuePtr &address,
                           const BaseSemantics::SValuePtr &value,
                           const BaseSemantics::SValuePtr &condition)
{
#ifndef NDEBUG
    size_t nbits = value->get_width();
    ASSERT_require2(nbits % 8 == 0, "write to memory must be in byte units");
    ASSERT_require(1==condition->get_width()); // FIXME: condition is not used
#endif
    if (condition->is_number() && !condition->get_number())
        return;

    // PartialSymbolicSemantics assumes that its memory state is capable of storing multi-byte values.
    currentState()->writeMemory(address, value, this, this);
}
    
BaseSemantics::SValuePtr
RiscOperators::readOrPeekMemory(RegisterDescriptor segreg,
                                const BaseSemantics::SValuePtr &address,
                                const BaseSemantics::SValuePtr &dflt_,
                                bool allowSideEffects) {
    BaseSemantics::SValuePtr dflt = dflt_;
    size_t nbits = dflt->get_width();
    ASSERT_require2(nbits % 8 == 0, "read from memory must be in byte units");

    // Use the initial memory state if there is one.
    if (initialState()) {
        if (allowSideEffects) {
            dflt = initialState()->readMemory(address, dflt, this, this);
        } else {
            dflt = initialState()->peekMemory(address, dflt, this, this);
        }
    }
    
    // Use the concrete MemoryMap if there is one.  Only those areas of the map that are readable and not writable are used.
    if (map && address->is_number()) {
        size_t nbytes = nbits/8;
        uint8_t *buf = new uint8_t[nbytes];
        size_t nread = map->require(MemoryMap::READABLE).prohibit(MemoryMap::WRITABLE)
                       .at(address->get_number()).limit(nbytes).read(buf).size();
        if (nread == nbytes) {
            if (nbytes > 1 && map->byteOrder() == ByteOrder::ORDER_UNSPECIFIED)
                throw BaseSemantics::Exception("multi-byte read with memory having unspecified byte order", currentInstruction());
            ByteOrder::convert(buf, nbytes, map->byteOrder(), ByteOrder::ORDER_LSB);
            uint64_t dflt_val = 0;
            for (size_t i=0; i<nbytes; ++i)
                dflt_val |= IntegerOps::shiftLeft2<uint64_t>(buf[i], 8*i);
            dflt = number_(nbits, dflt_val);
        }
        delete [] buf;
    }
    
    // PartialSymbolicSemantics assumes that its memory state is capable of storing multi-byte values.
    SValuePtr retval = SValue::promote(currentState()->readMemory(address, dflt, this, this));
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::readMemory(RegisterDescriptor segreg,
                          const BaseSemantics::SValuePtr &address,
                          const BaseSemantics::SValuePtr &dflt,
                          const BaseSemantics::SValuePtr &condition)
{
    ASSERT_require(1==condition->get_width()); // FIXME: condition is not used
    if (condition->is_number() && !condition->get_number())
        return dflt;
    return readOrPeekMemory(segreg, address, dflt, true /*allow side effects*/);
}

BaseSemantics::SValuePtr
RiscOperators::peekMemory(RegisterDescriptor segreg,
                          const BaseSemantics::SValuePtr &address,
                          const BaseSemantics::SValuePtr &dflt)
{
    return readOrPeekMemory(segreg, address, dflt, false /*no side effects allowed*/);
}

} // namespace
} // namespace
} // namespace
} // namespace

#endif
