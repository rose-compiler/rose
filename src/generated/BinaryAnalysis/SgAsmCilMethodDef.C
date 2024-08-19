//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCilMethodDef            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmCilMethodDef_IMPL
#include <SgAsmCilMethodDef.h>

uint32_t const&
SgAsmCilMethodDef::get_RVA() const {
    return p_RVA;
}

void
SgAsmCilMethodDef::set_RVA(uint32_t const& x) {
    this->p_RVA = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilMethodDef::get_ImplFlags() const {
    return p_ImplFlags;
}

void
SgAsmCilMethodDef::set_ImplFlags(uint32_t const& x) {
    this->p_ImplFlags = x;
    set_isModified(true);
}

uint16_t const&
SgAsmCilMethodDef::get_Flags() const {
    return p_Flags;
}

void
SgAsmCilMethodDef::set_Flags(uint16_t const& x) {
    this->p_Flags = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilMethodDef::get_Name() const {
    return p_Name;
}

void
SgAsmCilMethodDef::set_Name(uint32_t const& x) {
    this->p_Name = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilMethodDef::get_Signature() const {
    return p_Signature;
}

void
SgAsmCilMethodDef::set_Signature(uint32_t const& x) {
    this->p_Signature = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilMethodDef::get_ParamList() const {
    return p_ParamList;
}

void
SgAsmCilMethodDef::set_ParamList(uint32_t const& x) {
    this->p_ParamList = x;
    set_isModified(true);
}

SgAsmBlock* const&
SgAsmCilMethodDef::get_body() const {
    return p_body;
}

void
SgAsmCilMethodDef::set_body(SgAsmBlock* const& x) {
    this->p_body = x;
    set_isModified(true);
}

bool const&
SgAsmCilMethodDef::get_initLocals() const {
    return p_initLocals;
}

void
SgAsmCilMethodDef::set_initLocals(bool const& x) {
    this->p_initLocals = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilMethodDef::get_stackSize() const {
    return p_stackSize;
}

void
SgAsmCilMethodDef::set_stackSize(uint32_t const& x) {
    this->p_stackSize = x;
    set_isModified(true);
}

bool const&
SgAsmCilMethodDef::get_hasMoreSections() const {
    return p_hasMoreSections;
}

void
SgAsmCilMethodDef::set_hasMoreSections(bool const& x) {
    this->p_hasMoreSections = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilMethodDef::get_localVarSigTok() const {
    return p_localVarSigTok;
}

void
SgAsmCilMethodDef::set_localVarSigTok(uint32_t const& x) {
    this->p_localVarSigTok = x;
    set_isModified(true);
}

std::vector<SgAsmCilMethodData*> const&
SgAsmCilMethodDef::get_methodData() const {
    return p_methodData;
}

std::vector<SgAsmCilMethodData*>&
SgAsmCilMethodDef::get_methodData() {
    return p_methodData;
}

SgAsmCilMethodDef::~SgAsmCilMethodDef() {
    destructorHelper();
}

SgAsmCilMethodDef::SgAsmCilMethodDef()
    : p_RVA(0)
    , p_ImplFlags(0)
    , p_Flags(0)
    , p_Name(0)
    , p_Signature(0)
    , p_ParamList(0)
    , p_body(0)
    , p_initLocals(0)
    , p_stackSize(0)
    , p_hasMoreSections(0)
    , p_localVarSigTok(0) {}

void
SgAsmCilMethodDef::initializeProperties() {
    p_RVA = 0;
    p_ImplFlags = 0;
    p_Flags = 0;
    p_Name = 0;
    p_Signature = 0;
    p_ParamList = 0;
    p_body = 0;
    p_initLocals = 0;
    p_stackSize = 0;
    p_hasMoreSections = 0;
    p_localVarSigTok = 0;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
