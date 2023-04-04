#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfSection            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmElfSectionTableEntry* const&
SgAsmElfSection::get_section_entry() const {
    return p_section_entry;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSection::set_section_entry(SgAsmElfSectionTableEntry* const& x) {
    this->p_section_entry = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmElfSegmentTableEntry* const&
SgAsmElfSection::get_segment_entry() const {
    return p_segment_entry;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSection::set_segment_entry(SgAsmElfSegmentTableEntry* const& x) {
    this->p_segment_entry = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmElfSection::~SgAsmElfSection() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmElfSection::SgAsmElfSection()
#line 308 "src/Rosebud/RosettaGenerator.C"
    : p_linked_section(nullptr)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_section_entry(nullptr)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_segment_entry(nullptr) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSection::initializeProperties() {
    p_linked_section = nullptr;
    p_section_entry = nullptr;
    p_segment_entry = nullptr;
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
