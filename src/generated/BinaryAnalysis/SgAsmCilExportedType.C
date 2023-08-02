//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCilExportedType            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#include <sage3basic.h>

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
    : p_EventFlags({})
    , p_TypeDefIdName({})
    , p_TypeName({})
    , p_TypeNamespace({})
    , p_Implementation({}) {}

void
SgAsmCilExportedType::initializeProperties() {
    p_EventFlags = {};
    p_TypeDefIdName = {};
    p_TypeName = {};
    p_TypeNamespace = {};
    p_Implementation = {};
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
