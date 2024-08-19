//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCilExceptionData            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmCilExceptionData_IMPL
#include <SgAsmCilExceptionData.h>

std::uint32_t const&
SgAsmCilExceptionData::get_flags() const {
    return p_flags;
}

void
SgAsmCilExceptionData::set_flags(std::uint32_t const& x) {
    this->p_flags = x;
    set_isModified(true);
}

std::uint32_t const&
SgAsmCilExceptionData::get_tryOffset() const {
    return p_tryOffset;
}

void
SgAsmCilExceptionData::set_tryOffset(std::uint32_t const& x) {
    this->p_tryOffset = x;
    set_isModified(true);
}

std::uint32_t const&
SgAsmCilExceptionData::get_tryLength() const {
    return p_tryLength;
}

void
SgAsmCilExceptionData::set_tryLength(std::uint32_t const& x) {
    this->p_tryLength = x;
    set_isModified(true);
}

std::uint32_t const&
SgAsmCilExceptionData::get_handlerOffset() const {
    return p_handlerOffset;
}

void
SgAsmCilExceptionData::set_handlerOffset(std::uint32_t const& x) {
    this->p_handlerOffset = x;
    set_isModified(true);
}

std::uint32_t const&
SgAsmCilExceptionData::get_handlerLength() const {
    return p_handlerLength;
}

void
SgAsmCilExceptionData::set_handlerLength(std::uint32_t const& x) {
    this->p_handlerLength = x;
    set_isModified(true);
}

std::uint32_t const&
SgAsmCilExceptionData::get_classTokenOrFilterOffset() const {
    return p_classTokenOrFilterOffset;
}

void
SgAsmCilExceptionData::set_classTokenOrFilterOffset(std::uint32_t const& x) {
    this->p_classTokenOrFilterOffset = x;
    set_isModified(true);
}

SgAsmCilExceptionData::~SgAsmCilExceptionData() {
    destructorHelper();
}

SgAsmCilExceptionData::SgAsmCilExceptionData()
    : p_flags(0)
    , p_tryOffset(0)
    , p_tryLength(0)
    , p_handlerOffset(0)
    , p_handlerLength(0)
    , p_classTokenOrFilterOffset(0) {}

void
SgAsmCilExceptionData::initializeProperties() {
    p_flags = 0;
    p_tryOffset = 0;
    p_tryLength = 0;
    p_handlerOffset = 0;
    p_handlerLength = 0;
    p_classTokenOrFilterOffset = 0;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
