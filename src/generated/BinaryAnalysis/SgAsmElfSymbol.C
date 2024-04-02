//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfSymbol            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmElfSymbol_IMPL
#include <SgAsmElfSymbol.h>

unsigned char const&
SgAsmElfSymbol::get_st_info() const {
    return p_st_info;
}

void
SgAsmElfSymbol::set_st_info(unsigned char const& x) {
    this->p_st_info = x;
    set_isModified(true);
}

unsigned char const&
SgAsmElfSymbol::get_st_res1() const {
    return p_st_res1;
}

void
SgAsmElfSymbol::set_st_res1(unsigned char const& x) {
    this->p_st_res1 = x;
    set_isModified(true);
}

unsigned const&
SgAsmElfSymbol::get_st_shndx() const {
    return p_st_shndx;
}

void
SgAsmElfSymbol::set_st_shndx(unsigned const& x) {
    this->p_st_shndx = x;
    set_isModified(true);
}

rose_addr_t const&
SgAsmElfSymbol::get_st_size() const {
    return p_st_size;
}

void
SgAsmElfSymbol::set_st_size(rose_addr_t const& x) {
    this->p_st_size = x;
    set_isModified(true);
}

SgUnsignedCharList const&
SgAsmElfSymbol::get_extra() const {
    return p_extra;
}

SgUnsignedCharList&
SgAsmElfSymbol::get_extra() {
    return p_extra;
}

void
SgAsmElfSymbol::set_extra(SgUnsignedCharList const& x) {
    this->p_extra = x;
    set_isModified(true);
}

SgAsmElfSymbol::~SgAsmElfSymbol() {
    destructorHelper();
}

SgAsmElfSymbol::SgAsmElfSymbol()
    : p_st_info(0)
    , p_st_res1(0)
    , p_st_shndx(0)
    , p_st_size(0) {}

void
SgAsmElfSymbol::initializeProperties() {
    p_st_info = 0;
    p_st_res1 = 0;
    p_st_shndx = 0;
    p_st_size = 0;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
