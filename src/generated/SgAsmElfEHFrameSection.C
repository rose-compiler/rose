//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfEHFrameSection            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

SgAsmElfEHFrameEntryCIList* const&
SgAsmElfEHFrameSection::get_ci_entries() const {
    return p_ci_entries;
}

void
SgAsmElfEHFrameSection::set_ci_entries(SgAsmElfEHFrameEntryCIList* const& x) {
    this->p_ci_entries = x;
    set_isModified(true);
}

SgAsmElfEHFrameSection::~SgAsmElfEHFrameSection() {
    destructorHelper();
}

SgAsmElfEHFrameSection::SgAsmElfEHFrameSection()
    : p_ci_entries(createAndParent<SgAsmElfEHFrameEntryCIList>(this)) {}

void
SgAsmElfEHFrameSection::initializeProperties() {
    p_ci_entries = createAndParent<SgAsmElfEHFrameEntryCIList>(this);
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
