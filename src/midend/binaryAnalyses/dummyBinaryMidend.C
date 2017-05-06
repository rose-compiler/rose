#include "sage3basic.h"

#include "BinaryString.h"

namespace rose {
namespace BinaryAnalysis {


namespace Strings {
    void initDiagnostics() {}

bool
isDone(State st) {
    return st == FINAL_STATE || st == COMPLETED_STATE;
}

CodeValues
NoopCharacterEncodingForm::encode(CodePoint cp) {
    return CodeValues(1, cp);
}

State
NoopCharacterEncodingForm::decode(CodeValue cv) {
    return state_ = FINAL_STATE;
}

CodePoint
NoopCharacterEncodingForm::consume() {
    return cp_;
}

void
NoopCharacterEncodingForm::reset() {
}

CodeValues
Utf8CharacterEncodingForm::encode(CodePoint cp) {
    CodeValues cvs;
    return cvs;
}

State
Utf8CharacterEncodingForm::decode(CodeValue cv) {
    return state_ = FINAL_STATE;
}

CodePoint
Utf8CharacterEncodingForm::consume() {
    return cp_;
}

void
Utf8CharacterEncodingForm::reset() {
}
CodeValues
Utf16CharacterEncodingForm::encode(CodePoint cp) {
    CodeValues cvs;
    return cvs;
}

State
Utf16CharacterEncodingForm::decode(CodeValue cv) {
    return state_ = FINAL_STATE;
}

CodePoint
Utf16CharacterEncodingForm::consume() {
    return cp_;
}

void
Utf16CharacterEncodingForm::reset() {
}
std::string
BasicCharacterEncodingScheme::name() const {
    std::string s = ""; 
    return s;
}

Octets
BasicCharacterEncodingScheme::encode(CodeValue cv) {
    Octets octets;
    return octets;
}

State
BasicCharacterEncodingScheme::decode(Octet octet) {
    return state_ = FINAL_STATE;
}

CodeValue
BasicCharacterEncodingScheme::consume() {
    return cv_;
}
                    
void
BasicCharacterEncodingScheme::reset() {
};

std::string
BasicLengthEncodingScheme::name() const {
    std::string s = ""; 
    return s;
}

Octets
BasicLengthEncodingScheme::encode(size_t length) {
    Octets octets;
    return octets;
}

State
BasicLengthEncodingScheme::decode(Octet octet) {
    return state_ = FINAL_STATE;
}

size_t
BasicLengthEncodingScheme::consume() {
    return length_;
}
                    
void
BasicLengthEncodingScheme::reset() {
};

bool
PrintableAscii::isValid(CodePoint cp) {
    return cp <= 0x7f && (isprint(cp) || isspace(cp));
}

PrintableAscii::Ptr
printableAscii() {
    return PrintableAscii::instance();
}

CodePoints
StringEncodingScheme::consume() {
    CodePoints retval;
    return retval;
}

void
StringEncodingScheme::reset() {
}

std::string
LengthEncodedString::name() const {
    return " ";
}

Octets
LengthEncodedString::encode(const CodePoints &cps) {

    // Encode the code points
    Octets retval;
    return retval;
}

State
LengthEncodedString::decode(Octet octet) {
    return state_ = FINAL_STATE;
}

void
LengthEncodedString::reset() {
}

std::string
TerminatedString::name() const {
    std::string s="";
    return s;
}

Octets
TerminatedString::encode(const CodePoints &cps) {
    Octets retval;
    return retval;
}

State
TerminatedString::decode(Octet octet) {
    return state_ = FINAL_STATE;
}

void
TerminatedString::reset() {
}

} // namespace
} // namespace
} // namespace
