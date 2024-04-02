//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmNEModuleTable            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmNEModuleTable_IMPL
#include <sage3basic.h>

SgAsmNEStringTable* const&
SgAsmNEModuleTable::get_strtab() const {
    return p_strtab;
}

void
SgAsmNEModuleTable::set_strtab(SgAsmNEStringTable* const& x) {
    changeChildPointer(this->p_strtab, const_cast<SgAsmNEStringTable*&>(x));
    set_isModified(true);
}

SgAddressList const&
SgAsmNEModuleTable::get_nameOffsets() const {
    return p_nameOffsets;
}

void
SgAsmNEModuleTable::set_nameOffsets(SgAddressList const& x) {
    this->p_nameOffsets = x;
    set_isModified(true);
}

SgStringList const&
SgAsmNEModuleTable::get_names() const {
    return p_names;
}

void
SgAsmNEModuleTable::set_names(SgStringList const& x) {
    this->p_names = x;
    set_isModified(true);
}

SgAsmNEModuleTable::~SgAsmNEModuleTable() {
    destructorHelper();
}

SgAsmNEModuleTable::SgAsmNEModuleTable()
    : p_strtab(nullptr) {}

void
SgAsmNEModuleTable::initializeProperties() {
    p_strtab = nullptr;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
