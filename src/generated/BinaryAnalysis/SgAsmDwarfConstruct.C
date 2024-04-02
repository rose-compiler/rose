//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmDwarfConstruct            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmDwarfConstruct_IMPL
#include <SgAsmDwarfConstruct.h>

int const&
SgAsmDwarfConstruct::get_nesting_level() const {
    return p_nesting_level;
}

void
SgAsmDwarfConstruct::set_nesting_level(int const& x) {
    this->p_nesting_level = x;
    set_isModified(true);
}

uint64_t const&
SgAsmDwarfConstruct::get_offset() const {
    return p_offset;
}

void
SgAsmDwarfConstruct::set_offset(uint64_t const& x) {
    this->p_offset = x;
    set_isModified(true);
}

uint64_t const&
SgAsmDwarfConstruct::get_overall_offset() const {
    return p_overall_offset;
}

void
SgAsmDwarfConstruct::set_overall_offset(uint64_t const& x) {
    this->p_overall_offset = x;
    set_isModified(true);
}

std::string const&
SgAsmDwarfConstruct::get_name() const {
    return p_name;
}

void
SgAsmDwarfConstruct::set_name(std::string const& x) {
    this->p_name = x;
    set_isModified(true);
}

SgAsmDwarfLine* const&
SgAsmDwarfConstruct::get_source_position() const {
    return p_source_position;
}

void
SgAsmDwarfConstruct::set_source_position(SgAsmDwarfLine* const& x) {
    this->p_source_position = x;
    set_isModified(true);
}

SgAsmDwarfConstruct::~SgAsmDwarfConstruct() {
    destructorHelper();
}

SgAsmDwarfConstruct::SgAsmDwarfConstruct()
    : p_nesting_level(0)
    , p_offset(0)
    , p_overall_offset(0)
    , p_source_position(nullptr) {}

// The association between constructor arguments and their classes:
//    property=nesting_level    class=SgAsmDwarfConstruct
//    property=offset           class=SgAsmDwarfConstruct
//    property=overall_offset   class=SgAsmDwarfConstruct
SgAsmDwarfConstruct::SgAsmDwarfConstruct(int const& nesting_level,
                                         uint64_t const& offset,
                                         uint64_t const& overall_offset)
    : p_nesting_level(nesting_level)
    , p_offset(offset)
    , p_overall_offset(overall_offset)
    , p_source_position(nullptr) {}

void
SgAsmDwarfConstruct::initializeProperties() {
    p_nesting_level = 0;
    p_offset = 0;
    p_overall_offset = 0;
    p_source_position = nullptr;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
