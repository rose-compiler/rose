//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfSection            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#include <sage3basic.h>

SgAsmElfSectionTableEntry* const&
SgAsmElfSection::get_section_entry() const {
    return p_section_entry;
}

void
SgAsmElfSection::set_section_entry(SgAsmElfSectionTableEntry* const& x) {
    changeChildPointer(this->p_section_entry, const_cast<SgAsmElfSectionTableEntry*&>(x));
    set_isModified(true);
}

SgAsmElfSegmentTableEntry* const&
SgAsmElfSection::get_segment_entry() const {
    return p_segment_entry;
}

void
SgAsmElfSection::set_segment_entry(SgAsmElfSegmentTableEntry* const& x) {
    changeChildPointer(this->p_segment_entry, const_cast<SgAsmElfSegmentTableEntry*&>(x));
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

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
