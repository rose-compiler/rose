#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmFieldTable            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmFieldPtrList const&
SgAsmJvmFieldTable::get_fields() const {
    return p_fields;
}

#line 552 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmFieldPtrList&
SgAsmJvmFieldTable::get_fields() {
    return p_fields;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmFieldTable::set_fields(SgAsmJvmFieldPtrList const& x) {
    this->p_fields = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmFieldTable::~SgAsmJvmFieldTable() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmFieldTable::SgAsmJvmFieldTable() {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmFieldTable::initializeProperties() {
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
