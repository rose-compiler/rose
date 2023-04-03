#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmGenericDLL            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericString* const&
SgAsmGenericDLL::get_name() const {
    return p_name;
}

#line 487 "src/Rosebud/RosettaGenerator.C"
SgStringList const&
SgAsmGenericDLL::get_symbols() const {
    return p_symbols;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmGenericDLL::set_symbols(SgStringList const& x) {
    this->p_symbols = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericDLL::~SgAsmGenericDLL() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericDLL::SgAsmGenericDLL()
    : p_name(createAndParent<SgAsmGenericString>(this)) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmGenericDLL::initializeProperties() {
    p_name = createAndParent<SgAsmGenericString>(this);
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
