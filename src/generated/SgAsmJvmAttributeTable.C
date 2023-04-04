#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmAttributeTable            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmAttributePtrList const&
SgAsmJvmAttributeTable::get_attributes() const {
    return p_attributes;
}

#line 552 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmAttributePtrList&
SgAsmJvmAttributeTable::get_attributes() {
    return p_attributes;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmAttributeTable::set_attributes(SgAsmJvmAttributePtrList const& x) {
    this->p_attributes = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmAttributeTable::~SgAsmJvmAttributeTable() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmAttributeTable::SgAsmJvmAttributeTable() {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmAttributeTable::initializeProperties() {
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
