//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmExpression            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmExpression_IMPL
#include <SgAsmExpression.h>

SgAsmType* const&
SgAsmExpression::get_type() const {
    return p_type;
}

void
SgAsmExpression::set_type(SgAsmType* const& x) {
    this->p_type = x;
    set_isModified(true);
}

std::string const&
SgAsmExpression::get_comment() const {
    return p_comment;
}

void
SgAsmExpression::set_comment(std::string const& x) {
    this->p_comment = x;
    set_isModified(true);
}

SgAsmExpression::~SgAsmExpression() {
    destructorHelper();
}

SgAsmExpression::SgAsmExpression()
    : p_type(nullptr) {}

void
SgAsmExpression::initializeProperties() {
    p_type = nullptr;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
