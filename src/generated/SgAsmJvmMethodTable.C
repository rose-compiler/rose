#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmMethodTable            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmMethodPtrList const&
SgAsmJvmMethodTable::get_methods() const {
    return p_methods;
}

#line 493 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmMethodPtrList&
SgAsmJvmMethodTable::get_methods() {
    return p_methods;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmMethodTable::set_methods(SgAsmJvmMethodPtrList const& x) {
    this->p_methods = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmMethodTable::~SgAsmJvmMethodTable() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmMethodTable::SgAsmJvmMethodTable() {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmMethodTable::initializeProperties() {
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
