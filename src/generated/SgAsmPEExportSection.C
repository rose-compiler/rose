#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmPEExportSection            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmPEExportDirectory* const&
SgAsmPEExportSection::get_export_dir() const {
    return p_export_dir;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPEExportSection::set_export_dir(SgAsmPEExportDirectory* const& x) {
    this->p_export_dir = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmPEExportEntryList* const&
SgAsmPEExportSection::get_exports() const {
    return p_exports;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPEExportSection::set_exports(SgAsmPEExportEntryList* const& x) {
    this->p_exports = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmPEExportSection::~SgAsmPEExportSection() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmPEExportSection::SgAsmPEExportSection()
#line 308 "src/Rosebud/RosettaGenerator.C"
    : p_export_dir(nullptr)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_exports(createAndParent<SgAsmPEExportEntryList>(this)) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPEExportSection::initializeProperties() {
    p_export_dir = nullptr;
    p_exports = createAndParent<SgAsmPEExportEntryList>(this);
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
