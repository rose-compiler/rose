//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmPointerType            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmPointerType_IMPL
#include <SgAsmPointerType.h>

SgAsmType * const&
SgAsmPointerType::get_subtype() const {
    return p_subtype;
}

void
SgAsmPointerType::set_subtype(SgAsmType * const& x) {
    changeChildPointer(this->p_subtype, const_cast<SgAsmType *&>(x));
    set_isModified(true);
}

SgAsmPointerType::~SgAsmPointerType() {
    destructorHelper();
}

SgAsmPointerType::SgAsmPointerType() {}

void
SgAsmPointerType::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
