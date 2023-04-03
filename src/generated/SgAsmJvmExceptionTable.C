#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmExceptionTable            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmExceptionPtrList const&
SgAsmJvmExceptionTable::get_exceptions() const {
    return p_exceptions;
}

#line 493 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmExceptionPtrList&
SgAsmJvmExceptionTable::get_exceptions() {
    return p_exceptions;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmExceptionTable::set_exceptions(SgAsmJvmExceptionPtrList const& x) {
    this->p_exceptions = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmExceptionTable::~SgAsmJvmExceptionTable() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmExceptionTable::SgAsmJvmExceptionTable() {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmExceptionTable::initializeProperties() {
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
