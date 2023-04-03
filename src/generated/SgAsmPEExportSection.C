#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmPEExportSection            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmPEExportDirectory* const&
SgAsmPEExportSection::get_export_dir() const {
    return p_export_dir;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPEExportSection::set_export_dir(SgAsmPEExportDirectory* const& x) {
    this->p_export_dir = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmPEExportEntryList* const&
SgAsmPEExportSection::get_exports() const {
    return p_exports;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPEExportSection::set_exports(SgAsmPEExportEntryList* const& x) {
    this->p_exports = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmPEExportSection::~SgAsmPEExportSection() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmPEExportSection::SgAsmPEExportSection()
    : p_export_dir(nullptr)
    , p_exports(createAndParent<SgAsmPEExportEntryList>(this)) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPEExportSection::initializeProperties() {
    p_export_dir = nullptr;
    p_exports = createAndParent<SgAsmPEExportEntryList>(this);
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
