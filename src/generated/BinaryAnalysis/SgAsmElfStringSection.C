//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfStringSection            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmElfStringSection_IMPL
#include <sage3basic.h>

SgAsmElfStrtab* const&
SgAsmElfStringSection::get_strtab() const {
    return p_strtab;
}

void
SgAsmElfStringSection::set_strtab(SgAsmElfStrtab* const& x) {
    this->p_strtab = x;
    set_isModified(true);
}

SgAsmElfStringSection::~SgAsmElfStringSection() {
    destructorHelper();
}

SgAsmElfStringSection::SgAsmElfStringSection()
    : p_strtab(nullptr) {}

void
SgAsmElfStringSection::initializeProperties() {
    p_strtab = nullptr;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
