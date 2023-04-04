#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmInnerClasses            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmInnerClassesEntryPtrList const&
SgAsmJvmInnerClasses::get_classes() const {
    return p_classes;
}

#line 552 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmInnerClassesEntryPtrList&
SgAsmJvmInnerClasses::get_classes() {
    return p_classes;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmInnerClasses::set_classes(SgAsmJvmInnerClassesEntryPtrList const& x) {
    this->p_classes = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmInnerClasses::~SgAsmJvmInnerClasses() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmInnerClasses::SgAsmJvmInnerClasses() {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmInnerClasses::initializeProperties() {
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
