#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfSymbol            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
unsigned char const&
SgAsmElfSymbol::get_st_info() const {
    return p_st_info;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSymbol::set_st_info(unsigned char const& x) {
    this->p_st_info = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
unsigned char const&
SgAsmElfSymbol::get_st_res1() const {
    return p_st_res1;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSymbol::set_st_res1(unsigned char const& x) {
    this->p_st_res1 = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
unsigned const&
SgAsmElfSymbol::get_st_shndx() const {
    return p_st_shndx;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSymbol::set_st_shndx(unsigned const& x) {
    this->p_st_shndx = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
rose_addr_t const&
SgAsmElfSymbol::get_st_size() const {
    return p_st_size;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSymbol::set_st_size(rose_addr_t const& x) {
    this->p_st_size = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
SgUnsignedCharList const&
SgAsmElfSymbol::get_extra() const {
    return p_extra;
}

#line 552 "src/Rosebud/RosettaGenerator.C"
SgUnsignedCharList&
SgAsmElfSymbol::get_extra() {
    return p_extra;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSymbol::set_extra(SgUnsignedCharList const& x) {
    this->p_extra = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmElfSymbol::~SgAsmElfSymbol() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmElfSymbol::SgAsmElfSymbol()
#line 308 "src/Rosebud/RosettaGenerator.C"
    : p_st_info(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_st_res1(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_st_shndx(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_st_size(0) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSymbol::initializeProperties() {
    p_st_info = 0;
    p_st_res1 = 0;
    p_st_shndx = 0;
    p_st_size = 0;
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
