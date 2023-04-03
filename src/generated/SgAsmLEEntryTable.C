#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmLEEntryTable            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
SgSizeTList const&
SgAsmLEEntryTable::get_bundle_sizes() const {
    return p_bundle_sizes;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmLEEntryTable::set_bundle_sizes(SgSizeTList const& x) {
    this->p_bundle_sizes = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmLEEntryPointPtrList const&
SgAsmLEEntryTable::get_entries() const {
    return p_entries;
}

#line 493 "src/Rosebud/RosettaGenerator.C"
SgAsmLEEntryPointPtrList&
SgAsmLEEntryTable::get_entries() {
    return p_entries;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmLEEntryTable::set_entries(SgAsmLEEntryPointPtrList const& x) {
    this->p_entries = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmLEEntryTable::~SgAsmLEEntryTable() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmLEEntryTable::SgAsmLEEntryTable() {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmLEEntryTable::initializeProperties() {
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
