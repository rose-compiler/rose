//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmDwarfConstructList            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

SgAsmDwarfConstructPtrList const&
SgAsmDwarfConstructList::get_list() const {
    return p_list;
}

SgAsmDwarfConstructPtrList&
SgAsmDwarfConstructList::get_list() {
    return p_list;
}

void
SgAsmDwarfConstructList::set_list(SgAsmDwarfConstructPtrList const& x) {
    this->p_list = x;
    set_isModified(true);
}

SgAsmDwarfConstructList::~SgAsmDwarfConstructList() {
    destructorHelper();
}

SgAsmDwarfConstructList::SgAsmDwarfConstructList() {}

void
SgAsmDwarfConstructList::initializeProperties() {
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
