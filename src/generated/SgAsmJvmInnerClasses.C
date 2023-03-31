//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmInnerClasses                -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

SgAsmJvmInnerClassesEntryPtrList const&
SgAsmJvmInnerClasses::get_classes() const {
    return p_classes;
}

SgAsmJvmInnerClassesEntryPtrList&
SgAsmJvmInnerClasses::get_classes() {
    return p_classes;
}

void
SgAsmJvmInnerClasses::set_classes(SgAsmJvmInnerClassesEntryPtrList const& x) {
    this->p_classes = x;
    set_isModified(true);
}

SgAsmJvmInnerClasses::~SgAsmJvmInnerClasses() {
    destructorHelper();
}

SgAsmJvmInnerClasses::SgAsmJvmInnerClasses() {}

void
SgAsmJvmInnerClasses::initializeProperties() {
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
