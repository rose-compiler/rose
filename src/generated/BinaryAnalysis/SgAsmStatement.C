//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmStatement            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmStatement_IMPL
#include <SgAsmStatement.h>

Rose::BinaryAnalysis::Address const&
SgAsmStatement::get_address() const {
    return p_address;
}

void
SgAsmStatement::set_address(Rose::BinaryAnalysis::Address const& x) {
    this->p_address = x;
    set_isModified(true);
}

std::string const&
SgAsmStatement::get_comment() const {
    return p_comment;
}

void
SgAsmStatement::set_comment(std::string const& x) {
    this->p_comment = x;
    set_isModified(true);
}

SgAsmStatement::~SgAsmStatement() {
    destructorHelper();
}

SgAsmStatement::SgAsmStatement()
    : p_address(0) {}

// The association between constructor arguments and their classes:
//    property=address          class=SgAsmStatement
SgAsmStatement::SgAsmStatement(Rose::BinaryAnalysis::Address const& address)
    : p_address(address) {}

void
SgAsmStatement::initializeProperties() {
    p_address = 0;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
