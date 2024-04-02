//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmNEEntryTable            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmNEEntryTable_IMPL
#include <SgAsmNEEntryTable.h>

SgSizeTList const&
SgAsmNEEntryTable::get_bundle_sizes() const {
    return p_bundle_sizes;
}

void
SgAsmNEEntryTable::set_bundle_sizes(SgSizeTList const& x) {
    this->p_bundle_sizes = x;
    set_isModified(true);
}

SgAsmNEEntryPointPtrList const&
SgAsmNEEntryTable::get_entries() const {
    return p_entries;
}

SgAsmNEEntryPointPtrList&
SgAsmNEEntryTable::get_entries() {
    return p_entries;
}

void
SgAsmNEEntryTable::set_entries(SgAsmNEEntryPointPtrList const& x) {
    changeChildPointer(this->p_entries, const_cast<SgAsmNEEntryPointPtrList&>(x));
    set_isModified(true);
}

SgAsmNEEntryTable::~SgAsmNEEntryTable() {
    destructorHelper();
}

SgAsmNEEntryTable::SgAsmNEEntryTable() {}

void
SgAsmNEEntryTable::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
