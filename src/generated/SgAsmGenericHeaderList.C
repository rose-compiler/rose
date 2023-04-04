#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmGenericHeaderList            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericHeaderPtrList const&
SgAsmGenericHeaderList::get_headers() const {
    return p_headers;
}

#line 552 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericHeaderPtrList&
SgAsmGenericHeaderList::get_headers() {
    return p_headers;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmGenericHeaderList::set_headers(SgAsmGenericHeaderPtrList const& x) {
    this->p_headers = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericHeaderList::~SgAsmGenericHeaderList() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericHeaderList::SgAsmGenericHeaderList() {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmGenericHeaderList::initializeProperties() {
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
