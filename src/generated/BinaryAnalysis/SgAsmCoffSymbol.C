//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCoffSymbol            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmCoffSymbol_IMPL
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
SgAsmCoffSymbol::get_auxiliaryData() const {
    return p_auxiliaryData;
}

SgUnsignedCharList&
SgAsmCoffSymbol::get_auxiliaryData() {
    return p_auxiliaryData;
}

void
SgAsmCoffSymbol::set_auxiliaryData(SgUnsignedCharList const& x) {
    this->p_auxiliaryData = x;
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

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
