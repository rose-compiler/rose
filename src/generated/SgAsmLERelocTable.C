//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmLERelocTable                -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

SgAsmLERelocEntryPtrList const&
SgAsmLERelocTable::get_entries() const {
    return p_entries;
}

SgAsmLERelocEntryPtrList&
SgAsmLERelocTable::get_entries() {
    return p_entries;
}

void
SgAsmLERelocTable::set_entries(SgAsmLERelocEntryPtrList const& x) {
    this->p_entries = x;
    set_isModified(true);
}

SgAsmLERelocTable::~SgAsmLERelocTable() {
    destructorHelper();
}

SgAsmLERelocTable::SgAsmLERelocTable() {}

void
SgAsmLERelocTable::initializeProperties() {
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
