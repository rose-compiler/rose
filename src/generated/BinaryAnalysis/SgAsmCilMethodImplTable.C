//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCilMethodImplTable            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmCilMethodImplTable_IMPL
#include <SgAsmCilMethodImplTable.h>

std::vector<SgAsmCilMethodImpl*> const&
SgAsmCilMethodImplTable::get_elements() const {
    return p_elements;
}

std::vector<SgAsmCilMethodImpl*>&
SgAsmCilMethodImplTable::get_elements() {
    return p_elements;
}

SgAsmCilMethodImplTable::~SgAsmCilMethodImplTable() {
    destructorHelper();
}

SgAsmCilMethodImplTable::SgAsmCilMethodImplTable() {}

void
SgAsmCilMethodImplTable::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
