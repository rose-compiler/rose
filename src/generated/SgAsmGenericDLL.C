#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmGenericDLL            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericString* const&
SgAsmGenericDLL::get_name() const {
    return p_name;
}

#line 546 "src/Rosebud/RosettaGenerator.C"
SgStringList const&
SgAsmGenericDLL::get_symbols() const {
    return p_symbols;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmGenericDLL::set_symbols(SgStringList const& x) {
    this->p_symbols = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericDLL::~SgAsmGenericDLL() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericDLL::SgAsmGenericDLL()
#line 308 "src/Rosebud/RosettaGenerator.C"
    : p_name(createAndParent<SgAsmBasicString>(this)) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmGenericDLL::initializeProperties() {
    p_name = createAndParent<SgAsmBasicString>(this);
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
