//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCilFile            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmCilFile_IMPL
#include <SgAsmCilFile.h>

uint32_t const&
SgAsmCilFile::get_Flags() const {
    return p_Flags;
}

void
SgAsmCilFile::set_Flags(uint32_t const& x) {
    this->p_Flags = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilFile::get_Name() const {
    return p_Name;
}

void
SgAsmCilFile::set_Name(uint32_t const& x) {
    this->p_Name = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilFile::get_HashValue() const {
    return p_HashValue;
}

void
SgAsmCilFile::set_HashValue(uint32_t const& x) {
    this->p_HashValue = x;
    set_isModified(true);
}

SgAsmCilFile::~SgAsmCilFile() {
    destructorHelper();
}

SgAsmCilFile::SgAsmCilFile()
    : p_Flags(0)
    , p_Name(0)
    , p_HashValue(0) {}

void
SgAsmCilFile::initializeProperties() {
    p_Flags = 0;
    p_Name = 0;
    p_HashValue = 0;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
