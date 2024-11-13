#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/InstructionSemantics/PartialSymbolicSemantics.h>

#include <Rose/As.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/Exception.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/Merger.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/RegisterStateGeneric.h>
#include <Rose/BinaryAnalysis/MemoryMap.h>
#include <Rose/BinaryAnalysis/RegisterDictionary.h>
#include <Rose/FormatRestorer.h>

#include <Rose/CommandLine.h>
#include <integerOps.h>                                 // rose

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics {
namespace PartialSymbolicSemantics {

uint64_t
Formatter::rename(uint64_t orig_name)
{
    if (0==orig_name)
        return orig_name;

    // Previous version of this code was not only thread unsafe, but had a bug that caused it to return the original name
    // rather than the new name with it encounted the same name twice.
    Map::iterator found = renames.find(orig_name);
    if (renames.end() == found)
        found = renames.insert(std::make_pair(orig_name, SValue::nextName())).first;
    return found->second;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      SValue
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SValue::SValue(size_t nbits)
    : BaseSemantics::SValue(nbits), name(nextName()), offset(0), negate(false) {}

SValue::SValue(size_t nbits, uint64_t number)
    : BaseSemantics::SValue(nbits), name(0), offset(number), negate(false) {
    if (nbits <= 64) {
        this->offset &= IntegerOps::genMask<uint64_t>(nbits);
    } else {
        name = nextName();
        offset = 0;
    }
}

SValue::SValue(size_t nbits, uint64_t name, uint64_t offset, bool negate)
    : BaseSemantics::SValue(nbits), name(name), offset(offset), negate(negate) {
    this->offset &= IntegerOps::genMask<uint64_t>(nbits);
    ASSERT_require(nbits <= 64 || name != 0);
}

SValue::Ptr
SValue::instance() {
    return SValue::Ptr(new SValue(1));
}

SValue::Ptr
SValue::instance(size_t nbits) {
    return SValue::Ptr(new SValue(nbits));
}

SValue::Ptr
SValue::instance(size_t nbits, uint64_t value) {
    return SValue::Ptr(new SValue(nbits, value));
}

SValue::Ptr
SValue::instance(size_t nbits, uint64_t name, uint64_t offset, bool negate) {
    return SValue::Ptr(new SValue(nbits, name, offset, negate));
}

BaseSemantics::SValue::Ptr
SValue::bottom_(size_t nbits) const {
    return instance(nbits);
}

BaseSemantics::SValue::Ptr
SValue::undefined_(size_t nbits) const {
    return instance(nbits);
}

BaseSemantics::SValue::Ptr
SValue::unspecified_(size_t nbits) const {
    return instance(nbits);
}

BaseSemantics::SValue::Ptr
SValue::number_(size_t nbits, uint64_t value) const {
    return instance(nbits, value);
}

BaseSemantics::SValue::Ptr
SValue::copy(size_t new_width) const {
    SValue::Ptr retval(new SValue(*this));
    if (new_width!=0 && new_width!=retval->nBits())
        retval->set_width(new_width);
    return retval;
}

BaseSemantics::SValue::Ptr
SValue::create(size_t nbits, uint64_t name, uint64_t offset, bool negate) const {
    return instance(nbits, name, offset, negate);
}

SValue::Ptr
SValue::promote(const BaseSemantics::SValue::Ptr &v) {
    SValue::Ptr retval = as<SValue>(v);
    ASSERT_not_null(retval);
    return retval;
}

// class method
uint64_t
SValue::nextName() {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    static uint64_t seq = 0;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);
    return ++seq;                                       // first value returned should be one, not zero
}

Sawyer::Optional<BaseSemantics::SValue::Ptr>
SValue::createOptionalMerge(const BaseSemantics::SValue::Ptr &other_, const BaseSemantics::Merger::Ptr&,
                            const SmtSolverPtr &solver) const {
    if (mustEqual(other_, solver))
        return Sawyer::Nothing();
    return bottom_(nBits());
}

bool
SValue::may_equal(const BaseSemantics::SValue::Ptr &other_, const SmtSolverPtr &solver) const
{
    SValue::Ptr other = promote(other_);
    if (mustEqual(other, solver))
        return true;
    return this->name!=0 || other->name!=0;
}

bool
SValue::must_equal(const BaseSemantics::SValue::Ptr &other_, const SmtSolverPtr&) const
{
    SValue::Ptr other = promote(other_);
    return (this->name==other->name &&
            (!this->name || this->negate==other->negate) &&
            this->offset==other->offset);
}

void
SValue::hash(Combinatorics::Hasher &hasher) const {
    hasher.insert(nBits());
    hasher.insert(name);
    hasher.insert(offset);
    hasher.insert(negate);
}

void
SValue::print(std::ostream &stream, BaseSemantics::Formatter &formatter_) const
{
    FormatRestorer restorer(stream); // restore format flags when we leave this scope
    uint64_t sign_bit = (uint64_t)1 << (nBits()-1); /* e.g., 80000000 */
    uint64_t val_mask = sign_bit - 1;             /* e.g., 7fffffff */
    /*magnitude of negative value*/
    uint64_t negative = nBits()>1 && (offset & sign_bit) ? (~offset & val_mask) + 1 : 0;
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
        ASSERT_require(!negate);
        stream <<"0x" <<std::hex <<offset;
        if (negative)
            stream <<" (-0x" <<std::hex <<negative <<")";
    }

    stream <<"[" <<std::dec <<nBits() <<"]";
}

void
SValue::set_width(size_t nbits) {
    if (nbits > 64 && name == 0) {
        *this = SValue(nbits);
    } else {
        ASSERT_require(nbits <= 64 || name != 0);
        BaseSemantics::SValue::set_width(nbits);
        offset &= IntegerOps::genMask<uint64_t>(nbits);
    }
}


/*******************************************************************************************************************************
 *                                      State
 *******************************************************************************************************************************/

State::State(const BaseSemantics::RegisterState::Ptr &registers, const BaseSemantics::MemoryState::Ptr &memory)
    : BaseSemantics::State(registers, memory) {
    // This state should use PartialSymbolicSemantics values (or subclasses thereof)
    ASSERT_not_null(registers);
    (void) SValue::promote(registers->protoval());
    ASSERT_not_null(memory);
    (void) SValue::promote(memory->get_addr_protoval());
    (void) SValue::promote(memory->get_val_protoval());

    // This state should use a memory that is not byte restricted.
    MemoryState::Ptr mcl = MemoryState::promote(memory);
    ASSERT_require(!mcl->byteRestricted());
}

State::State(const State &other)
    : BaseSemantics::State(other) {}

State::Ptr
State::instance(const BaseSemantics::RegisterState::Ptr &registers, const BaseSemantics::MemoryState::Ptr &memory) {
    return State::Ptr(new State(registers, memory));
}

State::Ptr
State::instance(const State::Ptr &other) {
    return State::Ptr(new State(*other));
}

BaseSemantics::State::Ptr
State::create(const BaseSemantics::RegisterState::Ptr &registers, const BaseSemantics::MemoryState::Ptr &memory) const {
    return instance(registers, memory);
}

BaseSemantics::State::Ptr
State::clone() const {
    State::Ptr self = promote(boost::const_pointer_cast<BaseSemantics::State>(shared_from_this()));
    return instance(self);
}

State::Ptr
State::promote(const BaseSemantics::State::Ptr &x) {
    State::Ptr retval = as<State>(x);
    ASSERT_not_null(x);
    return retval;
}

void
State::print_diff_registers(std::ostream&, const State::Ptr &/*other_state*/, Formatter&) const
{
    ASSERT_not_implemented("[Robb P. Matzke 2014-10-07]");
}

bool
State::equal_registers(const State::Ptr &/*other*/) const
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

RiscOperators::RiscOperators(const BaseSemantics::SValue::Ptr &protoval, const SmtSolver::Ptr &solver)
    : BaseSemantics::RiscOperators(protoval, solver) {
    name("PartialSymbolic");
}

RiscOperators::RiscOperators(const BaseSemantics::State::Ptr &state, const SmtSolver::Ptr &solver)
    : BaseSemantics::RiscOperators(state, solver) {
    name("PartialSymbolic");
}

RiscOperators::~RiscOperators() {}

RiscOperators::Ptr
RiscOperators::instanceFromRegisters(const RegisterDictionary::Ptr &regdict) {
    BaseSemantics::SValue::Ptr protoval = SValue::instance();
    BaseSemantics::RegisterState::Ptr registers = RegisterState::instance(protoval, regdict);
    MemoryState::Ptr memory = MemoryState::instance(protoval, protoval);
    memory->byteRestricted(false); // because extracting bytes from a word results in new variables for this domain
    BaseSemantics::State::Ptr state = State::instance(registers, memory);
    SmtSolver::Ptr solver = SmtSolver::instance(Rose::CommandLine::genericSwitchArgs.smtSolver);
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

const MemoryMap::Ptr
RiscOperators::get_memory_map() const {
    return map;
}

void
RiscOperators::set_memory_map(const MemoryMap::Ptr &m) {
    map = m;
}

void
RiscOperators::interrupt(int /*major*/, int /*minor*/) {
    currentState()->clear();
}

BaseSemantics::SValue::Ptr
RiscOperators::and_(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &b_)
{
    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr b = SValue::promote(b_);
    ASSERT_require(a->nBits()==b->nBits());
    if ((!a->name && 0==a->offset) || (!b->name && 0==b->offset))
        return number_(a->nBits(), 0);
    if (a->name || b->name)
        return undefined_(a->nBits());
    return number_(a->nBits(), a->offset & b->offset);
}

BaseSemantics::SValue::Ptr
RiscOperators::or_(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &b_)
{
    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr b = SValue::promote(b_);
    ASSERT_require(a->nBits()==b->nBits());
    if (a->mustEqual(b))
        return a->copy();
    if (!a->name && !b->name)
        return number_(a->nBits(), a->offset | b->offset);
    if (!a->name && a->offset==IntegerOps::genMask<uint64_t>(a->nBits()))
        return a->copy();
    if (!b->name && b->offset==IntegerOps::genMask<uint64_t>(a->nBits()))
        return b->copy();
    return undefined_(a->nBits());
}

BaseSemantics::SValue::Ptr
RiscOperators::xor_(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &b_)
{
    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr b = SValue::promote(b_);
    ASSERT_require(a->nBits()==b->nBits());
    if (!a->name && !b->name)
        return number_(a->nBits(), a->offset ^ b->offset);
    if (a->mustEqual(b))
        return number_(a->nBits(), 0);
    if (!b->name) {
        if (0==b->offset)
            return a->copy();
        if (b->offset==IntegerOps::genMask<uint64_t>(a->nBits()))
            return invert(a);
    }
    if (!a->name) {
        if (0==a->offset)
            return b->copy();
        if (a->offset==IntegerOps::genMask<uint64_t>(a->nBits()))
            return invert(b);
    }
    return undefined_(a->nBits());
}

BaseSemantics::SValue::Ptr
RiscOperators::invert(const BaseSemantics::SValue::Ptr &a_)
{
    SValue::Ptr a = SValue::promote(a_);
    if (a->name)
        return a->create(a->nBits(), a->name, ~a->offset, !a->negate);
    return number_(a->nBits(), ~a->offset);
}

BaseSemantics::SValue::Ptr
RiscOperators::extract(const BaseSemantics::SValue::Ptr &a_, size_t begin_bit, size_t end_bit)
{
    SValue::Ptr a = SValue::promote(a_);
    ASSERT_require(end_bit<=a->nBits());
    ASSERT_require(begin_bit<end_bit);
    if (0==begin_bit) {
        if (end_bit==a->nBits())
            return a->copy();
        return a->copy(end_bit);
    }
    if (a->name)
        return undefined_(end_bit-begin_bit);
    return number_(end_bit-begin_bit, (a->offset >> begin_bit) & IntegerOps::genMask<uint64_t>(end_bit-begin_bit));
}

BaseSemantics::SValue::Ptr
RiscOperators::concat(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &b_)
{
    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr b = SValue::promote(b_);
    if (a->name || b->name)
        return undefined_(a->nBits() + b->nBits());
    if (a->nBits() + b->nBits() > 64)
        return undefined_(a->nBits() + b->nBits());
    return number_(a->nBits()+b->nBits(), a->offset | (b->offset << a->nBits()));
}

BaseSemantics::SValue::Ptr
RiscOperators::equalToZero(const BaseSemantics::SValue::Ptr &a_)
{
    SValue::Ptr a = SValue::promote(a_);
    if (a->name)
        return undefined_(1);
    return a->offset ? boolean_(false) : boolean_(true);
}

BaseSemantics::SValue::Ptr
RiscOperators::iteWithStatus(const BaseSemantics::SValue::Ptr &sel_,
                             const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &b_,
                             IteStatus &status)
{
    SValue::Ptr sel = SValue::promote(sel_);
    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr b = SValue::promote(b_);
    ASSERT_require(1==sel->nBits());
    ASSERT_require(a->nBits()==b->nBits());
    if (a->mustEqual(b)) {
        status = IteStatus::BOTH;
        return a->copy();
    } else if (sel->name) {
        status = IteStatus::NEITHER;
        return undefined_(a->nBits());
    } else if (sel->offset) {
        status = IteStatus::A;
        return a->copy();
    } else {
        status = IteStatus::B;
        return b->copy();
    }
}

BaseSemantics::SValue::Ptr
RiscOperators::leastSignificantSetBit(const BaseSemantics::SValue::Ptr &a_)
{
    SValue::Ptr a = SValue::promote(a_);
    if (a->name)
        return undefined_(a->nBits());
    for (size_t i=0; i<a->nBits(); ++i) {
        if (a->offset & ((uint64_t)1 << i))
            return number_(a->nBits(), i);
    }
    return number_(a->nBits(), 0);
}

BaseSemantics::SValue::Ptr
RiscOperators::mostSignificantSetBit(const BaseSemantics::SValue::Ptr &a_)
{
    SValue::Ptr a = SValue::promote(a_);
    if (a->name)
        return undefined_(a->nBits());
    for (size_t i=a->nBits(); i>0; --i) {
        if (a->offset & ((uint64_t)1 << (i-1)))
            return number_(a->nBits(), i-1);
    }
    return number_(a->nBits(), 0);
}

BaseSemantics::SValue::Ptr
RiscOperators::rotateLeft(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &sa_)
{
    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr sa = SValue::promote(sa_);
    if (!a->name && !sa->name)
        return number_(a->nBits(), IntegerOps::rotateLeft2(a->offset, sa->offset, a->nBits()));
    if (!sa->name && 0==sa->offset % a->nBits())
        return a->copy();
    return undefined_(a->nBits());
}

BaseSemantics::SValue::Ptr
RiscOperators::rotateRight(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &sa_)
{
    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr sa = SValue::promote(sa_);
    if (!a->name && !sa->name) {
        return number_(a->nBits(), IntegerOps::rotateRight2(a->offset, sa->offset, a->nBits()));
        size_t count = sa->offset % a->nBits();
        uint64_t n = (a->offset >> count) | (a->offset << (a->nBits()-count));
        return number_(a->nBits(), n);
    }
    if (!sa->name && 0==sa->offset % a->nBits())
        return a->copy();
    return undefined_(a->nBits());
}

BaseSemantics::SValue::Ptr
RiscOperators::shiftLeft(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &sa_)
{
    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr sa = SValue::promote(sa_);
    if (!a->name && !sa->name)
        return number_(a->nBits(), IntegerOps::shiftLeft2(a->offset, sa->offset, a->nBits()));
    if (!sa->name) {
        if (0==sa->offset)
            return a->copy();
        if (sa->offset>=a->nBits())
            return number_(a->nBits(), 0);
    }
    return undefined_(a->nBits());
}

BaseSemantics::SValue::Ptr
RiscOperators::shiftRight(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &sa_)
{
    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr sa = SValue::promote(sa_);
    if (!sa->name) {
        if (sa->offset>a->nBits())
            return number_(a->nBits(), 0);
        if (0==sa->offset)
            return a->copy();
    }
    if (!a->name && !sa->name)
        return number_(a->nBits(), IntegerOps::shiftRightLogical2(a->offset, sa->offset, a->nBits()));
    return undefined_(a->nBits());
}

BaseSemantics::SValue::Ptr
RiscOperators::shiftRightArithmetic(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &sa_)
{
    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr sa = SValue::promote(sa_);
    if (!sa->name && 0==sa->offset)
        return a->copy();
    if (!a->name && !sa->name)
        return number_(a->nBits(), IntegerOps::shiftRightArithmetic2(a->offset, sa->offset, a->nBits()));
    return undefined_(a->nBits());
}

BaseSemantics::SValue::Ptr
RiscOperators::add(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &b_)
{
    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr b = SValue::promote(b_);
    ASSERT_require(a->nBits()==b->nBits());
    if (a->name==b->name && (!a->name || a->negate!=b->negate)) {
        /* [V1+x] + [-V1+y] = [x+y]  or
         * [x] + [y] = [x+y] */
        return number_(a->nBits(), a->offset + b->offset);
    } else if (!a->name || !b->name) {
        /* [V1+x] + [y] = [V1+x+y]   or
         * [x] + [V2+y] = [V2+x+y]   or
         * [-V1+x] + [y] = [-V1+x+y] or
         * [x] + [-V2+y] = [-V2+x+y] */
        return a->create(a->nBits(), a->name+b->name, a->offset+b->offset, a->negate || b->negate);
    } else {
        return undefined_(a->nBits());
    }
}

BaseSemantics::SValue::Ptr
RiscOperators::addWithCarries(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &b_,
                              const BaseSemantics::SValue::Ptr &c_, BaseSemantics::SValue::Ptr &carry_out/*out*/)
{
    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr b = SValue::promote(b_);
    SValue::Ptr c = SValue::promote(c_);
    ASSERT_require(a->nBits()==b->nBits() && c->nBits()==1);
    int n_unknown = (a->name?1:0) + (b->name?1:0) + (c->name?1:0);
    if (n_unknown <= 1) {
        /* At most, one of the operands is an unknown value. See add() for more details. */
        uint64_t sum = a->offset + b->offset + c->offset;
        if (0==n_unknown) {
            carry_out = number_(a->nBits(), (a->offset ^ b->offset ^ sum)>>1);
        } else {
            carry_out = undefined_(a->nBits());
        }
        return a->create(a->nBits(), a->name+b->name+c->name, sum, a->negate||b->negate||c->negate);
    } else if (a->name==b->name && !c->name && a->negate!=b->negate) {
        /* A and B are known or have bases that cancel out, and C is known */
        uint64_t sum = a->offset + b->offset + c->offset;
        carry_out = number_(a->nBits(), (a->offset ^ b->offset ^ sum)>>1);
        return number_(a->nBits(), sum);
    } else {
        carry_out = undefined_(a->nBits());
        return undefined_(a->nBits());
    }
}

BaseSemantics::SValue::Ptr
RiscOperators::negate(const BaseSemantics::SValue::Ptr &a_)
{
    SValue::Ptr a = SValue::promote(a_);
    if (a->name)
        return a->create(a->nBits(), a->name, -a->offset, !a->negate);
    return number_(a->nBits(), -a->offset);
}

BaseSemantics::SValue::Ptr
RiscOperators::signedDivide(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &b_)
{
    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr b = SValue::promote(b_);
    if (!b->name) {
        if (0==b->offset)
            throw BaseSemantics::Exception("division by zero", currentInstruction());
        if (!a->name)
            return number_(a->nBits(),
                           (IntegerOps::signExtend2(a->offset, a->nBits(), 64) /
                            IntegerOps::signExtend2(b->offset, b->nBits(), 64)));
        if (1==b->offset)
            return a->copy();
        if (b->offset==IntegerOps::genMask<uint64_t>(b->nBits()))
            return negate(a);
        /*FIXME: also possible to return zero if B is large enough. [RPM 2010-05-18]*/
    }
    return undefined_(a->nBits());
}

BaseSemantics::SValue::Ptr
RiscOperators::signedModulo(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &b_)
{
    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr b = SValue::promote(b_);
    if (a->name || b->name)
        return undefined_(b->nBits());
    if (0==b->offset)
        throw BaseSemantics::Exception("division by zero", currentInstruction());
    return number_(b->nBits(),
                   (IntegerOps::signExtend2(a->offset, a->nBits(), 64) %
                    IntegerOps::signExtend2(b->offset, b->nBits(), 64)));
    /* FIXME: More folding possibilities... if 'b' is a power of two then we can return 'a' with the bitsize of 'b'. */
}

BaseSemantics::SValue::Ptr
RiscOperators::signedMultiply(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &b_)
{
    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr b = SValue::promote(b_);
    size_t retwidth = a->nBits() + b->nBits();
    if (retwidth > 64)
        return undefined_(retwidth);

    if (!a->name && !b->name) {
        return number_(retwidth,
                       (IntegerOps::signExtend2(a->offset, a->nBits(), 64) *
                        IntegerOps::signExtend2(b->offset, b->nBits(), 64)));
    }
    if (!b->name) {
        if (0==b->offset)
            return number_(retwidth, 0);
        if (1==b->offset)
            return signExtend(a, retwidth);
        if (b->offset==IntegerOps::genMask<uint64_t>(b->nBits()))
            return signExtend(negate(a), retwidth);
    }
    if (!a->name) {
        if (0==a->offset)
            return number_(retwidth, 0);
        if (1==a->offset)
            return signExtend(b, retwidth);
        if (a->offset==IntegerOps::genMask<uint64_t>(a->nBits()))
            return signExtend(negate(b), retwidth);
    }
    return undefined_(retwidth);
}

BaseSemantics::SValue::Ptr
RiscOperators::unsignedDivide(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &b_)
{
    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr b = SValue::promote(b_);
    if (!b->name) {
        if (0==b->offset)
            throw BaseSemantics::Exception("division by zero", currentInstruction());
        if (!a->name)
            return number_(a->nBits(), a->offset / b->offset);
        if (1==b->offset)
            return a->copy();
        /*FIXME: also possible to return zero if B is large enough. [RPM 2010-05-18]*/
    }
    return undefined_(a->nBits());
}

BaseSemantics::SValue::Ptr
RiscOperators::unsignedModulo(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &b_)
{
    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr b = SValue::promote(b_);
    if (!b->name) {
        if (0==b->offset)
            throw BaseSemantics::Exception("division by zero", currentInstruction());
        if (!a->name)
            return number_(b->nBits(), a->offset % b->offset);
        /* FIXME: More folding possibilities... if 'b' is a power of two then we can return 'a' with the
         * bitsize of 'b'. */
    }
    SValue::Ptr a2 = SValue::promote(unsignedExtend(a, 64));
    SValue::Ptr b2 = SValue::promote(unsignedExtend(b, 64));
    if (a2->mustEqual(b2))
        return b->copy();
    return undefined_(b->nBits());
}

BaseSemantics::SValue::Ptr
RiscOperators::unsignedMultiply(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &b_)
{
    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr b = SValue::promote(b_);
    size_t retwidth = a->nBits() + b->nBits();
    if (retwidth > 64)
        return undefined_(retwidth);
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

BaseSemantics::SValue::Ptr
RiscOperators::signExtend(const BaseSemantics::SValue::Ptr &a_, size_t new_width)
{
    SValue::Ptr a = SValue::promote(a_);
    if (new_width > 64)
        return undefined_(new_width);
    if (new_width==a->nBits())
        return a->copy();
    if (a->name)
        return undefined_(new_width);
    return number_(new_width, IntegerOps::signExtend2(a->offset, a->nBits(), new_width));
}

void
RiscOperators::writeMemory(RegisterDescriptor segreg,
                           const BaseSemantics::SValue::Ptr &address,
                           const BaseSemantics::SValue::Ptr &value,
                           const BaseSemantics::SValue::Ptr &condition)
{
#ifndef NDEBUG
    size_t nbits = value->nBits();
    ASSERT_require2(nbits % 8 == 0, "write to memory must be in byte units");
    ASSERT_require(1==condition->nBits()); // FIXME: condition is not used
#endif
    if (condition->isFalse())
        return;

    // Offset the address by the value of the segment register.
    BaseSemantics::SValue::Ptr adjustedVa;
    if (segreg.isEmpty()) {
        adjustedVa = address;
    } else {
        BaseSemantics::SValue::Ptr segregValue = readRegister(segreg, undefined_(segreg.nBits()));
        adjustedVa = add(address, signExtend(segregValue, address->nBits()));
    }


    // PartialSymbolicSemantics assumes that its memory state is capable of storing multi-byte values.
    currentState()->writeMemory(adjustedVa, value, this, this);
}
    
BaseSemantics::SValue::Ptr
RiscOperators::readOrPeekMemory(RegisterDescriptor segreg,
                                const BaseSemantics::SValue::Ptr &address,
                                const BaseSemantics::SValue::Ptr &dflt_,
                                bool allowSideEffects) {
    BaseSemantics::SValue::Ptr dflt = dflt_;
    size_t nbits = dflt->nBits();
    ASSERT_require2(nbits % 8 == 0, "read from memory must be in byte units");

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

    // Use the initial memory state if there is one.
    if (initialState()) {
        if (allowSideEffects) {
            dflt = initialState()->readMemory(adjustedVa, dflt, this, this);
        } else {
            dflt = initialState()->peekMemory(adjustedVa, dflt, this, this);
        }
    }
    
    // Use the concrete MemoryMap if there is one.  Only those areas of the map that are readable and not writable are used.
    if (map && adjustedVa->isConcrete()) {
        size_t nbytes = nbits/8;
        uint8_t *buf = new uint8_t[nbytes];
        size_t nread = map->require(MemoryMap::READABLE).prohibit(MemoryMap::WRITABLE)
                       .at(adjustedVa->toUnsigned().get()).limit(nbytes).read(buf).size();
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
    SValue::Ptr retval = SValue::promote(currentState()->readMemory(adjustedVa, dflt, this, this));
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::readMemory(RegisterDescriptor segreg,
                          const BaseSemantics::SValue::Ptr &address,
                          const BaseSemantics::SValue::Ptr &dflt,
                          const BaseSemantics::SValue::Ptr &condition)
{
    ASSERT_require(1==condition->nBits()); // FIXME: condition is not used
    if (condition->isFalse())
        return dflt;
    return readOrPeekMemory(segreg, address, dflt, true /*allow side effects*/);
}

BaseSemantics::SValue::Ptr
RiscOperators::peekMemory(RegisterDescriptor segreg,
                          const BaseSemantics::SValue::Ptr &address,
                          const BaseSemantics::SValue::Ptr &dflt)
{
    return readOrPeekMemory(segreg, address, dflt, false /*no side effects allowed*/);
}

} // namespace
} // namespace
} // namespace
} // namespace

#endif
