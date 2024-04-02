//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCilManifestResource            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmCilManifestResource_IMPL
#include <SgAsmCilManifestResource.h>

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

SgAsmCilManifestResource::SgAsmCilManifestResource()
    : p_Offset(0)
    , p_Flags(0)
    , p_Name(0)
    , p_Implementation(0) {}

void
SgAsmCilManifestResource::initializeProperties() {
    p_Offset = 0;
    p_Flags = 0;
    p_Name = 0;
    p_Implementation = 0;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
