//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmPEExportEntryList            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

SgAsmPEExportEntryPtrList const&
SgAsmPEExportEntryList::get_exports() const {
    return p_exports;
}

SgAsmPEExportEntryPtrList&
SgAsmPEExportEntryList::get_exports() {
    return p_exports;
}

void
SgAsmPEExportEntryList::set_exports(SgAsmPEExportEntryPtrList const& x) {
    this->p_exports = x;
    set_isModified(true);
}

SgAsmPEExportEntryList::~SgAsmPEExportEntryList() {
    destructorHelper();
}

SgAsmPEExportEntryList::SgAsmPEExportEntryList() {}

void
SgAsmPEExportEntryList::initializeProperties() {
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
