//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmNESection            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmNESection_IMPL
#include <SgAsmNESection.h>

SgAsmNESectionTableEntry* const&
SgAsmNESection::get_sectionTableEntry() const {
    return p_sectionTableEntry;
}

void
SgAsmNESection::set_sectionTableEntry(SgAsmNESectionTableEntry* const& x) {
    changeChildPointer(this->p_sectionTableEntry, const_cast<SgAsmNESectionTableEntry*&>(x));
    set_isModified(true);
}

SgAsmNERelocTable* const&
SgAsmNESection::get_relocationTable() const {
    return p_relocationTable;
}

void
SgAsmNESection::set_relocationTable(SgAsmNERelocTable* const& x) {
    changeChildPointer(this->p_relocationTable, const_cast<SgAsmNERelocTable*&>(x));
    set_isModified(true);
}

SgAsmNESection::~SgAsmNESection() {
    destructorHelper();
}

SgAsmNESection::SgAsmNESection()
    : p_sectionTableEntry(nullptr)
    , p_relocationTable(nullptr) {}

void
SgAsmNESection::initializeProperties() {
    p_sectionTableEntry = nullptr;
    p_relocationTable = nullptr;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
