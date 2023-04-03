#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCoffSymbol            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
std::string const&
SgAsmCoffSymbol::get_st_name() const {
    return p_st_name;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmCoffSymbol::set_st_name(std::string const& x) {
    this->p_st_name = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
rose_addr_t const&
SgAsmCoffSymbol::get_st_name_offset() const {
    return p_st_name_offset;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmCoffSymbol::set_st_name_offset(rose_addr_t const& x) {
    this->p_st_name_offset = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
int const&
SgAsmCoffSymbol::get_st_section_num() const {
    return p_st_section_num;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmCoffSymbol::set_st_section_num(int const& x) {
    this->p_st_section_num = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
unsigned const&
SgAsmCoffSymbol::get_st_type() const {
    return p_st_type;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmCoffSymbol::set_st_type(unsigned const& x) {
    this->p_st_type = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
unsigned const&
SgAsmCoffSymbol::get_st_storage_class() const {
    return p_st_storage_class;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmCoffSymbol::set_st_storage_class(unsigned const& x) {
    this->p_st_storage_class = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
unsigned const&
SgAsmCoffSymbol::get_st_num_aux_entries() const {
    return p_st_num_aux_entries;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmCoffSymbol::set_st_num_aux_entries(unsigned const& x) {
    this->p_st_num_aux_entries = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
SgUnsignedCharList const&
SgAsmCoffSymbol::get_aux_data() const {
    return p_aux_data;
}

#line 493 "src/Rosebud/RosettaGenerator.C"
SgUnsignedCharList&
SgAsmCoffSymbol::get_aux_data() {
    return p_aux_data;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmCoffSymbol::set_aux_data(SgUnsignedCharList const& x) {
    this->p_aux_data = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmCoffSymbol::~SgAsmCoffSymbol() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmCoffSymbol::SgAsmCoffSymbol()
    : p_st_name_offset(0)
    , p_st_section_num(0)
    , p_st_type(0)
    , p_st_storage_class(0)
    , p_st_num_aux_entries(0) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmCoffSymbol::initializeProperties() {
    p_st_name_offset = 0;
    p_st_section_num = 0;
    p_st_type = 0;
    p_st_storage_class = 0;
    p_st_num_aux_entries = 0;
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
