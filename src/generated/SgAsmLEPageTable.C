#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmLEPageTable            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmLEPageTableEntryPtrList const&
SgAsmLEPageTable::get_entries() const {
    return p_entries;
}

#line 552 "src/Rosebud/RosettaGenerator.C"
SgAsmLEPageTableEntryPtrList&
SgAsmLEPageTable::get_entries() {
    return p_entries;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmLEPageTable::set_entries(SgAsmLEPageTableEntryPtrList const& x) {
    this->p_entries = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmLEPageTable::~SgAsmLEPageTable() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmLEPageTable::SgAsmLEPageTable() {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmLEPageTable::initializeProperties() {
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
