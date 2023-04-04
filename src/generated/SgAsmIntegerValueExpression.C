#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmIntegerValueExpression            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
SgNode* const&
SgAsmIntegerValueExpression::get_baseNode() const {
    return p_baseNode;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmIntegerValueExpression::set_baseNode(SgNode* const& x) {
    this->p_baseNode = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmIntegerValueExpression::~SgAsmIntegerValueExpression() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmIntegerValueExpression::SgAsmIntegerValueExpression()
#line 308 "src/Rosebud/RosettaGenerator.C"
    : p_baseNode(nullptr) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmIntegerValueExpression::initializeProperties() {
    p_baseNode = nullptr;
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
