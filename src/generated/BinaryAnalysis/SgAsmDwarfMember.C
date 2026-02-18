//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmDwarfMember            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmDwarfMember_IMPL
#include <SgAsmDwarfMember.h>

uint64_t const&
SgAsmDwarfMember::get_data_bit_offset() const {
    return p_data_bit_offset;
}

void
SgAsmDwarfMember::set_data_bit_offset(uint64_t const& x) {
    this->p_data_bit_offset = x;
    set_isModified(true);
}

SgAsmDwarfMember::~SgAsmDwarfMember() {
    destructorHelper();
}

SgAsmDwarfMember::SgAsmDwarfMember()
    : p_data_bit_offset(0) {}

// The association between constructor arguments and their classes:
//    property=nesting_level    class=SgAsmDwarfConstruct
//    property=offset           class=SgAsmDwarfConstruct
//    property=overall_offset   class=SgAsmDwarfConstruct
SgAsmDwarfMember::SgAsmDwarfMember(int const& nesting_level,
                                   uint64_t const& offset,
                                   uint64_t const& overall_offset)
    : SgAsmDwarfConstruct(nesting_level, offset, overall_offset)
    , p_data_bit_offset(0) {}

void
SgAsmDwarfMember::initializeProperties() {
    p_data_bit_offset = 0;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
