#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmSynthesizedFieldDeclaration            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
std::string const&
SgAsmSynthesizedFieldDeclaration::get_name() const {
    return p_name;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmSynthesizedFieldDeclaration::set_name(std::string const& x) {
    this->p_name = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
uint64_t const&
SgAsmSynthesizedFieldDeclaration::get_offset() const {
    return p_offset;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmSynthesizedFieldDeclaration::set_offset(uint64_t const& x) {
    this->p_offset = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmSynthesizedFieldDeclaration::~SgAsmSynthesizedFieldDeclaration() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmSynthesizedFieldDeclaration::SgAsmSynthesizedFieldDeclaration()
    : p_offset(0) {}

#line 305 "src/Rosebud/RosettaGenerator.C"
// The association between constructor arguments and their classes:
//    property=address          class=SgAsmStatement
#line 313 "src/Rosebud/RosettaGenerator.C"
SgAsmSynthesizedFieldDeclaration::SgAsmSynthesizedFieldDeclaration(rose_addr_t const& address)
    : SgAsmSynthesizedDeclaration(address) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmSynthesizedFieldDeclaration::initializeProperties() {
    p_offset = 0;
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
