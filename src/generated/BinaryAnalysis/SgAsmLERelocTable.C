//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmLERelocTable            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmLERelocTable_IMPL
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
    changeChildPointer(this->p_entries, const_cast<SgAsmLERelocEntryPtrList&>(x));
    set_isModified(true);
}

SgAsmLERelocTable::~SgAsmLERelocTable() {
    destructorHelper();
}

SgAsmLERelocTable::SgAsmLERelocTable() {}

void
SgAsmLERelocTable::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
