#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmExpression            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmType* const&
SgAsmExpression::get_type() const {
    return p_type;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmExpression::set_type(SgAsmType* const& x) {
    this->p_type = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
std::string const&
SgAsmExpression::get_comment() const {
    return p_comment;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmExpression::set_comment(std::string const& x) {
    this->p_comment = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmExpression::~SgAsmExpression() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmExpression::SgAsmExpression()
    : p_type(nullptr) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmExpression::initializeProperties() {
    p_type = nullptr;
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
