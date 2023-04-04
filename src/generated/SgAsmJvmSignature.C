#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmSignature            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
uint16_t const&
SgAsmJvmSignature::get_signature_index() const {
    return p_signature_index;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmSignature::set_signature_index(uint16_t const& x) {
    this->p_signature_index = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmSignature::~SgAsmJvmSignature() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmSignature::SgAsmJvmSignature()
#line 308 "src/Rosebud/RosettaGenerator.C"
    : p_signature_index(0) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmSignature::initializeProperties() {
    p_signature_index = 0;
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
