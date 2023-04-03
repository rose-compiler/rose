#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmFieldTable            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmFieldPtrList const&
SgAsmJvmFieldTable::get_fields() const {
    return p_fields;
}

#line 493 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmFieldPtrList&
SgAsmJvmFieldTable::get_fields() {
    return p_fields;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmFieldTable::set_fields(SgAsmJvmFieldPtrList const& x) {
    this->p_fields = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmFieldTable::~SgAsmJvmFieldTable() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmFieldTable::SgAsmJvmFieldTable() {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmFieldTable::initializeProperties() {
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
