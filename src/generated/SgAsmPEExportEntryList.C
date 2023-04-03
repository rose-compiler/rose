#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmPEExportEntryList            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmPEExportEntryPtrList const&
SgAsmPEExportEntryList::get_exports() const {
    return p_exports;
}

#line 493 "src/Rosebud/RosettaGenerator.C"
SgAsmPEExportEntryPtrList&
SgAsmPEExportEntryList::get_exports() {
    return p_exports;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPEExportEntryList::set_exports(SgAsmPEExportEntryPtrList const& x) {
    this->p_exports = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmPEExportEntryList::~SgAsmPEExportEntryList() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmPEExportEntryList::SgAsmPEExportEntryList() {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPEExportEntryList::initializeProperties() {
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
