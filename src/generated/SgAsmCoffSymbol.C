//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCoffSymbol                -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

std::string const&
SgAsmCoffSymbol::get_st_name() const {
    return p_st_name;
}

void
SgAsmCoffSymbol::set_st_name(std::string const& x) {
    this->p_st_name = x;
    set_isModified(true);
}

rose_addr_t const&
SgAsmCoffSymbol::get_st_name_offset() const {
    return p_st_name_offset;
}

void
SgAsmCoffSymbol::set_st_name_offset(rose_addr_t const& x) {
    this->p_st_name_offset = x;
    set_isModified(true);
}

int const&
SgAsmCoffSymbol::get_st_section_num() const {
    return p_st_section_num;
}

void
SgAsmCoffSymbol::set_st_section_num(int const& x) {
    this->p_st_section_num = x;
    set_isModified(true);
}

unsigned const&
SgAsmCoffSymbol::get_st_type() const {
    return p_st_type;
}

void
SgAsmCoffSymbol::set_st_type(unsigned const& x) {
    this->p_st_type = x;
    set_isModified(true);
}

unsigned const&
SgAsmCoffSymbol::get_st_storage_class() const {
    return p_st_storage_class;
}

void
SgAsmCoffSymbol::set_st_storage_class(unsigned const& x) {
    this->p_st_storage_class = x;
    set_isModified(true);
}

unsigned const&
SgAsmCoffSymbol::get_st_num_aux_entries() const {
    return p_st_num_aux_entries;
}

void
SgAsmCoffSymbol::set_st_num_aux_entries(unsigned const& x) {
    this->p_st_num_aux_entries = x;
    set_isModified(true);
}

SgUnsignedCharList const&
SgAsmCoffSymbol::get_aux_data() const {
    return p_aux_data;
}

SgUnsignedCharList&
SgAsmCoffSymbol::get_aux_data() {
    return p_aux_data;
}

void
SgAsmCoffSymbol::set_aux_data(SgUnsignedCharList const& x) {
    this->p_aux_data = x;
    set_isModified(true);
}

SgAsmCoffSymbol::~SgAsmCoffSymbol() {
    destructorHelper();
}

SgAsmCoffSymbol::SgAsmCoffSymbol()
    : p_st_name_offset(0)
    , p_st_section_num(0)
    , p_st_type(0)
    , p_st_storage_class(0)
    , p_st_num_aux_entries(0) {}

void
SgAsmCoffSymbol::initializeProperties() {
    p_st_name_offset = 0;
    p_st_section_num = 0;
    p_st_type = 0;
    p_st_storage_class = 0;
    p_st_num_aux_entries = 0;
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
