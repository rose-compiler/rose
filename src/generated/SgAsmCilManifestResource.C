//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCilManifestResource            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

uint32_t const&
SgAsmCilManifestResource::get_Offset() const {
    return p_Offset;
}

void
SgAsmCilManifestResource::set_Offset(uint32_t const& x) {
    this->p_Offset = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilManifestResource::get_Flags() const {
    return p_Flags;
}

void
SgAsmCilManifestResource::set_Flags(uint32_t const& x) {
    this->p_Flags = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilManifestResource::get_Name() const {
    return p_Name;
}

void
SgAsmCilManifestResource::set_Name(uint32_t const& x) {
    this->p_Name = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilManifestResource::get_Implementation() const {
    return p_Implementation;
}

void
SgAsmCilManifestResource::set_Implementation(uint32_t const& x) {
    this->p_Implementation = x;
    set_isModified(true);
}

SgAsmCilManifestResource::~SgAsmCilManifestResource() {
    destructorHelper();
}

SgAsmCilManifestResource::SgAsmCilManifestResource() {}

void
SgAsmCilManifestResource::initializeProperties() {
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
