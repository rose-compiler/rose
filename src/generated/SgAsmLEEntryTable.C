#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmLEEntryTable            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
SgSizeTList const&
SgAsmLEEntryTable::get_bundle_sizes() const {
    return p_bundle_sizes;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmLEEntryTable::set_bundle_sizes(SgSizeTList const& x) {
    this->p_bundle_sizes = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmLEEntryPointPtrList const&
SgAsmLEEntryTable::get_entries() const {
    return p_entries;
}

#line 552 "src/Rosebud/RosettaGenerator.C"
SgAsmLEEntryPointPtrList&
SgAsmLEEntryTable::get_entries() {
    return p_entries;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmLEEntryTable::set_entries(SgAsmLEEntryPointPtrList const& x) {
    this->p_entries = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmLEEntryTable::~SgAsmLEEntryTable() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmLEEntryTable::SgAsmLEEntryTable() {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmLEEntryTable::initializeProperties() {
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
