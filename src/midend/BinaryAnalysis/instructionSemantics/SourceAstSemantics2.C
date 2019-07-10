#include <sage3basic.h>
#include <SourceAstSemantics2.h>

#include <AsmUnparser_compat.h>
#include <Disassembler.h>
#include <Partitioner2/Partitioner.h>
#include <TraceSemantics2.h>
#include <integerOps.h>

namespace P2 = Rose::BinaryAnalysis::Partitioner2;

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics2 {
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

void
SValue::print(std::ostream &out, BaseSemantics::Formatter &fmt) const {
    out <<ctext_;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Supporting functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RiscOperators::resetState() {
    // Initialize registers so they correspond to the C global variables we'll generate, and then lock the register state so
    // those registers don't change if we access subparts (like if we store EAX and write to AX).
    currentState()->clear();
    RegisterStatePtr registers = RegisterState::promote(currentState()->registerState());
    registers->initialize_large();
    registers->accessModifiesExistingLocations(false);
    RegisterState::RegPairs regpairs = registers->get_stored_registers();
    BOOST_FOREACH (RegisterState::RegPair &regpair, regpairs) {
        std::string varName = registerVariableName(regpair.desc);
        BaseSemantics::SValuePtr value = makeSValue(regpair.desc.nBits(), NULL, varName);
        registers->writeRegister(regpair.desc, value, this);
    }
    registers->eraseWriters();
    registers->eraseProperties();
    executionHalted_ = false;
}

BaseSemantics::SValuePtr
RiscOperators::makeSValue(size_t nbits, SgNode *ast, const std::string &ctext) {
    ASSERT_require(nbits > 0);
    SValuePtr v = SValue::promote(protoval()->undefined_(nbits));
    v->ctext(ctext);
    return v;
}

// Append a side effect to the list of side effects.
BaseSemantics::SValuePtr
RiscOperators::saveSideEffect(const BaseSemantics::SValuePtr &expression, const BaseSemantics::SValuePtr &location) {
    if (executionHalted_)
        return expression;
    BaseSemantics::SValuePtr retval;
    if (location)
        retval = undefined_(expression->get_width());
    sideEffects_.push_back(SideEffect(location, retval, expression));
    return retval;
}

BaseSemantics::SValuePtr
RiscOperators::substitute(const BaseSemantics::SValuePtr &expression) {
    BaseSemantics::SValuePtr retval = undefined_(expression->get_width());
    sideEffects_.push_back(SideEffect(BaseSemantics::SValuePtr(), retval, expression));
    return retval;
}


// C global variable name for a register.
std::string
RiscOperators::registerVariableName(RegisterDescriptor reg) {
    using namespace StringUtility;
    const RegisterDictionary *registers = currentState()->registerState()->get_register_dictionary();
    std::string name = registers->lookup(reg);
    if (name.empty()) {
        return ("R_" + numberToString(reg.majorNumber()) +
                "_" + numberToString(reg.minorNumber()) +
                "_" + numberToString(reg.offset()) +
                "_" + numberToString(reg.nBits()));
    }
    return "R_" + name;
}

// Create a mask consisting of nset shifted upward by sa.
BaseSemantics::SValuePtr
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

BaseSemantics::SValuePtr
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

BaseSemantics::SValuePtr
RiscOperators::rdtsc() {
    return makeSValue(64, NULL, "rdtsc()");
}

BaseSemantics::SValuePtr
RiscOperators::and_(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b) {
    std::string ctext = "(" + SValue::promote(a)->ctext() + " & " + SValue::promote(b)->ctext() + ")";
    return makeSValue(a->get_width(), NULL, ctext);
}

BaseSemantics::SValuePtr
RiscOperators::or_(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b) {
    std::string ctext = "(" + SValue::promote(a)->ctext() + " | " + SValue::promote(b)->ctext() + ")";
    return makeSValue(a->get_width(), NULL, ctext);
}

BaseSemantics::SValuePtr
RiscOperators::xor_(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b) {
    std::string ctext = "(" + SValue::promote(a)->ctext() + " ^ " + SValue::promote(b)->ctext() + ")";
    return makeSValue(a->get_width(), NULL, ctext);
}

BaseSemantics::SValuePtr
RiscOperators::invert(const BaseSemantics::SValuePtr &a) {
    SValuePtr mask = SValue::promote(makeMask(a->get_width(), a->get_width()));
    std::string ctext = "(~" + SValue::promote(a)->ctext() + " & " + mask->ctext() + ")";
    return makeSValue(a->get_width(), NULL, ctext);
}

BaseSemantics::SValuePtr
RiscOperators::extract(const BaseSemantics::SValuePtr &a, size_t begin_bit, size_t end_bit) {
    ASSERT_require(end_bit <= a->get_width());
    ASSERT_require(begin_bit < end_bit);
    if (0 == begin_bit && end_bit == a->get_width()) {
        return a->copy();
    } else {
        size_t newSize = end_bit - begin_bit;
        std::string ctext = "((" + SValue::promote(a)->ctext() + " >> " +
                            SValue::promote(number_(a->get_width(), begin_bit))->ctext() + ") & " +
                            SValue::promote(makeMask(a->get_width(), newSize))->ctext() + ")";
        return unsignedExtend(makeSValue(a->get_width(), NULL, ctext), newSize);
    }
}

BaseSemantics::SValuePtr
RiscOperators::concat(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b) {
    size_t resultWidth = a->get_width() + b->get_width();
    std::string resultType = SValue::unsignedTypeNameForSize(resultWidth);

    // ctext = ((resultType)a | ((resultType)b << (resultType)aWidth))
    SValuePtr aWidth = SValue::promote(number_(resultWidth, a->get_width()));
    std::string ctext = "((" + resultType + ")" + SValue::promote(a)->ctext() + " | "
                        "((" + resultType + ")" + SValue::promote(b)->ctext() + " << " +
                        "(" + resultType + ")" + aWidth->ctext() + "))";

    return makeSValue(a->get_width() + b->get_width(), NULL, ctext);
}

BaseSemantics::SValuePtr
RiscOperators::leastSignificantSetBit(const BaseSemantics::SValuePtr &a) {
    std::string ctext = "lssb" + StringUtility::numberToString(a->get_width()) + "(" + SValue::promote(a)->ctext() + ")";
    return makeSValue(a->get_width(), NULL, ctext);
}

BaseSemantics::SValuePtr
RiscOperators::mostSignificantSetBit(const BaseSemantics::SValuePtr &a) {
    std::string ctext = "mssb" + StringUtility::numberToString(a->get_width()) + "(" + SValue::promote(a)->ctext() + ")";
    return makeSValue(a->get_width(), NULL, ctext);
}

BaseSemantics::SValuePtr
RiscOperators::rotateLeft(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &sa) {
    // Note: '<<' and '>>' are not well defined in C when the shift amount is >= the size of the left operand.
    //   ctext = ((((a) << (             (sa) % (8*sizeof(a)))  |
    //             ((a) >> (8*sizeof(a)-((sa) % (8*sizeof(a)))))) & mask)
    std::string normalizedAmount = "((" + SValue::promote(sa)->ctext() + ") % " +
                                   StringUtility::numberToString(a->get_width()) + ")";
    std::string complementAmount = "(" + StringUtility::numberToString(a->get_width()) + " - " + normalizedAmount;
    std::string mask = SValue::promote(makeMask(a->get_width(), a->get_width()))->ctext();
    std::string ctext = "((((" + SValue::promote(a)->ctext() + ") << " + normalizedAmount + ") |"
                        "  ((" + SValue::promote(a)->ctext() + ") >> " + complementAmount + "))"
                        " & " + mask + ")";
    return makeSValue(a->get_width(), NULL, ctext);
}

BaseSemantics::SValuePtr
RiscOperators::rotateRight(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &sa) {
    // Note: '<<' and '>>' are not well defined in C when the shift amount is >= the size of the left operand.
    //   ((((a) >> (             (sa) % (8*sizeof(a)))  |
    //     ((a) << (8*sizeof(a)-((sa) % (8*sizeof(a)))))) & mask)
    std::string normalizedAmount = "((" + SValue::promote(sa)->ctext() + ") % " +
                                   StringUtility::numberToString(a->get_width()) + ")";
    std::string complementAmount = "(" + StringUtility::numberToString(a->get_width()) + " - " + normalizedAmount;
    std::string mask = SValue::promote(makeMask(a->get_width(), a->get_width()))->ctext();
    std::string ctext = "((((" + SValue::promote(a)->ctext() + ") >> " + normalizedAmount + ") |"
                        "  ((" + SValue::promote(a)->ctext() + ") << " + complementAmount + "))"
                        " & " + mask + ")";
    return makeSValue(a->get_width(), NULL, ctext);
}

BaseSemantics::SValuePtr
RiscOperators::shiftLeft(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &sa) {
    // '<<' is not well defined in C when the shift amount is >= the size of the left operand.
    //   (sa >= 8*sizeof(a) ? (aType)0 : ((a << sa) & mask))
    std::string mask = SValue::promote(makeMask(a->get_width(), a->get_width()))->ctext();
    std::string ctext = "(" + SValue::promote(sa)->ctext() + " >= " + StringUtility::numberToString(a->get_width()) + " ? " +
                        SValue::promote(number_(a->get_width(), 0))->ctext() + " : " +
                        "((" + SValue::promote(a)->ctext() + " << " + SValue::promote(sa)->ctext() + ")"
                        " & " + mask + "))";
    return makeSValue(a->get_width(), NULL, ctext);
}

BaseSemantics::SValuePtr
RiscOperators::shiftRight(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &sa) {
    // '>>' is not well defined in C when the shift amount is >= the size of the left operand.
    //   (sa >= 8*sizeof(a) ? (aType)0 : a >> sa)
    std::string ctext = "(" + SValue::promote(sa)->ctext() + " >= " + StringUtility::numberToString(a->get_width()) + " ? " +
                        SValue::promote(number_(a->get_width(), 0))->ctext() + " : " +
                        SValue::promote(a)->ctext() + " >> " + SValue::promote(sa)->ctext() + ")";
    return makeSValue(a->get_width(), NULL, ctext);
}

BaseSemantics::SValuePtr
RiscOperators::shiftRightArithmetic(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &sa) {
    // Do the right shift without any casting to signed types.
    //    signBits = (signBit ? ~(aType)0 : (aType)0)
    //    signExtension = (sa >= 8*sizeof(a) ? signBits : signBits << (8*sizeof(a) - sa))
    //    ctext = (sa >= 8*sizeof(a) ? signBits : (a >> sa) | signExtension)
    std::string signBit = SValue::promote(extract(a, a->get_width()-1, a->get_width()))->ctext();
    std::string signBits = "(" + signBit + " ? "
                           "~" + SValue::promote(number_(a->get_width(), 0))->ctext() + " : " +
                           SValue::promote(number_(a->get_width(), 0))->ctext() + ")";
    std::string width = SValue::promote(number_(a->get_width(), a->get_width()))->ctext();
    std::string signExtension = "(" + SValue::promote(sa)->ctext() + " >= " + width + " ? " +
                                signBits + " : " + signBits + " << (" + width + " - " + SValue::promote(sa)->ctext() + "))";
    std::string ctext = "(" + SValue::promote(sa)->ctext() + " >= " + width + " ? " +
                        signBits + " : "
                        "(" + SValue::promote(a)->ctext() + " >> " + SValue::promote(sa)->ctext() + ") | " + signExtension +
                        ")";
    return makeSValue(a->get_width(), NULL, ctext);
}

BaseSemantics::SValuePtr
RiscOperators::equalToZero(const BaseSemantics::SValuePtr &a) {
    std::string ctext =  "(" + SValue::promote(a)->ctext() + " == 0)";
    return makeSValue(1, NULL, ctext);
}

BaseSemantics::SValuePtr
RiscOperators::ite(const BaseSemantics::SValuePtr &sel, const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b) {
    // (sel ? a : b)
    std::string ctext = "(" + SValue::promote(sel)->ctext() + " ? " + SValue::promote(a)->ctext() + " : " +
                        SValue::promote(b)->ctext() + ")";
    return makeSValue(a->get_width(), NULL, ctext);
}

BaseSemantics::SValuePtr
RiscOperators::unsignedExtend(const BaseSemantics::SValuePtr &a, size_t newWidth) {
    std::string ctext;
    if (newWidth == a->get_width()) {
        ctext = SValue::promote(a)->ctext();
    } else if (newWidth < a->get_width()) {
        SValuePtr mask = SValue::promote(makeMask(a->get_width(), newWidth));
        std::string dstType = SValue::unsignedTypeNameForSize(newWidth);
        // ctext = ((dstType)(a & mask))
        ctext = "((" + dstType + ")(" + SValue::promote(a)->ctext() + " & " + mask->ctext() + "))";
    } else {
        ASSERT_require(newWidth > a->get_width());
        // ctext = ((dstType)a)
        std::string dstType = SValue::unsignedTypeNameForSize(newWidth);
        ctext = "((" + dstType + ")" + SValue::promote(a)->ctext() + ")";
    }
    return makeSValue(newWidth, NULL, ctext);
}

BaseSemantics::SValuePtr
RiscOperators::signExtend(const BaseSemantics::SValuePtr &a, size_t newWidth) {
    std::string ctext;
    if (newWidth == a->get_width()) {
        ctext = SValue::promote(a)->ctext();
    } else {
        ASSERT_require(newWidth >= a->get_width());
        // signBits = (signBit ? ~(dstType)0 : (dstType)0);
        // signExtension = (signBits << (dstType)aWidth)
        // ctext = (a | signExtension)
        std::string signBit = SValue::promote(extract(a, a->get_width()-1, a->get_width()))->ctext();
        std::string signBits = "(" + signBit + " ? "
                               "~" + SValue::promote(number_(newWidth, 0))->ctext() + " : " +
                               SValue::promote(number_(newWidth, 0))->ctext() + ")";
        std::string signExtension = "(" + signBits + " << " +
                                    SValue::promote(number_(newWidth, a->get_width()))->ctext() + ")";
        ctext = "(" + SValue::promote(a)->ctext() + " | " + signExtension + ")";
    }
    return makeSValue(newWidth, NULL, ctext);
}

BaseSemantics::SValuePtr
RiscOperators::add(const BaseSemantics::SValuePtr &a_, const BaseSemantics::SValuePtr &b_) {
    std::string mask = SValue::promote(makeMask(a_->get_width(), a_->get_width()))->ctext();
    std::string ctext = "((" + SValue::promote(a_)->ctext() + " + " + SValue::promote(b_)->ctext() + ")"
                        " & " + mask + ")";
    return makeSValue(a_->get_width(), NULL, ctext);
}

BaseSemantics::SValuePtr
RiscOperators::addWithCarries(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b,
                              const BaseSemantics::SValuePtr &c, BaseSemantics::SValuePtr &carry_out/*out*/) {
    BaseSemantics::SValuePtr aWide = signExtend(a, a->get_width()+1);
    BaseSemantics::SValuePtr bWide = signExtend(b, a->get_width()+1);
    BaseSemantics::SValuePtr cWide = signExtend(c, a->get_width()+1);
    BaseSemantics::SValuePtr sumWide = add(add(aWide, bWide), cWide);
    BaseSemantics::SValuePtr mask = makeMask(a->get_width(), a->get_width());

    // carry_out = ((sumWide >> 1) & mask)
    std::string carry_text = "((" + SValue::promote(sumWide)->ctext() +
                             "  >> " + SValue::promote(number_(sumWide->get_width(), 1))->ctext() + ")"
                             " & " + SValue::promote(mask)->ctext() + ")";
    carry_out = makeSValue(a->get_width(), NULL, carry_text);

    // ctext = ((a + b + c) & mask)
    std::string ctext = "((" +
                        SValue::promote(a)->ctext() + " + " +
                        SValue::promote(b)->ctext() + " + " +
                        SValue::promote(unsignedExtend(c, a->get_width()))->ctext() + ")"
                        " & " + SValue::promote(mask)->ctext() + ")";

    return makeSValue(a->get_width(), NULL, ctext);
}

BaseSemantics::SValuePtr
RiscOperators::negate(const BaseSemantics::SValuePtr &a) {
    std::string mask = SValue::promote(makeMask(a->get_width(), a->get_width()))->ctext();
    std::string ctext = "(-" + SValue::promote(a)->ctext() + " & " + mask + ")";
    return makeSValue(a->get_width(), NULL, ctext);
}

BaseSemantics::SValuePtr
RiscOperators::signedDivide(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b) {
    std::string ctext;
    std::string aSignedType = SValue::signedTypeNameForSize(a->get_width());
    std::string bSignedType = SValue::signedTypeNameForSize(b->get_width());
    std::string aUnsignedType = SValue::unsignedTypeNameForSize(a->get_width());
    if (b->get_width() <= a->get_width()) {
        // ctext = ((aUnsignedType)((aSignedType)a / (bSignedType)b))
        ctext = "((" + aUnsignedType + ")((" + aSignedType + ")" + SValue::promote(a)->ctext() + " / "
                "(" + bSignedType + ")" + SValue::promote(b)->ctext() + "))";
    } else {
        FIXME("not implemented yet"); // I don't think this normally happens in binaries [Robb P. Matzke 2015-09-29]
    }
    return makeSValue(a->get_width(), NULL, ctext);
}

BaseSemantics::SValuePtr
RiscOperators::signedModulo(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b) {
    TODO("[Robb P. Matzke 2015-09-23]: generate signed '%' expression");
}

BaseSemantics::SValuePtr
RiscOperators::signedMultiply(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b) {
    size_t operandsSize = std::max(a->get_width(), b->get_width());
    std::string aSignedType = SValue::signedTypeNameForSize(a->get_width());
    std::string bSignedType = SValue::signedTypeNameForSize(b->get_width());
    std::string operandsSignedType = SValue::signedTypeNameForSize(operandsSize);
    size_t productSize = a->get_width() + b->get_width();
    std::string productUnsignedType = SValue::unsignedTypeNameForSize(productSize);

    // ctext = ((productUnsigned)((operandsSigned)(aSigned)a * (operandsSigned)(bSigned)b))
    std::string ctext = "((" + productUnsignedType + ")("
                        "(" + operandsSignedType + ")(" + aSignedType + ")" + SValue::promote(a)->ctext() + " * " +
                        "(" + operandsSignedType + ")(" + bSignedType + ")" + SValue::promote(b)->ctext() + "))";
    return makeSValue(productSize, NULL, ctext);
}

BaseSemantics::SValuePtr
RiscOperators::unsignedDivide(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b) {
    TODO("[Robb P. Matzke 2015-09-23]: generate unsigned '/' expression");
}

BaseSemantics::SValuePtr
RiscOperators::unsignedModulo(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b) {
    TODO("[Robb P. Matzke 2015-09-23]: generate unsigned '%' expression");
}

BaseSemantics::SValuePtr
RiscOperators::unsignedMultiply(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b) {
    SValuePtr aWide = SValue::promote(unsignedExtend(a, a->get_width() + b->get_width()));
    SValuePtr bWide = SValue::promote(unsignedExtend(b, a->get_width() + b->get_width()));
    std::string ctext = "(" + aWide->ctext() + " * " + bWide->ctext() + ")";
    return makeSValue(a->get_width() + b->get_width(), NULL, ctext);
}

void
RiscOperators::interrupt(int majr, int minr) {
    std::ostringstream ctext;
    ctext <<"interrupt(" <<majr <<", " <<minr <<")";
    saveSideEffect(makeSValue(1, NULL, ctext.str()));
}

BaseSemantics::SValuePtr
RiscOperators::readRegister(RegisterDescriptor reg, const BaseSemantics::SValuePtr &dflt) {
    BaseSemantics::SValuePtr retval = Super::readRegister(reg, dflt);
    return substitute(retval);
}

BaseSemantics::SValuePtr
RiscOperators::peekRegister(RegisterDescriptor reg, const BaseSemantics::SValuePtr &dflt) {
    BaseSemantics::SValuePtr retval = Super::peekRegister(reg, dflt);
    return substitute(retval);
}

void
RiscOperators::writeRegister(RegisterDescriptor reg, const BaseSemantics::SValuePtr &value) {
   RegisterStatePtr registers = RegisterState::promote(currentState()->registerState());
   RegisterState::BitRange wantLocation = RegisterState::BitRange::baseSize(reg.offset(), reg.nBits());
   RegisterState::RegPairs regpairs = registers->overlappingRegisters(reg);
   BOOST_FOREACH (RegisterState::RegPair &regpair, regpairs) {
       RegisterState::BitRange storageLocation = regpair.location();
       RegisterState::BitRange overlapLocation = wantLocation & storageLocation;

       // Create the value to be written back to this storage location.
       BaseSemantics::SValuePtr toWrite;
       if (overlapLocation.least() > storageLocation.least()) {
           size_t offset = 0;
           size_t nbits = overlapLocation.least() - storageLocation.least();
           toWrite = extract(regpair.value, offset, offset+nbits);
       }
       {
           size_t offset = overlapLocation.least() - wantLocation.least();
           size_t nbits = overlapLocation.size();
           BaseSemantics::SValuePtr part = extract(value, offset, offset+nbits);
           toWrite = toWrite ? concat(toWrite, part) : part;
       }
       if (overlapLocation.greatest() < storageLocation.greatest()) {
           size_t offset = overlapLocation.greatest()+1 - storageLocation.least();
           size_t nbits = storageLocation.greatest() - overlapLocation.greatest();
           toWrite = concat(toWrite, extract(regpair.value, offset, offset+nbits));
       }
       ASSERT_require(toWrite->get_width() == regpair.value->get_width());
       regpair.value = toWrite;
   }

   // Substitute, and write substitution back to register state.
   BOOST_FOREACH (const RegisterState::RegPair &regpair, regpairs) {
       BaseSemantics::SValuePtr regVar = makeSValue(regpair.desc.nBits(), NULL, registerVariableName(regpair.desc));
       BaseSemantics::SValuePtr temp = saveSideEffect(regpair.value, regVar);
       registers->writeRegister(regpair.desc, temp, this);
   }
}

BaseSemantics::SValuePtr
RiscOperators::readMemory(RegisterDescriptor segreg, const BaseSemantics::SValuePtr &address,
                          const BaseSemantics::SValuePtr &dflt, const BaseSemantics::SValuePtr &cond) {
    ASSERT_require2(dflt->get_width() % 8 == 0, "readMemory size must be a multiple of a byte");
    size_t nBytes = dflt->get_width() >> 3;
    BaseSemantics::SValuePtr retval;
    BaseSemantics::MemoryStatePtr mem = currentState()->memoryState();
    for (size_t byteNum=0; byteNum<nBytes; ++byteNum) {
        size_t byteOffset = ByteOrder::ORDER_MSB==mem->get_byteOrder() ? nBytes-(byteNum+1) : byteNum;
        std::string ctext = "mem[" + SValue::promote(address)->ctext() +
                            "+" + StringUtility::numberToString(byteOffset) +
                            "]";
        BaseSemantics::SValuePtr byte = makeSValue(8, NULL, ctext);
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

BaseSemantics::SValuePtr
RiscOperators::peekMemory(RegisterDescriptor segreg, const BaseSemantics::SValuePtr &address,
                          const BaseSemantics::SValuePtr &dflt) {
    ASSERT_not_reachable("operation doesn't make sense in this domain");
}

void
RiscOperators::writeMemory(RegisterDescriptor segreg, const BaseSemantics::SValuePtr &address,
                           const BaseSemantics::SValuePtr &value, const BaseSemantics::SValuePtr &cond) {
    ASSERT_require2(value->get_width() % 8 == 0, "writeMemory size must be a multiple of a byte");
    size_t nBytes = value->get_width() >> 3;
    BaseSemantics::MemoryStatePtr mem = currentState()->memoryState();
    for (size_t byteNum=0; byteNum<nBytes; ++byteNum) {
        size_t byteOffset = ByteOrder::ORDER_MSB==mem->get_byteOrder() ? nBytes-(byteNum+1) : byteNum;
        BaseSemantics::SValuePtr byte = extract(value, 8*byteOffset, 8*(byteOffset+1));
        std::string lhs = "mem[" + SValue::promote(address)->ctext() +
                          " + " + SValue::promote(number_(address->get_width(), byteOffset))->ctext() +
                          "]";

        saveSideEffect(byte, makeSValue(8, NULL, lhs));
    }
}

} // namespace
} // namespace
} // namespace
} // namespace
