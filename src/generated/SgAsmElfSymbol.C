#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfSymbol            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
unsigned char const&
SgAsmElfSymbol::get_st_info() const {
    return p_st_info;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSymbol::set_st_info(unsigned char const& x) {
    this->p_st_info = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
unsigned char const&
SgAsmElfSymbol::get_st_res1() const {
    return p_st_res1;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSymbol::set_st_res1(unsigned char const& x) {
    this->p_st_res1 = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
unsigned const&
SgAsmElfSymbol::get_st_shndx() const {
    return p_st_shndx;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSymbol::set_st_shndx(unsigned const& x) {
    this->p_st_shndx = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
rose_addr_t const&
SgAsmElfSymbol::get_st_size() const {
    return p_st_size;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSymbol::set_st_size(rose_addr_t const& x) {
    this->p_st_size = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
SgUnsignedCharList const&
SgAsmElfSymbol::get_extra() const {
    return p_extra;
}

#line 493 "src/Rosebud/RosettaGenerator.C"
SgUnsignedCharList&
SgAsmElfSymbol::get_extra() {
    return p_extra;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSymbol::set_extra(SgUnsignedCharList const& x) {
    this->p_extra = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmElfSymbol::~SgAsmElfSymbol() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmElfSymbol::SgAsmElfSymbol()
    : p_st_info(0)
    , p_st_res1(0)
    , p_st_shndx(0)
    , p_st_size(0) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSymbol::initializeProperties() {
    p_st_info = 0;
    p_st_res1 = 0;
    p_st_shndx = 0;
    p_st_size = 0;
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
