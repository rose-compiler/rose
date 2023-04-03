#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfSymverNeededSection            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmElfSymverNeededEntryList* const&
SgAsmElfSymverNeededSection::get_entries() const {
    return p_entries;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSymverNeededSection::set_entries(SgAsmElfSymverNeededEntryList* const& x) {
    this->p_entries = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmElfSymverNeededSection::~SgAsmElfSymverNeededSection() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmElfSymverNeededSection::SgAsmElfSymverNeededSection()
    : p_entries(createAndParent<SgAsmElfSymverNeededEntryList>(this)) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSymverNeededSection::initializeProperties() {
    p_entries = createAndParent<SgAsmElfSymverNeededEntryList>(this);
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
