//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmGenericHeaderList            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmGenericHeaderList_IMPL
#include <sage3basic.h>

SgAsmGenericHeaderPtrList const&
SgAsmGenericHeaderList::get_headers() const {
    return p_headers;
}

SgAsmGenericHeaderPtrList&
SgAsmGenericHeaderList::get_headers() {
    return p_headers;
}

void
SgAsmGenericHeaderList::set_headers(SgAsmGenericHeaderPtrList const& x) {
    changeChildPointer(this->p_headers, const_cast<SgAsmGenericHeaderPtrList&>(x));
    set_isModified(true);
}

SgAsmGenericHeaderList::~SgAsmGenericHeaderList() {
    destructorHelper();
}

SgAsmGenericHeaderList::SgAsmGenericHeaderList() {}

void
SgAsmGenericHeaderList::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
