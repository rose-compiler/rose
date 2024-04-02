//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfSymverDefinedEntryList            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmElfSymverDefinedEntryList_IMPL
#include <sage3basic.h>

SgAsmElfSymverDefinedEntryPtrList const&
SgAsmElfSymverDefinedEntryList::get_entries() const {
    return p_entries;
}

SgAsmElfSymverDefinedEntryPtrList&
SgAsmElfSymverDefinedEntryList::get_entries() {
    return p_entries;
}

void
SgAsmElfSymverDefinedEntryList::set_entries(SgAsmElfSymverDefinedEntryPtrList const& x) {
    changeChildPointer(this->p_entries, const_cast<SgAsmElfSymverDefinedEntryPtrList&>(x));
    set_isModified(true);
}

SgAsmElfSymverDefinedEntryList::~SgAsmElfSymverDefinedEntryList() {
    destructorHelper();
}

SgAsmElfSymverDefinedEntryList::SgAsmElfSymverDefinedEntryList() {}

void
SgAsmElfSymverDefinedEntryList::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
