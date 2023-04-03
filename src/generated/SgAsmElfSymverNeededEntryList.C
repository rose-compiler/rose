#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfSymverNeededEntryList            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmElfSymverNeededEntryPtrList const&
SgAsmElfSymverNeededEntryList::get_entries() const {
    return p_entries;
}

#line 493 "src/Rosebud/RosettaGenerator.C"
SgAsmElfSymverNeededEntryPtrList&
SgAsmElfSymverNeededEntryList::get_entries() {
    return p_entries;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSymverNeededEntryList::set_entries(SgAsmElfSymverNeededEntryPtrList const& x) {
    this->p_entries = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmElfSymverNeededEntryList::~SgAsmElfSymverNeededEntryList() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmElfSymverNeededEntryList::SgAsmElfSymverNeededEntryList() {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSymverNeededEntryList::initializeProperties() {
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
