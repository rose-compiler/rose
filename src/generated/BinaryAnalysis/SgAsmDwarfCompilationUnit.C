//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmDwarfCompilationUnit            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmDwarfCompilationUnit_IMPL
#include <SgAsmDwarfCompilationUnit.h>

std::string const&
SgAsmDwarfCompilationUnit::get_producer() const {
    return p_producer;
}

void
SgAsmDwarfCompilationUnit::set_producer(std::string const& x) {
    this->p_producer = x;
    set_isModified(true);
}

std::string const&
SgAsmDwarfCompilationUnit::get_language() const {
    return p_language;
}

void
SgAsmDwarfCompilationUnit::set_language(std::string const& x) {
    this->p_language = x;
    set_isModified(true);
}

uint64_t const&
SgAsmDwarfCompilationUnit::get_low_pc() const {
    return p_low_pc;
}

void
SgAsmDwarfCompilationUnit::set_low_pc(uint64_t const& x) {
    this->p_low_pc = x;
    set_isModified(true);
}

uint64_t const&
SgAsmDwarfCompilationUnit::get_hi_pc() const {
    return p_hi_pc;
}

void
SgAsmDwarfCompilationUnit::set_hi_pc(uint64_t const& x) {
    this->p_hi_pc = x;
    set_isModified(true);
}

int const&
SgAsmDwarfCompilationUnit::get_version_stamp() const {
    return p_version_stamp;
}

void
SgAsmDwarfCompilationUnit::set_version_stamp(int const& x) {
    this->p_version_stamp = x;
    set_isModified(true);
}

uint64_t const&
SgAsmDwarfCompilationUnit::get_abbrev_offset() const {
    return p_abbrev_offset;
}

void
SgAsmDwarfCompilationUnit::set_abbrev_offset(uint64_t const& x) {
    this->p_abbrev_offset = x;
    set_isModified(true);
}

uint64_t const&
SgAsmDwarfCompilationUnit::get_address_size() const {
    return p_address_size;
}

void
SgAsmDwarfCompilationUnit::set_address_size(uint64_t const& x) {
    this->p_address_size = x;
    set_isModified(true);
}

uint64_t const&
SgAsmDwarfCompilationUnit::get_offset_length() const {
    return p_offset_length;
}

void
SgAsmDwarfCompilationUnit::set_offset_length(uint64_t const& x) {
    this->p_offset_length = x;
    set_isModified(true);
}

SgAsmDwarfLineList* const&
SgAsmDwarfCompilationUnit::get_line_info() const {
    return p_line_info;
}

void
SgAsmDwarfCompilationUnit::set_line_info(SgAsmDwarfLineList* const& x) {
    changeChildPointer(this->p_line_info, const_cast<SgAsmDwarfLineList*&>(x));
    set_isModified(true);
}

SgAsmDwarfConstructList* const&
SgAsmDwarfCompilationUnit::get_language_constructs() const {
    return p_language_constructs;
}

void
SgAsmDwarfCompilationUnit::set_language_constructs(SgAsmDwarfConstructList* const& x) {
    changeChildPointer(this->p_language_constructs, const_cast<SgAsmDwarfConstructList*&>(x));
    set_isModified(true);
}

SgAsmDwarfMacroList* const&
SgAsmDwarfCompilationUnit::get_macro_info() const {
    return p_macro_info;
}

void
SgAsmDwarfCompilationUnit::set_macro_info(SgAsmDwarfMacroList* const& x) {
    changeChildPointer(this->p_macro_info, const_cast<SgAsmDwarfMacroList*&>(x));
    set_isModified(true);
}

SgAsmDwarfCompilationUnit::~SgAsmDwarfCompilationUnit() {
    destructorHelper();
}

SgAsmDwarfCompilationUnit::SgAsmDwarfCompilationUnit()
    : p_low_pc(0)
    , p_hi_pc(0)
    , p_version_stamp(0)
    , p_abbrev_offset(0)
    , p_address_size(0)
    , p_offset_length(0)
    , p_line_info(nullptr)
    , p_language_constructs(nullptr)
    , p_macro_info(nullptr) {}

// The association between constructor arguments and their classes:
//    property=nesting_level    class=SgAsmDwarfConstruct
//    property=offset           class=SgAsmDwarfConstruct
//    property=overall_offset   class=SgAsmDwarfConstruct
SgAsmDwarfCompilationUnit::SgAsmDwarfCompilationUnit(int const& nesting_level,
                                                     uint64_t const& offset,
                                                     uint64_t const& overall_offset)
    : SgAsmDwarfConstruct(nesting_level, offset, overall_offset)
    , p_low_pc(0)
    , p_hi_pc(0)
    , p_version_stamp(0)
    , p_abbrev_offset(0)
    , p_address_size(0)
    , p_offset_length(0)
    , p_line_info(nullptr)
    , p_language_constructs(nullptr)
    , p_macro_info(nullptr) {}

void
SgAsmDwarfCompilationUnit::initializeProperties() {
    p_low_pc = 0;
    p_hi_pc = 0;
    p_version_stamp = 0;
    p_abbrev_offset = 0;
    p_address_size = 0;
    p_offset_length = 0;
    p_line_info = nullptr;
    p_language_constructs = nullptr;
    p_macro_info = nullptr;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
