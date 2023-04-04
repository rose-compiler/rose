#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmExpression            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmType* const&
SgAsmExpression::get_type() const {
    return p_type;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmExpression::set_type(SgAsmType* const& x) {
    this->p_type = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
std::string const&
SgAsmExpression::get_comment() const {
    return p_comment;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmExpression::set_comment(std::string const& x) {
    this->p_comment = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmExpression::~SgAsmExpression() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmExpression::SgAsmExpression()
#line 308 "src/Rosebud/RosettaGenerator.C"
    : p_type(nullptr) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmExpression::initializeProperties() {
    p_type = nullptr;
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
