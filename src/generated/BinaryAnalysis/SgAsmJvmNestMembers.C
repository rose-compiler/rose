//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmNestMembers            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#include <sage3basic.h>

SgUnsigned16List const&
SgAsmJvmNestMembers::get_classes() const {
    return p_classes;
}

void
SgAsmJvmNestMembers::set_classes(SgUnsigned16List const& x) {
    this->p_classes = x;
    set_isModified(true);
}

SgAsmJvmNestMembers::~SgAsmJvmNestMembers() {
    destructorHelper();
}

SgAsmJvmNestMembers::SgAsmJvmNestMembers() {}

void
SgAsmJvmNestMembers::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
