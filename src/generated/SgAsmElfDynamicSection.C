#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfDynamicSection            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmElfDynamicEntryList* const&
SgAsmElfDynamicSection::get_entries() const {
    return p_entries;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfDynamicSection::set_entries(SgAsmElfDynamicEntryList* const& x) {
    this->p_entries = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmElfDynamicSection::~SgAsmElfDynamicSection() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmElfDynamicSection::SgAsmElfDynamicSection()
    : p_entries(createAndParent<SgAsmElfDynamicEntryList>(this)) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfDynamicSection::initializeProperties() {
    p_entries = createAndParent<SgAsmElfDynamicEntryList>(this);
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
