//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmNERelocTable                -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

SgAsmNERelocEntryPtrList const&
SgAsmNERelocTable::get_entries() const {
    return p_entries;
}

SgAsmNERelocEntryPtrList&
SgAsmNERelocTable::get_entries() {
    return p_entries;
}

void
SgAsmNERelocTable::set_entries(SgAsmNERelocEntryPtrList const& x) {
    this->p_entries = x;
    set_isModified(true);
}

SgAsmNERelocTable::~SgAsmNERelocTable() {
    destructorHelper();
}

SgAsmNERelocTable::SgAsmNERelocTable() {}

void
SgAsmNERelocTable::initializeProperties() {
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
