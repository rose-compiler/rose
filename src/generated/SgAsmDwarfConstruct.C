#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmDwarfConstruct            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
int const&
SgAsmDwarfConstruct::get_nesting_level() const {
    return p_nesting_level;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmDwarfConstruct::set_nesting_level(int const& x) {
    this->p_nesting_level = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
uint64_t const&
SgAsmDwarfConstruct::get_offset() const {
    return p_offset;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmDwarfConstruct::set_offset(uint64_t const& x) {
    this->p_offset = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
uint64_t const&
SgAsmDwarfConstruct::get_overall_offset() const {
    return p_overall_offset;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmDwarfConstruct::set_overall_offset(uint64_t const& x) {
    this->p_overall_offset = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
std::string const&
SgAsmDwarfConstruct::get_name() const {
    return p_name;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmDwarfConstruct::set_name(std::string const& x) {
    this->p_name = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmDwarfLine* const&
SgAsmDwarfConstruct::get_source_position() const {
    return p_source_position;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmDwarfConstruct::set_source_position(SgAsmDwarfLine* const& x) {
    this->p_source_position = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmDwarfConstruct::~SgAsmDwarfConstruct() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmDwarfConstruct::SgAsmDwarfConstruct()
    : p_nesting_level(0)
    , p_offset(0)
    , p_overall_offset(0)
    , p_source_position(nullptr) {}

#line 305 "src/Rosebud/RosettaGenerator.C"
// The association between constructor arguments and their classes:
//    property=nesting_level    class=SgAsmDwarfConstruct
//    property=offset           class=SgAsmDwarfConstruct
//    property=overall_offset   class=SgAsmDwarfConstruct
#line 313 "src/Rosebud/RosettaGenerator.C"
SgAsmDwarfConstruct::SgAsmDwarfConstruct(int const& nesting_level,
                                         uint64_t const& offset,
                                         uint64_t const& overall_offset)
    : p_nesting_level(nesting_level)
    , p_offset(offset)
    , p_overall_offset(overall_offset) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmDwarfConstruct::initializeProperties() {
    p_nesting_level = 0;
    p_offset = 0;
    p_overall_offset = 0;
    p_source_position = nullptr;
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
