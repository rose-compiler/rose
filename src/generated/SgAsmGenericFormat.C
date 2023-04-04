//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmGenericFormat            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

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

ByteOrder::Endianness const&
SgAsmGenericFormat::get_sex() const {
    return p_sex;
}

void
SgAsmGenericFormat::set_sex(ByteOrder::Endianness const& x) {
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
SgAsmGenericFormat::get_is_current_version() const {
    return p_is_current_version;
}

void
SgAsmGenericFormat::set_is_current_version(bool const& x) {
    this->p_is_current_version = x;
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
SgAsmGenericFormat::get_abi_version() const {
    return p_abi_version;
}

void
SgAsmGenericFormat::set_abi_version(unsigned const& x) {
    this->p_abi_version = x;
    set_isModified(true);
}

size_t const&
SgAsmGenericFormat::get_word_size() const {
    return p_word_size;
}

void
SgAsmGenericFormat::set_word_size(size_t const& x) {
    this->p_word_size = x;
    set_isModified(true);
}

SgAsmGenericFormat::~SgAsmGenericFormat() {
    destructorHelper();
}

SgAsmGenericFormat::SgAsmGenericFormat()
    : p_family(SgAsmGenericFormat::FAMILY_UNSPECIFIED)
    , p_purpose(SgAsmGenericFormat::PURPOSE_EXECUTABLE)
    , p_sex(ByteOrder::ORDER_UNSPECIFIED)
    , p_version(0)
    , p_is_current_version(false)
    , p_abi(SgAsmGenericFormat::ABI_UNSPECIFIED)
    , p_abi_version(0)
    , p_word_size(0) {}

void
SgAsmGenericFormat::initializeProperties() {
    p_family = SgAsmGenericFormat::FAMILY_UNSPECIFIED;
    p_purpose = SgAsmGenericFormat::PURPOSE_EXECUTABLE;
    p_sex = ByteOrder::ORDER_UNSPECIFIED;
    p_version = 0;
    p_is_current_version = false;
    p_abi = SgAsmGenericFormat::ABI_UNSPECIFIED;
    p_abi_version = 0;
    p_word_size = 0;
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
