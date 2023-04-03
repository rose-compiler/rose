#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmGenericHeaderList            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericHeaderPtrList const&
SgAsmGenericHeaderList::get_headers() const {
    return p_headers;
}

#line 493 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericHeaderPtrList&
SgAsmGenericHeaderList::get_headers() {
    return p_headers;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmGenericHeaderList::set_headers(SgAsmGenericHeaderPtrList const& x) {
    this->p_headers = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericHeaderList::~SgAsmGenericHeaderList() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericHeaderList::SgAsmGenericHeaderList() {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmGenericHeaderList::initializeProperties() {
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
