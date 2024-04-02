//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmPEExportEntryList            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmPEExportEntryList_IMPL
#include <SgAsmPEExportEntryList.h>

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
    changeChildPointer(this->p_exports, const_cast<SgAsmPEExportEntryPtrList&>(x));
    set_isModified(true);
}

SgAsmPEExportEntryList::~SgAsmPEExportEntryList() {
    destructorHelper();
}

SgAsmPEExportEntryList::SgAsmPEExportEntryList() {}

void
SgAsmPEExportEntryList::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
