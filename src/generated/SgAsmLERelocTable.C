#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmLERelocTable            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmLERelocEntryPtrList const&
SgAsmLERelocTable::get_entries() const {
    return p_entries;
}

#line 493 "src/Rosebud/RosettaGenerator.C"
SgAsmLERelocEntryPtrList&
SgAsmLERelocTable::get_entries() {
    return p_entries;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmLERelocTable::set_entries(SgAsmLERelocEntryPtrList const& x) {
    this->p_entries = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmLERelocTable::~SgAsmLERelocTable() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmLERelocTable::SgAsmLERelocTable() {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmLERelocTable::initializeProperties() {
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
