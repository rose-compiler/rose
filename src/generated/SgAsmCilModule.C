//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCilModule            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

uint16_t const&
SgAsmCilModule::get_Generation() const {
    return p_Generation;
}

void
SgAsmCilModule::set_Generation(uint16_t const& x) {
    this->p_Generation = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilModule::get_Name() const {
    return p_Name;
}

void
SgAsmCilModule::set_Name(uint32_t const& x) {
    this->p_Name = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilModule::get_Mvid() const {
    return p_Mvid;
}

void
SgAsmCilModule::set_Mvid(uint32_t const& x) {
    this->p_Mvid = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilModule::get_Encld() const {
    return p_Encld;
}

void
SgAsmCilModule::set_Encld(uint32_t const& x) {
    this->p_Encld = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilModule::get_EncBaseId() const {
    return p_EncBaseId;
}

void
SgAsmCilModule::set_EncBaseId(uint32_t const& x) {
    this->p_EncBaseId = x;
    set_isModified(true);
}

SgAsmCilModule::~SgAsmCilModule() {
    destructorHelper();
}

SgAsmCilModule::SgAsmCilModule() {}

void
SgAsmCilModule::initializeProperties() {
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
