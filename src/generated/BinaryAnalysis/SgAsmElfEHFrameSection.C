//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfEHFrameSection            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmElfEHFrameSection_IMPL
#include <sage3basic.h>

SgAsmElfEHFrameEntryCIList* const&
SgAsmElfEHFrameSection::get_ci_entries() const {
    return p_ci_entries;
}

void
SgAsmElfEHFrameSection::set_ci_entries(SgAsmElfEHFrameEntryCIList* const& x) {
    changeChildPointer(this->p_ci_entries, const_cast<SgAsmElfEHFrameEntryCIList*&>(x));
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

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
