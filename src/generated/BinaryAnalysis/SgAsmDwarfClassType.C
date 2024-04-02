//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmDwarfClassType            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmDwarfClassType_IMPL
#include <sage3basic.h>

SgAsmDwarfConstructList* const&
SgAsmDwarfClassType::get_body() const {
    return p_body;
}

void
SgAsmDwarfClassType::set_body(SgAsmDwarfConstructList* const& x) {
    changeChildPointer(this->p_body, const_cast<SgAsmDwarfConstructList*&>(x));
    set_isModified(true);
}

SgAsmDwarfClassType::~SgAsmDwarfClassType() {
    destructorHelper();
}

SgAsmDwarfClassType::SgAsmDwarfClassType()
    : p_body(nullptr) {}

// The association between constructor arguments and their classes:
//    property=nesting_level    class=SgAsmDwarfConstruct
//    property=offset           class=SgAsmDwarfConstruct
//    property=overall_offset   class=SgAsmDwarfConstruct
SgAsmDwarfClassType::SgAsmDwarfClassType(int const& nesting_level,
                                         uint64_t const& offset,
                                         uint64_t const& overall_offset)
    : SgAsmDwarfConstruct(nesting_level, offset, overall_offset)
    , p_body(nullptr) {}

void
SgAsmDwarfClassType::initializeProperties() {
    p_body = nullptr;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
