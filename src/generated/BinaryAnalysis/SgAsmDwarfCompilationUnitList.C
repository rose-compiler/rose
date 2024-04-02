//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmDwarfCompilationUnitList            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmDwarfCompilationUnitList_IMPL
#include <SgAsmDwarfCompilationUnitList.h>

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
    changeChildPointer(this->p_cu_list, const_cast<SgAsmDwarfCompilationUnitPtrList&>(x));
    set_isModified(true);
}

SgAsmDwarfCompilationUnitList::~SgAsmDwarfCompilationUnitList() {
    destructorHelper();
}

SgAsmDwarfCompilationUnitList::SgAsmDwarfCompilationUnitList() {}

void
SgAsmDwarfCompilationUnitList::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
