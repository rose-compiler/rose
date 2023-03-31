//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfSection                -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

SgAsmElfSectionTableEntry* const&
SgAsmElfSection::get_section_entry() const {
    return p_section_entry;
}

void
SgAsmElfSection::set_section_entry(SgAsmElfSectionTableEntry* const& x) {
    this->p_section_entry = x;
    set_isModified(true);
}

SgAsmElfSegmentTableEntry* const&
SgAsmElfSection::get_segment_entry() const {
    return p_segment_entry;
}

void
SgAsmElfSection::set_segment_entry(SgAsmElfSegmentTableEntry* const& x) {
    this->p_segment_entry = x;
    set_isModified(true);
}

SgAsmElfSection::~SgAsmElfSection() {
    destructorHelper();
}

SgAsmElfSection::SgAsmElfSection()
    : p_linked_section(nullptr)
    , p_section_entry(nullptr)
    , p_segment_entry(nullptr) {}

void
SgAsmElfSection::initializeProperties() {
    p_linked_section = nullptr;
    p_section_entry = nullptr;
    p_segment_entry = nullptr;
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
