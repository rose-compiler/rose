#include "sage3basic.h"
#include "MultiSemantics2.h"

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics2 {
namespace MultiSemantics {

/*******************************************************************************************************************************
 *                                      Semantic values
 *******************************************************************************************************************************/

void
SValue::init(const SValue &other)
{
    for (size_t i=0; i<subvalues.size(); ++i) {
        if (subvalues[i]!=NULL)
            subvalues[i] = subvalues[i]->copy();
    }
}

BaseSemantics::SValuePtr
SValue::bottom_(size_t nbits) const {
    SValuePtr retval = create_empty(nbits);
    ASSERT_require(retval->subvalues.empty());
    for (size_t i=0; i<subvalues.size(); ++i)
        retval->subvalues.push_back(subvalues[i]!=NULL ? subvalues[i]->bottom_(nbits) : BaseSemantics::SValuePtr());
    return retval;
}

BaseSemantics::SValuePtr
SValue::undefined_(size_t nbits) const
{
    SValuePtr retval = create_empty(nbits);
    ASSERT_require(retval->subvalues.empty());
    for (size_t i=0; i<subvalues.size(); ++i)
        retval->subvalues.push_back(subvalues[i]!=NULL ? subvalues[i]->undefined_(nbits) : BaseSemantics::SValuePtr());
    return retval;
}

BaseSemantics::SValuePtr
SValue::unspecified_(size_t nbits) const
{
    SValuePtr retval = create_empty(nbits);
    ASSERT_require(retval->subvalues.empty());
    for (size_t i=0; i<subvalues.size(); ++i)
        retval->subvalues.push_back(subvalues[i]!=NULL ? subvalues[i]->unspecified_(nbits) : BaseSemantics::SValuePtr());
    return retval;
}

BaseSemantics::SValuePtr
SValue::number_(size_t nbits, uint64_t number) const
{
    SValuePtr retval = create_empty(nbits);
    ASSERT_require(retval->subvalues.empty());
    for (size_t i=0; i<subvalues.size(); ++i)
        retval->subvalues.push_back(subvalues[i]!=NULL ? subvalues[i]->number_(nbits, number) : BaseSemantics::SValuePtr());
    return retval;
}

Sawyer::Optional<BaseSemantics::SValuePtr>
SValue::createOptionalMerge(const BaseSemantics::SValuePtr &other_, const BaseSemantics::MergerPtr &merger,
                            const SmtSolverPtr &solver) const {
    SValuePtr other = SValue::promote(other_);
    SValuePtr retval = create_empty(other->get_width());
    bool changed = false;
    for (size_t i=0; i<subvalues.size(); ++i) {
        BaseSemantics::SValuePtr thisValue = subvalues[i];
        BaseSemantics::SValuePtr otherValue = other->subvalues[i];
        if (otherValue) {
            if (thisValue==NULL) {
                retval->subvalues.push_back(otherValue);
                changed = true;
            } else if (BaseSemantics::SValuePtr mergedValue =
                       thisValue->createOptionalMerge(otherValue, merger, solver).orDefault()) {
                changed = true;
                retval->subvalues.push_back(mergedValue);
            } else {
                retval->subvalues.push_back(thisValue);
            }
        } else {
            retval->subvalues.push_back(thisValue);
        }
    }
    return changed ? Sawyer::Optional<BaseSemantics::SValuePtr>(retval) : Sawyer::Nothing();
}

bool
SValue::may_equal(const BaseSemantics::SValuePtr &other_, const SmtSolverPtr &solver) const 
{
    SValuePtr other = SValue::promote(other_);
    for (size_t i=0; i<subvalues.size(); ++i) {
        if (is_valid(i) && other->is_valid(i) && get_subvalue(i)->may_equal(other->get_subvalue(i), solver))
            return true;
    }
    return false;
}

bool
SValue::must_equal(const BaseSemantics::SValuePtr &other_, const SmtSolverPtr &solver) const
{
    SValuePtr other = SValue::promote(other_);
    size_t nconsidered = 0;
    for (size_t i=0; i<subvalues.size(); ++i) {
        if (is_valid(i) && other->is_valid(i)) {
            if (!get_subvalue(i)->must_equal(other->get_subvalue(i), solver))
                return false;
            ++nconsidered;
        }
    }
    return nconsidered>0;
}

void
SValue::set_width(size_t nbits)
{
    BaseSemantics::SValue::set_width(nbits);
    for (size_t i=0; i<subvalues.size(); ++i) {
        if (is_valid(i))
            subvalues[i]->set_width(nbits);
    }
}

bool
SValue::isBottom() const {
    for (size_t i=0; i<subvalues.size(); ++i) {
        if (is_valid(i) && !subvalues[i]->isBottom())
            return false;
    }
    return true;
}

bool
SValue::is_number() const
{
    uint64_t number = 0;
    size_t nnumbers = 0;
    for (size_t i=0; i<subvalues.size(); ++i) {
        if (is_valid(i)) {
            if (!subvalues[i]->is_number())
                return false;
            if (0==nnumbers++) {
                number = subvalues[i]->get_number();
            } else if (number != subvalues[i]->get_number()) {
                return false;
            }
        }
    }
    return nnumbers>0;
}

uint64_t
SValue::get_number() const
{
    for (size_t i=0; i<subvalues.size(); ++i) {
        if (is_valid(i))
            return subvalues[i]->get_number();
    }
    ASSERT_not_reachable("not a number");
}

void
SValue::print(std::ostream &output, BaseSemantics::Formatter &formatter_) const
{
    Formatter *formatter = dynamic_cast<Formatter*>(&formatter_);

    size_t nprinted = 0;
    output <<"{";
    for (size_t i=0; i<subvalues.size(); ++i) {
        if (is_valid(i)) {
            output <<(nprinted++?", ":"");
            if (formatter && i<formatter->subdomain_names.size()) {
                output <<formatter->subdomain_names[i] <<"=";
            } else {
                output <<"subdomain-" <<i <<"=";
            }
            subvalues[i]->print(output, formatter_);
        }
    }
    output <<"}";
}

void
SValue::invalidate(size_t idx)
{
    if (idx<subvalues.size())
        subvalues[idx] = BaseSemantics::SValuePtr();
}



/*******************************************************************************************************************************
 *                                      Subdomain cursor
 *******************************************************************************************************************************/

void
RiscOperators::Cursor::init(const SValuePtr &arg1, const SValuePtr &arg2, const SValuePtr &arg3)
{
    ASSERT_require((arg1==NULL && arg2==NULL && arg3==NULL) ||
                   (arg1!=NULL && arg2==NULL && arg3==NULL) ||
                   (arg1!=NULL && arg2!=NULL && arg3==NULL) ||
                   (arg1!=NULL && arg2!=NULL && arg3!=NULL));
    if (arg1!=NULL)
        inputs_.push_back(arg1);
    if (arg2!=NULL)
        inputs_.push_back(arg2);
    if (arg3!=NULL)
        inputs_.push_back(arg3);
    init();
}

void
RiscOperators::Cursor::init()
{
    skip_invalid();
    if (!at_end())
        ops_->before(idx_);
}

RiscOperators::Cursor::Inputs
RiscOperators::Cursor::inputs(const BaseSemantics::SValuePtr &arg1, const BaseSemantics::SValuePtr &arg2,
                              const BaseSemantics::SValuePtr &arg3)
{
    ASSERT_require((arg1==NULL && arg2==NULL && arg3==NULL) ||
                   (arg1!=NULL && arg2==NULL && arg3==NULL) ||
                   (arg1!=NULL && arg2!=NULL && arg3==NULL) ||
                   (arg1!=NULL && arg2!=NULL && arg3!=NULL));
    Inputs inputs;
    if (arg1!=NULL)
        inputs.push_back(SValue::promote(arg1));
    if (arg2!=NULL)
        inputs.push_back(SValue::promote(arg2));
    if (arg3!=NULL)
        inputs.push_back(SValue::promote(arg3));
    return inputs;
}

bool
RiscOperators::Cursor::at_end() const
{
    return idx_ >= ops_->nsubdomains();
}

void
RiscOperators::Cursor::next()
{
    ops_->after(idx_);
    ++idx_;
    skip_invalid();
    if (!at_end())
        ops_->before(idx_);
}

size_t
RiscOperators::Cursor::idx() const
{
    ASSERT_require(!at_end());
    return idx_;
}

BaseSemantics::SValuePtr
RiscOperators::Cursor::operator()(const BaseSemantics::SValuePtr &a_) const
{
    ASSERT_require(!at_end());
    SValuePtr a = SValue::promote(a_);
    return a->get_subvalue(idx_);
}

BaseSemantics::RiscOperatorsPtr
RiscOperators::Cursor::operator->() const
{
    ASSERT_require(!at_end());
    return ops_->get_subdomain(idx_);
}

BaseSemantics::RiscOperatorsPtr
RiscOperators::Cursor::operator*() const
{
    ASSERT_require(!at_end());
    return ops_->get_subdomain(idx_);
}

void
RiscOperators::Cursor::skip_invalid()
{
    while (idx_<ops_->nsubdomains() && (!ops_->is_active(idx_) || !inputs_are_valid()))
        ++idx_;
}

bool
RiscOperators::Cursor::inputs_are_valid() const
{
    for (Inputs::const_iterator ii=inputs_.begin(); ii!=inputs_.end(); ++ii) {
        if (!(*ii)->is_valid(idx_))
            return false;
    }
    return true;
}

#ifdef SUBDOMAINS
#error "SUBDOMAINS is already defined"
#endif
#define SUBDOMAINS(CURSOR, INPUTS) for (Cursor CURSOR(this, Cursor::inputs INPUTS); !CURSOR.at_end(); CURSOR.next())

/*******************************************************************************************************************************
 *                                      RISC operators
 *******************************************************************************************************************************/

RiscOperatorsPtr
RiscOperators::promote(const BaseSemantics::RiscOperatorsPtr &ops)
{
    RiscOperatorsPtr retval = boost::dynamic_pointer_cast<RiscOperators>(ops);
    ASSERT_not_null(retval);
    return retval;
}

size_t
RiscOperators::add_subdomain(const BaseSemantics::RiscOperatorsPtr &subdomain, const std::string &name, bool activate)
{
    ASSERT_not_null(subdomain);
    size_t idx = subdomains.size();
    subdomains.push_back(subdomain);
    active.push_back(activate);
    if (idx>=formatter.subdomain_names.size())
        formatter.subdomain_names.resize(idx+1, "");
    formatter.subdomain_names[idx] = name;
    SValue::promote(protoval())->set_subvalue(idx, subdomain->protoval());
    return idx;
}

BaseSemantics::RiscOperatorsPtr
RiscOperators::get_subdomain(size_t idx) const
{
    if (idx>=subdomains.size())
        return BaseSemantics::RiscOperatorsPtr();
    return subdomains[idx];
}

void
RiscOperators::set_active(size_t idx, bool status)
{
    if (!status) {
        if (idx<subdomains.size())
            active[idx] = false;
    } else {
        ASSERT_require(idx<subdomains.size() && subdomains[idx]!=NULL);
        active[idx] = true;
    }
}

void
RiscOperators::print(std::ostream &stream, BaseSemantics::Formatter &formatter) const
{
    for (Subdomains::const_iterator sdi=subdomains.begin(); sdi!=subdomains.end(); ++sdi)
        stream <<"== " <<(*sdi)->name() <<" ==\n" <<(**sdi + formatter);
}

void
RiscOperators::startInstruction(SgAsmInstruction *insn)
{
    BaseSemantics::RiscOperators::startInstruction(insn);
    SUBDOMAINS(sd, ())
        sd->startInstruction(insn);
}

void
RiscOperators::finishInstruction(SgAsmInstruction *insn)
{
    SUBDOMAINS(sd, ())
        sd->finishInstruction(insn);
    BaseSemantics::RiscOperators::finishInstruction(insn);
}

BaseSemantics::SValuePtr
RiscOperators::undefined_(size_t nbits)
{
    SValuePtr retval = svalue_empty(nbits);
    SUBDOMAINS(sd, ())
        retval->set_subvalue(sd.idx(), sd->undefined_(nbits));
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::bottom_(size_t nbits)
{
    SValuePtr retval = svalue_empty(nbits);
    SUBDOMAINS(sd, ())
        retval->set_subvalue(sd.idx(), sd->bottom_(nbits));
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::unspecified_(size_t nbits)
{
    SValuePtr retval = svalue_empty(nbits);
    SUBDOMAINS(sd, ())
        retval->set_subvalue(sd.idx(), sd->unspecified_(nbits));
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::number_(size_t nbits, uint64_t value)
{
    SValuePtr retval = svalue_empty(nbits);
    SUBDOMAINS(sd, ())
        retval->set_subvalue(sd.idx(), sd->number_(nbits, value));
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::boolean_(bool value)
{
    SValuePtr retval = svalue_empty(1);
    SUBDOMAINS(sd, ())
        retval->set_subvalue(sd.idx(), sd->boolean_(value));
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::filterCallTarget(const BaseSemantics::SValuePtr &a)
{
    SValuePtr retval = svalue_empty(a->get_width());
    SUBDOMAINS(sd, (a))
        retval->set_subvalue(sd.idx(), sd->filterCallTarget(sd(a)));
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::filterReturnTarget(const BaseSemantics::SValuePtr &a)
{
    SValuePtr retval = svalue_empty(a->get_width());
    SUBDOMAINS(sd, (a))
        retval->set_subvalue(sd.idx(), sd->filterReturnTarget(sd(a)));
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::filterIndirectJumpTarget(const BaseSemantics::SValuePtr &a)
{
    SValuePtr retval = svalue_empty(a->get_width());
    SUBDOMAINS(sd, (a))
        retval->set_subvalue(sd.idx(), sd->filterIndirectJumpTarget(sd(a)));
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::and_(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b)
{
    SValuePtr retval = svalue_empty(a->get_width());
    SUBDOMAINS(sd, (a, b))
        retval->set_subvalue(sd.idx(), sd->and_(sd(a), sd(b)));
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::or_(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b)
{
    SValuePtr retval = svalue_empty(a->get_width());
    SUBDOMAINS(sd, (a, b))
        retval->set_subvalue(sd.idx(), sd->or_(sd(a), sd(b)));
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::xor_(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b)
{
    SValuePtr retval = svalue_empty(a->get_width());
    SUBDOMAINS(sd, (a, b))
        retval->set_subvalue(sd.idx(), sd->xor_(sd(a), sd(b)));
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::invert(const BaseSemantics::SValuePtr &a)
{
    SValuePtr retval = svalue_empty(a->get_width());
    SUBDOMAINS(sd, (a))
        retval->set_subvalue(sd.idx(), sd->invert(sd(a)));
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::extract(const BaseSemantics::SValuePtr &a, size_t begin_bit, size_t end_bit)
{
    ASSERT_require(end_bit <= a->get_width());
    ASSERT_require(end_bit > begin_bit);
    SValuePtr retval = svalue_empty(end_bit-begin_bit);
    SUBDOMAINS(sd, (a))
        retval->set_subvalue(sd.idx(), sd->extract(sd(a), begin_bit, end_bit));
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::concat(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b)
{
    SValuePtr retval = svalue_empty(a->get_width() + b->get_width());
    SUBDOMAINS(sd, (a, b))
        retval->set_subvalue(sd.idx(), sd->concat(sd(a), sd(b)));
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::leastSignificantSetBit(const BaseSemantics::SValuePtr &a)
{
    SValuePtr retval = svalue_empty(a->get_width());
    SUBDOMAINS(sd, (a))
        retval->set_subvalue(sd.idx(), sd->leastSignificantSetBit(sd(a)));
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::mostSignificantSetBit(const BaseSemantics::SValuePtr &a)
{
    SValuePtr retval = svalue_empty(a->get_width());
    SUBDOMAINS(sd, (a))
        retval->set_subvalue(sd.idx(), sd->mostSignificantSetBit(sd(a)));
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::rotateLeft(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &nbits)
{
    SValuePtr retval = svalue_empty(a->get_width());
    SUBDOMAINS(sd, (a, nbits))
        retval->set_subvalue(sd.idx(), sd->rotateLeft(sd(a), sd(nbits)));
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::rotateRight(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &nbits)
{
    SValuePtr retval = svalue_empty(a->get_width());
    SUBDOMAINS(sd, (a, nbits))
        retval->set_subvalue(sd.idx(), sd->rotateRight(sd(a), sd(nbits)));
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::shiftLeft(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &nbits)
{
    SValuePtr retval = svalue_empty(a->get_width());
    SUBDOMAINS(sd, (a, nbits))
        retval->set_subvalue(sd.idx(), sd->shiftLeft(sd(a), sd(nbits)));
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::shiftRight(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &nbits)
{
    SValuePtr retval = svalue_empty(a->get_width());
    SUBDOMAINS(sd, (a, nbits))
        retval->set_subvalue(sd.idx(), sd->shiftRight(sd(a), sd(nbits)));
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::shiftRightArithmetic(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &nbits)
{
    SValuePtr retval = svalue_empty(a->get_width());
    SUBDOMAINS(sd, (a, nbits))
        retval->set_subvalue(sd.idx(), sd->shiftRightArithmetic(sd(a), sd(nbits)));
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::equalToZero(const BaseSemantics::SValuePtr &a)
{
    SValuePtr retval = svalue_empty(1);
    SUBDOMAINS(sd, (a))
        retval->set_subvalue(sd.idx(), sd->equalToZero(sd(a)));
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::ite(const BaseSemantics::SValuePtr &cond, const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b)
{
    SValuePtr retval = svalue_empty(a->get_width());
    SUBDOMAINS(sd, (cond, a, b))
        retval->set_subvalue(sd.idx(), sd->ite(sd(cond), sd(a), sd(b)));
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::unsignedExtend(const BaseSemantics::SValuePtr &a, size_t new_width)
{
    SValuePtr retval = svalue_empty(new_width);
    SUBDOMAINS(sd, (a))
        retval->set_subvalue(sd.idx(), sd->unsignedExtend(sd(a), new_width));
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::signExtend(const BaseSemantics::SValuePtr &a, size_t new_width)
{
    SValuePtr retval = svalue_empty(new_width);
    SUBDOMAINS(sd, (a))
        retval->set_subvalue(sd.idx(), sd->signExtend(sd(a), new_width));
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::add(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b)
{
    SValuePtr retval = svalue_empty(a->get_width());
    SUBDOMAINS(sd, (a, b))
        retval->set_subvalue(sd.idx(), sd->add(sd(a), sd(b)));
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::addWithCarries(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b,
                              const BaseSemantics::SValuePtr &c, BaseSemantics::SValuePtr &carry_out_/*output*/)
{
    SValuePtr retval = svalue_empty(a->get_width());
    SValuePtr carry_out = svalue_empty(a->get_width());
    SUBDOMAINS(sd, (a, b, c)) {
        BaseSemantics::SValuePtr co;
        retval->set_subvalue(sd.idx(), sd->addWithCarries(sd(a), sd(b), sd(c), co/*out*/));
        carry_out->set_subvalue(sd.idx(), co);
    }
    carry_out_ = carry_out;
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::negate(const BaseSemantics::SValuePtr &a)
{
    SValuePtr retval = svalue_empty(a->get_width());
    SUBDOMAINS(sd, (a))
        retval->set_subvalue(sd.idx(), sd->negate(sd(a)));
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::signedDivide(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b)
{
    SValuePtr retval = svalue_empty(a->get_width());
    SUBDOMAINS(sd, (a, b))
        retval->set_subvalue(sd.idx(), sd->signedDivide(sd(a), sd(b)));
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::signedModulo(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b)
{
    SValuePtr retval = svalue_empty(b->get_width());
    SUBDOMAINS(sd, (a, b))
        retval->set_subvalue(sd.idx(), sd->signedModulo(sd(a), sd(b)));
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::signedMultiply(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b)
{
    SValuePtr retval = svalue_empty(a->get_width() + b->get_width());
    SUBDOMAINS(sd, (a, b))
        retval->set_subvalue(sd.idx(), sd->signedMultiply(sd(a), sd(b)));
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::unsignedDivide(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b)
{
    SValuePtr retval = svalue_empty(a->get_width());
    SUBDOMAINS(sd, (a, b))
        retval->set_subvalue(sd.idx(), sd->unsignedDivide(sd(a), sd(b)));
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::unsignedModulo(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b)
{
    SValuePtr retval = svalue_empty(b->get_width());
    SUBDOMAINS(sd, (a, b))
        retval->set_subvalue(sd.idx(), sd->unsignedModulo(sd(a), sd(b)));
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::unsignedMultiply(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b)
{
    SValuePtr retval = svalue_empty(a->get_width() + b->get_width());
    SUBDOMAINS(sd, (a, b))
        retval->set_subvalue(sd.idx(), sd->unsignedMultiply(sd(a), sd(b)));
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::fpFromInteger(const BaseSemantics::SValuePtr &a, SgAsmFloatType *at) {
    SValuePtr retval = svalue_empty(at->get_nBits());
    SUBDOMAINS(sd, (a))
        retval->set_subvalue(sd.idx(), sd->fpFromInteger(sd(a), at));
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::fpToInteger(const BaseSemantics::SValuePtr &a, SgAsmFloatType *at, const BaseSemantics::SValuePtr &b) {
    SValuePtr retval = svalue_empty(b->get_width());
    SUBDOMAINS(sd, (a, b))
        retval->set_subvalue(sd.idx(), sd->fpToInteger(sd(a), at, sd(b)));
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::fpConvert(const BaseSemantics::SValuePtr &a, SgAsmFloatType *at, SgAsmFloatType *bt) {
    SValuePtr retval = svalue_empty(bt->get_nBits());
    SUBDOMAINS(sd, (a))
        retval->set_subvalue(sd.idx(), sd->fpConvert(sd(a), at, bt));
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::fpIsNan(const BaseSemantics::SValuePtr &a, SgAsmFloatType *at) {
    SValuePtr retval = svalue_empty(1);
    SUBDOMAINS(sd, (a))
        retval->set_subvalue(sd.idx(), sd->fpIsNan(sd(a), at));
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::fpIsDenormalized(const BaseSemantics::SValuePtr &a, SgAsmFloatType *at) {
    SValuePtr retval = svalue_empty(1);
    SUBDOMAINS(sd, (a))
        retval->set_subvalue(sd.idx(), sd->fpIsDenormalized(sd(a), at));
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::fpIsZero(const BaseSemantics::SValuePtr &a, SgAsmFloatType *at) {
    SValuePtr retval = svalue_empty(1);
    SUBDOMAINS(sd, (a))
        retval->set_subvalue(sd.idx(), sd->fpIsZero(sd(a), at));
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::fpIsInfinity(const BaseSemantics::SValuePtr &a, SgAsmFloatType *at) {
    SValuePtr retval = svalue_empty(1);
    SUBDOMAINS(sd, (a))
        retval->set_subvalue(sd.idx(), sd->fpIsInfinity(sd(a), at));
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::fpSign(const BaseSemantics::SValuePtr &a, SgAsmFloatType *at) {
    SValuePtr retval = svalue_empty(1);
    SUBDOMAINS(sd, (a))
        retval->set_subvalue(sd.idx(), sd->fpSign(sd(a), at));
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::fpEffectiveExponent(const BaseSemantics::SValuePtr &a, SgAsmFloatType *at) {
    SValuePtr retval = svalue_empty(at->exponentBits().size());
    SUBDOMAINS(sd, (a))
        retval->set_subvalue(sd.idx(), sd->fpEffectiveExponent(sd(a), at));
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::fpAdd(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b, SgAsmFloatType *abt) {
    SValuePtr retval = svalue_empty(abt->get_nBits());
    SUBDOMAINS(sd, (a, b))
        retval->set_subvalue(sd.idx(), sd->fpAdd(sd(a), sd(b), abt));
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::fpSubtract(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b, SgAsmFloatType *abt) {
    SValuePtr retval = svalue_empty(abt->get_nBits());
    SUBDOMAINS(sd, (a, b))
        retval->set_subvalue(sd.idx(), sd->fpSubtract(sd(a), sd(b), abt));
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::fpMultiply(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b, SgAsmFloatType *abt) {
    SValuePtr retval = svalue_empty(abt->get_nBits());
    SUBDOMAINS(sd, (a, b))
        retval->set_subvalue(sd.idx(), sd->fpMultiply(sd(a), sd(b), abt));
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::fpDivide(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b, SgAsmFloatType *abt) {
    SValuePtr retval = svalue_empty(abt->get_nBits());
    SUBDOMAINS(sd, (a, b))
        retval->set_subvalue(sd.idx(), sd->fpDivide(sd(a), sd(b), abt));
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::fpSquareRoot(const BaseSemantics::SValuePtr &a, SgAsmFloatType *at) {
    SValuePtr retval = svalue_empty(at->get_nBits());
    SUBDOMAINS(sd, (a))
        retval->set_subvalue(sd.idx(), sd->fpSquareRoot(sd(a), at));
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::fpRoundTowardZero(const BaseSemantics::SValuePtr &a, SgAsmFloatType *at) {
    SValuePtr retval = svalue_empty(at->get_nBits());
    SUBDOMAINS(sd, (a))
        retval->set_subvalue(sd.idx(), sd->fpRoundTowardZero(sd(a), at));
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::readRegister(RegisterDescriptor reg, const BaseSemantics::SValuePtr &dflt)
{
    SValuePtr retval = svalue_empty(reg.nBits());
    SUBDOMAINS(sd, ())
        retval->set_subvalue(sd.idx(), sd->readRegister(reg, sd(dflt)));
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::peekRegister(RegisterDescriptor reg, const BaseSemantics::SValuePtr &dflt)
{
    SValuePtr retval = svalue_empty(reg.nBits());
    SUBDOMAINS(sd, ())
        retval->set_subvalue(sd.idx(), sd->peekRegister(reg, sd(dflt)));
    return retval;
}

void
RiscOperators::writeRegister(RegisterDescriptor reg, const BaseSemantics::SValuePtr &a)
{
    SUBDOMAINS(sd, (a))
        sd->writeRegister(reg, sd(a));
}

BaseSemantics::SValuePtr
RiscOperators::readMemory(RegisterDescriptor segreg, const BaseSemantics::SValuePtr &addr,
                          const BaseSemantics::SValuePtr &dflt, const BaseSemantics::SValuePtr &cond)
{
    SValuePtr retval = svalue_empty(dflt->get_width());
    SUBDOMAINS(sd, (addr, cond))
        retval->set_subvalue(sd.idx(), sd->readMemory(segreg, sd(addr), sd(dflt), sd(cond)));
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::peekMemory(RegisterDescriptor segreg, const BaseSemantics::SValuePtr &addr,
                          const BaseSemantics::SValuePtr &dflt)
{
    SValuePtr retval = svalue_empty(dflt->get_width());
    SUBDOMAINS(sd, (addr))
        retval->set_subvalue(sd.idx(), sd->peekMemory(segreg, sd(addr), sd(dflt)));
    return retval;
}

void
RiscOperators::writeMemory(RegisterDescriptor segreg, const BaseSemantics::SValuePtr &addr,
                           const BaseSemantics::SValuePtr &data, const BaseSemantics::SValuePtr &cond)
{
    SUBDOMAINS(sd, (addr, data, cond))
        sd->writeMemory(segreg, sd(addr), sd(data), cond);
}

} // namespace
} // namespace
} // namespace
} // namespace
