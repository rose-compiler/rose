//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmPEStringSection            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmPEStringSection_IMPL
#include <SgAsmPEStringSection.h>

SgAsmCoffStrtab* const&
SgAsmPEStringSection::get_strtab() const {
    return p_strtab;
}

void
SgAsmPEStringSection::set_strtab(SgAsmCoffStrtab* const& x) {
    this->p_strtab = x;
    set_isModified(true);
}

SgAsmPEStringSection::~SgAsmPEStringSection() {
    destructorHelper();
}

SgAsmPEStringSection::SgAsmPEStringSection()
    : p_strtab(nullptr) {}

void
SgAsmPEStringSection::initializeProperties() {
    p_strtab = nullptr;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
