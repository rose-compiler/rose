//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmGenericDLL                -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

SgAsmGenericString* const&
SgAsmGenericDLL::get_name() const {
    return p_name;
}

SgStringList const&
SgAsmGenericDLL::get_symbols() const {
    return p_symbols;
}

void
SgAsmGenericDLL::set_symbols(SgStringList const& x) {
    this->p_symbols = x;
    set_isModified(true);
}

SgAsmGenericDLL::~SgAsmGenericDLL() {
    destructorHelper();
}

SgAsmGenericDLL::SgAsmGenericDLL()
    : p_name(createAndParent<SgAsmGenericString>(this)) {}

void
SgAsmGenericDLL::initializeProperties() {
    p_name = createAndParent<SgAsmGenericString>(this);
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
