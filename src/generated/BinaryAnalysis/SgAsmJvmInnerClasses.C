//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmInnerClasses            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmJvmInnerClasses_IMPL
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

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
