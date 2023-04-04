#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfEHFrameEntryFDList            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmElfEHFrameEntryFDPtrList const&
SgAsmElfEHFrameEntryFDList::get_entries() const {
    return p_entries;
}

#line 552 "src/Rosebud/RosettaGenerator.C"
SgAsmElfEHFrameEntryFDPtrList&
SgAsmElfEHFrameEntryFDList::get_entries() {
    return p_entries;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfEHFrameEntryFDList::set_entries(SgAsmElfEHFrameEntryFDPtrList const& x) {
    this->p_entries = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmElfEHFrameEntryFDList::~SgAsmElfEHFrameEntryFDList() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmElfEHFrameEntryFDList::SgAsmElfEHFrameEntryFDList() {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfEHFrameEntryFDList::initializeProperties() {
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
