//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfSection            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmElfSection_IMPL
#include <SgAsmElfSection.h>

SgAsmElfSection* const&
SgAsmElfSection::get_linkedSection() const {
    return p_linkedSection;
}

SgAsmElfSectionTableEntry* const&
SgAsmElfSection::get_sectionEntry() const {
    return p_sectionEntry;
}

void
SgAsmElfSection::set_sectionEntry(SgAsmElfSectionTableEntry* const& x) {
    changeChildPointer(this->p_sectionEntry, const_cast<SgAsmElfSectionTableEntry*&>(x));
    set_isModified(true);
}

SgAsmElfSegmentTableEntry* const&
SgAsmElfSection::get_segmentEntry() const {
    return p_segmentEntry;
}

void
SgAsmElfSection::set_segmentEntry(SgAsmElfSegmentTableEntry* const& x) {
    changeChildPointer(this->p_segmentEntry, const_cast<SgAsmElfSegmentTableEntry*&>(x));
    set_isModified(true);
}

SgAsmElfSection::~SgAsmElfSection() {
    destructorHelper();
}

SgAsmElfSection::SgAsmElfSection()
    : p_linkedSection(nullptr)
    , p_sectionEntry(nullptr)
    , p_segmentEntry(nullptr) {}

void
SgAsmElfSection::initializeProperties() {
    p_linkedSection = nullptr;
    p_sectionEntry = nullptr;
    p_segmentEntry = nullptr;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
