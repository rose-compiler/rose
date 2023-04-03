#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmStatement            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
rose_addr_t const&
SgAsmStatement::get_address() const {
    return p_address;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmStatement::set_address(rose_addr_t const& x) {
    this->p_address = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
std::string const&
SgAsmStatement::get_comment() const {
    return p_comment;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmStatement::set_comment(std::string const& x) {
    this->p_comment = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmStatement::~SgAsmStatement() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmStatement::SgAsmStatement()
    : p_address(0) {}

#line 305 "src/Rosebud/RosettaGenerator.C"
// The association between constructor arguments and their classes:
//    property=address          class=SgAsmStatement
#line 313 "src/Rosebud/RosettaGenerator.C"
SgAsmStatement::SgAsmStatement(rose_addr_t const& address)
    : p_address(address) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmStatement::initializeProperties() {
    p_address = 0;
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
