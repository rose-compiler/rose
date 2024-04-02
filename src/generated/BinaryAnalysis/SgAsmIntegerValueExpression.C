//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmIntegerValueExpression            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmIntegerValueExpression_IMPL
#include <SgAsmIntegerValueExpression.h>

SgNode* const&
SgAsmIntegerValueExpression::get_baseNode() const {
    return p_baseNode;
}

void
SgAsmIntegerValueExpression::set_baseNode(SgNode* const& x) {
    this->p_baseNode = x;
    set_isModified(true);
}

SgAsmIntegerValueExpression::~SgAsmIntegerValueExpression() {
    destructorHelper();
}

SgAsmIntegerValueExpression::SgAsmIntegerValueExpression()
    : p_baseNode(nullptr) {}

void
SgAsmIntegerValueExpression::initializeProperties() {
    p_baseNode = nullptr;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
