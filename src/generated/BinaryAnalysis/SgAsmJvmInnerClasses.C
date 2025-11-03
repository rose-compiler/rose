//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmInnerClasses            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmJvmInnerClasses_IMPL
#include <SgAsmJvmInnerClasses.h>

std::vector<SgAsmJvmInnerClasses::Entry*> const&
SgAsmJvmInnerClasses::get_classes() const {
    return p_classes;
}

std::vector<SgAsmJvmInnerClasses::Entry*>&
SgAsmJvmInnerClasses::get_classes() {
    return p_classes;
}

SgAsmJvmInnerClasses::~SgAsmJvmInnerClasses() {
    destructorHelper();
}

SgAsmJvmInnerClasses::SgAsmJvmInnerClasses() {}

void
SgAsmJvmInnerClasses::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
