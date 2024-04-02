//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmNERelocTable            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmNERelocTable_IMPL
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
    changeChildPointer(this->p_entries, const_cast<SgAsmNERelocEntryPtrList&>(x));
    set_isModified(true);
}

SgAsmNERelocTable::~SgAsmNERelocTable() {
    destructorHelper();
}

SgAsmNERelocTable::SgAsmNERelocTable() {}

void
SgAsmNERelocTable::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
