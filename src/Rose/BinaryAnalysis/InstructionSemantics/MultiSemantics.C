#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/InstructionSemantics/MultiSemantics.h>

#include <SgAsmFloatType.h>

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics {
namespace MultiSemantics {

/*******************************************************************************************************************************
 *                                      Semantic values
 *******************************************************************************************************************************/

SValue::SValue(size_t nbits)
    : BaseSemantics::SValue(nbits) {}

SValue::SValue(const SValue &other)
    : BaseSemantics::SValue(other.nBits()) {
    init(other);
}

SValue::Ptr
SValue::instance() {
    return SValue::Ptr(new SValue(1));
}

SValue::Ptr
SValue::promote(const BaseSemantics::SValue::Ptr &v) { // hot
    SValue::Ptr retval = v.dynamicCast<SValue>();
    ASSERT_not_null(retval);
    return retval;
}

void
SValue::init(const SValue &/*other*/) {
    for (size_t i=0; i<subvalues.size(); ++i) {
        if (subvalues[i]!=NULL)
            subvalues[i] = subvalues[i]->copy();
    }
}

BaseSemantics::SValue::Ptr
SValue::bottom_(size_t nbits) const {
    SValue::Ptr retval = create_empty(nbits);
    ASSERT_require(retval->subvalues.empty());
    for (size_t i=0; i<subvalues.size(); ++i)
        retval->subvalues.push_back(subvalues[i]!=NULL ? subvalues[i]->bottom_(nbits) : BaseSemantics::SValue::Ptr());
    return retval;
}

BaseSemantics::SValue::Ptr
SValue::undefined_(size_t nbits) const
{
    SValue::Ptr retval = create_empty(nbits);
    ASSERT_require(retval->subvalues.empty());
    for (size_t i=0; i<subvalues.size(); ++i)
        retval->subvalues.push_back(subvalues[i]!=NULL ? subvalues[i]->undefined_(nbits) : BaseSemantics::SValue::Ptr());
    return retval;
}

BaseSemantics::SValue::Ptr
SValue::unspecified_(size_t nbits) const
{
    SValue::Ptr retval = create_empty(nbits);
    ASSERT_require(retval->subvalues.empty());
    for (size_t i=0; i<subvalues.size(); ++i)
        retval->subvalues.push_back(subvalues[i]!=NULL ? subvalues[i]->unspecified_(nbits) : BaseSemantics::SValue::Ptr());
    return retval;
}

BaseSemantics::SValue::Ptr
SValue::number_(size_t nbits, uint64_t number) const
{
    SValue::Ptr retval = create_empty(nbits);
    ASSERT_require(retval->subvalues.empty());
    for (size_t i=0; i<subvalues.size(); ++i)
        retval->subvalues.push_back(subvalues[i]!=NULL ? subvalues[i]->number_(nbits, number) : BaseSemantics::SValue::Ptr());
    return retval;
}

SValue::Ptr
SValue::create_empty(size_t nbits) const {
    return SValue::Ptr(new SValue(nbits));
}

BaseSemantics::SValue::Ptr
SValue::copy(size_t /*new_width*/) const {
    return BaseSemantics::SValue::Ptr(new SValue(*this));
}

Sawyer::Optional<BaseSemantics::SValue::Ptr>
SValue::createOptionalMerge(const BaseSemantics::SValue::Ptr &other_, const BaseSemantics::Merger::Ptr &merger,
                            const SmtSolverPtr &solver) const {
    SValue::Ptr other = SValue::promote(other_);
    SValue::Ptr retval = create_empty(other->nBits());
    bool changed = false;
    for (size_t i=0; i<subvalues.size(); ++i) {
        BaseSemantics::SValue::Ptr thisValue = subvalues[i];
        BaseSemantics::SValue::Ptr otherValue = other->subvalues[i];
        if (otherValue) {
            if (thisValue==NULL) {
                retval->subvalues.push_back(otherValue);
                changed = true;
            } else if (BaseSemantics::SValue::Ptr mergedValue =
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
    return changed ? Sawyer::Optional<BaseSemantics::SValue::Ptr>(retval) : Sawyer::Nothing();
}

bool
SValue::may_equal(const BaseSemantics::SValue::Ptr &other_, const SmtSolverPtr &solver) const
{
    SValue::Ptr other = SValue::promote(other_);
    for (size_t i=0; i<subvalues.size(); ++i) {
        if (is_valid(i) && other->is_valid(i) && get_subvalue(i)->mayEqual(other->get_subvalue(i), solver))
            return true;
    }
    return false;
}

bool
SValue::must_equal(const BaseSemantics::SValue::Ptr &other_, const SmtSolverPtr &solver) const
{
    SValue::Ptr other = SValue::promote(other_);
    size_t nconsidered = 0;
    for (size_t i=0; i<subvalues.size(); ++i) {
        if (is_valid(i) && other->is_valid(i)) {
            if (!get_subvalue(i)->mustEqual(other->get_subvalue(i), solver))
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
SValue::hash(Combinatorics::Hasher &hasher) const {
    for (const BaseSemantics::SValue::Ptr &subvalue: subvalues)
        subvalue->hash(hasher);
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
        subvalues[idx] = BaseSemantics::SValue::Ptr();
}

bool
SValue::is_valid(size_t idx) const { // hot
    return idx<subvalues.size() && subvalues[idx]!=NULL;
}

BaseSemantics::SValue::Ptr
SValue::get_subvalue(size_t idx) const { // hot
    ASSERT_require(idx<subvalues.size() && subvalues[idx]!=NULL); // you should have called is_valid() first
    return subvalues[idx];
}

void
SValue::set_subvalue(size_t idx, const BaseSemantics::SValue::Ptr &value) {
    ASSERT_require(value==NULL || value->nBits()==nBits());
    if (idx>=subvalues.size())
        subvalues.resize(idx+1);
    subvalues[idx] = value;
}


/*******************************************************************************************************************************
 *                                      Subdomain cursor
 *******************************************************************************************************************************/

RiscOperators::Cursor::Cursor(RiscOperators *ops, const SValue::Ptr &arg1, const SValue::Ptr &arg2, const SValue::Ptr &arg3)
    : ops_(ops), idx_(0) {
    init(arg1, arg2, arg3);
}

RiscOperators::Cursor::Cursor(RiscOperators *ops, const Inputs &inputs)
    : ops_(ops), inputs_(inputs), idx_(0) {
    init();
}

void
RiscOperators::Cursor::init(const SValue::Ptr &arg1, const SValue::Ptr &arg2, const SValue::Ptr &arg3)
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
RiscOperators::Cursor::inputs(const BaseSemantics::SValue::Ptr &arg1, const BaseSemantics::SValue::Ptr &arg2,
                              const BaseSemantics::SValue::Ptr &arg3)
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

BaseSemantics::SValue::Ptr
RiscOperators::Cursor::operator()(const BaseSemantics::SValue::Ptr &a_) const
{
    ASSERT_require(!at_end());
    SValue::Ptr a = SValue::promote(a_);
    return a->get_subvalue(idx_);
}

BaseSemantics::RiscOperators::Ptr
RiscOperators::Cursor::operator->() const
{
    ASSERT_require(!at_end());
    return ops_->get_subdomain(idx_);
}

BaseSemantics::RiscOperators::Ptr
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

RiscOperators::RiscOperators(const BaseSemantics::SValue::Ptr &protoval, const SmtSolver::Ptr &solver)
    : BaseSemantics::RiscOperators(protoval, solver) {
    name("Multi");
    (void) SValue::promote(protoval); // check that its dynamic type is a MultiSemantics::SValue
}

RiscOperators::RiscOperators(const BaseSemantics::State::Ptr &state, const SmtSolver::Ptr &solver)
    : BaseSemantics::RiscOperators(state, solver) {
    name("Multi");
    (void) SValue::promote(state->protoval());      // dynamic type must be a MultiSemantics::SValue
}

RiscOperators::~RiscOperators() {}

RiscOperators::Ptr
RiscOperators::instanceFromRegisters(const RegisterDictionary::Ptr&) {
    BaseSemantics::SValue::Ptr protoval = SValue::instance();
    return Ptr(new RiscOperators(protoval, SmtSolver::Ptr()));
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
RiscOperators::promote(const BaseSemantics::RiscOperators::Ptr &ops)
{
    RiscOperators::Ptr retval = boost::dynamic_pointer_cast<RiscOperators>(ops);
    ASSERT_not_null(retval);
    return retval;
}

size_t
RiscOperators::nsubdomains() const {
    return subdomains.size();
}

bool
RiscOperators::is_active(size_t idx) const {
    return idx<subdomains.size() && subdomains[idx]!=NULL && active[idx];
}

void
RiscOperators::clear_active(size_t idx) {
    set_active(idx, false);
}

size_t
RiscOperators::add_subdomain(const BaseSemantics::RiscOperators::Ptr &subdomain, const std::string &name, bool activate)
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

BaseSemantics::RiscOperators::Ptr
RiscOperators::get_subdomain(size_t idx) const
{
    if (idx>=subdomains.size())
        return BaseSemantics::RiscOperators::Ptr();
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

BaseSemantics::SValue::Ptr
RiscOperators::undefined_(size_t nbits)
{
    SValue::Ptr retval = svalue_empty(nbits);
    SUBDOMAINS(sd, ())
        retval->set_subvalue(sd.idx(), sd->undefined_(nbits));
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::bottom_(size_t nbits)
{
    SValue::Ptr retval = svalue_empty(nbits);
    SUBDOMAINS(sd, ())
        retval->set_subvalue(sd.idx(), sd->bottom_(nbits));
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::unspecified_(size_t nbits)
{
    SValue::Ptr retval = svalue_empty(nbits);
    SUBDOMAINS(sd, ())
        retval->set_subvalue(sd.idx(), sd->unspecified_(nbits));
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::number_(size_t nbits, uint64_t value)
{
    SValue::Ptr retval = svalue_empty(nbits);
    SUBDOMAINS(sd, ())
        retval->set_subvalue(sd.idx(), sd->number_(nbits, value));
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::boolean_(bool value)
{
    SValue::Ptr retval = svalue_empty(1);
    SUBDOMAINS(sd, ())
        retval->set_subvalue(sd.idx(), sd->boolean_(value));
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::filterCallTarget(const BaseSemantics::SValue::Ptr &a)
{
    SValue::Ptr retval = svalue_empty(a->nBits());
    SUBDOMAINS(sd, (a))
        retval->set_subvalue(sd.idx(), sd->filterCallTarget(sd(a)));
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::filterReturnTarget(const BaseSemantics::SValue::Ptr &a)
{
    SValue::Ptr retval = svalue_empty(a->nBits());
    SUBDOMAINS(sd, (a))
        retval->set_subvalue(sd.idx(), sd->filterReturnTarget(sd(a)));
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::filterIndirectJumpTarget(const BaseSemantics::SValue::Ptr &a)
{
    SValue::Ptr retval = svalue_empty(a->nBits());
    SUBDOMAINS(sd, (a))
        retval->set_subvalue(sd.idx(), sd->filterIndirectJumpTarget(sd(a)));
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::and_(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &b)
{
    SValue::Ptr retval = svalue_empty(a->nBits());
    SUBDOMAINS(sd, (a, b))
        retval->set_subvalue(sd.idx(), sd->and_(sd(a), sd(b)));
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::or_(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &b)
{
    SValue::Ptr retval = svalue_empty(a->nBits());
    SUBDOMAINS(sd, (a, b))
        retval->set_subvalue(sd.idx(), sd->or_(sd(a), sd(b)));
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::xor_(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &b)
{
    SValue::Ptr retval = svalue_empty(a->nBits());
    SUBDOMAINS(sd, (a, b))
        retval->set_subvalue(sd.idx(), sd->xor_(sd(a), sd(b)));
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::invert(const BaseSemantics::SValue::Ptr &a)
{
    SValue::Ptr retval = svalue_empty(a->nBits());
    SUBDOMAINS(sd, (a))
        retval->set_subvalue(sd.idx(), sd->invert(sd(a)));
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::extract(const BaseSemantics::SValue::Ptr &a, size_t begin_bit, size_t end_bit)
{
    ASSERT_require(end_bit <= a->nBits());
    ASSERT_require(end_bit > begin_bit);
    SValue::Ptr retval = svalue_empty(end_bit-begin_bit);
    SUBDOMAINS(sd, (a))
        retval->set_subvalue(sd.idx(), sd->extract(sd(a), begin_bit, end_bit));
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::concat(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &b)
{
    SValue::Ptr retval = svalue_empty(a->nBits() + b->nBits());
    SUBDOMAINS(sd, (a, b))
        retval->set_subvalue(sd.idx(), sd->concat(sd(a), sd(b)));
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::leastSignificantSetBit(const BaseSemantics::SValue::Ptr &a)
{
    SValue::Ptr retval = svalue_empty(a->nBits());
    SUBDOMAINS(sd, (a))
        retval->set_subvalue(sd.idx(), sd->leastSignificantSetBit(sd(a)));
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::mostSignificantSetBit(const BaseSemantics::SValue::Ptr &a)
{
    SValue::Ptr retval = svalue_empty(a->nBits());
    SUBDOMAINS(sd, (a))
        retval->set_subvalue(sd.idx(), sd->mostSignificantSetBit(sd(a)));
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::rotateLeft(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &nbits)
{
    SValue::Ptr retval = svalue_empty(a->nBits());
    SUBDOMAINS(sd, (a, nbits))
        retval->set_subvalue(sd.idx(), sd->rotateLeft(sd(a), sd(nbits)));
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::rotateRight(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &nbits)
{
    SValue::Ptr retval = svalue_empty(a->nBits());
    SUBDOMAINS(sd, (a, nbits))
        retval->set_subvalue(sd.idx(), sd->rotateRight(sd(a), sd(nbits)));
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::shiftLeft(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &nbits)
{
    SValue::Ptr retval = svalue_empty(a->nBits());
    SUBDOMAINS(sd, (a, nbits))
        retval->set_subvalue(sd.idx(), sd->shiftLeft(sd(a), sd(nbits)));
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::shiftRight(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &nbits)
{
    SValue::Ptr retval = svalue_empty(a->nBits());
    SUBDOMAINS(sd, (a, nbits))
        retval->set_subvalue(sd.idx(), sd->shiftRight(sd(a), sd(nbits)));
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::shiftRightArithmetic(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &nbits)
{
    SValue::Ptr retval = svalue_empty(a->nBits());
    SUBDOMAINS(sd, (a, nbits))
        retval->set_subvalue(sd.idx(), sd->shiftRightArithmetic(sd(a), sd(nbits)));
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::equalToZero(const BaseSemantics::SValue::Ptr &a)
{
    SValue::Ptr retval = svalue_empty(1);
    SUBDOMAINS(sd, (a))
        retval->set_subvalue(sd.idx(), sd->equalToZero(sd(a)));
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::iteWithStatus(const BaseSemantics::SValue::Ptr &cond,
                             const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &b,
                             IteStatus &status)
{
    SValue::Ptr retval = svalue_empty(a->nBits());
    unsigned mergedStatus = 0;
    SUBDOMAINS(sd, (cond, a, b)) {
        IteStatus s = IteStatus::NEITHER;
        retval->set_subvalue(sd.idx(), sd->iteWithStatus(sd(cond), sd(a), sd(b), s));
        switch (s) {
            case IteStatus::NEITHER:
                break;
            case IteStatus::A:
                mergedStatus |= 0x1;
                break;
            case IteStatus::B:
                mergedStatus |= 0x2;
                break;
            case IteStatus::BOTH:
                mergedStatus = 3;
                break;
        }
    }

    switch (mergedStatus) {
        case 0:
            status = IteStatus::NEITHER;
            break;
        case 1:
            status = IteStatus::A;
            break;
        case 2:
            status = IteStatus::B;
            break;
        case 3:
            status = IteStatus::BOTH;
            break;
    }
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::unsignedExtend(const BaseSemantics::SValue::Ptr &a, size_t new_width)
{
    SValue::Ptr retval = svalue_empty(new_width);
    SUBDOMAINS(sd, (a))
        retval->set_subvalue(sd.idx(), sd->unsignedExtend(sd(a), new_width));
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::signExtend(const BaseSemantics::SValue::Ptr &a, size_t new_width)
{
    SValue::Ptr retval = svalue_empty(new_width);
    SUBDOMAINS(sd, (a))
        retval->set_subvalue(sd.idx(), sd->signExtend(sd(a), new_width));
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::add(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &b)
{
    SValue::Ptr retval = svalue_empty(a->nBits());
    SUBDOMAINS(sd, (a, b))
        retval->set_subvalue(sd.idx(), sd->add(sd(a), sd(b)));
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::addWithCarries(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &b,
                              const BaseSemantics::SValue::Ptr &c, BaseSemantics::SValue::Ptr &carry_out_/*output*/)
{
    SValue::Ptr retval = svalue_empty(a->nBits());
    SValue::Ptr carry_out = svalue_empty(a->nBits());
    SUBDOMAINS(sd, (a, b, c)) {
        BaseSemantics::SValue::Ptr co;
        retval->set_subvalue(sd.idx(), sd->addWithCarries(sd(a), sd(b), sd(c), co/*out*/));
        carry_out->set_subvalue(sd.idx(), co);
    }
    carry_out_ = carry_out;
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::negate(const BaseSemantics::SValue::Ptr &a)
{
    SValue::Ptr retval = svalue_empty(a->nBits());
    SUBDOMAINS(sd, (a))
        retval->set_subvalue(sd.idx(), sd->negate(sd(a)));
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::signedDivide(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &b)
{
    SValue::Ptr retval = svalue_empty(a->nBits());
    SUBDOMAINS(sd, (a, b))
        retval->set_subvalue(sd.idx(), sd->signedDivide(sd(a), sd(b)));
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::signedModulo(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &b)
{
    SValue::Ptr retval = svalue_empty(b->nBits());
    SUBDOMAINS(sd, (a, b))
        retval->set_subvalue(sd.idx(), sd->signedModulo(sd(a), sd(b)));
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::signedMultiply(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &b)
{
    SValue::Ptr retval = svalue_empty(a->nBits() + b->nBits());
    SUBDOMAINS(sd, (a, b))
        retval->set_subvalue(sd.idx(), sd->signedMultiply(sd(a), sd(b)));
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::unsignedDivide(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &b)
{
    SValue::Ptr retval = svalue_empty(a->nBits());
    SUBDOMAINS(sd, (a, b))
        retval->set_subvalue(sd.idx(), sd->unsignedDivide(sd(a), sd(b)));
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::unsignedModulo(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &b)
{
    SValue::Ptr retval = svalue_empty(b->nBits());
    SUBDOMAINS(sd, (a, b))
        retval->set_subvalue(sd.idx(), sd->unsignedModulo(sd(a), sd(b)));
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::unsignedMultiply(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &b)
{
    SValue::Ptr retval = svalue_empty(a->nBits() + b->nBits());
    SUBDOMAINS(sd, (a, b))
        retval->set_subvalue(sd.idx(), sd->unsignedMultiply(sd(a), sd(b)));
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::fpFromInteger(const BaseSemantics::SValue::Ptr &a, SgAsmFloatType *at) {
    SValue::Ptr retval = svalue_empty(at->get_nBits());
    SUBDOMAINS(sd, (a))
        retval->set_subvalue(sd.idx(), sd->fpFromInteger(sd(a), at));
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::fpToInteger(const BaseSemantics::SValue::Ptr &a, SgAsmFloatType *at, const BaseSemantics::SValue::Ptr &b) {
    SValue::Ptr retval = svalue_empty(b->nBits());
    SUBDOMAINS(sd, (a, b))
        retval->set_subvalue(sd.idx(), sd->fpToInteger(sd(a), at, sd(b)));
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::fpConvert(const BaseSemantics::SValue::Ptr &a, SgAsmFloatType *at, SgAsmFloatType *bt) {
    SValue::Ptr retval = svalue_empty(bt->get_nBits());
    SUBDOMAINS(sd, (a))
        retval->set_subvalue(sd.idx(), sd->fpConvert(sd(a), at, bt));
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::fpIsNan(const BaseSemantics::SValue::Ptr &a, SgAsmFloatType *at) {
    SValue::Ptr retval = svalue_empty(1);
    SUBDOMAINS(sd, (a))
        retval->set_subvalue(sd.idx(), sd->fpIsNan(sd(a), at));
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::fpIsDenormalized(const BaseSemantics::SValue::Ptr &a, SgAsmFloatType *at) {
    SValue::Ptr retval = svalue_empty(1);
    SUBDOMAINS(sd, (a))
        retval->set_subvalue(sd.idx(), sd->fpIsDenormalized(sd(a), at));
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::fpIsZero(const BaseSemantics::SValue::Ptr &a, SgAsmFloatType *at) {
    SValue::Ptr retval = svalue_empty(1);
    SUBDOMAINS(sd, (a))
        retval->set_subvalue(sd.idx(), sd->fpIsZero(sd(a), at));
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::fpIsInfinity(const BaseSemantics::SValue::Ptr &a, SgAsmFloatType *at) {
    SValue::Ptr retval = svalue_empty(1);
    SUBDOMAINS(sd, (a))
        retval->set_subvalue(sd.idx(), sd->fpIsInfinity(sd(a), at));
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::fpSign(const BaseSemantics::SValue::Ptr &a, SgAsmFloatType *at) {
    SValue::Ptr retval = svalue_empty(1);
    SUBDOMAINS(sd, (a))
        retval->set_subvalue(sd.idx(), sd->fpSign(sd(a), at));
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::fpEffectiveExponent(const BaseSemantics::SValue::Ptr &a, SgAsmFloatType *at) {
    SValue::Ptr retval = svalue_empty(at->exponentBits().size());
    SUBDOMAINS(sd, (a))
        retval->set_subvalue(sd.idx(), sd->fpEffectiveExponent(sd(a), at));
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::fpAdd(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &b, SgAsmFloatType *abt) {
    SValue::Ptr retval = svalue_empty(abt->get_nBits());
    SUBDOMAINS(sd, (a, b))
        retval->set_subvalue(sd.idx(), sd->fpAdd(sd(a), sd(b), abt));
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::fpSubtract(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &b, SgAsmFloatType *abt) {
    SValue::Ptr retval = svalue_empty(abt->get_nBits());
    SUBDOMAINS(sd, (a, b))
        retval->set_subvalue(sd.idx(), sd->fpSubtract(sd(a), sd(b), abt));
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::fpMultiply(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &b, SgAsmFloatType *abt) {
    SValue::Ptr retval = svalue_empty(abt->get_nBits());
    SUBDOMAINS(sd, (a, b))
        retval->set_subvalue(sd.idx(), sd->fpMultiply(sd(a), sd(b), abt));
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::fpDivide(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &b, SgAsmFloatType *abt) {
    SValue::Ptr retval = svalue_empty(abt->get_nBits());
    SUBDOMAINS(sd, (a, b))
        retval->set_subvalue(sd.idx(), sd->fpDivide(sd(a), sd(b), abt));
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::fpSquareRoot(const BaseSemantics::SValue::Ptr &a, SgAsmFloatType *at) {
    SValue::Ptr retval = svalue_empty(at->get_nBits());
    SUBDOMAINS(sd, (a))
        retval->set_subvalue(sd.idx(), sd->fpSquareRoot(sd(a), at));
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::fpRoundTowardZero(const BaseSemantics::SValue::Ptr &a, SgAsmFloatType *at) {
    SValue::Ptr retval = svalue_empty(at->get_nBits());
    SUBDOMAINS(sd, (a))
        retval->set_subvalue(sd.idx(), sd->fpRoundTowardZero(sd(a), at));
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::reinterpret(const BaseSemantics::SValue::Ptr &a, SgAsmType *type) {
    SValue::Ptr retval = svalue_empty(type->get_nBits());
    SUBDOMAINS(sd, (a))
        retval->set_subvalue(sd.idx(), sd->reinterpret(sd(a), type));
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::readRegister(RegisterDescriptor reg, const BaseSemantics::SValue::Ptr &dflt)
{
    SValue::Ptr retval = svalue_empty(reg.nBits());
    SUBDOMAINS(sd, ())
        retval->set_subvalue(sd.idx(), sd->readRegister(reg, sd(dflt)));
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::peekRegister(RegisterDescriptor reg, const BaseSemantics::SValue::Ptr &dflt)
{
    SValue::Ptr retval = svalue_empty(reg.nBits());
    SUBDOMAINS(sd, ())
        retval->set_subvalue(sd.idx(), sd->peekRegister(reg, sd(dflt)));
    return retval;
}

void
RiscOperators::writeRegister(RegisterDescriptor reg, const BaseSemantics::SValue::Ptr &a)
{
    SUBDOMAINS(sd, (a))
        sd->writeRegister(reg, sd(a));
}

BaseSemantics::SValue::Ptr
RiscOperators::readMemory(RegisterDescriptor segreg, const BaseSemantics::SValue::Ptr &addr,
                          const BaseSemantics::SValue::Ptr &dflt, const BaseSemantics::SValue::Ptr &cond)
{
    SValue::Ptr retval = svalue_empty(dflt->nBits());
    SUBDOMAINS(sd, (addr, cond))
        retval->set_subvalue(sd.idx(), sd->readMemory(segreg, sd(addr), sd(dflt), sd(cond)));
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::peekMemory(RegisterDescriptor segreg, const BaseSemantics::SValue::Ptr &addr,
                          const BaseSemantics::SValue::Ptr &dflt)
{
    SValue::Ptr retval = svalue_empty(dflt->nBits());
    SUBDOMAINS(sd, (addr))
        retval->set_subvalue(sd.idx(), sd->peekMemory(segreg, sd(addr), sd(dflt)));
    return retval;
}

void
RiscOperators::writeMemory(RegisterDescriptor segreg, const BaseSemantics::SValue::Ptr &addr,
                           const BaseSemantics::SValue::Ptr &data, const BaseSemantics::SValue::Ptr &cond)
{
    SUBDOMAINS(sd, (addr, data, cond))
        sd->writeMemory(segreg, sd(addr), sd(data), cond);
}

} // namespace
} // namespace
} // namespace
} // namespace

#endif
