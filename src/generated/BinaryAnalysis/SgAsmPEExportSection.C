//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmPEExportSection            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#include <sage3basic.h>

SgAsmPEExportDirectory* const&
SgAsmPEExportSection::get_export_dir() const {
    return p_export_dir;
}

void
SgAsmPEExportSection::set_export_dir(SgAsmPEExportDirectory* const& x) {
    changeChildPointer(this->p_export_dir, const_cast<SgAsmPEExportDirectory*&>(x));
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
    : p_export_dir(nullptr)
    , p_exports(createAndParent<SgAsmPEExportEntryList>(this)) {}

void
SgAsmPEExportSection::initializeProperties() {
    p_export_dir = nullptr;
    p_exports = createAndParent<SgAsmPEExportEntryList>(this);
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
