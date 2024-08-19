//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCilMethodData            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmCilMethodData_IMPL
#include <SgAsmCilMethodData.h>

uint64_t const&
SgAsmCilMethodData::get_kind() const {
    return p_kind;
}

void
SgAsmCilMethodData::set_kind(uint64_t const& x) {
    this->p_kind = x;
    set_isModified(true);
}

std::uint32_t const&
SgAsmCilMethodData::get_dataSize() const {
    return p_dataSize;
}

void
SgAsmCilMethodData::set_dataSize(std::uint32_t const& x) {
    this->p_dataSize = x;
    set_isModified(true);
}

std::vector<SgAsmCilExceptionData*> const&
SgAsmCilMethodData::get_Clauses() const {
    return p_clauses;
}

std::vector<SgAsmCilExceptionData*>&
SgAsmCilMethodData::get_Clauses() {
    return p_clauses;
}

SgAsmCilMethodData::~SgAsmCilMethodData() {
    destructorHelper();
}

SgAsmCilMethodData::SgAsmCilMethodData()
    : p_kind(0)
    , p_dataSize(0) {}

void
SgAsmCilMethodData::initializeProperties() {
    p_kind = 0;
    p_dataSize = 0;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
