//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmGenericFormat            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmGenericFormat_IMPL
#include <SgAsmGenericFormat.h>

SgAsmGenericFormat::ExecFamily const&
SgAsmGenericFormat::get_family() const {
    return p_family;
}

void
SgAsmGenericFormat::set_family(SgAsmGenericFormat::ExecFamily const& x) {
    this->p_family = x;
    set_isModified(true);
}

SgAsmGenericFormat::ExecPurpose const&
SgAsmGenericFormat::get_purpose() const {
    return p_purpose;
}

void
SgAsmGenericFormat::set_purpose(SgAsmGenericFormat::ExecPurpose const& x) {
    this->p_purpose = x;
    set_isModified(true);
}

Rose::BinaryAnalysis::ByteOrder::Endianness const&
SgAsmGenericFormat::get_sex() const {
    return p_sex;
}

void
SgAsmGenericFormat::set_sex(Rose::BinaryAnalysis::ByteOrder::Endianness const& x) {
    this->p_sex = x;
    set_isModified(true);
}

unsigned const&
SgAsmGenericFormat::get_version() const {
    return p_version;
}

void
SgAsmGenericFormat::set_version(unsigned const& x) {
    this->p_version = x;
    set_isModified(true);
}

bool const&
SgAsmGenericFormat::get_isCurrentVersion() const {
    return p_isCurrentVersion;
}

void
SgAsmGenericFormat::set_isCurrentVersion(bool const& x) {
    this->p_isCurrentVersion = x;
    set_isModified(true);
}

SgAsmGenericFormat::ExecABI const&
SgAsmGenericFormat::get_abi() const {
    return p_abi;
}

void
SgAsmGenericFormat::set_abi(SgAsmGenericFormat::ExecABI const& x) {
    this->p_abi = x;
    set_isModified(true);
}

unsigned const&
SgAsmGenericFormat::get_abiVersion() const {
    return p_abiVersion;
}

void
SgAsmGenericFormat::set_abiVersion(unsigned const& x) {
    this->p_abiVersion = x;
    set_isModified(true);
}

size_t const&
SgAsmGenericFormat::get_wordSize() const {
    return p_wordSize;
}

void
SgAsmGenericFormat::set_wordSize(size_t const& x) {
    this->p_wordSize = x;
    set_isModified(true);
}

SgAsmGenericFormat::~SgAsmGenericFormat() {
    destructorHelper();
}

SgAsmGenericFormat::SgAsmGenericFormat()
    : p_family(SgAsmGenericFormat::FAMILY_UNSPECIFIED)
    , p_purpose(SgAsmGenericFormat::PURPOSE_EXECUTABLE)
    , p_sex(Rose::BinaryAnalysis::ByteOrder::ORDER_UNSPECIFIED)
    , p_version(0)
    , p_isCurrentVersion(false)
    , p_abi(SgAsmGenericFormat::ABI_UNSPECIFIED)
    , p_abiVersion(0)
    , p_wordSize(0) {}

void
SgAsmGenericFormat::initializeProperties() {
    p_family = SgAsmGenericFormat::FAMILY_UNSPECIFIED;
    p_purpose = SgAsmGenericFormat::PURPOSE_EXECUTABLE;
    p_sex = Rose::BinaryAnalysis::ByteOrder::ORDER_UNSPECIFIED;
    p_version = 0;
    p_isCurrentVersion = false;
    p_abi = SgAsmGenericFormat::ABI_UNSPECIFIED;
    p_abiVersion = 0;
    p_wordSize = 0;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
