#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmStatement            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
rose_addr_t const&
SgAsmStatement::get_address() const {
    return p_address;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmStatement::set_address(rose_addr_t const& x) {
    this->p_address = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
std::string const&
SgAsmStatement::get_comment() const {
    return p_comment;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmStatement::set_comment(std::string const& x) {
    this->p_comment = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmStatement::~SgAsmStatement() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmStatement::SgAsmStatement()
#line 308 "src/Rosebud/RosettaGenerator.C"
    : p_address(0) {}

#line 349 "src/Rosebud/RosettaGenerator.C"
// The association between constructor arguments and their classes:
//    property=address          class=SgAsmStatement
#line 357 "src/Rosebud/RosettaGenerator.C"
SgAsmStatement::SgAsmStatement(rose_addr_t const& address)
    : p_address(address) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmStatement::initializeProperties() {
    p_address = 0;
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
