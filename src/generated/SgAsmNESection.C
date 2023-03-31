//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmNESection                -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

SgAsmNESectionTableEntry* const&
SgAsmNESection::get_st_entry() const {
    return p_st_entry;
}

void
SgAsmNESection::set_st_entry(SgAsmNESectionTableEntry* const& x) {
    this->p_st_entry = x;
    set_isModified(true);
}

SgAsmNERelocTable* const&
SgAsmNESection::get_reloc_table() const {
    return p_reloc_table;
}

void
SgAsmNESection::set_reloc_table(SgAsmNERelocTable* const& x) {
    this->p_reloc_table = x;
    set_isModified(true);
}

SgAsmNESection::~SgAsmNESection() {
    destructorHelper();
}

SgAsmNESection::SgAsmNESection()
    : p_st_entry(nullptr)
    , p_reloc_table(nullptr) {}

void
SgAsmNESection::initializeProperties() {
    p_st_entry = nullptr;
    p_reloc_table = nullptr;
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
