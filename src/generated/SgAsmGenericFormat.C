#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmGenericFormat            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericFormat::ExecFamily const&
SgAsmGenericFormat::get_family() const {
    return p_family;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmGenericFormat::set_family(SgAsmGenericFormat::ExecFamily const& x) {
    this->p_family = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericFormat::ExecPurpose const&
SgAsmGenericFormat::get_purpose() const {
    return p_purpose;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmGenericFormat::set_purpose(SgAsmGenericFormat::ExecPurpose const& x) {
    this->p_purpose = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
ByteOrder::Endianness const&
SgAsmGenericFormat::get_sex() const {
    return p_sex;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmGenericFormat::set_sex(ByteOrder::Endianness const& x) {
    this->p_sex = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
unsigned const&
SgAsmGenericFormat::get_version() const {
    return p_version;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmGenericFormat::set_version(unsigned const& x) {
    this->p_version = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
bool const&
SgAsmGenericFormat::get_is_current_version() const {
    return p_is_current_version;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmGenericFormat::set_is_current_version(bool const& x) {
    this->p_is_current_version = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericFormat::ExecABI const&
SgAsmGenericFormat::get_abi() const {
    return p_abi;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmGenericFormat::set_abi(SgAsmGenericFormat::ExecABI const& x) {
    this->p_abi = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
unsigned const&
SgAsmGenericFormat::get_abi_version() const {
    return p_abi_version;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmGenericFormat::set_abi_version(unsigned const& x) {
    this->p_abi_version = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
size_t const&
SgAsmGenericFormat::get_word_size() const {
    return p_word_size;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmGenericFormat::set_word_size(size_t const& x) {
    this->p_word_size = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericFormat::~SgAsmGenericFormat() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericFormat::SgAsmGenericFormat()
#line 308 "src/Rosebud/RosettaGenerator.C"
    : p_family(SgAsmGenericFormat::FAMILY_UNSPECIFIED)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_purpose(SgAsmGenericFormat::PURPOSE_EXECUTABLE)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_sex(ByteOrder::ORDER_UNSPECIFIED)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_version(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_is_current_version(false)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_abi(SgAsmGenericFormat::ABI_UNSPECIFIED)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_abi_version(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_word_size(0) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
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

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
