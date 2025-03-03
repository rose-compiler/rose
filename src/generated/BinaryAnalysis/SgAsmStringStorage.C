//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmStringStorage            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmStringStorage_IMPL
#include <SgAsmStringStorage.h>

SgAsmGenericStrtab* const&
SgAsmStringStorage::get_strtab() const {
    return p_strtab;
}

void
SgAsmStringStorage::set_strtab(SgAsmGenericStrtab* const& x) {
    this->p_strtab = x;
    set_isModified(true);
}

std::string const&
SgAsmStringStorage::get_string() const {
    return p_string;
}

void
SgAsmStringStorage::set_string(std::string const& x) {
    this->p_string = x;
    set_isModified(true);
}

Rose::BinaryAnalysis::Address const&
SgAsmStringStorage::get_offset() const {
    return p_offset;
}

void
SgAsmStringStorage::set_offset(Rose::BinaryAnalysis::Address const& x) {
    this->p_offset = x;
    set_isModified(true);
}

SgAsmStringStorage::~SgAsmStringStorage() {
    destructorHelper();
}

SgAsmStringStorage::SgAsmStringStorage()
    : p_strtab(nullptr)
    , p_offset(0) {}

void
SgAsmStringStorage::initializeProperties() {
    p_strtab = nullptr;
    p_offset = 0;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
