//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmNEModuleTable            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

SgAsmNEStringTable* const&
SgAsmNEModuleTable::get_strtab() const {
    return p_strtab;
}

void
SgAsmNEModuleTable::set_strtab(SgAsmNEStringTable* const& x) {
    this->p_strtab = x;
    set_isModified(true);
}

SgAddressList const&
SgAsmNEModuleTable::get_name_offsets() const {
    return p_name_offsets;
}

void
SgAsmNEModuleTable::set_name_offsets(SgAddressList const& x) {
    this->p_name_offsets = x;
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

#endif // ROSE_ENABLE_BINARY_ANALYSIS
