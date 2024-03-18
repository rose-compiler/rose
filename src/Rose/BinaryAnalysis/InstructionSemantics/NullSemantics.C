#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/InstructionSemantics/NullSemantics.h>

#include <Rose/BinaryAnalysis/RegisterDictionary.h>

namespace Rose {
namespace BinaryAnalysis {                              // documented elsewhere
namespace InstructionSemantics {                        // documented elsewhere
namespace NullSemantics {                               // documented in the header

/*******************************************************************************************************************************
 *                                      RISC operators
 *******************************************************************************************************************************/
RiscOperators::RiscOperators(const BaseSemantics::SValue::Ptr &protoval, const SmtSolver::Ptr &solver)
    : BaseSemantics::RiscOperators(protoval, solver) {
    name("Null");
}

RiscOperators::RiscOperators(const BaseSemantics::State::Ptr &state, const SmtSolver::Ptr &solver)
    : BaseSemantics::RiscOperators(state, solver) {
    name("Null");
}

RiscOperators::~RiscOperators() {}


// class method
RiscOperators::Ptr
RiscOperators::instanceFromRegisters(const RegisterDictionary::Ptr &regdict) {
    BaseSemantics::SValue::Ptr protoval = SValue::instance();
    BaseSemantics::RegisterState::Ptr registers = RegisterState::instance(protoval, regdict);
    BaseSemantics::MemoryState::Ptr memory = MemoryState::instance(protoval, protoval);
    BaseSemantics::State::Ptr state = State::instance(registers, memory);
    SmtSolver::Ptr solver;
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

BaseSemantics::SValue::Ptr
RiscOperators::and_(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &b_)
{
    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr b = SValue::promote(b_);
    ASSERT_require(a->nBits()==b->nBits());
    return undefined_(a->nBits());
}

BaseSemantics::SValue::Ptr
RiscOperators::or_(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &b_)
{
    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr b = SValue::promote(b_);
    ASSERT_require(a->nBits()==b->nBits());
    return undefined_(a->nBits());
}

BaseSemantics::SValue::Ptr
RiscOperators::xor_(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &b_)
{
    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr b = SValue::promote(b_);
    ASSERT_require(a->nBits()==b->nBits());
    return undefined_(a->nBits());
}

BaseSemantics::SValue::Ptr
RiscOperators::invert(const BaseSemantics::SValue::Ptr &a_)
{
    SValue::Ptr a = SValue::promote(a_);
    return undefined_(a->nBits());
}

BaseSemantics::SValue::Ptr
RiscOperators::extract(const BaseSemantics::SValue::Ptr &a_, size_t begin_bit, size_t end_bit)
{
    SValue::Ptr a = SValue::promote(a_);
    ASSERT_require(end_bit<=a->nBits());
    ASSERT_require(begin_bit<end_bit);
    return undefined_(end_bit-begin_bit);
}

BaseSemantics::SValue::Ptr
RiscOperators::concat(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &b_)
{
    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr b = SValue::promote(b_);
    return undefined_(a->nBits() + b->nBits());
}

BaseSemantics::SValue::Ptr
RiscOperators::leastSignificantSetBit(const BaseSemantics::SValue::Ptr &a_)
{
    SValue::Ptr a = SValue::promote(a_);
    return undefined_(a->nBits());
}

BaseSemantics::SValue::Ptr
RiscOperators::mostSignificantSetBit(const BaseSemantics::SValue::Ptr &a_)
{
    SValue::Ptr a = SValue::promote(a_);
    return undefined_(a->nBits());
}

BaseSemantics::SValue::Ptr
RiscOperators::rotateLeft(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &sa_)
{
    SValue::Ptr a = SValue::promote(a_);
    SValue::promote(sa_);
    return undefined_(a->nBits());
}

BaseSemantics::SValue::Ptr
RiscOperators::rotateRight(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &sa_)
{
    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr sa = SValue::promote(sa_);
    return undefined_(a->nBits());
}

BaseSemantics::SValue::Ptr
RiscOperators::shiftLeft(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &sa_)
{
    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr sa = SValue::promote(sa_);
    return undefined_(a->nBits());
}

BaseSemantics::SValue::Ptr
RiscOperators::shiftRight(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &sa_)
{
    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr sa = SValue::promote(sa_);
    return undefined_(a->nBits());
}

BaseSemantics::SValue::Ptr
RiscOperators::shiftRightArithmetic(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &sa_)
{
    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr sa = SValue::promote(sa_);
    return undefined_(a->nBits());
}

BaseSemantics::SValue::Ptr
RiscOperators::equalToZero(const BaseSemantics::SValue::Ptr &a_)
{
    SValue::promote(a_);
    return undefined_(1);
}

BaseSemantics::SValue::Ptr
RiscOperators::iteWithStatus(const BaseSemantics::SValue::Ptr &sel_, const BaseSemantics::SValue::Ptr &a_,
                             const BaseSemantics::SValue::Ptr &b_, IteStatus &status) {
    SValue::Ptr sel = SValue::promote(sel_);
    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr b = SValue::promote(b_);
    ASSERT_require(1 == sel->nBits());
    ASSERT_require(a->nBits() == b->nBits());
    status = IteStatus::NEITHER;
    return undefined_(a->nBits());
}

BaseSemantics::SValue::Ptr
RiscOperators::signExtend(const BaseSemantics::SValue::Ptr &a_, size_t new_width)
{
    SValue::Ptr a = SValue::promote(a_);
    if (new_width==a->nBits())
        return a;
    return undefined_(new_width);
}

BaseSemantics::SValue::Ptr
RiscOperators::add(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &b_)
{
    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr b = SValue::promote(b_);
    ASSERT_require(a->nBits()==b->nBits());
    return undefined_(a->nBits());
}

BaseSemantics::SValue::Ptr
RiscOperators::addWithCarries(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &b_,
                              const BaseSemantics::SValue::Ptr &c_, BaseSemantics::SValue::Ptr &carry_out/*out*/)
{
    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr b = SValue::promote(b_);
    SValue::Ptr c = SValue::promote(c_);
    ASSERT_require(a->nBits()==b->nBits() && c->nBits()==1);
    carry_out = undefined_(a->nBits());
    return undefined_(a->nBits());
}

BaseSemantics::SValue::Ptr
RiscOperators::negate(const BaseSemantics::SValue::Ptr &a_)
{
    SValue::Ptr a = SValue::promote(a_);
    return undefined_(a->nBits());
}

BaseSemantics::SValue::Ptr
RiscOperators::signedDivide(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &b_)
{
    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr b = SValue::promote(b_);
    return undefined_(a->nBits());
}

BaseSemantics::SValue::Ptr
RiscOperators::signedModulo(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &b_)
{
    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr b = SValue::promote(b_);
    return undefined_(b->nBits());
}

BaseSemantics::SValue::Ptr
RiscOperators::signedMultiply(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &b_)
{
    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr b = SValue::promote(b_);
    size_t retwidth = a->nBits() + b->nBits();
    return undefined_(retwidth);
}

BaseSemantics::SValue::Ptr
RiscOperators::unsignedDivide(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &b_)
{
    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr b = SValue::promote(b_);
    return undefined_(a->nBits());
}

BaseSemantics::SValue::Ptr
RiscOperators::unsignedModulo(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &b_)
{
    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr b = SValue::promote(b_);
    return undefined_(b->nBits());
}

BaseSemantics::SValue::Ptr
RiscOperators::unsignedMultiply(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &b_)
{
    SValue::Ptr a = SValue::promote(a_);
    SValue::Ptr b = SValue::promote(b_);
    size_t retwidth = a->nBits() + b->nBits();
    return undefined_(retwidth);
}

BaseSemantics::SValue::Ptr
RiscOperators::readMemory(RegisterDescriptor /*segreg*/, const BaseSemantics::SValue::Ptr &addr,
                          const BaseSemantics::SValue::Ptr &dflt, const BaseSemantics::SValue::Ptr &cond)
{
    ASSERT_not_null(currentState());
    (void) SValue::promote(addr);
    (void) SValue::promote(cond);
    return dflt->copy();
}

BaseSemantics::SValue::Ptr
RiscOperators::peekMemory(RegisterDescriptor /*segreg*/, const BaseSemantics::SValue::Ptr &addr,
                          const BaseSemantics::SValue::Ptr &dflt)
{
    ASSERT_not_null(currentState());
    (void) SValue::promote(addr);
    return dflt->copy();
}

void
RiscOperators::writeMemory(RegisterDescriptor /*segreg*/, const BaseSemantics::SValue::Ptr &addr,
                         const BaseSemantics::SValue::Ptr &data, const BaseSemantics::SValue::Ptr &cond)
{
    ASSERT_not_null(currentState());
    (void) SValue::promote(addr);
    (void) SValue::promote(data);
    (void) SValue::promote(cond);
}

} // namespace
} // namespace
} // namespace
} // namespace

#endif
