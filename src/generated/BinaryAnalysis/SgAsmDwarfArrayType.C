//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmDwarfArrayType            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmDwarfArrayType_IMPL
#include <SgAsmDwarfArrayType.h>

SgAsmDwarfConstructList* const&
SgAsmDwarfArrayType::get_body() const {
    return p_body;
}

void
SgAsmDwarfArrayType::set_body(SgAsmDwarfConstructList* const& x) {
    changeChildPointer(this->p_body, const_cast<SgAsmDwarfConstructList*&>(x));
    set_isModified(true);
}

SgAsmDwarfArrayType::~SgAsmDwarfArrayType() {
    destructorHelper();
}

SgAsmDwarfArrayType::SgAsmDwarfArrayType()
    : p_body(nullptr) {}

// The association between constructor arguments and their classes:
//    property=nesting_level    class=SgAsmDwarfConstruct
//    property=offset           class=SgAsmDwarfConstruct
//    property=overall_offset   class=SgAsmDwarfConstruct
SgAsmDwarfArrayType::SgAsmDwarfArrayType(int const& nesting_level,
                                         uint64_t const& offset,
                                         uint64_t const& overall_offset)
    : SgAsmDwarfConstruct(nesting_level, offset, overall_offset)
    , p_body(nullptr) {}

void
SgAsmDwarfArrayType::initializeProperties() {
    p_body = nullptr;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
