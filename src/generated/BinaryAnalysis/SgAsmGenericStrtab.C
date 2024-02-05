//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmGenericStrtab            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#include <sage3basic.h>

SgAsmGenericSection* const&
SgAsmGenericStrtab::get_container() const {
    return p_container;
}

void
SgAsmGenericStrtab::set_container(SgAsmGenericSection* const& x) {
    this->p_container = x;
    set_isModified(true);
}

SgAsmGenericStrtab::referenced_t const&
SgAsmGenericStrtab::get_storageList() const {
    return p_storageList;
}

void
SgAsmGenericStrtab::set_storageList(SgAsmGenericStrtab::referenced_t const& x) {
    this->p_storageList = x;
    set_isModified(true);
}

Rose::BinaryAnalysis::AddressIntervalSet const&
SgAsmGenericStrtab::get_freeList() const {
    return p_freeList;
}

Rose::BinaryAnalysis::AddressIntervalSet&
SgAsmGenericStrtab::get_freeList() {
    return p_freeList;
}

SgAsmStringStorage* const&
SgAsmGenericStrtab::get_dontFree() const {
    return p_dontFree;
}

void
SgAsmGenericStrtab::set_dontFree(SgAsmStringStorage* const& x) {
    this->p_dontFree = x;
    set_isModified(true);
}

size_t const&
SgAsmGenericStrtab::get_numberFreed() const {
    return p_numberFreed;
}

void
SgAsmGenericStrtab::set_numberFreed(size_t const& x) {
    this->p_numberFreed = x;
    set_isModified(true);
}

SgAsmGenericStrtab::~SgAsmGenericStrtab() {
    destructorHelper();
}

SgAsmGenericStrtab::SgAsmGenericStrtab()
    : p_container(nullptr)
    , p_dontFree(nullptr)
    , p_numberFreed(0) {}

void
SgAsmGenericStrtab::initializeProperties() {
    p_container = nullptr;
    p_dontFree = nullptr;
    p_numberFreed = 0;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
