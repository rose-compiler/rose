#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmSignature            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
uint16_t const&
SgAsmJvmSignature::get_signature_index() const {
    return p_signature_index;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmSignature::set_signature_index(uint16_t const& x) {
    this->p_signature_index = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmSignature::~SgAsmJvmSignature() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmSignature::SgAsmJvmSignature()
    : p_signature_index(0) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmSignature::initializeProperties() {
    p_signature_index = 0;
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
