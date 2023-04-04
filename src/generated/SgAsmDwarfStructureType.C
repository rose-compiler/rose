//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmDwarfStructureType            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

SgAsmDwarfConstructList* const&
SgAsmDwarfStructureType::get_body() const {
    return p_body;
}

void
SgAsmDwarfStructureType::set_body(SgAsmDwarfConstructList* const& x) {
    this->p_body = x;
    set_isModified(true);
}

SgAsmDwarfStructureType::~SgAsmDwarfStructureType() {
    destructorHelper();
}

SgAsmDwarfStructureType::SgAsmDwarfStructureType()
    : p_body(nullptr) {}

// The association between constructor arguments and their classes:
//    property=nesting_level    class=SgAsmDwarfConstruct
//    property=offset           class=SgAsmDwarfConstruct
//    property=overall_offset   class=SgAsmDwarfConstruct
SgAsmDwarfStructureType::SgAsmDwarfStructureType(int const& nesting_level,
                                                 uint64_t const& offset,
                                                 uint64_t const& overall_offset)
    : SgAsmDwarfConstruct(nesting_level, offset, overall_offset) {}

void
SgAsmDwarfStructureType::initializeProperties() {
    p_body = nullptr;
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
