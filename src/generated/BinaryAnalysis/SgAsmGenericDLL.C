//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmGenericDLL            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmGenericDLL_IMPL
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
    : p_name(createAndParent<SgAsmBasicString>(this)) {}

void
SgAsmGenericDLL::initializeProperties() {
    p_name = createAndParent<SgAsmBasicString>(this);
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
