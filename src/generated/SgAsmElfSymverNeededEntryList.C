//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfSymverNeededEntryList                -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

SgAsmElfSymverNeededEntryPtrList const&
SgAsmElfSymverNeededEntryList::get_entries() const {
    return p_entries;
}

SgAsmElfSymverNeededEntryPtrList&
SgAsmElfSymverNeededEntryList::get_entries() {
    return p_entries;
}

void
SgAsmElfSymverNeededEntryList::set_entries(SgAsmElfSymverNeededEntryPtrList const& x) {
    this->p_entries = x;
    set_isModified(true);
}

SgAsmElfSymverNeededEntryList::~SgAsmElfSymverNeededEntryList() {
    destructorHelper();
}

SgAsmElfSymverNeededEntryList::SgAsmElfSymverNeededEntryList() {}

void
SgAsmElfSymverNeededEntryList::initializeProperties() {
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
