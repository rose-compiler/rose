#include "sage3basic.h"
#include "TraceSemantics2.h"
#include "AsmUnparser_compat.h"

namespace rose {
namespace BinaryAnalysis {
namespace InstructionSemantics2 {
namespace TraceSemantics {

void
RiscOperators::LinePrefix::set_insn(SgAsmInstruction *insn)
{
    if ((cur_insn = insn))
        ++ninsns;
}

void
RiscOperators::LinePrefix::operator()(FILE *f)
{
    const char *sep = "";
    if (ops!=NULL) {
        fprintf(f, "%s@%p", ops->get_name().c_str(), ops.get());
        sep = " ";
    }
    if (f && cur_insn) {
        fprintf(f, "%sinsn@%s[%zu]", sep, StringUtility::addrToString(cur_insn->get_address()).c_str(), ninsns-1);
        sep = " ";
    }
    if (*sep)
        fputs(": ", f);
}

std::string
RiscOperators::toString(const BaseSemantics::SValuePtr &a)
{
    // FIXME: if there's a way to determine if "a" is not a subclass of the subdomain's protoval class then we could also spit
    // out a warning. [Robb P. Matzke 2013-09-13]
    std::ostringstream ss;
    if (a==NULL) {
        ss <<"NULL";
    } else if (0==a->get_width()) {
        ss <<"PROTOVAL";
    } else {
        ss <<*a;
    }
    return ss.str();
}

void
RiscOperators::check_equal_widths(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b)
{
    if (a!=NULL && b!=NULL && a->get_width()!=b->get_width())
        mesg.mesg("value width violation; see documentation for this RISC operator!");
}

const BaseSemantics::SValuePtr &
RiscOperators::check_width(const BaseSemantics::SValuePtr &a, size_t nbits, const std::string &what)
{
    if (a==NULL || a->get_width()!=nbits)
        mesg.mesg("expected %s to be %zu bits wide; see documentation for this RISC operator!",
                  (what.empty()?std::string("result"):what).c_str(), nbits);
    return a;
}

std::string
RiscOperators::register_name(const RegisterDescriptor &a) 
{
    BaseSemantics::StatePtr state = subdomain->get_state();
    BaseSemantics::RegisterStatePtr regstate;
    if (state!=NULL)
        regstate = state->get_register_state();
    RegisterNames regnames(regstate!=NULL ? regstate->get_register_dictionary() : NULL);
    return regnames(a);
}

void
RiscOperators::before(const std::string &operator_name)
{
    mesg.multipart(operator_name, "%s()", operator_name.c_str());
    check_subdomain();
}

void
RiscOperators::before(const std::string &operator_name, const RegisterDescriptor &a)
{
    check_subdomain();
    mesg.multipart(operator_name, "%s(%s)", operator_name.c_str(), register_name(a).c_str());
}

void
RiscOperators::before(const std::string &operator_name, const RegisterDescriptor &a, const BaseSemantics::SValuePtr &b)
{
    check_subdomain();
    mesg.multipart(operator_name, "%s(%s, %s)", operator_name.c_str(), register_name(a).c_str(), toString(b).c_str());
}

void
RiscOperators::before(const std::string &operator_name, const RegisterDescriptor &a, const BaseSemantics::SValuePtr &b,
                      const BaseSemantics::SValuePtr &c, size_t d)
{
    check_subdomain();
    mesg.multipart(operator_name, "%s(%s, %s, %s, %zu)",
                   operator_name.c_str(), register_name(a).c_str(), toString(b).c_str(), toString(c).c_str(), d);
}

void
RiscOperators::before(const std::string &operator_name, const RegisterDescriptor &a, const BaseSemantics::SValuePtr &b,
                      const BaseSemantics::SValuePtr &c, const BaseSemantics::SValuePtr &d)
{
    check_subdomain();
    mesg.multipart(operator_name, "%s(%s, %s, %s, %s)",
                   operator_name.c_str(), register_name(a).c_str(), toString(b).c_str(), toString(c).c_str(),
                   toString(d).c_str());
}

void
RiscOperators::before(const std::string &operator_name, SgAsmInstruction *insn)
{
    mesg.multipart(operator_name, "%s(%s)", operator_name.c_str(), StringUtility::trim(unparseInstruction(insn)).c_str());
    check_subdomain();
}

void
RiscOperators::before(const std::string &operator_name, size_t a)
{
    mesg.multipart(operator_name, "%s(%zu)", operator_name.c_str(), a);
    check_subdomain();
}

void
RiscOperators::before(const std::string &operator_name, size_t a, uint64_t b)
{
    mesg.multipart(operator_name, "%s(%zu, %"PRIu64")", operator_name.c_str(), a, b);
    check_subdomain();
}

void
RiscOperators::before(const std::string &operator_name, const BaseSemantics::SValuePtr &a)
{
    mesg.multipart(operator_name, "%s(%s)", operator_name.c_str(), toString(a).c_str());
    check_subdomain();
}

void
RiscOperators::before(const std::string &operator_name, const BaseSemantics::SValuePtr &a, size_t b)
{
    mesg.multipart(operator_name, "%s(%s, %zu)", operator_name.c_str(), toString(a).c_str(), b);
    check_subdomain();
}

void
RiscOperators::before(const std::string &operator_name, const BaseSemantics::SValuePtr &a, size_t b, size_t c)
{
    mesg.multipart(operator_name, "%s(%s, %zu, %zu)", operator_name.c_str(), toString(a).c_str(), b, c);
    check_subdomain();
}

void
RiscOperators::before(const std::string &operator_name, const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b)
{
    mesg.multipart(operator_name, "%s(%s, %s)", operator_name.c_str(), toString(a).c_str(), toString(b).c_str());
    check_subdomain();
}

void
RiscOperators::before(const std::string &operator_name, const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b,
                      const BaseSemantics::SValuePtr &c)
{
    mesg.multipart(operator_name, "%s(%s, %s, %s)",
                   operator_name.c_str(), toString(a).c_str(), toString(b).c_str(), toString(c).c_str());
    check_subdomain();
}

void
RiscOperators::after()
{
    mesg.multipart_end();
}

const BaseSemantics::SValuePtr &
RiscOperators::after(const BaseSemantics::SValuePtr &retval)
{
    mesg.more(" = %s", toString(retval).c_str());
    mesg.multipart_end();
    return retval;
}

const BaseSemantics::SValuePtr &
RiscOperators::after(const BaseSemantics::SValuePtr &retval, const BaseSemantics::SValuePtr &ret2)
{
    mesg.more(" = %s\nalso returns: %s", toString(retval).c_str(), toString(ret2).c_str());
    mesg.multipart_end();
    return retval;
}

void
RiscOperators::after(const BaseSemantics::Exception &e)
{
    mesg.more(" = Exception(%s)", e.what());
    mesg.multipart_end();
}

void
RiscOperators::after_exception()
{
    mesg.more(" = <Exception>");
    mesg.multipart_end();
}

BaseSemantics::SValuePtr
RiscOperators::get_protoval() const
{
    check_subdomain();
    return subdomain->get_protoval();
}

void
RiscOperators::set_solver(SMTSolver *solver)
{
    check_subdomain();
    subdomain->set_solver(solver);
}

SMTSolver *
RiscOperators::get_solver() const
{
    check_subdomain();
    return subdomain->get_solver();
}

BaseSemantics::StatePtr
RiscOperators::get_state() const
{
    check_subdomain();
    return subdomain->get_state();
}

void
RiscOperators::set_state(const BaseSemantics::StatePtr &state)
{
    check_subdomain();
    subdomain->set_state(state);
}

void
RiscOperators::print(std::ostream &stream, BaseSemantics::Formatter &fmt) const
{
    check_subdomain();
    subdomain->print(stream, fmt);
}

size_t
RiscOperators::get_ninsns() const
{
    check_subdomain();
    return subdomain->get_ninsns();
}

void
RiscOperators::set_ninsns(size_t n)
{
    check_subdomain();
    subdomain->set_ninsns(n);
}

SgAsmInstruction *
RiscOperators::get_insn() const
{
    check_subdomain();
    return subdomain->get_insn();
}

void
RiscOperators::startInstruction(SgAsmInstruction *insn)
{
    line_prefix.set_insn(insn);
    before("startInstruction", insn);
    try {
        subdomain->startInstruction(insn);
        after();
    } catch (const BaseSemantics::Exception &e) {
        after(e);
        throw;
    } catch (...) {
        after_exception();
        throw;
    }
}

void
RiscOperators::finishInstruction(SgAsmInstruction *insn)
{
    before("finishInstruction", insn);
    try {
        subdomain->finishInstruction(insn);
        after();
    } catch (const BaseSemantics::Exception &e) {
        after(e);
        throw;
    } catch (...) {
        after_exception();
        throw;
    }
}

BaseSemantics::SValuePtr
RiscOperators::undefined_(size_t nbits)
{
    before("undefined_", nbits);
    try {
        return check_width(after(subdomain->undefined_(nbits)), nbits);
    } catch (const BaseSemantics::Exception &e) {
        after(e);
        throw;
    } catch (...) {
        after_exception();
        throw;
    }
}

BaseSemantics::SValuePtr
RiscOperators::number_(size_t nbits, uint64_t value)
{
    before("number_", nbits, value);
    try {
        return check_width(after(subdomain->number_(nbits, value)), nbits);
    } catch (const BaseSemantics::Exception &e) {
        after(e);
        throw;
    } catch (...) {
        after_exception();
        throw;
    }
}

BaseSemantics::SValuePtr
RiscOperators::boolean_(bool value)
{
    before("boolean_", value);
    try {
        return check_width(after(subdomain->boolean_(value)), 1);
    } catch (const BaseSemantics::Exception &e) {
        after(e);
        throw;
    } catch (...) {
        after_exception();
        throw;
    }
}

BaseSemantics::SValuePtr
RiscOperators::filterCallTarget(const BaseSemantics::SValuePtr &a)
{
    before("filterCallTarget", a);
    try {
        return check_width(after(subdomain->filterCallTarget(a)), a->get_width());
    } catch (const BaseSemantics::Exception &e) {
        after(e);
        throw;
    } catch (...) {
        after_exception();
        throw;
    }
}

BaseSemantics::SValuePtr
RiscOperators::filterReturnTarget(const BaseSemantics::SValuePtr &a)
{
    before("filterReturnTarget", a);
    try {
        return check_width(after(subdomain->filterReturnTarget(a)), a->get_width());
    } catch (const BaseSemantics::Exception &e) {
        after(e);
        throw;
    } catch (...) {
        after_exception();
        throw;
    }
}

BaseSemantics::SValuePtr
RiscOperators::filterIndirectJumpTarget(const BaseSemantics::SValuePtr &a)
{
    before("filterIndirectJumpTarget", a);
    try {
        return check_width(after(subdomain->filterIndirectJumpTarget(a)), a->get_width());
    } catch (const BaseSemantics::Exception &e) {
        after(e);
        throw;
    } catch (...) {
        after_exception();
        throw;
    }
}

void
RiscOperators::hlt()
{
    before("hlt");
    try {
        subdomain->hlt();
        after();
    } catch (const BaseSemantics::Exception &e) {
        after(e);
        throw;
    } catch (...) {
        after_exception();
        throw;
    }
}

void
RiscOperators::cpuid()
{
    before("cpuid");
    try {
        subdomain->cpuid();
        after();
    } catch (const BaseSemantics::Exception &e) {
        after(e);
        throw;
    } catch (...) {
        after_exception();
        throw;
    }
}

BaseSemantics::SValuePtr
RiscOperators::rdtsc()
{
    before("rdtsc");
    try {
        return check_width(after(subdomain->rdtsc()), 64);
    } catch (const BaseSemantics::Exception &e) {
        after(e);
        throw;
    } catch (...) {
        after_exception();
        throw;
    }
}

BaseSemantics::SValuePtr
RiscOperators::and_(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b)
{
    before("and_", a, b);
    try {
        check_equal_widths(a, b);
        return check_width(after(subdomain->and_(a, b)), a->get_width());
    } catch (const BaseSemantics::Exception &e) {
        after(e);
        throw;
    } catch (...) {
        after_exception();
        throw;
    }
}

BaseSemantics::SValuePtr
RiscOperators::or_(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b)
{
    before("or_", a, b);
    try {
        check_equal_widths(a, b);
        return check_width(after(subdomain->or_(a, b)), a->get_width());
    } catch (const BaseSemantics::Exception &e) {
        after(e);
        throw;
    } catch (...) {
        after_exception();
        throw;
    }
}

BaseSemantics::SValuePtr
RiscOperators::xor_(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b)
{
    before("xor_", a, b);
    try {
        check_equal_widths(a, b);
        return check_width(after(subdomain->xor_(a, b)), a->get_width());;
    } catch (const BaseSemantics::Exception &e) {
        after(e);
        throw;
    } catch (...) {
        after_exception();
        throw;
    }
}

BaseSemantics::SValuePtr
RiscOperators::invert(const BaseSemantics::SValuePtr &a)
{
    before("invert", a);
    try {
        return check_width(after(subdomain->invert(a)), a->get_width());
    } catch (const BaseSemantics::Exception &e) {
        after(e);
        throw;
    } catch (...) {
        after_exception();
        throw;
    }
}

BaseSemantics::SValuePtr
RiscOperators::extract(const BaseSemantics::SValuePtr &a, size_t b, size_t c)
{
    before("extract", a, b, c);
    try {
        return check_width(after(subdomain->extract(a, b, c)), c-b);
    } catch (const BaseSemantics::Exception &e) {
        after(e);
        throw;
    } catch (...) {
        after_exception();
        throw;
    }
}

BaseSemantics::SValuePtr
RiscOperators::concat(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b)
{
    before("concat", a, b);
    try {
        return check_width(after(subdomain->concat(a, b)), a->get_width()+b->get_width());
    } catch (const BaseSemantics::Exception &e) {
        after(e);
        throw;
    } catch (...) {
        after_exception();
        throw;
    }
}

BaseSemantics::SValuePtr
RiscOperators::leastSignificantSetBit(const BaseSemantics::SValuePtr &a)
{
    before("leastSignificantSetBit", a);
    try {
        return check_width(after(subdomain->leastSignificantSetBit(a)), a->get_width());
    } catch (const BaseSemantics::Exception &e) {
        after(e);
        throw;
    } catch (...) {
        after_exception();
        throw;
    }
}

BaseSemantics::SValuePtr
RiscOperators::mostSignificantSetBit(const BaseSemantics::SValuePtr &a)
{
    before("mostSignificantSetBit", a);
    try {
        return check_width(after(subdomain->mostSignificantSetBit(a)), a->get_width());
    } catch (const BaseSemantics::Exception &e) {
        after(e);
        throw;
    } catch (...) {
        after_exception();
        throw;
    }
}

BaseSemantics::SValuePtr
RiscOperators::rotateLeft(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b)
{
    before("rotateLeft", a, b);
    try {
        return check_width(after(subdomain->rotateLeft(a, b)), a->get_width());
    } catch (const BaseSemantics::Exception &e) {
        after(e);
        throw;
    } catch (...) {
        after_exception();
        throw;
    }
}

BaseSemantics::SValuePtr
RiscOperators::rotateRight(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b)
{
    before("rotateRight", a, b);
    try {
        return check_width(after(subdomain->rotateRight(a, b)), a->get_width());
    } catch (const BaseSemantics::Exception &e) {
        after(e);
        throw;
    } catch (...) {
        after_exception();
        throw;
    }
}

BaseSemantics::SValuePtr
RiscOperators::shiftLeft(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b)
{
    before("shiftLeft", a, b);
    try {
        return check_width(after(subdomain->shiftLeft(a, b)), a->get_width());
    } catch (const BaseSemantics::Exception &e) {
        after(e);
        throw;
    } catch (...) {
        after_exception();
        throw;
    }
}

BaseSemantics::SValuePtr
RiscOperators::shiftRight(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b)
{
    before("shiftRight", a, b);
    try {
        return check_width(after(subdomain->shiftRight(a, b)), a->get_width());
    } catch (const BaseSemantics::Exception &e) {
        after(e);
        throw;
    } catch (...) {
        after_exception();
        throw;
    }
}

BaseSemantics::SValuePtr
RiscOperators::shiftRightArithmetic(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b)
{
    before("shiftRightArithmetic", a, b);
    try {
        return check_width(after(subdomain->shiftRightArithmetic(a, b)), a->get_width());
    } catch (const BaseSemantics::Exception &e) {
        after(e);
        throw;
    } catch (...) {
        after_exception();
        throw;
    }
}

BaseSemantics::SValuePtr
RiscOperators::equalToZero(const BaseSemantics::SValuePtr &a)
{
    before("equalToZero", a);
    try {
        return check_width(after(subdomain->equalToZero(a)), 1);
    } catch (const BaseSemantics::Exception &e) {
        after(e);
        throw;
    } catch (...) {
        after_exception();
        throw;
    }
}

BaseSemantics::SValuePtr
RiscOperators::ite(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b, const BaseSemantics::SValuePtr &c)
{
    before("ite", a, b, c);
    try {
        check_equal_widths(b, c);
        return check_width(after(subdomain->ite(a, b, c)), b->get_width());
    } catch (const BaseSemantics::Exception &e) {
        after(e);
        throw;
    } catch (...) {
        after_exception();
        throw;
    }
}

BaseSemantics::SValuePtr
RiscOperators::unsignedExtend(const BaseSemantics::SValuePtr &a, size_t b)
{
    before("unsignedExtend", a, b);
    try {
        return check_width(after(subdomain->unsignedExtend(a, b)), b);
    } catch (const BaseSemantics::Exception &e) {
        after(e);
        throw;
    } catch (...) {
        after_exception();
        throw;
    }
}

BaseSemantics::SValuePtr
RiscOperators::signExtend(const BaseSemantics::SValuePtr &a, size_t b)
{
    before("signExtend", a, b);
    try {
        return check_width(after(subdomain->signExtend(a, b)), b);
    } catch (const BaseSemantics::Exception &e) {
        after(e);
        throw;
    } catch (...) {
        after_exception();
        throw;
    }
}

BaseSemantics::SValuePtr
RiscOperators::add(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b)
{
    before("add", a, b);
    try {
        check_equal_widths(a, b);
        return check_width(after(subdomain->add(a, b)), a->get_width());
    } catch (const BaseSemantics::Exception &e) {
        after(e);
        throw;
    } catch (...) {
        after_exception();
        throw;
    }
}

BaseSemantics::SValuePtr
RiscOperators::addWithCarries(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b,
                              const BaseSemantics::SValuePtr &c, BaseSemantics::SValuePtr &d/*out*/)
{
    before("addWithCarries", a, b, c);
    try {
        check_equal_widths(a, b);
        check_width(c, 1);
        BaseSemantics::SValuePtr retval = subdomain->addWithCarries(a, b, c, d);
        after(retval, d);
        check_width(retval, a->get_width());
        check_width(d, a->get_width());
        return retval;
    } catch (const BaseSemantics::Exception &e) {
        after(e);
        throw;
    } catch (...) {
        after_exception();
        throw;
    }
}

BaseSemantics::SValuePtr
RiscOperators::negate(const BaseSemantics::SValuePtr &a)
{
    before("negate", a);
    try {
        return check_width(after(subdomain->negate(a)), a->get_width());
    } catch (const BaseSemantics::Exception &e) {
        after(e);
        throw;
    } catch (...) {
        after_exception();
        throw;
    }
}

BaseSemantics::SValuePtr
RiscOperators::signedDivide(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b)
{
    before("signedDivide", a, b);
    try {
        return check_width(after(subdomain->signedDivide(a, b)), a->get_width());
    } catch (const BaseSemantics::Exception &e) {
        after(e);
        throw;
    } catch (...) {
        after_exception();
        throw;
    }
}

BaseSemantics::SValuePtr
RiscOperators::signedModulo(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b)
{
    before("signedModulo", a, b);
    try {
        return check_width(after(subdomain->signedModulo(a, b)), b->get_width());
    } catch (const BaseSemantics::Exception &e) {
        after(e);
        throw;
    } catch (...) {
        after_exception();
        throw;
    }
}

BaseSemantics::SValuePtr
RiscOperators::signedMultiply(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b)
{
    before("signedMultiply", a, b);
    try {
        return check_width(after(subdomain->signedMultiply(a, b)), a->get_width()+b->get_width());
    } catch (const BaseSemantics::Exception &e) {
        after(e);
        throw;
    } catch (...) {
        after_exception();
        throw;
    }
}

BaseSemantics::SValuePtr
RiscOperators::unsignedDivide(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b)
{
    before("unsignedDivide", a, b);
    try {
        return check_width(after(subdomain->unsignedDivide(a, b)), a->get_width());
    } catch (const BaseSemantics::Exception &e) {
        after(e);
        throw;
    } catch (...) {
        after_exception();
        throw;
    }
}

BaseSemantics::SValuePtr
RiscOperators::unsignedModulo(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b)
{
    before("unsignedModulo", a, b);
    try {
        return check_width(after(subdomain->unsignedModulo(a, b)), b->get_width());
    } catch (const BaseSemantics::Exception &e) {
        after(e);
        throw;
    } catch (...) {
        after_exception();
        throw;
    }
}

BaseSemantics::SValuePtr
RiscOperators::unsignedMultiply(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b)
{
    before("unsignedMultiply", a, b);
    try {
        return check_width(after(subdomain->unsignedMultiply(a, b)), a->get_width()+b->get_width());
    } catch (const BaseSemantics::Exception &e) {
        after(e);
        throw;
    } catch (...) {
        after_exception();
        throw;
    }
}

void
RiscOperators::interrupt(int a, int b)
{
    before("interrupt", a, b);
    try {
        subdomain->interrupt(a, b);
        after();
    } catch (const BaseSemantics::Exception &e) {
        after(e);
        throw;
    } catch (...) {
        after_exception();
        throw;
    }
}

BaseSemantics::SValuePtr
RiscOperators::readRegister(const RegisterDescriptor &a)
{
    before("readRegister", a);
    try {
        return check_width(after(subdomain->readRegister(a)), a.get_nbits());
    } catch (const BaseSemantics::Exception &e) {
        after(e);
        throw;
    } catch (...) {
        after_exception();
        throw;
    }
}

void
RiscOperators::writeRegister(const RegisterDescriptor &a, const BaseSemantics::SValuePtr &b)
{
    before("writeRegister", a, b);
    try {
        subdomain->writeRegister(a, b);
        after();
    } catch (const BaseSemantics::Exception &e) {
        after(e);
        throw;
    } catch (...) {
        after_exception();
        throw;
    }
}

BaseSemantics::SValuePtr
RiscOperators::readMemory(const RegisterDescriptor &a, const BaseSemantics::SValuePtr &b, const BaseSemantics::SValuePtr &c,
                          const BaseSemantics::SValuePtr &d)
{
    before("readMemory", a, b, c, d);
    try {
        return check_width(after(subdomain->readMemory(a, b, c, d)), c->get_width());
    } catch (const BaseSemantics::Exception &e) {
        after(e);
        throw;
    } catch (...) {
        after_exception();
        throw;
    }
}

void
RiscOperators::writeMemory(const RegisterDescriptor &a, const BaseSemantics::SValuePtr &b, const BaseSemantics::SValuePtr &c,
                           const BaseSemantics::SValuePtr &d)
{
    before("writeMemory", a, b, c, d);
    try {
        subdomain->writeMemory(a, b, c, d);
        after();
    } catch (const BaseSemantics::Exception &e) {
        after(e);
        throw;
    } catch (...) {
        after_exception();
        throw;
    }
}

} // namespace
} // namespace
} // namespace
} // namespace
