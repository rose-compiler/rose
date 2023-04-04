#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmNEEntryTable            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
SgSizeTList const&
SgAsmNEEntryTable::get_bundle_sizes() const {
    return p_bundle_sizes;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmNEEntryTable::set_bundle_sizes(SgSizeTList const& x) {
    this->p_bundle_sizes = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmNEEntryPointPtrList const&
SgAsmNEEntryTable::get_entries() const {
    return p_entries;
}

#line 552 "src/Rosebud/RosettaGenerator.C"
SgAsmNEEntryPointPtrList&
SgAsmNEEntryTable::get_entries() {
    return p_entries;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmNEEntryTable::set_entries(SgAsmNEEntryPointPtrList const& x) {
    this->p_entries = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmNEEntryTable::~SgAsmNEEntryTable() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmNEEntryTable::SgAsmNEEntryTable() {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmNEEntryTable::initializeProperties() {
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
