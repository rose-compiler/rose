#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCilClassLayout            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
uint16_t const&
SgAsmCilClassLayout::get_PackingSize() const {
    return p_PackingSize;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmCilClassLayout::set_PackingSize(uint16_t const& x) {
    this->p_PackingSize = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
uint32_t const&
SgAsmCilClassLayout::get_ClassSize() const {
    return p_ClassSize;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmCilClassLayout::set_ClassSize(uint32_t const& x) {
    this->p_ClassSize = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
uint32_t const&
SgAsmCilClassLayout::get_Parent() const {
    return p_Parent;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmCilClassLayout::set_Parent(uint32_t const& x) {
    this->p_Parent = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmCilClassLayout::~SgAsmCilClassLayout() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmCilClassLayout::SgAsmCilClassLayout() {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmCilClassLayout::initializeProperties() {
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
