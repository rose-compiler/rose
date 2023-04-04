#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmDwarfCompilationUnit            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
std::string const&
SgAsmDwarfCompilationUnit::get_producer() const {
    return p_producer;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmDwarfCompilationUnit::set_producer(std::string const& x) {
    this->p_producer = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
std::string const&
SgAsmDwarfCompilationUnit::get_language() const {
    return p_language;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmDwarfCompilationUnit::set_language(std::string const& x) {
    this->p_language = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
uint64_t const&
SgAsmDwarfCompilationUnit::get_low_pc() const {
    return p_low_pc;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmDwarfCompilationUnit::set_low_pc(uint64_t const& x) {
    this->p_low_pc = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
uint64_t const&
SgAsmDwarfCompilationUnit::get_hi_pc() const {
    return p_hi_pc;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmDwarfCompilationUnit::set_hi_pc(uint64_t const& x) {
    this->p_hi_pc = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
int const&
SgAsmDwarfCompilationUnit::get_version_stamp() const {
    return p_version_stamp;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmDwarfCompilationUnit::set_version_stamp(int const& x) {
    this->p_version_stamp = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
uint64_t const&
SgAsmDwarfCompilationUnit::get_abbrev_offset() const {
    return p_abbrev_offset;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmDwarfCompilationUnit::set_abbrev_offset(uint64_t const& x) {
    this->p_abbrev_offset = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
uint64_t const&
SgAsmDwarfCompilationUnit::get_address_size() const {
    return p_address_size;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmDwarfCompilationUnit::set_address_size(uint64_t const& x) {
    this->p_address_size = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
uint64_t const&
SgAsmDwarfCompilationUnit::get_offset_length() const {
    return p_offset_length;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmDwarfCompilationUnit::set_offset_length(uint64_t const& x) {
    this->p_offset_length = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmDwarfLineList* const&
SgAsmDwarfCompilationUnit::get_line_info() const {
    return p_line_info;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmDwarfCompilationUnit::set_line_info(SgAsmDwarfLineList* const& x) {
    this->p_line_info = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmDwarfConstructList* const&
SgAsmDwarfCompilationUnit::get_language_constructs() const {
    return p_language_constructs;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmDwarfCompilationUnit::set_language_constructs(SgAsmDwarfConstructList* const& x) {
    this->p_language_constructs = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmDwarfMacroList* const&
SgAsmDwarfCompilationUnit::get_macro_info() const {
    return p_macro_info;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmDwarfCompilationUnit::set_macro_info(SgAsmDwarfMacroList* const& x) {
    this->p_macro_info = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmDwarfCompilationUnit::~SgAsmDwarfCompilationUnit() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmDwarfCompilationUnit::SgAsmDwarfCompilationUnit()
#line 308 "src/Rosebud/RosettaGenerator.C"
    : p_low_pc(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_hi_pc(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_version_stamp(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_abbrev_offset(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_address_size(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_offset_length(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_line_info(nullptr)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_language_constructs(nullptr)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_macro_info(nullptr) {}

#line 349 "src/Rosebud/RosettaGenerator.C"
// The association between constructor arguments and their classes:
//    property=nesting_level    class=SgAsmDwarfConstruct
//    property=offset           class=SgAsmDwarfConstruct
//    property=overall_offset   class=SgAsmDwarfConstruct
#line 357 "src/Rosebud/RosettaGenerator.C"
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

#line 448 "src/Rosebud/RosettaGenerator.C"
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

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
