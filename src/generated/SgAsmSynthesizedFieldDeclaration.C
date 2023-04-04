#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmSynthesizedFieldDeclaration            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
std::string const&
SgAsmSynthesizedFieldDeclaration::get_name() const {
    return p_name;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmSynthesizedFieldDeclaration::set_name(std::string const& x) {
    this->p_name = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
uint64_t const&
SgAsmSynthesizedFieldDeclaration::get_offset() const {
    return p_offset;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmSynthesizedFieldDeclaration::set_offset(uint64_t const& x) {
    this->p_offset = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmSynthesizedFieldDeclaration::~SgAsmSynthesizedFieldDeclaration() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmSynthesizedFieldDeclaration::SgAsmSynthesizedFieldDeclaration()
#line 308 "src/Rosebud/RosettaGenerator.C"
    : p_offset(0) {}

#line 349 "src/Rosebud/RosettaGenerator.C"
// The association between constructor arguments and their classes:
//    property=address          class=SgAsmStatement
#line 357 "src/Rosebud/RosettaGenerator.C"
SgAsmSynthesizedFieldDeclaration::SgAsmSynthesizedFieldDeclaration(rose_addr_t const& address)
    : SgAsmSynthesizedDeclaration(address)
    , p_offset(0) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmSynthesizedFieldDeclaration::initializeProperties() {
    p_offset = 0;
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
