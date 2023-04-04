//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmLEEntryTable            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

SgSizeTList const&
SgAsmLEEntryTable::get_bundle_sizes() const {
    return p_bundle_sizes;
}

void
SgAsmLEEntryTable::set_bundle_sizes(SgSizeTList const& x) {
    this->p_bundle_sizes = x;
    set_isModified(true);
}

SgAsmLEEntryPointPtrList const&
SgAsmLEEntryTable::get_entries() const {
    return p_entries;
}

SgAsmLEEntryPointPtrList&
SgAsmLEEntryTable::get_entries() {
    return p_entries;
}

void
SgAsmLEEntryTable::set_entries(SgAsmLEEntryPointPtrList const& x) {
    this->p_entries = x;
    set_isModified(true);
}

SgAsmLEEntryTable::~SgAsmLEEntryTable() {
    destructorHelper();
}

SgAsmLEEntryTable::SgAsmLEEntryTable() {}

void
SgAsmLEEntryTable::initializeProperties() {
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
