//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCilModuleTable            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmCilModuleTable_IMPL
#include <SgAsmCilModuleTable.h>

std::vector<SgAsmCilModule*> const&
SgAsmCilModuleTable::get_elements() const {
    return p_elements;
}

std::vector<SgAsmCilModule*>&
SgAsmCilModuleTable::get_elements() {
    return p_elements;
}

SgAsmCilModuleTable::~SgAsmCilModuleTable() {
    destructorHelper();
}

SgAsmCilModuleTable::SgAsmCilModuleTable() {}

void
SgAsmCilModuleTable::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
