#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include "sage3basic.h"
#include "NullSemantics2.h"

namespace Rose {
namespace BinaryAnalysis { // documented elsewhere
namespace InstructionSemantics2 { // documented elsewhere
namespace NullSemantics { // documented in the header

/*******************************************************************************************************************************
 *                                      RISC operators
 *******************************************************************************************************************************/

// class method
RiscOperatorsPtr
RiscOperators::instance(const RegisterDictionary *regdict)
{
    BaseSemantics::SValuePtr protoval = SValue::instance();
    BaseSemantics::RegisterStatePtr registers = RegisterState::instance(protoval, regdict);
    BaseSemantics::MemoryStatePtr memory = MemoryState::instance(protoval, protoval);
    BaseSemantics::StatePtr state = State::instance(registers, memory);
    SmtSolverPtr solver;
    return RiscOperatorsPtr(new RiscOperators(state, solver));
}

BaseSemantics::SValuePtr
RiscOperators::and_(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    ASSERT_require(a->get_width()==b->get_width());
    return undefined_(a->get_width());
}

BaseSemantics::SValuePtr
RiscOperators::or_(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    ASSERT_require(a->get_width()==b->get_width());
    return undefined_(a->get_width());
}

BaseSemantics::SValuePtr
RiscOperators::xor_(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    ASSERT_require(a->get_width()==b->get_width());
    return undefined_(a->get_width());
}

BaseSemantics::SValuePtr
RiscOperators::invert(const BaseSemantics::SValuePtr &a_)
{
    SValuePtr a = SValue::promote(a_);
    return undefined_(a->get_width());
}

BaseSemantics::SValuePtr
RiscOperators::extract(const BaseSemantics::SValuePtr &a_, size_t begin_bit, size_t end_bit)
{
    SValuePtr a = SValue::promote(a_);
    ASSERT_require(end_bit<=a->get_width());
    ASSERT_require(begin_bit<end_bit);
    return undefined_(end_bit-begin_bit);
}

BaseSemantics::SValuePtr
RiscOperators::concat(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    return undefined_(a->get_width() + b->get_width());
}

BaseSemantics::SValuePtr
RiscOperators::leastSignificantSetBit(const BaseSemantics::SValuePtr &a_)
{
    SValuePtr a = SValue::promote(a_);
    return undefined_(a->get_width());
}

BaseSemantics::SValuePtr
RiscOperators::mostSignificantSetBit(const BaseSemantics::SValuePtr &a_)
{
    SValuePtr a = SValue::promote(a_);
    return undefined_(a->get_width());
}

BaseSemantics::SValuePtr
RiscOperators::rotateLeft(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &sa_)
{
    SValuePtr a = SValue::promote(a_);
    SValue::promote(sa_);
    return undefined_(a->get_width());
}

BaseSemantics::SValuePtr
RiscOperators::rotateRight(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &sa_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr sa = SValue::promote(sa_);
    return undefined_(a->get_width());
}

BaseSemantics::SValuePtr
RiscOperators::shiftLeft(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &sa_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr sa = SValue::promote(sa_);
    return undefined_(a->get_width());
}

BaseSemantics::SValuePtr
RiscOperators::shiftRight(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &sa_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr sa = SValue::promote(sa_);
    return undefined_(a->get_width());
}

BaseSemantics::SValuePtr
RiscOperators::shiftRightArithmetic(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &sa_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr sa = SValue::promote(sa_);
    return undefined_(a->get_width());
}

BaseSemantics::SValuePtr
RiscOperators::equalToZero(const BaseSemantics::SValuePtr &a_)
{
    SValue::promote(a_);
    return undefined_(1);
}

BaseSemantics::SValuePtr
RiscOperators::ite(const BaseSemantics::SValuePtr &sel_, const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr sel = SValue::promote(sel_);
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    ASSERT_require(1==sel->get_width());
    ASSERT_require(a->get_width()==b->get_width());
    return undefined_(a->get_width());
}

BaseSemantics::SValuePtr
RiscOperators::signExtend(const BaseSemantics::SValuePtr &a_, size_t new_width)
{
    SValuePtr a = SValue::promote(a_);
    if (new_width==a->get_width())
        return a;
    return undefined_(new_width);
}

BaseSemantics::SValuePtr
RiscOperators::add(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    ASSERT_require(a->get_width()==b->get_width());
    return undefined_(a->get_width());
}

BaseSemantics::SValuePtr
RiscOperators::addWithCarries(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_,
                              const BaseSemantics::SValuePtr &c_, BaseSemantics::SValuePtr &carry_out/*out*/)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    SValuePtr c = SValue::promote(c_);
    ASSERT_require(a->get_width()==b->get_width() && c->get_width()==1);
    carry_out = undefined_(a->get_width());
    return undefined_(a->get_width());
}

BaseSemantics::SValuePtr
RiscOperators::negate(const BaseSemantics::SValuePtr &a_)
{
    SValuePtr a = SValue::promote(a_);
    return undefined_(a->get_width());
}

BaseSemantics::SValuePtr
RiscOperators::signedDivide(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    return undefined_(a->get_width());
}

BaseSemantics::SValuePtr
RiscOperators::signedModulo(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    return undefined_(b->get_width());
}

BaseSemantics::SValuePtr
RiscOperators::signedMultiply(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    size_t retwidth = a->get_width() + b->get_width();
    return undefined_(retwidth);
}

BaseSemantics::SValuePtr
RiscOperators::unsignedDivide(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    return undefined_(a->get_width());
}

BaseSemantics::SValuePtr
RiscOperators::unsignedModulo(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    return undefined_(b->get_width());
}

BaseSemantics::SValuePtr
RiscOperators::unsignedMultiply(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_)
{
    SValuePtr a = SValue::promote(a_);
    SValuePtr b = SValue::promote(b_);
    size_t retwidth = a->get_width() + b->get_width();
    return undefined_(retwidth);
}

BaseSemantics::SValuePtr
RiscOperators::readMemory(RegisterDescriptor segreg, const BaseSemantics::SValuePtr &addr,
                          const BaseSemantics::SValuePtr &dflt, const BaseSemantics::SValuePtr &cond)
{
    ASSERT_not_null(currentState());
    (void) SValue::promote(addr);
    (void) SValue::promote(cond);
    return dflt->copy();
}

BaseSemantics::SValuePtr
RiscOperators::peekMemory(RegisterDescriptor segreg, const BaseSemantics::SValuePtr &addr,
                          const BaseSemantics::SValuePtr &dflt)
{
    ASSERT_not_null(currentState());
    (void) SValue::promote(addr);
    return dflt->copy();
}

void
RiscOperators::writeMemory(RegisterDescriptor segreg, const BaseSemantics::SValuePtr &addr,
                         const BaseSemantics::SValuePtr &data, const BaseSemantics::SValuePtr &cond)
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
