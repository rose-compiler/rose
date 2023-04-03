#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfRelocSection            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
bool const&
SgAsmElfRelocSection::get_uses_addend() const {
    return p_uses_addend;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfRelocSection::set_uses_addend(bool const& x) {
    this->p_uses_addend = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmElfSection* const&
SgAsmElfRelocSection::get_target_section() const {
    return p_target_section;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfRelocSection::set_target_section(SgAsmElfSection* const& x) {
    this->p_target_section = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmElfRelocEntryList* const&
SgAsmElfRelocSection::get_entries() const {
    return p_entries;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfRelocSection::set_entries(SgAsmElfRelocEntryList* const& x) {
    this->p_entries = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmElfRelocSection::~SgAsmElfRelocSection() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmElfRelocSection::SgAsmElfRelocSection()
    : p_uses_addend(true)
    , p_target_section(nullptr)
    , p_entries(createAndParent<SgAsmElfRelocEntryList>(this)) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfRelocSection::initializeProperties() {
    p_uses_addend = true;
    p_target_section = nullptr;
    p_entries = createAndParent<SgAsmElfRelocEntryList>(this);
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
