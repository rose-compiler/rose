#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/InstructionSemantics/NullSemantics.h>

#include <Rose/As.h>
#include <Rose/BinaryAnalysis/RegisterDictionary.h>

namespace Rose {
namespace BinaryAnalysis {                              // documented elsewhere
namespace InstructionSemantics {                        // documented elsewhere
namespace NullSemantics {                               // documented in the header

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SValue
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SValue::~SValue() {}

SValue::SValue(const size_t nbits)
    : BaseSemantics::SValue(nbits) {}

SValue::Ptr
SValue::instance() {
    return Ptr(new SValue(1));
}

SValue::Ptr
SValue::instance(const size_t nbits) {
    return Ptr(new SValue(nbits));
}

SValue::Ptr
SValue::instance(const size_t nbits, uint64_t /*number*/) {
    return Ptr(new SValue(nbits));            // the number is not important in this domain
}

SValuePtr
SValue::instance(const SValue::Ptr &other) {
    return Ptr(new SValue(*other));
}

BaseSemantics::SValue::Ptr
SValue::bottom_(const size_t nBits) const {
    return instance(nBits);
}

BaseSemantics::SValue::Ptr
SValue::undefined_(const size_t nBits) const {
    return instance(nBits);
}

BaseSemantics::SValue::Ptr
SValue::unspecified_(const size_t nBits) const {
    return instance(nBits);
}

BaseSemantics::SValue::Ptr
SValue::number_(const size_t nBits, const uint64_t number) const {
    return instance(nBits, number);
}

BaseSemantics::SValue::Ptr
SValue::copy(const size_t new_width) const {
    Ptr retval(new SValue(*this));
    if (new_width != 0 && new_width != retval->nBits())
        retval->set_width(new_width);
    return retval;
}

Sawyer::Optional<BaseSemantics::SValue::Ptr>
SValue::createOptionalMerge(const BaseSemantics::SValue::Ptr&, const BaseSemantics::Merger::Ptr&, const SmtSolver::Ptr&) const {
    return Sawyer::Nothing();
}

SValue::Ptr
SValue::promote(const BaseSemantics::SValue::Ptr &v) {
    Ptr retval = as<SValue>(v);
    ASSERT_not_null(retval);
    return retval;
}

bool
SValue::isBottom() const {
    return false;
}

void
SValue::print(std::ostream &stream, BaseSemantics::Formatter&) const {
    stream <<"VOID[" <<nBits() <<"]";
}

void
SValue::hash(Combinatorics::Hasher &hasher) const {
    hasher.insert(0);                               // hash depends on number of SValues hashed, but not any content
}

bool
SValue::is_number() const {
    return false;
}

uint64_t
SValue::get_number() const {
    ASSERT_not_reachable("not a number");
    uint64_t retval;
    return retval;
}

bool
SValue::may_equal(const BaseSemantics::SValue::Ptr&, const SmtSolver::Ptr&) const {
    return true;
}

bool
SValue::must_equal(const BaseSemantics::SValue::Ptr &other, const SmtSolver::Ptr&) const {
    return this == getRawPointer(other); // must be equal if they're both the same object
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// RegisterState
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RegisterState::~RegisterState() {}

RegisterState::RegisterState(const RegisterState &other)
    : BaseSemantics::RegisterState(other) {}

RegisterState::RegisterState(const BaseSemantics::SValue::Ptr &protoval, const RegisterDictionary::Ptr &regdict)
    : BaseSemantics::RegisterState(protoval, regdict) {}

RegisterState::Ptr
RegisterState::instance(const BaseSemantics::SValue::Ptr &protoval, const RegisterDictionary::Ptr &regdict) {
    return Ptr(new RegisterState(protoval, regdict));
}

BaseSemantics::RegisterState::Ptr
RegisterState::create(const BaseSemantics::SValue::Ptr &protoval, const RegisterDictionary::Ptr &regdict) const {
    return instance(protoval, regdict);
}

BaseSemantics::AddressSpace::Ptr
RegisterState::clone() const {
    return Ptr(new RegisterState(*this));
}

RegisterState::Ptr
RegisterState::promote(const BaseSemantics::AddressSpace::Ptr &from) {
    Ptr retval = as<RegisterState>(from);
    ASSERT_not_null(retval);
    return retval;
}

bool
RegisterState::merge(const BaseSemantics::AddressSpace::Ptr&, BaseSemantics::RiscOperators*, BaseSemantics::RiscOperators*) {
    return false;
}

void
RegisterState::clear() {}

void
RegisterState::zero() {}

BaseSemantics::SValue::Ptr
RegisterState::readRegister(const RegisterDescriptor reg, const BaseSemantics::SValue::Ptr &/*dflt*/,
                            BaseSemantics::RiscOperators*) {
    return protoval()->undefined_(reg.nBits());
}

BaseSemantics::SValue::Ptr
RegisterState::peekRegister(const RegisterDescriptor reg, const BaseSemantics::SValue::Ptr &/*dflt*/,
                            BaseSemantics::RiscOperators*) {
    return protoval()->undefined_(reg.nBits());
}

void
RegisterState::writeRegister(const RegisterDescriptor, const BaseSemantics::SValue::Ptr&, BaseSemantics::RiscOperators*) {}

void
RegisterState::updateReadProperties(const RegisterDescriptor) {}

void
RegisterState::updateWriteProperties(const RegisterDescriptor, const BaseSemantics::InputOutputProperty) {}

void
RegisterState::hash(Combinatorics::Hasher&, BaseSemantics::RiscOperators*, BaseSemantics::RiscOperators*) const {}

void
RegisterState::print(std::ostream&, BaseSemantics::Formatter&) const {}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MemoryState
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

MemoryState::~MemoryState() {}

MemoryState::MemoryState(const BaseSemantics::SValue::Ptr &addrProtoval, const BaseSemantics::SValue::Ptr &valProtoval)
    : BaseSemantics::MemoryState(addrProtoval, valProtoval) {}

MemoryState::MemoryState(const MemoryState::Ptr &other)
    : BaseSemantics::MemoryState(other) {}

MemoryState::Ptr
MemoryState::instance(const BaseSemantics::SValue::Ptr &addrProtoval, const BaseSemantics::SValue::Ptr &valProtoval) {
    return Ptr(new MemoryState(addrProtoval, valProtoval));
}

BaseSemantics::MemoryState::Ptr
MemoryState::create(const BaseSemantics::SValuePtr &addrProtoval, const BaseSemantics::SValuePtr &valProtoval) const {
    return instance(addrProtoval, valProtoval);
}

BaseSemantics::AddressSpace::Ptr
MemoryState::clone() const {
    return Ptr(new MemoryState(*this));
}

MemoryState::Ptr
MemoryState::promote(const BaseSemantics::AddressSpace::Ptr &x) {
    Ptr retval = as<MemoryState>(x);
    ASSERT_not_null(x);
    return retval;
}

BaseSemantics::SValue::Ptr
MemoryState::readMemory(const BaseSemantics::SValue::Ptr &/*address*/, const BaseSemantics::SValue::Ptr &dflt,
                        BaseSemantics::RiscOperators */*addrOps*/, BaseSemantics::RiscOperators */*valOps*/) {
    return dflt->copy();
}

void
MemoryState::writeMemory(const BaseSemantics::SValue::Ptr &/*addr*/, const BaseSemantics::SValue::Ptr &/*value*/,
                         BaseSemantics::RiscOperators */*addrOps*/, BaseSemantics::RiscOperators */*valOps*/) {}

BaseSemantics::SValue::Ptr
MemoryState::peekMemory(const BaseSemantics::SValue::Ptr &/*address*/, const BaseSemantics::SValue::Ptr &dflt,
                        BaseSemantics::RiscOperators */*addrOps*/, BaseSemantics::RiscOperators */*valOps*/) {
    return dflt->copy();
}

void
MemoryState::hash(Combinatorics::Hasher&, BaseSemantics::RiscOperators */*addrOps*/,
                  BaseSemantics::RiscOperators */*valOps*/) const {}

void
MemoryState::print(std::ostream&, BaseSemantics::Formatter&) const {}

bool
MemoryState::merge(const BaseSemantics::AddressSpace::Ptr &/*other*/, BaseSemantics::RiscOperators */*addrOps*/,
                   BaseSemantics::RiscOperators */*valOps*/) {
    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// RiscOperators
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
