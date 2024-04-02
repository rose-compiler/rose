//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmLEPageTable            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmLEPageTable_IMPL
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
    changeChildPointer(this->p_entries, const_cast<SgAsmLEPageTableEntryPtrList&>(x));
    set_isModified(true);
}

SgAsmLEPageTable::~SgAsmLEPageTable() {
    destructorHelper();
}

SgAsmLEPageTable::SgAsmLEPageTable() {}

void
SgAsmLEPageTable::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
