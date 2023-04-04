#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfRelocSection            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
bool const&
SgAsmElfRelocSection::get_uses_addend() const {
    return p_uses_addend;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfRelocSection::set_uses_addend(bool const& x) {
    this->p_uses_addend = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmElfSection* const&
SgAsmElfRelocSection::get_target_section() const {
    return p_target_section;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfRelocSection::set_target_section(SgAsmElfSection* const& x) {
    this->p_target_section = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmElfRelocEntryList* const&
SgAsmElfRelocSection::get_entries() const {
    return p_entries;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfRelocSection::set_entries(SgAsmElfRelocEntryList* const& x) {
    this->p_entries = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmElfRelocSection::~SgAsmElfRelocSection() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmElfRelocSection::SgAsmElfRelocSection()
#line 308 "src/Rosebud/RosettaGenerator.C"
    : p_uses_addend(true)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_target_section(nullptr)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_entries(createAndParent<SgAsmElfRelocEntryList>(this)) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfRelocSection::initializeProperties() {
    p_uses_addend = true;
    p_target_section = nullptr;
    p_entries = createAndParent<SgAsmElfRelocEntryList>(this);
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
