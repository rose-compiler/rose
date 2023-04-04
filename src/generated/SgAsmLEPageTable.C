//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmLEPageTable            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

SgAsmLEPageTableEntryPtrList const&
SgAsmLEPageTable::get_entries() const {
    return p_entries;
}

SgAsmLEPageTableEntryPtrList&
SgAsmLEPageTable::get_entries() {
    return p_entries;
}

void
SgAsmLEPageTable::set_entries(SgAsmLEPageTableEntryPtrList const& x) {
    this->p_entries = x;
    set_isModified(true);
}

SgAsmLEPageTable::~SgAsmLEPageTable() {
    destructorHelper();
}

SgAsmLEPageTable::SgAsmLEPageTable() {}

void
SgAsmLEPageTable::initializeProperties() {
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
