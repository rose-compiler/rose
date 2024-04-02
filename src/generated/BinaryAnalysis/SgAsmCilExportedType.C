//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCilExportedType            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmCilExportedType_IMPL
#include <SgAsmCilExportedType.h>

uint32_t const&
SgAsmCilExportedType::get_EventFlags() const {
    return p_EventFlags;
}

void
SgAsmCilExportedType::set_EventFlags(uint32_t const& x) {
    this->p_EventFlags = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilExportedType::get_TypeDefIdName() const {
    return p_TypeDefIdName;
}

void
SgAsmCilExportedType::set_TypeDefIdName(uint32_t const& x) {
    this->p_TypeDefIdName = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilExportedType::get_TypeName() const {
    return p_TypeName;
}

void
SgAsmCilExportedType::set_TypeName(uint32_t const& x) {
    this->p_TypeName = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilExportedType::get_TypeNamespace() const {
    return p_TypeNamespace;
}

void
SgAsmCilExportedType::set_TypeNamespace(uint32_t const& x) {
    this->p_TypeNamespace = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilExportedType::get_Implementation() const {
    return p_Implementation;
}

void
SgAsmCilExportedType::set_Implementation(uint32_t const& x) {
    this->p_Implementation = x;
    set_isModified(true);
}

SgAsmCilExportedType::~SgAsmCilExportedType() {
    destructorHelper();
}

SgAsmCilExportedType::SgAsmCilExportedType()
    : p_EventFlags(0)
    , p_TypeDefIdName(0)
    , p_TypeName(0)
    , p_TypeNamespace(0)
    , p_Implementation(0) {}

void
SgAsmCilExportedType::initializeProperties() {
    p_EventFlags = 0;
    p_TypeDefIdName = 0;
    p_TypeName = 0;
    p_TypeNamespace = 0;
    p_Implementation = 0;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
