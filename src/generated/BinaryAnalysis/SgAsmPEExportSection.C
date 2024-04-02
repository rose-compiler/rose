//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmPEExportSection            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmPEExportSection_IMPL
#include <SgAsmPEExportSection.h>

SgAsmPEExportDirectory* const&
SgAsmPEExportSection::get_exportDirectory() const {
    return p_exportDirectory;
}

void
SgAsmPEExportSection::set_exportDirectory(SgAsmPEExportDirectory* const& x) {
    changeChildPointer(this->p_exportDirectory, const_cast<SgAsmPEExportDirectory*&>(x));
    set_isModified(true);
}

SgAsmPEExportEntryList* const&
SgAsmPEExportSection::get_exports() const {
    return p_exports;
}

void
SgAsmPEExportSection::set_exports(SgAsmPEExportEntryList* const& x) {
    changeChildPointer(this->p_exports, const_cast<SgAsmPEExportEntryList*&>(x));
    set_isModified(true);
}

SgAsmPEExportSection::~SgAsmPEExportSection() {
    destructorHelper();
}

SgAsmPEExportSection::SgAsmPEExportSection()
    : p_exportDirectory(nullptr)
    , p_exports(createAndParent<SgAsmPEExportEntryList>(this)) {}

void
SgAsmPEExportSection::initializeProperties() {
    p_exportDirectory = nullptr;
    p_exports = createAndParent<SgAsmPEExportEntryList>(this);
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
