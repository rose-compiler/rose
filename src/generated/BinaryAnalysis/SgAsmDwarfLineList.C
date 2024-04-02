//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmDwarfLineList            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmDwarfLineList_IMPL
#include <SgAsmDwarfLineList.h>

SgAsmDwarfLinePtrList const&
SgAsmDwarfLineList::get_line_list() const {
    return p_line_list;
}

SgAsmDwarfLinePtrList&
SgAsmDwarfLineList::get_line_list() {
    return p_line_list;
}

void
SgAsmDwarfLineList::set_line_list(SgAsmDwarfLinePtrList const& x) {
    changeChildPointer(this->p_line_list, const_cast<SgAsmDwarfLinePtrList&>(x));
    set_isModified(true);
}

SgAsmDwarfLineList::~SgAsmDwarfLineList() {
    destructorHelper();
}

SgAsmDwarfLineList::SgAsmDwarfLineList() {}

void
SgAsmDwarfLineList::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
