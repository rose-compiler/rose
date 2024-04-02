//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmValueExpression            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmValueExpression_IMPL
#include <SgAsmValueExpression.h>

SgAsmValueExpression* const&
SgAsmValueExpression::get_unfoldedExpression() const {
    return p_unfoldedExpression;
}

void
SgAsmValueExpression::set_unfoldedExpression(SgAsmValueExpression* const& x) {
    changeChildPointer(this->p_unfoldedExpression, const_cast<SgAsmValueExpression*&>(x));
    set_isModified(true);
}

unsigned short const&
SgAsmValueExpression::get_bitOffset() const {
    return p_bitOffset;
}

void
SgAsmValueExpression::set_bitOffset(unsigned short const& x) {
    this->p_bitOffset = x;
    set_isModified(true);
}

unsigned short const&
SgAsmValueExpression::get_bitSize() const {
    return p_bitSize;
}

void
SgAsmValueExpression::set_bitSize(unsigned short const& x) {
    this->p_bitSize = x;
    set_isModified(true);
}

SgSymbol* const&
SgAsmValueExpression::get_symbol() const {
    return p_symbol;
}

void
SgAsmValueExpression::set_symbol(SgSymbol* const& x) {
    this->p_symbol = x;
    set_isModified(true);
}

SgAsmValueExpression::~SgAsmValueExpression() {
    destructorHelper();
}

SgAsmValueExpression::SgAsmValueExpression()
    : p_unfoldedExpression(nullptr)
    , p_bitOffset(0)
    , p_bitSize(0)
    , p_symbol(nullptr) {}

void
SgAsmValueExpression::initializeProperties() {
    p_unfoldedExpression = nullptr;
    p_bitOffset = 0;
    p_bitSize = 0;
    p_symbol = nullptr;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
