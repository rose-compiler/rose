#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmPEExportEntryList            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmPEExportEntryPtrList const&
SgAsmPEExportEntryList::get_exports() const {
    return p_exports;
}

#line 552 "src/Rosebud/RosettaGenerator.C"
SgAsmPEExportEntryPtrList&
SgAsmPEExportEntryList::get_exports() {
    return p_exports;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPEExportEntryList::set_exports(SgAsmPEExportEntryPtrList const& x) {
    this->p_exports = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmPEExportEntryList::~SgAsmPEExportEntryList() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmPEExportEntryList::SgAsmPEExportEntryList() {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPEExportEntryList::initializeProperties() {
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
