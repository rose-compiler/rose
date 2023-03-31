//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmIntegerValueExpression                -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

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

#endif // ROSE_ENABLE_BINARY_ANALYSIS
