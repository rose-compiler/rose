#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfSymverNeededSection            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmElfSymverNeededEntryList* const&
SgAsmElfSymverNeededSection::get_entries() const {
    return p_entries;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSymverNeededSection::set_entries(SgAsmElfSymverNeededEntryList* const& x) {
    this->p_entries = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmElfSymverNeededSection::~SgAsmElfSymverNeededSection() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmElfSymverNeededSection::SgAsmElfSymverNeededSection()
#line 308 "src/Rosebud/RosettaGenerator.C"
    : p_entries(createAndParent<SgAsmElfSymverNeededEntryList>(this)) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSymverNeededSection::initializeProperties() {
    p_entries = createAndParent<SgAsmElfSymverNeededEntryList>(this);
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
