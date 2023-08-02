//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCilTypeDef            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#include <sage3basic.h>

uint32_t const&
SgAsmCilTypeDef::get_Flags() const {
    return p_Flags;
}

void
SgAsmCilTypeDef::set_Flags(uint32_t const& x) {
    this->p_Flags = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilTypeDef::get_TypeName() const {
    return p_TypeName;
}

void
SgAsmCilTypeDef::set_TypeName(uint32_t const& x) {
    this->p_TypeName = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilTypeDef::get_TypeNamespace() const {
    return p_TypeNamespace;
}

void
SgAsmCilTypeDef::set_TypeNamespace(uint32_t const& x) {
    this->p_TypeNamespace = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilTypeDef::get_Extends() const {
    return p_Extends;
}

void
SgAsmCilTypeDef::set_Extends(uint32_t const& x) {
    this->p_Extends = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilTypeDef::get_FieldList() const {
    return p_FieldList;
}

void
SgAsmCilTypeDef::set_FieldList(uint32_t const& x) {
    this->p_FieldList = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilTypeDef::get_MethodList() const {
    return p_MethodList;
}

void
SgAsmCilTypeDef::set_MethodList(uint32_t const& x) {
    this->p_MethodList = x;
    set_isModified(true);
}

SgAsmCilTypeDef::~SgAsmCilTypeDef() {
    destructorHelper();
}

SgAsmCilTypeDef::SgAsmCilTypeDef()
    : p_Flags({})
    , p_TypeName({})
    , p_TypeNamespace({})
    , p_Extends({})
    , p_FieldList({})
    , p_MethodList({}) {}

void
SgAsmCilTypeDef::initializeProperties() {
    p_Flags = {};
    p_TypeName = {};
    p_TypeNamespace = {};
    p_Extends = {};
    p_FieldList = {};
    p_MethodList = {};
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
