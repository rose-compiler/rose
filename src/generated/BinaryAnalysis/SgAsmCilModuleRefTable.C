//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCilModuleRefTable            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmCilModuleRefTable_IMPL
#include <SgAsmCilModuleRefTable.h>

std::vector<SgAsmCilModuleRef*> const&
SgAsmCilModuleRefTable::get_elements() const {
    return p_elements;
}

std::vector<SgAsmCilModuleRef*>&
SgAsmCilModuleRefTable::get_elements() {
    return p_elements;
}

SgAsmCilModuleRefTable::~SgAsmCilModuleRefTable() {
    destructorHelper();
}

SgAsmCilModuleRefTable::SgAsmCilModuleRefTable() {}

void
SgAsmCilModuleRefTable::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
