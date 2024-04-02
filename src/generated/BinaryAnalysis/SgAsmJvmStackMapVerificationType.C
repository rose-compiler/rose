//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmStackMapVerificationType            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmJvmStackMapVerificationType_IMPL
#include <SgAsmJvmStackMapVerificationType.h>

uint8_t const&
SgAsmJvmStackMapVerificationType::get_tag() const {
    return p_tag;
}

void
SgAsmJvmStackMapVerificationType::set_tag(uint8_t const& x) {
    this->p_tag = x;
    set_isModified(true);
}

uint16_t const&
SgAsmJvmStackMapVerificationType::get_cpool_index() const {
    return p_cpool_index;
}

void
SgAsmJvmStackMapVerificationType::set_cpool_index(uint16_t const& x) {
    this->p_cpool_index = x;
    set_isModified(true);
}

uint16_t const&
SgAsmJvmStackMapVerificationType::get_offset() const {
    return p_offset;
}

void
SgAsmJvmStackMapVerificationType::set_offset(uint16_t const& x) {
    this->p_offset = x;
    set_isModified(true);
}

SgAsmJvmStackMapVerificationType::~SgAsmJvmStackMapVerificationType() {
    destructorHelper();
}

SgAsmJvmStackMapVerificationType::SgAsmJvmStackMapVerificationType()
    : p_tag(9)
    , p_cpool_index(0)
    , p_offset(0) {}

void
SgAsmJvmStackMapVerificationType::initializeProperties() {
    p_tag = 9;
    p_cpool_index = 0;
    p_offset = 0;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
