//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmDwarfConstructList            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmDwarfConstructList_IMPL
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
    changeChildPointer(this->p_list, const_cast<SgAsmDwarfConstructPtrList&>(x));
    set_isModified(true);
}

SgAsmDwarfConstructList::~SgAsmDwarfConstructList() {
    destructorHelper();
}

SgAsmDwarfConstructList::SgAsmDwarfConstructList() {}

void
SgAsmDwarfConstructList::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
