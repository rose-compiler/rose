#include "sage3basic.h"
#include "TraceSemantics2.h"
#include "AsmUnparser_compat.h"

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics2 {
namespace TraceSemantics {

void
RiscOperators::linePrefix() {
    if (stream_) {
        const char *sep = "";
        stream_ <<indentation_;

        if (showingSubdomain_ && subdomain_) {
            stream_ <<subdomain_->name() <<"@" <<subdomain_.get();
            sep = " ";
        }
        if (showingInstructionVa_) {
            if (SgAsmInstruction *insn = currentInstruction()) {
                stream_ <<sep <<"insn@" <<StringUtility::addrToString(insn->get_address()) <<"[" <<(nInsns()-1) <<"]";
                sep = " ";
            }
        }
        if (*sep)
            stream_ <<": ";
    }
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

std::string
RiscOperators::toString(SgAsmFloatType *type) {
    ASSERT_not_null(type);
    if (type == SageBuilderAsm::buildIeee754Binary32()) {
        return "float";
    } else if (type == SageBuilderAsm::buildIeee754Binary64()) {
        return "double";
    } else {
        return type->toString();
    }
}

void
RiscOperators::check_equal_widths(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b)
{
    if (a!=NULL && b!=NULL && a->get_width()!=b->get_width())
        stream_ <<"value width violation; see documentation for this RISC operator!\n";
}

const BaseSemantics::SValuePtr &
RiscOperators::check_width(const BaseSemantics::SValuePtr &a, size_t nbits, const std::string &what)
{
    if (a==NULL || a->get_width()!=nbits)
        stream_ <<"expected " <<(what.empty()?std::string("result"):what)
                <<" to be " <<nbits <<" bits wide; see documentation for this RISC operator!\n";
    return a;
}

std::string
RiscOperators::register_name(RegisterDescriptor a) 
{
    BaseSemantics::StatePtr state = subdomain_->currentState();
    BaseSemantics::RegisterStatePtr regstate;
    if (state!=NULL)
        regstate = state->registerState();
    RegisterNames regnames(regstate!=NULL ? regstate->get_register_dictionary() : NULL);
    return regnames(a);
}

void
RiscOperators::before(const std::string &operator_name)
{
    linePrefix();
    stream_ <<operator_name <<"()";
    checkSubdomain();
}

void
RiscOperators::before(const std::string &operator_name, RegisterDescriptor a)
{
    checkSubdomain();
    linePrefix();
    SAWYER_MESG(stream_) <<operator_name <<"(" <<register_name(a) <<")";
}

void
RiscOperators::before(const std::string &operator_name, RegisterDescriptor a, const BaseSemantics::SValuePtr &b)
{
    checkSubdomain();
    linePrefix();
    SAWYER_MESG(stream_) <<operator_name <<"(" <<register_name(a) <<", " <<toString(b) <<")";
}

void
RiscOperators::before(const std::string &operator_name, RegisterDescriptor a, const BaseSemantics::SValuePtr &b,
                      const BaseSemantics::SValuePtr &c)
{
    checkSubdomain();
    linePrefix();
    SAWYER_MESG(stream_) <<operator_name <<"(" <<register_name(a) <<", " <<toString(b) <<", " <<toString(c) <<")";
}

void
RiscOperators::before(const std::string &operator_name, RegisterDescriptor a, const BaseSemantics::SValuePtr &b,
                      const BaseSemantics::SValuePtr &c, size_t d)
{
    checkSubdomain();
    linePrefix();
    SAWYER_MESG(stream_) <<operator_name <<"(" <<register_name(a) <<", " <<toString(b) <<", " <<toString(c) <<", " <<d <<")";
}

void
RiscOperators::before(const std::string &operator_name, RegisterDescriptor a, const BaseSemantics::SValuePtr &b,
                      const BaseSemantics::SValuePtr &c, const BaseSemantics::SValuePtr &d)
{
    checkSubdomain();
    linePrefix();
    SAWYER_MESG(stream_) <<operator_name <<"(" <<register_name(a) <<", " <<toString(b) <<", " <<toString(c) <<", "
                         <<toString(d) <<")";
}

void
RiscOperators::before(const std::string &operator_name, SgAsmInstruction *insn, bool showAddress)
{
    linePrefix();
    if (showAddress) {
        SAWYER_MESG(stream_) <<operator_name <<"(" <<insn->toString() <<")";
    } else {
        SAWYER_MESG(stream_) <<operator_name <<"(" <<StringUtility::trim(unparseInstruction(insn)) <<")";
    }
    checkSubdomain();
}

void
RiscOperators::before(const std::string &operator_name, size_t a)
{
    linePrefix();
    SAWYER_MESG(stream_) <<operator_name <<"(" <<a <<")";
    checkSubdomain();
}

void
RiscOperators::before(const std::string &operator_name, size_t a, uint64_t b)
{
    linePrefix();
    SAWYER_MESG(stream_) <<operator_name <<"(" <<a <<", " <<b <<")";
    checkSubdomain();
}

void
RiscOperators::before(const std::string &operator_name, const BaseSemantics::SValuePtr &a)
{
    linePrefix();
    SAWYER_MESG(stream_) <<operator_name <<"(" <<toString(a) <<")";
    checkSubdomain();
}

void
RiscOperators::before(const std::string &operator_name, const BaseSemantics::SValuePtr &a, size_t b)
{
    linePrefix();
    SAWYER_MESG(stream_) <<operator_name <<"(" <<toString(a) <<", " <<b <<")";
    checkSubdomain();
}

void
RiscOperators::before(const std::string &operator_name, const BaseSemantics::SValuePtr &a, size_t b, size_t c)
{
    linePrefix();
    SAWYER_MESG(stream_) <<operator_name <<"(" <<toString(a) <<", " <<b <<", " <<c <<")";
    checkSubdomain();
}

void
RiscOperators::before(const std::string &operator_name, const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b)
{
    linePrefix();
    SAWYER_MESG(stream_) <<operator_name <<"(" <<toString(a) <<", " <<toString(b) <<")";
    checkSubdomain();
}

void
RiscOperators::before(const std::string &operator_name, const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b,
                      const BaseSemantics::SValuePtr &c)
{
    linePrefix();
    SAWYER_MESG(stream_) <<operator_name <<"(" <<toString(a) <<", " <<toString(b) <<", " <<toString(c) <<")";
    checkSubdomain();
}

void
RiscOperators::before(const std::string &operator_name, const BaseSemantics::SValuePtr &a, SgAsmFloatType *at) {
    linePrefix();
    SAWYER_MESG(stream_) <<operator_name <<"(" <<toString(a) <<", " <<toString(at) <<")";
    checkSubdomain();
}

void
RiscOperators::before(const std::string &operator_name, const BaseSemantics::SValuePtr &a, SgAsmFloatType *at,
                      const BaseSemantics::SValuePtr &b) {
    linePrefix();
    SAWYER_MESG(stream_) <<operator_name <<"(" <<toString(a) <<", " <<toString(at) <<", " <<toString(b) <<")";
    checkSubdomain();
}

void
RiscOperators::before(const std::string &operator_name, const BaseSemantics::SValuePtr &a, SgAsmFloatType *at,
                      SgAsmFloatType *bt) {
    linePrefix();
    SAWYER_MESG(stream_) <<operator_name <<"(" <<toString(a) <<", " <<toString(at) <<", " <<toString(bt) <<")";
    checkSubdomain();
}

void
RiscOperators::before(const std::string &operator_name, const BaseSemantics::SValuePtr &a,
                      const BaseSemantics::SValuePtr &b, SgAsmFloatType *abt) {
    linePrefix();
    SAWYER_MESG(stream_) <<operator_name <<"(" <<toString(a) <<", " <<toString(b) <<", " <<toString(abt) <<")";
    checkSubdomain();
}

void
RiscOperators::after()
{
    stream_ <<"\n";
}

const BaseSemantics::SValuePtr &
RiscOperators::after(const BaseSemantics::SValuePtr &retval)
{
    SAWYER_MESG(stream_) <<" = " <<toString(retval) <<"\n";
    return retval;
}

const BaseSemantics::SValuePtr &
RiscOperators::after(const BaseSemantics::SValuePtr &retval, const BaseSemantics::SValuePtr &ret2)
{
    SAWYER_MESG(stream_) <<" = " <<toString(retval) <<"\n";
    linePrefix();
    SAWYER_MESG(stream_) <<"also returns: " <<toString(ret2) <<"\n";
    return retval;
}

void
RiscOperators::after(const BaseSemantics::Exception &e)
{
    SAWYER_MESG(stream_) <<" = Exception(" <<e.what() <<")\n";
}

void
RiscOperators::after_exception()
{
    stream_ <<" = <Exception>\n";
}

BaseSemantics::SValuePtr
RiscOperators::protoval() const
{
    checkSubdomain();
    return subdomain_->protoval();
}

void
RiscOperators::solver(const SmtSolverPtr &s)
{
    checkSubdomain();
    subdomain_->solver(s);
}

SmtSolverPtr
RiscOperators::solver() const
{
    checkSubdomain();
    return subdomain_->solver();
}

BaseSemantics::StatePtr
RiscOperators::currentState() const
{
    checkSubdomain();
    return subdomain_->currentState();
}

void
RiscOperators::currentState(const BaseSemantics::StatePtr &state)
{
    checkSubdomain();
    subdomain_->currentState(state);
}

void
RiscOperators::print(std::ostream &stream, BaseSemantics::Formatter &fmt) const
{
    checkSubdomain();
    subdomain_->print(stream, fmt);
}

size_t
RiscOperators::nInsns() const
{
    checkSubdomain();
    return subdomain_->nInsns();
}

void
RiscOperators::nInsns(size_t n)
{
    checkSubdomain();
    subdomain_->nInsns(n);
}

SgAsmInstruction *
RiscOperators::currentInstruction() const
{
    checkSubdomain();
    return subdomain_->currentInstruction();
}

void
RiscOperators::startInstruction(SgAsmInstruction *insn)
{
    BaseSemantics::RiscOperators::startInstruction(insn);
    before("startInstruction", insn, true /*show address*/);
    try {
        subdomain_->startInstruction(insn);
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
    before("finishInstruction", insn, false /*hide address*/); // address is part of prefix
    try {
        subdomain_->finishInstruction(insn);
        after();
    } catch (const BaseSemantics::Exception &e) {
        after(e);
        throw;
    } catch (...) {
        after_exception();
        throw;
    }
    BaseSemantics::RiscOperators::finishInstruction(insn);
}

BaseSemantics::SValuePtr
RiscOperators::undefined_(size_t nbits)
{
    before("undefined_", nbits);
    try {
        return check_width(after(subdomain_->undefined_(nbits)), nbits);
    } catch (const BaseSemantics::Exception &e) {
        after(e);
        throw;
    } catch (...) {
        after_exception();
        throw;
    }
}

BaseSemantics::SValuePtr
RiscOperators::unspecified_(size_t nbits)
{
    before("unspecified_", nbits);
    try {
        return check_width(after(subdomain_->unspecified_(nbits)), nbits);
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
        return check_width(after(subdomain_->number_(nbits, value)), nbits);
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
        return check_width(after(subdomain_->boolean_(value)), 1);
    } catch (const BaseSemantics::Exception &e) {
        after(e);
        throw;
    } catch (...) {
        after_exception();
        throw;
    }
}

BaseSemantics::SValuePtr
RiscOperators::bottom_(size_t nbits)
{
    before("bottom_", nbits);
    try {
        return check_width(after(subdomain_->bottom_(nbits)), nbits);
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
        return check_width(after(subdomain_->filterCallTarget(a)), a->get_width());
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
        return check_width(after(subdomain_->filterReturnTarget(a)), a->get_width());
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
        return check_width(after(subdomain_->filterIndirectJumpTarget(a)), a->get_width());
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
        subdomain_->hlt();
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
        subdomain_->cpuid();
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
        return check_width(after(subdomain_->rdtsc()), 64);
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
        return check_width(after(subdomain_->and_(a, b)), a->get_width());
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
        return check_width(after(subdomain_->or_(a, b)), a->get_width());
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
        return check_width(after(subdomain_->xor_(a, b)), a->get_width());;
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
        return check_width(after(subdomain_->invert(a)), a->get_width());
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
        return check_width(after(subdomain_->extract(a, b, c)), c-b);
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
        return check_width(after(subdomain_->concat(a, b)), a->get_width()+b->get_width());
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
        return check_width(after(subdomain_->leastSignificantSetBit(a)), a->get_width());
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
        return check_width(after(subdomain_->mostSignificantSetBit(a)), a->get_width());
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
        return check_width(after(subdomain_->rotateLeft(a, b)), a->get_width());
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
        return check_width(after(subdomain_->rotateRight(a, b)), a->get_width());
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
        return check_width(after(subdomain_->shiftLeft(a, b)), a->get_width());
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
        return check_width(after(subdomain_->shiftRight(a, b)), a->get_width());
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
        return check_width(after(subdomain_->shiftRightArithmetic(a, b)), a->get_width());
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
        return check_width(after(subdomain_->equalToZero(a)), 1);
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
        return check_width(after(subdomain_->ite(a, b, c)), b->get_width());
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
        return check_width(after(subdomain_->unsignedExtend(a, b)), b);
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
        return check_width(after(subdomain_->signExtend(a, b)), b);
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
        return check_width(after(subdomain_->add(a, b)), a->get_width());
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
        BaseSemantics::SValuePtr retval = subdomain_->addWithCarries(a, b, c, d);
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
        return check_width(after(subdomain_->negate(a)), a->get_width());
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
        return check_width(after(subdomain_->signedDivide(a, b)), a->get_width());
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
        return check_width(after(subdomain_->signedModulo(a, b)), b->get_width());
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
        return check_width(after(subdomain_->signedMultiply(a, b)), a->get_width()+b->get_width());
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
        return check_width(after(subdomain_->unsignedDivide(a, b)), a->get_width());
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
        return check_width(after(subdomain_->unsignedModulo(a, b)), b->get_width());
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
        return check_width(after(subdomain_->unsignedMultiply(a, b)), a->get_width()+b->get_width());
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
        subdomain_->interrupt(a, b);
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
RiscOperators::fpFromInteger(const BaseSemantics::SValuePtr &a, SgAsmFloatType *at) {
    before("fpFromInteger", a, at);
    try {
        return check_width(after(subdomain_->fpFromInteger(a, at)), at->get_nBits());
    } catch (const BaseSemantics::Exception &e) {
        after(e);
        throw;
    } catch (...) {
        after_exception();
        throw;
    }
}

BaseSemantics::SValuePtr
RiscOperators::fpToInteger(const BaseSemantics::SValuePtr &a, SgAsmFloatType *at, const BaseSemantics::SValuePtr &b) {
    before("fpToInteger", a, at, b);
    try {
        return check_width(after(subdomain_->fpToInteger(a, at, b)), b->get_width());
    } catch (const BaseSemantics::Exception &e) {
        after(e);
        throw;
    } catch (...) {
        after_exception();
        throw;
    }
}

BaseSemantics::SValuePtr
RiscOperators::fpConvert(const BaseSemantics::SValuePtr &a, SgAsmFloatType *at, SgAsmFloatType *bt) {
    before("fpConvert", a, at, bt);
    try {
        return check_width(after(subdomain_->fpConvert(a, at, bt)), bt->get_nBits());
    } catch (const BaseSemantics::Exception &e) {
        after(e);
        throw;
    } catch (...) {
        after_exception();
        throw;
    }
}

BaseSemantics::SValuePtr
RiscOperators::fpIsNan(const BaseSemantics::SValuePtr &a, SgAsmFloatType *at) {
    before("fpIsNan", a, at);
    try {
        return check_width(after(subdomain_->fpIsNan(a, at)), 1);
    } catch (const BaseSemantics::Exception &e) {
        after(e);
        throw;
    } catch (...) {
        after_exception();
        throw;
    }
}

BaseSemantics::SValuePtr
RiscOperators::fpIsDenormalized(const BaseSemantics::SValuePtr &a, SgAsmFloatType *at) {
    before("fpIsDenormalized", a, at);
    try {
        return check_width(after(subdomain_->fpIsDenormalized(a, at)), 1);
    } catch (const BaseSemantics::Exception &e) {
        after(e);
        throw;
    } catch (...) {
        after_exception();
        throw;
    }
}

BaseSemantics::SValuePtr
RiscOperators::fpIsZero(const BaseSemantics::SValuePtr &a, SgAsmFloatType *at) {
    before("fpIsZero", a, at);
    try {
        return check_width(after(subdomain_->fpIsZero(a, at)), 1);
    } catch (const BaseSemantics::Exception &e) {
        after(e);
        throw;
    } catch (...) {
        after_exception();
        throw;
    }
}

BaseSemantics::SValuePtr
RiscOperators::fpIsInfinity(const BaseSemantics::SValuePtr &a, SgAsmFloatType *at) {
    before("fpIsInfinity", a, at);
    try {
        return check_width(after(subdomain_->fpIsInfinity(a, at)), 1);
    } catch (const BaseSemantics::Exception &e) {
        after(e);
        throw;
    } catch (...) {
        after_exception();
        throw;
    }
}

BaseSemantics::SValuePtr
RiscOperators::fpSign(const BaseSemantics::SValuePtr &a, SgAsmFloatType *at) {
    before("fpSign", a, at);
    try {
        return check_width(after(subdomain_->fpSign(a, at)), 1);
    } catch (const BaseSemantics::Exception &e) {
        after(e);
        throw;
    } catch (...) {
        after_exception();
        throw;
    }
}

BaseSemantics::SValuePtr
RiscOperators::fpEffectiveExponent(const BaseSemantics::SValuePtr &a, SgAsmFloatType *at) {
    before("fpEffectiveExponent", a, at);
    try {
        return check_width(after(subdomain_->fpEffectiveExponent(a, at)), at->exponentBits().size());
    } catch (const BaseSemantics::Exception &e) {
        after(e);
        throw;
    } catch (...) {
        after_exception();
        throw;
    }
}

BaseSemantics::SValuePtr
RiscOperators::fpAdd(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b, SgAsmFloatType *abt) {
    before("fpAdd", a, b, abt);
    try {
        return check_width(after(subdomain_->fpAdd(a, b, abt)), abt->get_nBits());
    } catch (const BaseSemantics::Exception &e) {
        after(e);
        throw;
    } catch (...) {
        after_exception();
        throw;
    }
}

BaseSemantics::SValuePtr
RiscOperators::fpSubtract(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b, SgAsmFloatType *abt) {
    before("fpSubtract", a, b, abt);
    try {
        return check_width(after(subdomain_->fpSubtract(a, b, abt)), abt->get_nBits());
    } catch (const BaseSemantics::Exception &e) {
        after(e);
        throw;
    } catch (...) {
        after_exception();
        throw;
    }
}

BaseSemantics::SValuePtr
RiscOperators::fpMultiply(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b, SgAsmFloatType *abt) {
    before("fpMultiply", a, b, abt);
    try {
        return check_width(after(subdomain_->fpMultiply(a, b, abt)), abt->get_nBits());
    } catch (const BaseSemantics::Exception &e) {
        after(e);
        throw;
    } catch (...) {
        after_exception();
        throw;
    }
}

BaseSemantics::SValuePtr
RiscOperators::fpDivide(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b, SgAsmFloatType *abt) {
    before("fpDivide", a, b, abt);
    try {
        return check_width(after(subdomain_->fpDivide(a, b, abt)), abt->get_nBits());
    } catch (const BaseSemantics::Exception &e) {
        after(e);
        throw;
    } catch (...) {
        after_exception();
        throw;
    }
}

BaseSemantics::SValuePtr
RiscOperators::fpSquareRoot(const BaseSemantics::SValuePtr &a, SgAsmFloatType *at) {
    before("fpSquareRoot", a, at);
    try {
        return check_width(after(subdomain_->fpSquareRoot(a, at)), at->get_nBits());
    } catch (const BaseSemantics::Exception &e) {
        after(e);
        throw;
    } catch (...) {
        after_exception();
        throw;
    }
}

BaseSemantics::SValuePtr
RiscOperators::fpRoundTowardZero(const BaseSemantics::SValuePtr &a, SgAsmFloatType *at) {
    before("fpRoundTowardZero", a, at);
    try {
        return check_width(after(subdomain_->fpRoundTowardZero(a, at)), at->get_nBits());
    } catch (const BaseSemantics::Exception &e) {
        after(e);
        throw;
    } catch (...) {
        after_exception();
        throw;
    }
}

BaseSemantics::SValuePtr
RiscOperators::readRegister(RegisterDescriptor a, const BaseSemantics::SValuePtr &b)
{
    before("readRegister", a, b);
    try {
        return check_width(after(subdomain_->readRegister(a, b)), a.nBits());
    } catch (const BaseSemantics::Exception &e) {
        after(e);
        throw;
    } catch (...) {
        after_exception();
        throw;
    }
}

BaseSemantics::SValuePtr
RiscOperators::peekRegister(RegisterDescriptor a, const BaseSemantics::SValuePtr &b)
{
    before("peekRegister", a, b);
    try {
        return check_width(after(subdomain_->peekRegister(a, b)), a.nBits());
    } catch (const BaseSemantics::Exception &e) {
        after(e);
        throw;
    } catch (...) {
        after_exception();
        throw;
    }
}

void
RiscOperators::writeRegister(RegisterDescriptor a, const BaseSemantics::SValuePtr &b)
{
    before("writeRegister", a, b);
    try {
        subdomain_->writeRegister(a, b);
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
RiscOperators::readMemory(RegisterDescriptor a, const BaseSemantics::SValuePtr &b, const BaseSemantics::SValuePtr &c,
                          const BaseSemantics::SValuePtr &d)
{
    before("readMemory", a, b, c, d);
    try {
        return check_width(after(subdomain_->readMemory(a, b, c, d)), c->get_width());
    } catch (const BaseSemantics::Exception &e) {
        after(e);
        throw;
    } catch (...) {
        after_exception();
        throw;
    }
}

BaseSemantics::SValuePtr
RiscOperators::peekMemory(RegisterDescriptor a, const BaseSemantics::SValuePtr &b, const BaseSemantics::SValuePtr &c)
{
    before("peekMemory", a, b, c);
    try {
        return check_width(after(subdomain_->peekMemory(a, b, c)), c->get_width());
    } catch (const BaseSemantics::Exception &e) {
        after(e);
        throw;
    } catch (...) {
        after_exception();
        throw;
    }
}

void
RiscOperators::writeMemory(RegisterDescriptor a, const BaseSemantics::SValuePtr &b, const BaseSemantics::SValuePtr &c,
                           const BaseSemantics::SValuePtr &d)
{
    before("writeMemory", a, b, c, d);
    try {
        subdomain_->writeMemory(a, b, c, d);
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
