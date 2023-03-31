//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmDwarfLineList                -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

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
    this->p_line_list = x;
    set_isModified(true);
}

SgAsmDwarfLineList::~SgAsmDwarfLineList() {
    destructorHelper();
}

SgAsmDwarfLineList::SgAsmDwarfLineList() {}

void
SgAsmDwarfLineList::initializeProperties() {
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
