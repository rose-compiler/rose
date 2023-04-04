#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCilField            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
uint16_t const&
SgAsmCilField::get_Flags() const {
    return p_Flags;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmCilField::set_Flags(uint16_t const& x) {
    this->p_Flags = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
uint32_t const&
SgAsmCilField::get_Name() const {
    return p_Name;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmCilField::set_Name(uint32_t const& x) {
    this->p_Name = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
uint32_t const&
SgAsmCilField::get_Signature() const {
    return p_Signature;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmCilField::set_Signature(uint32_t const& x) {
    this->p_Signature = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmCilField::~SgAsmCilField() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmCilField::SgAsmCilField() {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmCilField::initializeProperties() {
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
