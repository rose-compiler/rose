#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/InstructionSemantics/SourceAstSemantics.h>

#include <Rose/BinaryAnalysis/Disassembler/Base.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/TraceSemantics.h>
#include <Rose/BinaryAnalysis/RegisterDictionary.h>
#include <integerOps.h>

namespace P2 = Rose::BinaryAnalysis::Partitioner2;

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics {
namespace SourceAstSemantics {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      SValue
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

size_t SValue::nVariables_ = 0;

SValue::SValue(size_t nbits): BaseSemantics::SValue(nbits) {
    ctext_ = "V_" + StringUtility::numberToString(nVariables_++);
}

// class method
std::string
SValue::unsignedTypeNameForSize(size_t nbits) {
    if (nbits <= 8)
        return "uint8_t";
    if (nbits <= 16)
        return "uint16_t";
    if (nbits <= 32)
        return "uint32_t";
    if (nbits <= 64)
        return "uint64_t";

    // The following types might not exist, but they're needed because some architectures (Intel MIC, Xeon Phi) have registers
    // that are this wide.
    if (nbits <= 128)
        return "uint128_t";
    if (nbits <= 256)
        return "uint256_t";
    if (nbits <= 512)
        return "uint512_t";
    TODO("[Robb P. Matzke 2015-09-29]: cannot represent types larger than 512 bits");
}

// class method
std::string
SValue::signedTypeNameForSize(size_t nbits) {
    std::string ut = unsignedTypeNameForSize(nbits);
    ASSERT_require(!ut.empty() && ut[0]=='u');
    return ut.substr(1);
}

SValue::SValue(size_t nbits, uint64_t number): BaseSemantics::SValue(nbits) {
    if (nbits < 8*sizeof(int)) {
        ctext_ = "(" + unsignedTypeNameForSize(nbits) + ")" + StringUtility::intToHex(number);
    } else if (nbits == 8*sizeof(int)) {
        ctext_ = StringUtility::intToHex(number);
    } else if (nbits <= 8*sizeof(long)) {
        ctext_ = StringUtility::intToHex(number) + "ul";
    } else if (nbits <= 8*sizeof(long long)) {
        ctext_ = StringUtility::intToHex(number) + "ull";
    } else {
        TODO("[Robb P. Matzke 2015-09-30]: need to figure out how to build integers up to 512 bits wide");
    }
}

SValue::SValue(const SValue &other): BaseSemantics::SValue(other) {
    ctext_ = other.ctext_;
}

SValue::Ptr
SValue::instance() {
    return SValue::Ptr(new SValue(1));
}

SValue::Ptr
SValue::instance_undefined(size_t nbits) {
    return SValue::Ptr(new SValue(nbits));
}

SValue::Ptr
SValue::instance_integer(size_t nbits, uint64_t value) {
    return SValue::Ptr(new SValue(nbits, value));
}

BaseSemantics::SValue::Ptr
SValue::bottom_(size_t nbits) const {
    return instance_undefined(nbits);
}

BaseSemantics::SValue::Ptr
SValue::undefined_(size_t nbits) const {
    return instance_undefined(nbits);
}

BaseSemantics::SValue::Ptr
SValue::unspecified_(size_t nbits) const {
    return instance_undefined(nbits);
}

BaseSemantics::SValue::Ptr
SValue::number_(size_t nbits, uint64_t value) const {
    return instance_integer(nbits, value);
}

BaseSemantics::SValue::Ptr
SValue::boolean_(bool value) const {
    return instance_integer(1, value ? 1 : 0);
}

BaseSemantics::SValue::Ptr
SValue::copy(size_t new_width) const {
    SValue::Ptr retval(new SValue(*this));
    if (new_width!=0 && new_width!=retval->nBits())
        retval->set_width(new_width);
    return retval;
}

Sawyer::Optional<BaseSemantics::SValue::Ptr>
SValue::createOptionalMerge(const BaseSemantics::SValue::Ptr&, const BaseSemantics::Merger::Ptr&, const SmtSolver::Ptr&) const {
    throw BaseSemantics::NotImplemented("SourceAstSemantics is not suitable for dataflow analysis", NULL);
}

SValue::Ptr
SValue::promote(const BaseSemantics::SValue::Ptr &v) { // hot
    SValue::Ptr retval = v.dynamicCast<SValue>();
    ASSERT_not_null(retval);
    return retval;
}

bool
SValue::isBottom() const {
    return false;
}

bool
SValue::may_equal(const BaseSemantics::SValue::Ptr &/*other*/, const SmtSolver::Ptr&) const {
    ASSERT_not_reachable("no implementation necessary");
}

bool
SValue::must_equal(const BaseSemantics::SValue::Ptr &/*other*/, const SmtSolver::Ptr&) const {
    ASSERT_not_reachable("no implementation necessary");
}

void
SValue::set_width(size_t /*nbits*/) {
    ASSERT_not_reachable("no implementation necessary");
}

bool
SValue::is_number() const {
    return false;
}

uint64_t
SValue::get_number() const {
    ASSERT_not_reachable("no implementation necessary");
}

void
SValue::hash(Combinatorics::Hasher &hasher) const {
    hasher.insert(nBits());
    hasher.insert(nVariables_);
    hasher.insert(ctext_);
}

void
SValue::print(std::ostream &out, BaseSemantics::Formatter&) const {
    out <<ctext_;
}

const std::string&
SValue::ctext() const {
    return ctext_;
}

void
SValue::ctext(const std::string &s) {
    ctext_ = s;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      RiscOperators::SideEffect
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RiscOperators::SideEffect::~SideEffect() {}

RiscOperators::SideEffect::SideEffect() {}

RiscOperators::SideEffect::SideEffect(const BaseSemantics::SValue::Ptr &location, const BaseSemantics::SValue::Ptr &temporary,
                                      const BaseSemantics::SValue::Ptr &expression)
    : location(location), temporary(temporary), expression(expression) {}

bool
RiscOperators::SideEffect::isValid() const {
    return expression != NULL;
}

bool
RiscOperators::SideEffect::isSubstitution() const {
    return isValid() && location==NULL && temporary!=NULL;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Supporting functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RiscOperators::RiscOperators(const BaseSemantics::SValue::Ptr &protoval, const SmtSolver::Ptr &solver)
    : BaseSemantics::RiscOperators(protoval, solver), executionHalted_(false) {
    name("SourceAstSemantics");
    (void) SValue::promote(protoval); // make sure its dynamic type is a SourceAstSemantics::SValue
}

RiscOperators::RiscOperators(const BaseSemantics::State::Ptr &state, const SmtSolver::Ptr &solver)
    : BaseSemantics::RiscOperators(state, solver), executionHalted_(false) {
    name("SourceAstSemantics");
    (void) SValue::promote(state->protoval());      // values must have SourceAstSemantics::SValue dynamic type
}

RiscOperators::Ptr
RiscOperators::instanceFromRegisters(const RegisterDictionary::Ptr &regdict, const SmtSolver::Ptr &solver) {
    BaseSemantics::SValue::Ptr protoval = SValue::instance();
    RegisterState::Ptr registers = RegisterState::instance(protoval, regdict);
    BaseSemantics::MemoryState::Ptr memory = MemoryState::instance(protoval, protoval);
    BaseSemantics::State::Ptr state = State::instance(registers, memory);
    Ptr ops = Ptr(new RiscOperators(state, solver));
    ops->resetState();
    return ops;
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
    Ptr retval = boost::dynamic_pointer_cast<RiscOperators>(x);
    ASSERT_not_null(retval);
    return retval;
}

void
RiscOperators::resetState() {
    // Initialize registers so they correspond to the C global variables we'll generate, and then lock the register state so
    // those registers don't change if we access subparts (like if we store EAX and write to AX).
    currentState()->clear();
    RegisterState::Ptr registers = RegisterState::promote(currentState()->registerState());
    registers->initialize_large();
    registers->accessModifiesExistingLocations(false);
    RegisterState::RegPairs regpairs = registers->get_stored_registers();
    for (RegisterState::RegPair &regpair: regpairs) {
        std::string varName = registerVariableName(regpair.desc);
        BaseSemantics::SValue::Ptr value = makeSValue(regpair.desc.nBits(), NULL, varName);
        registers->writeRegister(regpair.desc, value, this);
    }
    registers->eraseWriters();
    registers->eraseProperties();
    executionHalted_ = false;
}

BaseSemantics::SValue::Ptr
RiscOperators::makeSValue(size_t nbits, SgNode */*ast*/, const std::string &ctext) {
    ASSERT_require(nbits > 0);
    SValue::Ptr v = SValue::promote(protoval()->undefined_(nbits));
    v->ctext(ctext);
    return v;
}

// Append a side effect to the list of side effects.
BaseSemantics::SValue::Ptr
RiscOperators::saveSideEffect(const BaseSemantics::SValue::Ptr &expression, const BaseSemantics::SValue::Ptr &location) {
    if (executionHalted_)
        return expression;
    BaseSemantics::SValue::Ptr retval;
    if (location)
        retval = undefined_(expression->nBits());
    sideEffects_.push_back(SideEffect(location, retval, expression));
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::substitute(const BaseSemantics::SValue::Ptr &expression) {
    BaseSemantics::SValue::Ptr retval = undefined_(expression->nBits());
    sideEffects_.push_back(SideEffect(BaseSemantics::SValue::Ptr(), retval, expression));
    return retval;
}

const RiscOperators::SideEffects&
RiscOperators::sideEffects() const {
    return sideEffects_;
}

// C global variable name for a register.
std::string
RiscOperators::registerVariableName(RegisterDescriptor reg) {
    using namespace StringUtility;
    RegisterDictionary::Ptr registers = currentState()->registerState()->registerDictionary();
    std::string name = registers->lookup(reg);
    if (name.empty()) {
        return ("R_" + numberToString(reg.majorNumber()) +
                "_" + numberToString(reg.minorNumber()) +
                "_" + numberToString(reg.offset()) +
                "_" + numberToString(reg.nBits()));
    }
    return "R_" + name;
}

void
RiscOperators::reset() {
    sideEffects_.clear();
    executionHalted_ = false;
    resetState();
}

// Create a mask consisting of nset shifted upward by sa.
BaseSemantics::SValue::Ptr
RiscOperators::makeMask(size_t nBits, size_t nSet, size_t sa) {
    if (sa >= nBits)
        return number_(nBits, 0);
    nSet = std::min(nSet, nBits-sa);
    if (0 == nSet)
        return number_(nBits, 0);

    ASSERT_require(nBits <= 64);
    uint64_t i = IntegerOps::genMask<uint64_t>(sa, sa+nSet-1);
    return number_(nBits, i);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      RiscOperators operations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BaseSemantics::SValue::Ptr
RiscOperators::unspecified_(size_t nbits) {
    ASSERT_require(nbits <= 32);
    uint32_t mask = IntegerOps::genMask<uint32_t>(nbits);
    return makeSValue(nbits, NULL, "(unspecified() & " + StringUtility::intToHex(mask) + ")");
}

void
RiscOperators::hlt() {
    saveSideEffect(makeSValue(1, NULL, "hlt()"));
    haltExecution();
}

void
RiscOperators::cpuid() {
    saveSideEffect(makeSValue(1, NULL, "cpuid()"));
}

BaseSemantics::SValue::Ptr
RiscOperators::rdtsc() {
    return makeSValue(64, NULL, "rdtsc()");
}

BaseSemantics::SValue::Ptr
RiscOperators::and_(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &b) {
    std::string ctext = "(" + SValue::promote(a)->ctext() + " & " + SValue::promote(b)->ctext() + ")";
    return makeSValue(a->nBits(), NULL, ctext);
}

BaseSemantics::SValue::Ptr
RiscOperators::or_(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &b) {
    std::string ctext = "(" + SValue::promote(a)->ctext() + " | " + SValue::promote(b)->ctext() + ")";
    return makeSValue(a->nBits(), NULL, ctext);
}

BaseSemantics::SValue::Ptr
RiscOperators::xor_(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &b) {
    std::string ctext = "(" + SValue::promote(a)->ctext() + " ^ " + SValue::promote(b)->ctext() + ")";
    return makeSValue(a->nBits(), NULL, ctext);
}

BaseSemantics::SValue::Ptr
RiscOperators::invert(const BaseSemantics::SValue::Ptr &a) {
    SValue::Ptr mask = SValue::promote(makeMask(a->nBits(), a->nBits()));
    std::string ctext = "(~" + SValue::promote(a)->ctext() + " & " + mask->ctext() + ")";
    return makeSValue(a->nBits(), NULL, ctext);
}

BaseSemantics::SValue::Ptr
RiscOperators::extract(const BaseSemantics::SValue::Ptr &a, size_t begin_bit, size_t end_bit) {
    ASSERT_require(end_bit <= a->nBits());
    ASSERT_require(begin_bit < end_bit);
    if (0 == begin_bit && end_bit == a->nBits()) {
        return a->copy();
    } else {
        size_t newSize = end_bit - begin_bit;
        std::string ctext = "((" + SValue::promote(a)->ctext() + " >> " +
                            SValue::promote(number_(a->nBits(), begin_bit))->ctext() + ") & " +
                            SValue::promote(makeMask(a->nBits(), newSize))->ctext() + ")";
        return unsignedExtend(makeSValue(a->nBits(), NULL, ctext), newSize);
    }
}

BaseSemantics::SValue::Ptr
RiscOperators::concat(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &b) {
    size_t resultWidth = a->nBits() + b->nBits();
    std::string resultType = SValue::unsignedTypeNameForSize(resultWidth);

    // ctext = ((resultType)a | ((resultType)b << (resultType)aWidth))
    SValue::Ptr aWidth = SValue::promote(number_(resultWidth, a->nBits()));
    std::string ctext = "((" + resultType + ")" + SValue::promote(a)->ctext() + " | "
                        "((" + resultType + ")" + SValue::promote(b)->ctext() + " << " +
                        "(" + resultType + ")" + aWidth->ctext() + "))";

    return makeSValue(a->nBits() + b->nBits(), NULL, ctext);
}

BaseSemantics::SValue::Ptr
RiscOperators::leastSignificantSetBit(const BaseSemantics::SValue::Ptr &a) {
    std::string ctext = "lssb" + StringUtility::numberToString(a->nBits()) + "(" + SValue::promote(a)->ctext() + ")";
    return makeSValue(a->nBits(), NULL, ctext);
}

BaseSemantics::SValue::Ptr
RiscOperators::mostSignificantSetBit(const BaseSemantics::SValue::Ptr &a) {
    std::string ctext = "mssb" + StringUtility::numberToString(a->nBits()) + "(" + SValue::promote(a)->ctext() + ")";
    return makeSValue(a->nBits(), NULL, ctext);
}

BaseSemantics::SValue::Ptr
RiscOperators::rotateLeft(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &sa) {
    // Note: '<<' and '>>' are not well defined in C when the shift amount is >= the size of the left operand.
    //   ctext = ((((a) << (             (sa) % (8*sizeof(a)))  |
    //             ((a) >> (8*sizeof(a)-((sa) % (8*sizeof(a)))))) & mask)
    std::string normalizedAmount = "((" + SValue::promote(sa)->ctext() + ") % " +
                                   StringUtility::numberToString(a->nBits()) + ")";
    std::string complementAmount = "(" + StringUtility::numberToString(a->nBits()) + " - " + normalizedAmount;
    std::string mask = SValue::promote(makeMask(a->nBits(), a->nBits()))->ctext();
    std::string ctext = "((((" + SValue::promote(a)->ctext() + ") << " + normalizedAmount + ") |"
                        "  ((" + SValue::promote(a)->ctext() + ") >> " + complementAmount + "))"
                        " & " + mask + ")";
    return makeSValue(a->nBits(), NULL, ctext);
}

BaseSemantics::SValue::Ptr
RiscOperators::rotateRight(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &sa) {
    // Note: '<<' and '>>' are not well defined in C when the shift amount is >= the size of the left operand.
    //   ((((a) >> (             (sa) % (8*sizeof(a)))  |
    //     ((a) << (8*sizeof(a)-((sa) % (8*sizeof(a)))))) & mask)
    std::string normalizedAmount = "((" + SValue::promote(sa)->ctext() + ") % " +
                                   StringUtility::numberToString(a->nBits()) + ")";
    std::string complementAmount = "(" + StringUtility::numberToString(a->nBits()) + " - " + normalizedAmount;
    std::string mask = SValue::promote(makeMask(a->nBits(), a->nBits()))->ctext();
    std::string ctext = "((((" + SValue::promote(a)->ctext() + ") >> " + normalizedAmount + ") |"
                        "  ((" + SValue::promote(a)->ctext() + ") << " + complementAmount + "))"
                        " & " + mask + ")";
    return makeSValue(a->nBits(), NULL, ctext);
}

BaseSemantics::SValue::Ptr
RiscOperators::shiftLeft(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &sa) {
    // '<<' is not well defined in C when the shift amount is >= the size of the left operand.
    //   (sa >= 8*sizeof(a) ? (aType)0 : ((a << sa) & mask))
    std::string mask = SValue::promote(makeMask(a->nBits(), a->nBits()))->ctext();
    std::string ctext = "(" + SValue::promote(sa)->ctext() + " >= " + StringUtility::numberToString(a->nBits()) + " ? " +
                        SValue::promote(number_(a->nBits(), 0))->ctext() + " : " +
                        "((" + SValue::promote(a)->ctext() + " << " + SValue::promote(sa)->ctext() + ")"
                        " & " + mask + "))";
    return makeSValue(a->nBits(), NULL, ctext);
}

BaseSemantics::SValue::Ptr
RiscOperators::shiftRight(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &sa) {
    // '>>' is not well defined in C when the shift amount is >= the size of the left operand.
    //   (sa >= 8*sizeof(a) ? (aType)0 : a >> sa)
    std::string ctext = "(" + SValue::promote(sa)->ctext() + " >= " + StringUtility::numberToString(a->nBits()) + " ? " +
                        SValue::promote(number_(a->nBits(), 0))->ctext() + " : " +
                        SValue::promote(a)->ctext() + " >> " + SValue::promote(sa)->ctext() + ")";
    return makeSValue(a->nBits(), NULL, ctext);
}

BaseSemantics::SValue::Ptr
RiscOperators::shiftRightArithmetic(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &sa) {
    // Do the right shift without any casting to signed types.
    //    signBits = (signBit ? ~(aType)0 : (aType)0)
    //    signExtension = (sa >= 8*sizeof(a) ? signBits : signBits << (8*sizeof(a) - sa))
    //    ctext = (sa >= 8*sizeof(a) ? signBits : (a >> sa) | signExtension)
    std::string signBit = SValue::promote(extract(a, a->nBits()-1, a->nBits()))->ctext();
    std::string signBits = "(" + signBit + " ? "
                           "~" + SValue::promote(number_(a->nBits(), 0))->ctext() + " : " +
                           SValue::promote(number_(a->nBits(), 0))->ctext() + ")";
    std::string width = SValue::promote(number_(a->nBits(), a->nBits()))->ctext();
    std::string signExtension = "(" + SValue::promote(sa)->ctext() + " >= " + width + " ? " +
                                signBits + " : " + signBits + " << (" + width + " - " + SValue::promote(sa)->ctext() + "))";
    std::string ctext = "(" + SValue::promote(sa)->ctext() + " >= " + width + " ? " +
                        signBits + " : "
                        "(" + SValue::promote(a)->ctext() + " >> " + SValue::promote(sa)->ctext() + ") | " + signExtension +
                        ")";
    return makeSValue(a->nBits(), NULL, ctext);
}

BaseSemantics::SValue::Ptr
RiscOperators::equalToZero(const BaseSemantics::SValue::Ptr &a) {
    std::string ctext =  "(" + SValue::promote(a)->ctext() + " == 0)";
    return makeSValue(1, NULL, ctext);
}

BaseSemantics::SValue::Ptr
RiscOperators::iteWithStatus(const BaseSemantics::SValue::Ptr &sel, const BaseSemantics::SValue::Ptr &a,
                             const BaseSemantics::SValue::Ptr &b, IteStatus &status) {
    // (sel ? a : b)
    std::string ctext = "(" + SValue::promote(sel)->ctext() + " ? " + SValue::promote(a)->ctext() + " : " +
                        SValue::promote(b)->ctext() + ")";
    status = IteStatus::BOTH;
    return makeSValue(a->nBits(), NULL, ctext);
}

BaseSemantics::SValue::Ptr
RiscOperators::unsignedExtend(const BaseSemantics::SValue::Ptr &a, size_t newWidth) {
    std::string ctext;
    if (newWidth == a->nBits()) {
        ctext = SValue::promote(a)->ctext();
    } else if (newWidth < a->nBits()) {
        SValue::Ptr mask = SValue::promote(makeMask(a->nBits(), newWidth));
        std::string dstType = SValue::unsignedTypeNameForSize(newWidth);
        // ctext = ((dstType)(a & mask))
        ctext = "((" + dstType + ")(" + SValue::promote(a)->ctext() + " & " + mask->ctext() + "))";
    } else {
        ASSERT_require(newWidth > a->nBits());
        // ctext = ((dstType)a)
        std::string dstType = SValue::unsignedTypeNameForSize(newWidth);
        ctext = "((" + dstType + ")" + SValue::promote(a)->ctext() + ")";
    }
    return makeSValue(newWidth, NULL, ctext);
}

BaseSemantics::SValue::Ptr
RiscOperators::signExtend(const BaseSemantics::SValue::Ptr &a, size_t newWidth) {
    std::string ctext;
    if (newWidth == a->nBits()) {
        ctext = SValue::promote(a)->ctext();
    } else {
        ASSERT_require(newWidth >= a->nBits());
        // signBits = (signBit ? ~(dstType)0 : (dstType)0);
        // signExtension = (signBits << (dstType)aWidth)
        // ctext = (a | signExtension)
        std::string signBit = SValue::promote(extract(a, a->nBits()-1, a->nBits()))->ctext();
        std::string signBits = "(" + signBit + " ? "
                               "~" + SValue::promote(number_(newWidth, 0))->ctext() + " : " +
                               SValue::promote(number_(newWidth, 0))->ctext() + ")";
        std::string signExtension = "(" + signBits + " << " +
                                    SValue::promote(number_(newWidth, a->nBits()))->ctext() + ")";
        ctext = "(" + SValue::promote(a)->ctext() + " | " + signExtension + ")";
    }
    return makeSValue(newWidth, NULL, ctext);
}

BaseSemantics::SValue::Ptr
RiscOperators::add(const BaseSemantics::SValue::Ptr &a_, const BaseSemantics::SValue::Ptr &b_) {
    std::string mask = SValue::promote(makeMask(a_->nBits(), a_->nBits()))->ctext();
    std::string ctext = "((" + SValue::promote(a_)->ctext() + " + " + SValue::promote(b_)->ctext() + ")"
                        " & " + mask + ")";
    return makeSValue(a_->nBits(), NULL, ctext);
}

BaseSemantics::SValue::Ptr
RiscOperators::addWithCarries(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &b,
                              const BaseSemantics::SValue::Ptr &c, BaseSemantics::SValue::Ptr &carry_out/*out*/) {
    BaseSemantics::SValue::Ptr aWide = signExtend(a, a->nBits()+1);
    BaseSemantics::SValue::Ptr bWide = signExtend(b, a->nBits()+1);
    BaseSemantics::SValue::Ptr cWide = signExtend(c, a->nBits()+1);
    BaseSemantics::SValue::Ptr sumWide = add(add(aWide, bWide), cWide);
    BaseSemantics::SValue::Ptr mask = makeMask(a->nBits(), a->nBits());

    // carry_out = ((sumWide >> 1) & mask)
    std::string carry_text = "((" + SValue::promote(sumWide)->ctext() +
                             "  >> " + SValue::promote(number_(sumWide->nBits(), 1))->ctext() + ")"
                             " & " + SValue::promote(mask)->ctext() + ")";
    carry_out = makeSValue(a->nBits(), NULL, carry_text);

    // ctext = ((a + b + c) & mask)
    std::string ctext = "((" +
                        SValue::promote(a)->ctext() + " + " +
                        SValue::promote(b)->ctext() + " + " +
                        SValue::promote(unsignedExtend(c, a->nBits()))->ctext() + ")"
                        " & " + SValue::promote(mask)->ctext() + ")";

    return makeSValue(a->nBits(), NULL, ctext);
}

BaseSemantics::SValue::Ptr
RiscOperators::negate(const BaseSemantics::SValue::Ptr &a) {
    std::string mask = SValue::promote(makeMask(a->nBits(), a->nBits()))->ctext();
    std::string ctext = "(-" + SValue::promote(a)->ctext() + " & " + mask + ")";
    return makeSValue(a->nBits(), NULL, ctext);
}

BaseSemantics::SValue::Ptr
RiscOperators::signedDivide(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &b) {
    std::string ctext;
    std::string aSignedType = SValue::signedTypeNameForSize(a->nBits());
    std::string bSignedType = SValue::signedTypeNameForSize(b->nBits());
    std::string aUnsignedType = SValue::unsignedTypeNameForSize(a->nBits());
    if (b->nBits() <= a->nBits()) {
        // ctext = ((aUnsignedType)((aSignedType)a / (bSignedType)b))
        ctext = "((" + aUnsignedType + ")((" + aSignedType + ")" + SValue::promote(a)->ctext() + " / "
                "(" + bSignedType + ")" + SValue::promote(b)->ctext() + "))";
    } else {
        FIXME("not implemented yet"); // I don't think this normally happens in binaries [Robb P. Matzke 2015-09-29]
    }
    return makeSValue(a->nBits(), NULL, ctext);
}

BaseSemantics::SValue::Ptr
RiscOperators::signedModulo(const BaseSemantics::SValue::Ptr&, const BaseSemantics::SValue::Ptr&) {
    TODO("[Robb P. Matzke 2015-09-23]: generate signed '%' expression");
}

BaseSemantics::SValue::Ptr
RiscOperators::signedMultiply(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &b) {
    size_t operandsSize = std::max(a->nBits(), b->nBits());
    std::string aSignedType = SValue::signedTypeNameForSize(a->nBits());
    std::string bSignedType = SValue::signedTypeNameForSize(b->nBits());
    std::string operandsSignedType = SValue::signedTypeNameForSize(operandsSize);
    size_t productSize = a->nBits() + b->nBits();
    std::string productUnsignedType = SValue::unsignedTypeNameForSize(productSize);

    // ctext = ((productUnsigned)((operandsSigned)(aSigned)a * (operandsSigned)(bSigned)b))
    std::string ctext = "((" + productUnsignedType + ")("
                        "(" + operandsSignedType + ")(" + aSignedType + ")" + SValue::promote(a)->ctext() + " * " +
                        "(" + operandsSignedType + ")(" + bSignedType + ")" + SValue::promote(b)->ctext() + "))";
    return makeSValue(productSize, NULL, ctext);
}

BaseSemantics::SValue::Ptr
RiscOperators::unsignedDivide(const BaseSemantics::SValue::Ptr&, const BaseSemantics::SValue::Ptr&) {
    TODO("[Robb P. Matzke 2015-09-23]: generate unsigned '/' expression");
}

BaseSemantics::SValue::Ptr
RiscOperators::unsignedModulo(const BaseSemantics::SValue::Ptr&, const BaseSemantics::SValue::Ptr&) {
    TODO("[Robb P. Matzke 2015-09-23]: generate unsigned '%' expression");
}

BaseSemantics::SValue::Ptr
RiscOperators::unsignedMultiply(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &b) {
    SValue::Ptr aWide = SValue::promote(unsignedExtend(a, a->nBits() + b->nBits()));
    SValue::Ptr bWide = SValue::promote(unsignedExtend(b, a->nBits() + b->nBits()));
    std::string ctext = "(" + aWide->ctext() + " * " + bWide->ctext() + ")";
    return makeSValue(a->nBits() + b->nBits(), NULL, ctext);
}

void
RiscOperators::interrupt(int majr, int minr) {
    std::ostringstream ctext;
    ctext <<"interrupt(" <<majr <<", " <<minr <<")";
    saveSideEffect(makeSValue(1, NULL, ctext.str()));
}

BaseSemantics::SValue::Ptr
RiscOperators::readRegister(RegisterDescriptor reg, const BaseSemantics::SValue::Ptr &dflt) {
    BaseSemantics::SValue::Ptr retval = Super::readRegister(reg, dflt);
    return substitute(retval);
}

BaseSemantics::SValue::Ptr
RiscOperators::peekRegister(RegisterDescriptor reg, const BaseSemantics::SValue::Ptr &dflt) {
    BaseSemantics::SValue::Ptr retval = Super::peekRegister(reg, dflt);
    return substitute(retval);
}

void
RiscOperators::writeRegister(RegisterDescriptor reg, const BaseSemantics::SValue::Ptr &value) {
   RegisterState::Ptr registers = RegisterState::promote(currentState()->registerState());
   RegisterState::BitRange wantLocation = RegisterState::BitRange::baseSize(reg.offset(), reg.nBits());
   RegisterState::RegPairs regpairs = registers->overlappingRegisters(reg);
   for (RegisterState::RegPair &regpair: regpairs) {
       RegisterState::BitRange storageLocation = regpair.location();
       RegisterState::BitRange overlapLocation = wantLocation & storageLocation;

       // Create the value to be written back to this storage location.
       BaseSemantics::SValue::Ptr toWrite;
       if (overlapLocation.least() > storageLocation.least()) {
           size_t offset = 0;
           size_t nbits = overlapLocation.least() - storageLocation.least();
           toWrite = extract(regpair.value, offset, offset+nbits);
       }
       {
           size_t offset = overlapLocation.least() - wantLocation.least();
           size_t nbits = overlapLocation.size();
           BaseSemantics::SValue::Ptr part = extract(value, offset, offset+nbits);
           toWrite = toWrite ? concat(toWrite, part) : part;
       }
       if (overlapLocation.greatest() < storageLocation.greatest()) {
           size_t offset = overlapLocation.greatest()+1 - storageLocation.least();
           size_t nbits = storageLocation.greatest() - overlapLocation.greatest();
           toWrite = concat(toWrite, extract(regpair.value, offset, offset+nbits));
       }
       ASSERT_require(toWrite->nBits() == regpair.value->nBits());
       regpair.value = toWrite;
   }

   // Substitute, and write substitution back to register state.
   for (const RegisterState::RegPair &regpair: regpairs) {
       BaseSemantics::SValue::Ptr regVar = makeSValue(regpair.desc.nBits(), NULL, registerVariableName(regpair.desc));
       BaseSemantics::SValue::Ptr temp = saveSideEffect(regpair.value, regVar);
       registers->writeRegister(regpair.desc, temp, this);
   }
}

BaseSemantics::SValue::Ptr
RiscOperators::readMemory(RegisterDescriptor segreg, const BaseSemantics::SValue::Ptr &address,
                          const BaseSemantics::SValue::Ptr &dflt, const BaseSemantics::SValue::Ptr &/*cond*/) {
    ASSERT_require2(dflt->nBits() % 8 == 0, "readMemory size must be a multiple of a byte");
    size_t nBytes = dflt->nBits() >> 3;
    BaseSemantics::SValue::Ptr retval;
    BaseSemantics::MemoryState::Ptr mem = currentState()->memoryState();

    // Offset the address by the value of the segment register.
    BaseSemantics::SValue::Ptr adjustedVa;
    if (segreg.isEmpty()) {
        adjustedVa = address;
    } else {
        BaseSemantics::SValue::Ptr segregValue = readRegister(segreg, undefined_(segreg.nBits()));
        adjustedVa = add(address, signExtend(segregValue, address->nBits()));
    }

    for (size_t byteNum=0; byteNum<nBytes; ++byteNum) {
        size_t byteOffset = ByteOrder::ORDER_MSB==mem->get_byteOrder() ? nBytes-(byteNum+1) : byteNum;
        std::string ctext = "mem[" + SValue::promote(adjustedVa)->ctext() +
                            "+" + StringUtility::numberToString(byteOffset) +
                            "]";
        BaseSemantics::SValue::Ptr byte = makeSValue(8, NULL, ctext);
        if (retval == NULL) {
            retval = byte;
        } else if (ByteOrder::ORDER_MSB == mem->get_byteOrder()) {
            retval = concat(byte, retval);
        } else if (ByteOrder::ORDER_LSB == mem->get_byteOrder()) {
            retval = concat(retval, byte);
        } else {
            // See BaseSemantics::MemoryState::set_byteOrder
            throw BaseSemantics::Exception("multi-byte read with memory having unspecified byte order", currentInstruction());
        }
    }
    return retval;
}

BaseSemantics::SValue::Ptr
RiscOperators::peekMemory(RegisterDescriptor /*segreg*/, const BaseSemantics::SValue::Ptr &/*address*/,
                          const BaseSemantics::SValue::Ptr &/*dflt*/) {
    ASSERT_not_reachable("operation doesn't make sense in this domain");
}

void
RiscOperators::writeMemory(RegisterDescriptor segreg, const BaseSemantics::SValue::Ptr &address,
                           const BaseSemantics::SValue::Ptr &value, const BaseSemantics::SValue::Ptr &/*cond*/) {
    ASSERT_require2(value->nBits() % 8 == 0, "writeMemory size must be a multiple of a byte");
    size_t nBytes = value->nBits() >> 3;
    BaseSemantics::MemoryState::Ptr mem = currentState()->memoryState();

    // Offset the address by the value of the segment register.
    BaseSemantics::SValue::Ptr adjustedVa;
    if (segreg.isEmpty()) {
        adjustedVa = address;
    } else {
        BaseSemantics::SValue::Ptr segregValue = readRegister(segreg, undefined_(segreg.nBits()));
        adjustedVa = add(address, signExtend(segregValue, address->nBits()));
    }

    for (size_t byteNum=0; byteNum<nBytes; ++byteNum) {
        size_t byteOffset = ByteOrder::ORDER_MSB==mem->get_byteOrder() ? nBytes-(byteNum+1) : byteNum;
        BaseSemantics::SValue::Ptr byte = extract(value, 8*byteOffset, 8*(byteOffset+1));
        std::string lhs = "mem[" + SValue::promote(adjustedVa)->ctext() +
                          " + " + SValue::promote(number_(adjustedVa->nBits(), byteOffset))->ctext() +
                          "]";

        saveSideEffect(byte, makeSValue(8, NULL, lhs));
    }
}

} // namespace
} // namespace
} // namespace
} // namespace

#endif
