//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfEHFrameEntryCIList            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

SgAsmElfEHFrameEntryCIPtrList const&
SgAsmElfEHFrameEntryCIList::get_entries() const {
    return p_entries;
}

SgAsmElfEHFrameEntryCIPtrList&
SgAsmElfEHFrameEntryCIList::get_entries() {
    return p_entries;
}

void
SgAsmElfEHFrameEntryCIList::set_entries(SgAsmElfEHFrameEntryCIPtrList const& x) {
    this->p_entries = x;
    set_isModified(true);
}

SgAsmElfEHFrameEntryCIList::~SgAsmElfEHFrameEntryCIList() {
    destructorHelper();
}

SgAsmElfEHFrameEntryCIList::SgAsmElfEHFrameEntryCIList() {}

void
SgAsmElfEHFrameEntryCIList::initializeProperties() {
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
