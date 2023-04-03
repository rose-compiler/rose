#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfSymverDefinedEntryList            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmElfSymverDefinedEntryPtrList const&
SgAsmElfSymverDefinedEntryList::get_entries() const {
    return p_entries;
}

#line 493 "src/Rosebud/RosettaGenerator.C"
SgAsmElfSymverDefinedEntryPtrList&
SgAsmElfSymverDefinedEntryList::get_entries() {
    return p_entries;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSymverDefinedEntryList::set_entries(SgAsmElfSymverDefinedEntryPtrList const& x) {
    this->p_entries = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmElfSymverDefinedEntryList::~SgAsmElfSymverDefinedEntryList() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmElfSymverDefinedEntryList::SgAsmElfSymverDefinedEntryList() {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSymverDefinedEntryList::initializeProperties() {
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
