#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmExceptionTable            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmExceptionPtrList const&
SgAsmJvmExceptionTable::get_exceptions() const {
    return p_exceptions;
}

#line 552 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmExceptionPtrList&
SgAsmJvmExceptionTable::get_exceptions() {
    return p_exceptions;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmExceptionTable::set_exceptions(SgAsmJvmExceptionPtrList const& x) {
    this->p_exceptions = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmExceptionTable::~SgAsmJvmExceptionTable() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmExceptionTable::SgAsmJvmExceptionTable() {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmExceptionTable::initializeProperties() {
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
