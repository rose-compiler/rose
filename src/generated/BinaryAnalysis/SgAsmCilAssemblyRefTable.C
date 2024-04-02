//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCilAssemblyRefTable            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmCilAssemblyRefTable_IMPL
#include <SgAsmCilAssemblyRefTable.h>

std::vector<SgAsmCilAssemblyRef*> const&
SgAsmCilAssemblyRefTable::get_elements() const {
    return p_elements;
}

std::vector<SgAsmCilAssemblyRef*>&
SgAsmCilAssemblyRefTable::get_elements() {
    return p_elements;
}

SgAsmCilAssemblyRefTable::~SgAsmCilAssemblyRefTable() {
    destructorHelper();
}

SgAsmCilAssemblyRefTable::SgAsmCilAssemblyRefTable() {}

void
SgAsmCilAssemblyRefTable::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
