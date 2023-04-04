//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmDwarfCompilationUnitList            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

SgAsmDwarfCompilationUnitPtrList const&
SgAsmDwarfCompilationUnitList::get_cu_list() const {
    return p_cu_list;
}

SgAsmDwarfCompilationUnitPtrList&
SgAsmDwarfCompilationUnitList::get_cu_list() {
    return p_cu_list;
}

void
SgAsmDwarfCompilationUnitList::set_cu_list(SgAsmDwarfCompilationUnitPtrList const& x) {
    this->p_cu_list = x;
    set_isModified(true);
}

SgAsmDwarfCompilationUnitList::~SgAsmDwarfCompilationUnitList() {
    destructorHelper();
}

SgAsmDwarfCompilationUnitList::SgAsmDwarfCompilationUnitList() {}

void
SgAsmDwarfCompilationUnitList::initializeProperties() {
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
