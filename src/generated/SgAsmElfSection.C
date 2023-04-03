#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfSection            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmElfSectionTableEntry* const&
SgAsmElfSection::get_section_entry() const {
    return p_section_entry;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSection::set_section_entry(SgAsmElfSectionTableEntry* const& x) {
    this->p_section_entry = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmElfSegmentTableEntry* const&
SgAsmElfSection::get_segment_entry() const {
    return p_segment_entry;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSection::set_segment_entry(SgAsmElfSegmentTableEntry* const& x) {
    this->p_segment_entry = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmElfSection::~SgAsmElfSection() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmElfSection::SgAsmElfSection()
    : p_linked_section(nullptr)
    , p_section_entry(nullptr)
    , p_segment_entry(nullptr) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSection::initializeProperties() {
    p_linked_section = nullptr;
    p_section_entry = nullptr;
    p_segment_entry = nullptr;
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
