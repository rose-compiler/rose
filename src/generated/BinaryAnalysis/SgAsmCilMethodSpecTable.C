//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCilMethodSpecTable            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmCilMethodSpecTable_IMPL
#include <SgAsmCilMethodSpecTable.h>

std::vector<SgAsmCilMethodSpec*> const&
SgAsmCilMethodSpecTable::get_elements() const {
    return p_elements;
}

std::vector<SgAsmCilMethodSpec*>&
SgAsmCilMethodSpecTable::get_elements() {
    return p_elements;
}

SgAsmCilMethodSpecTable::~SgAsmCilMethodSpecTable() {
    destructorHelper();
}

SgAsmCilMethodSpecTable::SgAsmCilMethodSpecTable() {}

void
SgAsmCilMethodSpecTable::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
