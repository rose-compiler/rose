#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfNoteSection            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmElfNoteEntryList* const&
SgAsmElfNoteSection::get_entries() const {
    return p_entries;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfNoteSection::set_entries(SgAsmElfNoteEntryList* const& x) {
    this->p_entries = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmElfNoteSection::~SgAsmElfNoteSection() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmElfNoteSection::SgAsmElfNoteSection()
    : p_entries(createAndParent<SgAsmElfNoteEntryList>(this)) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfNoteSection::initializeProperties() {
    p_entries = createAndParent<SgAsmElfNoteEntryList>(this);
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
