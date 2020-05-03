#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include <sage3basic.h>
#include <BinaryString.h>

#include <Sawyer/ProgressBar.h>

using namespace Rose::Diagnostics;

namespace Rose {
namespace BinaryAnalysis {
namespace Strings {

Sawyer::Message::Facility mlog;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Utility functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
initDiagnostics(void) {
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        Diagnostics::initAndRegister(&mlog, "Rose::BinaryAnalysis::Strings");
        mlog.comment("detecting string constants");
    }
}
    
bool
isDone(State st) {
    return st == FINAL_STATE || st == COMPLETED_STATE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      NoopCharacterEncodingForm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CodeValues
NoopCharacterEncodingForm::encode(CodePoint cp) {
    return CodeValues(1, cp);
}

State
NoopCharacterEncodingForm::decode(CodeValue cv) {
    if (state_ != INITIAL_STATE)
        return state_ = ERROR_STATE;
    cp_ = cv;
    return state_ = FINAL_STATE;
}

CodePoint
NoopCharacterEncodingForm::consume() {
    ASSERT_require(isDone(state_));
    state_ = INITIAL_STATE;
    return cp_;
}

void
NoopCharacterEncodingForm::reset() {
    state_ = INITIAL_STATE;
    cp_ = 0;
}

NoopCharacterEncodingForm::Ptr
noopCharacterEncodingForm() {
    return NoopCharacterEncodingForm::instance();
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Utf8CharacterEncodingForm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CodeValues
Utf8CharacterEncodingForm::encode(CodePoint cp) {
    CodeValues cvs;
    if (cp <= 0x7f) {
        cvs.push_back(cp);
    } else {
        unsigned prefixLength = 3;                                                      // 0b110x_xxxx
        while (1) {
            cvs.push_back(0x80 | (cp & 0x3f));
            cp >>= 6;
            
            unsigned mask = ((unsigned)1 << (8-prefixLength)) - 1;                      // 0b0001_1111 if prefixLength==3
            if ((cp & ~mask) == 0) {
                unsigned prefix = ((unsigned)1 << prefixLength) - 2;                    // 0bxxxx_x110 if prefixLength==3
                prefix <<= 8 - prefixLength;                                            // 0b110x_xxxx if prefixLength==3
                cvs.push_back(prefix | cp);
                break;
            }

            if (++prefixLength >= 8)
                throw Exception("invalid code point for " + name() + ": " + StringUtility::addrToString(cp));
        }
        std::reverse(cvs.begin(), cvs.end());
    }
    return cvs;
}

State
Utf8CharacterEncodingForm::decode(CodeValue cv) {
    switch (state_) {
        case ERROR_STATE:
        case FINAL_STATE:
        case COMPLETED_STATE:
            return state_ = ERROR_STATE;
        case INITIAL_STATE:
            if (cv <= 0x7f) {                           // 0b0xxx_xxxx
                cp_ = cv;
                return state_ = FINAL_STATE;
            } else if ((cv & 0xe0) == 0xc0) {           // 0x110x_xxxx
                cp_ = cv & 0x1f;
                return state_ = State(1);
            } else if ((cv & 0xf0) == 0xe0) {           // 0x1110_xxxx
                cp_ = cv & 0x0f;
                return state_ = State(2);
            } else if ((cv & 0xf8) == 0xf0) {           // 0x1111_0xxx
                cp_ = cv & 0x07;
                return state_ = State(3);
            } else if ((cv & 0xfc) == 0xf8) {           // 0x1111_10xx
                cp_ = cv & 0x03;
                return state_ = State(4);
            } else if ((cv & 0xfe) == 0xfc) {           // 0x1111_110x
                cp_ = cv & 0x01;
                return state_ = State(5);
            } else {                                    // invalid prefix
                return state_ = ERROR_STATE;
            }
        default:
            ASSERT_require(state_ >= 1 && state_ <= 5);
            if ((cv & 0xc0) == 0x80) {                  // 0x10xx_xxxx
                cp_ = (cp_ << 6) | (cv & 0x3f);
                return state_ = (1==state_ ? FINAL_STATE : State(state_ - 1));
            } else {
                return state_ = ERROR_STATE;                   // invalid continuation octet
            }
    }
}

CodePoint
Utf8CharacterEncodingForm::consume() {
    ASSERT_require(isDone(state_));
    state_ = INITIAL_STATE;
    return cp_;
}

void
Utf8CharacterEncodingForm::reset() {
    state_ = INITIAL_STATE;
    cp_ = 0;
}

Utf8CharacterEncodingForm::Ptr
utf8CharacterEncodingForm() {
    return Utf8CharacterEncodingForm::instance();
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Utf16CharacterEncodingForm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CodeValues
Utf16CharacterEncodingForm::encode(CodePoint cp) {
    CodeValues cvs;
    if (cp <= 0x0000d7ff || (cp >= 0x0000e000 && cp <= 0x0000ffff)) {
        cvs.push_back(cp);
    } else if (cp >= 0x00010000 && cp <= 0x0010ffff) {
        CodePoint hiSurrogate = ((cp - 0x0001000) >> 10) + 0x0000d800;   // a.k.a., leading surrogate
        CodePoint loSurrogate = ((cp - 0x0001000) & 0x3ff) + 0x0000dc00; // a.k.a., trailing surrogate
        ASSERT_require(hiSurrogate >= 0xd800 && hiSurrogate <= 0xdbff);
        ASSERT_require(loSurrogate >= 0xdc00 && loSurrogate <= 0xdfff);
        cvs.push_back(hiSurrogate);
        cvs.push_back(loSurrogate);
    } else {
        throw Exception("attempt to encode a reserved Unicode code point");
    }
    return cvs;
}

State
Utf16CharacterEncodingForm::decode(CodeValue cv) {
    switch (state_) {
        case ERROR_STATE:
        case FINAL_STATE:
        case COMPLETED_STATE:
            return state_ = ERROR_STATE;
        case INITIAL_STATE:
            if (cv <= 0xd7ff || (cv >= 0xe000 && cv <= 0xffff)) {
                cp_ = cv;
                return state_ = FINAL_STATE;
            } else if (cv >= 0xd800 && cv <= 0xdbff) {
                cp_ = (cv - 0xd800) << 10;              // high/leasing surrogate
                return state_ = State(1);
            }
        case USER_DEFINED_1:                            // second of two 16-bit code values
            if (cv >= 0xdc00 && cv <= 0xdfff) {
                cp_ |= cv - 0xdc00;
                return state_ = FINAL_STATE;
            } else {
                return state_ = ERROR_STATE;            // expected a low/trailing surrogate
            }
        default:
            ASSERT_not_reachable("invalid decoder state");
    }
}

CodePoint
Utf16CharacterEncodingForm::consume() {
    ASSERT_require(isDone(state_));
    state_ = INITIAL_STATE;
    return cp_;
}

void
Utf16CharacterEncodingForm::reset() {
    state_ = INITIAL_STATE;
    cp_ = 0;
}

Utf16CharacterEncodingForm::Ptr
utf16CharacterEncodingForm() {
    return Utf16CharacterEncodingForm::instance();
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      BasicCharacterEncodingScheme
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::string
BasicCharacterEncodingScheme::name() const {
    std::string s = StringUtility::numberToString(octetsPerValue_) + "-byte";
    if (octetsPerValue_ > 1) {
        switch (sex_) {
            case ByteOrder::ORDER_LSB:
                s += " little-endian";
                break;
            case ByteOrder::ORDER_MSB:
                s += " big-endian";
                break;
            default:
                s += " unknown-endian";
                break;
        }
    }
    return s;
}

Octets
BasicCharacterEncodingScheme::encode(CodeValue cv) {
    Octets octets;
    for (size_t i=0; i<octetsPerValue_; ++i) {
        octets.push_back(cv & 0xff);
        cv >>= 8;
    }
    if (cv)
        throw Exception("encoding overflow: value too large to encode");
    if (octetsPerValue_>1 && sex_==ByteOrder::ORDER_MSB)
        std::reverse(octets.begin(), octets.end());
    return octets;
}

State
BasicCharacterEncodingScheme::decode(Octet octet) {
    switch (state_) {
        case ERROR_STATE:
        case FINAL_STATE:
        case COMPLETED_STATE:
            return state_ = ERROR_STATE;
        case INITIAL_STATE:
            cv_ = octet;
            return state_ = (1==octetsPerValue_ ? FINAL_STATE : State(octetsPerValue_ - 1));
        default:
            ASSERT_require(state_ >= 1 && (size_t)state_ < octetsPerValue_);
            switch (sex_) {
                case ByteOrder::ORDER_LSB:
                    cv_ |= octet << (8*(octetsPerValue_ - state_));
                    break;
                case ByteOrder::ORDER_MSB:
                    cv_ = (cv_ << 8) | octet;
                    break;
                default:
                    ASSERT_not_reachable("invalid byte order");
            }
            return state_ = (1==state_ ? FINAL_STATE : State(state_-1));
    }
}

CodeValue
BasicCharacterEncodingScheme::consume() {
    ASSERT_require(isDone(state_));
    state_ = INITIAL_STATE;
    return cv_;
}
                    
void
BasicCharacterEncodingScheme::reset() {
    state_ = INITIAL_STATE;
    cv_ = 0;
};

BasicCharacterEncodingScheme::Ptr
basicCharacterEncodingScheme(size_t octetsPerValue, ByteOrder::Endianness sex) {
    return BasicCharacterEncodingScheme::instance(octetsPerValue, sex);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      BasicLengthEncodingScheme
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::string
BasicLengthEncodingScheme::name() const {
    std::string s = StringUtility::numberToString(octetsPerValue_) + "-byte";
    if (octetsPerValue_ > 1) {
        switch (sex_) {
            case ByteOrder::ORDER_LSB:
                s += " little-endian";
                break;
            case ByteOrder::ORDER_MSB:
                s += " big-endian";
                break;
            default:
                s += " unknown-endian";
                break;
        }
    }
    return s;
}

Octets
BasicLengthEncodingScheme::encode(size_t length) {
    Octets octets;
    for (size_t i=0; i<octetsPerValue_; ++i) {
        octets.push_back(length & 0xff);
        length >>= 8;
    }
    if (length)
        throw Exception("encoding overflow: value too large to encode");
    if (octetsPerValue_>1 && sex_==ByteOrder::ORDER_MSB)
        std::reverse(octets.begin(), octets.end());
    return octets;
}

State
BasicLengthEncodingScheme::decode(Octet octet) {
    switch (state_) {
        case ERROR_STATE:
        case FINAL_STATE:
        case COMPLETED_STATE:
            return state_ = ERROR_STATE;
        case INITIAL_STATE:
            length_ = octet;
            return state_ = (1==octetsPerValue_ ? FINAL_STATE : State(octetsPerValue_-1));
        default:
            ASSERT_require(state_ >= 1 && (size_t)state_ < octetsPerValue_);
            switch (sex_) {
                case ByteOrder::ORDER_LSB:
                    length_ |= octet << (8*(octetsPerValue_ - state_));
                    break;
                case ByteOrder::ORDER_MSB:
                    length_ = (length_ << 8) | octet;
                    break;
                default:
                    ASSERT_not_reachable("invalid byte order");
            }
            return state_ = (1==state_ ? FINAL_STATE : State(state_-1));
    }
}

size_t
BasicLengthEncodingScheme::consume() {
    ASSERT_require(isDone(state_));
    state_ = INITIAL_STATE;
    return length_;
}
                    
void
BasicLengthEncodingScheme::reset() {
    state_ = INITIAL_STATE;
    length_ = 0;
};

BasicLengthEncodingScheme::Ptr
basicLengthEncodingScheme(size_t octetsPerValue, ByteOrder::Endianness sex) {
    return BasicLengthEncodingScheme::instance(octetsPerValue, sex);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      PrintableAscii
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool
PrintableAscii::isValid(CodePoint cp) {
    return cp <= 0x7f && (isprint(cp) || isspace(cp));
}

PrintableAscii::Ptr
printableAscii() {
    return PrintableAscii::instance();
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      AnyCodePoint
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

AnyCodePoint::Ptr
anyCodePoint() {
    return AnyCodePoint::instance();
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      StringEncodingScheme
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CodePoints
StringEncodingScheme::consume() {
    CodePoints retval;
    std::swap(retval, codePoints_);
    return retval;
}

void
StringEncodingScheme::reset() {
    state_ = INITIAL_STATE;
    codePoints_.clear();
    nCodePoints_ = 0;
    cef_->reset();
    ces_->reset();
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      LengthEncodedString
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::string
LengthEncodedString::name() const {
    return les_->name() + " length"
        " followed by " + cef_->name() + " code points"
        " and " + ces_->name() + " code values"
        " for " + cpp_->name();
}

Octets
LengthEncodedString::encode(const CodePoints &cps) {

    // Encode the code points
    Octets charOctets;
    BOOST_FOREACH (CodePoint cp, cps) {
        BOOST_FOREACH (CodeValue cv, cef_->encode(cp)) {
            Octets v = ces_->encode(cv);
            charOctets.insert(charOctets.end(), v.begin(), v.end());
        }
    }

    // Encode the length
    // FIXME[Robb P. Matzke 2015-05-05]: is length always measured in characters, or can it be octets?
    Octets retval = les_->encode(cps.size());
    retval.insert(retval.end(), charOctets.begin(), charOctets.end());
    return retval;
}

State
LengthEncodedString::decode(Octet octet) {
    switch (state_) {
        case ERROR_STATE:
        case FINAL_STATE:
        case COMPLETED_STATE:
            return state_ = ERROR_STATE;
        case INITIAL_STATE:
        case USER_DEFINED_1: {                          // 1 means we're decodig the length
            State st = les_->decode(octet);
            if (isDone(st)) {
                declaredLength_ = les_->consume();
                return state_ = (*declaredLength_==0 ? FINAL_STATE : State(2));
            } else if (st==ERROR_STATE) {
                return state_ = ERROR_STATE;
            } else {
                return state_ = State(1);
            }
        }
        case USER_DEFINED_2: {                          // 2 means we're decoding the characters
            State st = les_->decode(octet);
            if (isDone(st)) {
                CodeValue cv = les_->consume();
                st = cef_->decode(cv);
                if (isDone(st)) {
                    CodePoint cp = cef_->consume();
                    if (cpp_->isValid(cp)) {
                        codePoints_.push_back(cp);
                        if (++nCodePoints_ == *declaredLength_)
                            return state_ = FINAL_STATE;
                    } else {
                        return state_ = ERROR_STATE;    // not a valid character
                    }
                }
            }
            return state_ = (st==ERROR_STATE ? ERROR_STATE : State(2));
        }
        default:
            ASSERT_not_reachable("invalid decoder state");
    }
}

void
LengthEncodedString::reset() {
    StringEncodingScheme::reset();
    declaredLength_ = Sawyer::Nothing();
}

LengthEncodedString::Ptr
lengthEncodedString(const LengthEncodingScheme::Ptr &les, const CharacterEncodingForm::Ptr &cef,
                    const CharacterEncodingScheme::Ptr &ces, const CodePointPredicate::Ptr &cpp) {
    return LengthEncodedString::instance(les, cef, ces, cpp);
}

LengthEncodedString::Ptr
lengthEncodedPrintableAscii(size_t lengthSize, ByteOrder::Endianness order) {
    return lengthEncodedString(basicLengthEncodingScheme(lengthSize, order),
                               noopCharacterEncodingForm(),
                               basicCharacterEncodingScheme(1),
                               printableAscii());
}

LengthEncodedString::Ptr
lengthEncodedPrintableAsciiWide(size_t lengthSize, ByteOrder::Endianness order, size_t charSize) {
    return lengthEncodedString(basicLengthEncodingScheme(lengthSize, order),
                               noopCharacterEncodingForm(),
                               basicCharacterEncodingScheme(charSize, order),
                               printableAscii());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      TerminatedString
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::string
TerminatedString::name() const {
    std::string s;
    if (terminators_.size()==1 && terminators_.front()==0) {
        s = "zero-terminated";
    } else {
        s = "terminated";
    }
    s += " " + cef_->name() + " code points"
         " and " + ces_->name() + " code values"
         " for " + cpp_->name();
    return s;
}

Octets
TerminatedString::encode(const CodePoints &cps) {
    Octets retval;

    // Encode the characters of the string
    BOOST_FOREACH (CodePoint cp, cps) {
        BOOST_FOREACH (CodeValue cv, cef_->encode(cp)) {
            Octets v = ces_->encode(cv);
            retval.insert(retval.end(), v.begin(), v.end());
        }
    }

    // Encode the terminator
    if (!terminators_.empty()) {
        BOOST_FOREACH (CodeValue cv, cef_->encode(terminators_.front())) {
            Octets v = ces_->encode(cv);
            retval.insert(retval.end(), v.begin(), v.end());
        }
    }

    return retval;
}

State
TerminatedString::decode(Octet octet) {
    switch (state_) {
        case ERROR_STATE:
        case FINAL_STATE:
            return state_ = ERROR_STATE;
        case INITIAL_STATE:
        case COMPLETED_STATE:
        case USER_DEFINED_1: {                                       // 1 means we're decoding characters
            State st = ces_->decode(octet);
            if (isDone(st)) {
                CodeValue cv = ces_->consume();
                st = cef_->decode(cv);
                if (isDone(st)) {
                    CodePoint cp = cef_->consume();
                    if (std::find(terminators_.begin(), terminators_.end(), cp) != terminators_.end()) {
                        terminated_ = cp;
                        return state_ = FINAL_STATE;
                    } else if (cpp_->isValid(cp)) {
                        codePoints_.push_back(cp);
                        ++nCodePoints_;
                        return state_ = (terminators_.empty() ? COMPLETED_STATE : State(1));
                    } else {
                        return state_ = ERROR_STATE;    // not a valid character
                    }
                }
            }
            return state_ = (st==ERROR_STATE ? ERROR_STATE : State(1));
        }
        default:
            ASSERT_not_reachable("invalid decoder state");
    }
}

void
TerminatedString::reset() {
    StringEncodingScheme::reset();
    terminated_ = Sawyer::Nothing();
}

TerminatedString::Ptr
nulTerminatedPrintableAscii() {
    return TerminatedString::instance(noopCharacterEncodingForm(),
                                      basicCharacterEncodingScheme(1),
                                      printableAscii(),
                                      CodePoints(1, 0));
}

TerminatedString::Ptr
nulTerminatedPrintableAsciiWide(size_t charSize, ByteOrder::Endianness order) {
    return TerminatedString::instance(noopCharacterEncodingForm(),
                                      basicCharacterEncodingScheme(charSize, order),
                                      printableAscii(),
                                      CodePoints(1, 0));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      EncodedString
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::string
EncodedString::narrow() const {
    std::string s;
    BOOST_FOREACH (CodePoint cp, codePoints())
        s += char(cp);
    return s;
}

std::wstring
EncodedString::wide() const {
    std::wstring s;
    BOOST_FOREACH (CodePoint cp, codePoints())
        s += wchar_t(cp);
    return s;
}

void
EncodedString::decode(const MemoryMap &map) {
    std::vector<uint8_t> octets(where_.size());
    if (where_.size() != map.at(where_).read(octets).size())
        throw Exception("short read when decoding string");
    encoder_->reset();
    BOOST_FOREACH (uint8_t octet, octets)
        encoder_->decode(octet);
    if (!isDone(encoder_->state()))
        throw Exception("error decoding string");
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      StringFinder
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// class method
Sawyer::CommandLine::SwitchGroup
StringFinder::commandLineSwitches(Settings &settings) {
    using namespace Sawyer::CommandLine;
    SwitchGroup sg("String-finder switches");

    sg.insert(Switch("minimum-length")
              .argument("n", nonNegativeIntegerParser(settings.minLength))
              .doc("Minimum length for matched strings. The minimum length is measured in terms of code-points, which "
                   "loosely correspond to characters. Length fields and termination characters are not counted. If the "
                   "minimum is set to a value larger than the maximum then no strings can be matched. The default is " +
                   StringUtility::plural(settings.minLength, "code points") + "."));

    sg.insert(Switch("maximum-length")
              .argument("n", nonNegativeIntegerParser(settings.maxLength))
              .doc("Maximum length for matched strings. The maximum length is measured in terms of code-points, which "
                   "loosely correspond to characters. Length fields and termination characters are not counted. If the "
                   "maximum is set to a value smaller than the minimum then no strings can be matched. The default is " +
                   StringUtility::plural(settings.maxLength, "code points") + "."));

    sg.insert(Switch("maximum-overlap")
              .argument("n", nonNegativeIntegerParser(settings.maxOverlap))
              .doc("The number of strings that can overlap at a single address per encoder.  For instance, for C-style "
                   "NUL-terminated ASCII strings encoded as bytes, if memory contains the consecutive values 'a', 'n', "
                   "'i', 'm', 'a', 'l', '\0' then up to seven strings are possible: \"animal\", \"nimal\", \"imal\", "
                   "\"mal\", \"al\", \"l\", and \"\".  If the maximum overlap is set to three then only \"animal\", "
                   "\"nimal\", and \"imal\" are found. Setting the maximum overlap to zero has the same effect as setting it "
                   "to one: no overlapping is allowed.  The overlap limits are applied before results are pruned based "
                   "on length, so if the minimum legnth is four, the \"imal\" and shorter strings won't be found even "
                   "though they are decoded under the covers."
                   "\n\n"
                   "A maximum overlap of at least two is recommended if two-byte-per-character encoding is used when "
                   "detecting NUL-terminated ASCII strings. The reason is that one decoder will be active at one address "
                   "while another decoder is desired for the next address; then if the first address proves to not be part "
                   "of a string, the second address can still be detected as a string.  Similarly, a maximum overlap of at "
                   "least four is recommended for four-byte-per-character encodings.  Length-encoded strings will have "
                   "similar issues."));

    sg.insert(Switch("keep-only-longest")
              .intrinsicValue(true, settings.keepingOnlyLongest)
              .doc("Causes only the longest detected strings to be kept.  The altorighm sorts all detected strings by "
                   "decreasing length, then removes any string whose memory addresses overlap with any prior string "
                   "in the list.  The @s{keep-all} switch is the inverse. The default is " +
                   std::string(settings.keepingOnlyLongest?"@s{keep-only-longest}":"@s{keep-all}") + "."));
    sg.insert(Switch("keep-all")
              .key("keep-only-longest")
              .intrinsicValue(false, settings.keepingOnlyLongest)
              .hidden(true));

    return sg;
}

Sawyer::CommandLine::SwitchGroup
StringFinder::commandLineSwitches() {
    return commandLineSwitches(settings_);
}

StringFinder&
StringFinder::insertCommonEncoders(ByteOrder::Endianness sex) {
    encoders_.push_back(nulTerminatedPrintableAscii());
    encoders_.push_back(nulTerminatedPrintableAsciiWide(2, sex));
    encoders_.push_back(nulTerminatedPrintableAsciiWide(4, sex));

    encoders_.push_back(lengthEncodedPrintableAscii(2, sex));
    encoders_.push_back(lengthEncodedPrintableAscii(4, sex));
    encoders_.push_back(lengthEncodedPrintableAsciiWide(2, sex, 2));
    encoders_.push_back(lengthEncodedPrintableAsciiWide(2, sex, 4));
    encoders_.push_back(lengthEncodedPrintableAsciiWide(4, sex, 4));

    return *this;
}

StringFinder&
StringFinder::insertUncommonEncoders(ByteOrder::Endianness sex) {
    // This encoder finds printable ASCII that's not necessarily NUL-terminated
    TerminatedString::Ptr te = nulTerminatedPrintableAscii();
    te->terminators().clear();
    encoders_.push_back(te);
    return *this;
}

struct Finding {
    StringEncodingScheme::Ptr encoder;
    rose_addr_t startVa;
    rose_addr_t nBytes;
    Finding()
        : startVa(0), nBytes(0) {}
    Finding(const StringEncodingScheme::Ptr &enc, rose_addr_t va)
        : encoder(enc->clone()), startVa(va), nBytes(0) {
        encoder->reset();
    }
};

static bool
hasNullEncoder(const Finding &finding) {
    return finding.encoder == NULL;
}

static bool
byDecreasingLength(const EncodedString &a, const EncodedString &b) {
    return a.length() > b.length();
}

static bool
isNullString(const EncodedString &a) {
    return a.where().isEmpty();
}

class StringSearcher {
    typedef std::vector<StringEncodingScheme::Ptr> StringEncodingSchemes;
    StringEncodingSchemes protoEncoders_;
    std::vector<std::vector<Finding> > findings_;
    std::vector<Finding> results_;
    rose_addr_t bufferVa_;
    size_t minLength_, maxLength_;                      // limits on number of code points per string
    bool discardCodePoints_;                            // throw away decoded code points?
    size_t maxOverlap_;                                 // allow one encoder to match overlapping strings?
    Sawyer::Optional<rose_addr_t> anchored_;            // are strings anchored to starting address?
    Sawyer::ProgressBar<size_t> progress_;
public:
    StringSearcher(const std::vector<StringEncodingScheme::Ptr> &encoders,
                   size_t minLength, size_t maxLength, bool discardCodePoints, size_t maxOverlap,
                   size_t nBytesToCheck)
        : protoEncoders_(encoders), bufferVa_(0), minLength_(minLength), maxLength_(maxLength),
          discardCodePoints_(discardCodePoints), maxOverlap_(maxOverlap), progress_(mlog[MARCH], "scanned bytes") {
        findings_.resize(encoders.size());
        progress_.value(0, nBytesToCheck);
        progress_.suffix(" addresses");
    }

    // anchor the search to a particular address
    void anchor(rose_addr_t startVa) { anchored_ = startVa; }

    // obtain the final results
    const std::vector<Finding>& results() const { return results_; }

    // search for strings
    bool operator()(const MemoryMap::Super &map, const AddressInterval &interval) {
        if (interval.least() > bufferVa_) {
            // We skipped across some unmapped memory, so reap all decoders, saving strings for those decoders that are in a
            // COMPLETED_STATE.
            for (size_t i=0; i<findings_.size(); ++i) {
                for (size_t j=0; j<findings_[i].size(); ++j) {
                    if (findings_[i][j].encoder->state() == COMPLETED_STATE &&
                        findings_[i][j].encoder->length() >= minLength_ &&
                        findings_[i][j].encoder->length() <= maxLength_) {
                        results_.push_back(findings_[i][j]);
                    }
                }
                findings_[i].clear();
            }
        }

        std::vector<uint8_t> buffer(4096);              // arbitrary
        rose_addr_t bufferVa = interval.least();
        while (1) {
            size_t nread = map.at(bufferVa).atOrBefore(interval.greatest()).read(buffer).size();
            progress_.value(progress_.value()+nread);
            ASSERT_require(nread > 0);
            for (size_t offset=0; offset<nread; ++offset) {

                // Create new encoders starting at this address. It the string searching is configured so as to find only those
                // strings that start at a particular address, then terminate the search early once all those strings are done
                // being parsed.
                if (!anchored_ || *anchored_==bufferVa+offset) {
                    for (size_t i=0; i<findings_.size(); ++i) {
                        if (findings_[i].size() < maxOverlap_)
                            findings_[i].push_back(Finding(protoEncoders_[i], bufferVa + offset));
                    }
                } else {
                    bool haveDecoders = false;
                    for (size_t i=0; i<findings_.size(); ++i) {
                        if (!findings_[i].empty()) {
                            haveDecoders = true;
                            break;
                        }
                    }
                    if (!haveDecoders)
                        return false;
                }

                // Decode this next octet, removing decoders that encounter errors, and saving those which enter their final
                // state. If a decoder enters the complete (but not final) state then save the string as it exists at that
                // point, but do not remove the decoder.
                Octet octet = buffer[offset];
                for (size_t i=0; i<findings_.size(); ++i) {
                    for (size_t j=0; j<findings_[i].size(); ++j) {
                        State st = findings_[i][j].encoder->decode(octet);
                        ++findings_[i][j].nBytes;
                        if (discardCodePoints_)
                            findings_[i][j].encoder->consume();
                        if (ERROR_STATE == st || findings_[i][j].encoder->length() > maxLength_) {
                            findings_[i][j].encoder = StringEncodingScheme::Ptr();
                        } else if (FINAL_STATE == st) {
                            if (findings_[i][j].encoder->length() >= minLength_ &&
                                findings_[i][j].encoder->length() <= maxLength_) {
                                results_.push_back(findings_[i][j]);
                            }
                            findings_[i][j].encoder = StringEncodingScheme::Ptr();
                        } else if (COMPLETED_STATE == st &&
                                   findings_[i][j].encoder->length() >= minLength_ &&
                                   findings_[i][j].encoder->length() <= maxLength_) {
                            Finding fcopy = findings_[i][j];
                            fcopy.encoder = fcopy.encoder->clone();
                            results_.push_back(fcopy);
                        }
                    }
                    findings_[i].erase(std::remove_if(findings_[i].begin(), findings_[i].end(), hasNullEncoder),
                                       findings_[i].end());
                }
            }
            if (bufferVa + (nread-1) == interval.greatest())
                break;                                  // prevent possible overflow
            bufferVa += nread;
            ASSERT_forbid(bufferVa > interval.greatest());
        }
        return true;                                    // keep going
    }
};

StringFinder&
StringFinder::find(const MemoryMap::ConstConstraints &constraints, Sawyer::Container::MatchFlags flags) {
    strings_.clear();
    if (settings_.minLength > settings_.maxLength || encoders_.empty())
        return *this;

    size_t nBytesToCheck = 0;
    BOOST_FOREACH (const MemoryMap::Node &node, constraints.nodes(Sawyer::Container::MATCH_NONCONTIGUOUS))
        nBytesToCheck += node.key().size();

    StringSearcher stringFinder(encoders_, settings_.minLength, settings_.maxLength, discardingCodePoints_,
                                settings_.maxOverlap, nBytesToCheck);
    if (constraints.isAnchored())
        stringFinder.anchor(constraints.anchored().least());
    constraints.traverse(stringFinder, flags);

    BOOST_FOREACH (const Finding &finding, stringFinder.results())
        strings_.push_back(EncodedString(finding.encoder, AddressInterval::baseSize(finding.startVa, finding.nBytes)));

    if (settings_.keepingOnlyLongest) {
        AddressIntervalSet stringAddresses;
        std::sort(strings_.begin(), strings_.end(), byDecreasingLength);
        BOOST_FOREACH (EncodedString &string, strings_) {
            if (stringAddresses.isOverlapping(string.where())) {
                string = EncodedString();               // mark for erasing
            } else {
                stringAddresses.insert(string.where());
            }
        }
        strings_.erase(std::remove_if(strings_.begin(), strings_.end(), isNullString), strings_.end());
    }
    
    return *this;
}

std::ostream&
StringFinder::print(std::ostream &out) const {
    BOOST_FOREACH (const EncodedString &string, strings_) {
        out <<StringUtility::addrToString(string.address())
            <<" " <<string.encoder()->name()
            <<" \"" <<StringUtility::cEscape(string.narrow()) <<"\"\n";
    }
    return out;
}

std::ostream&
operator<<(std::ostream &out, const StringFinder &x) {
    return x.print(out);
}

} // namespace
} // namespace
} // namespace

#endif
