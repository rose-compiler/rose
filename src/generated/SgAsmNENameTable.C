//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmNENameTable            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

SgStringList const&
SgAsmNENameTable::get_names() const {
    return p_names;
}

void
SgAsmNENameTable::set_names(SgStringList const& x) {
    this->p_names = x;
    set_isModified(true);
}

SgUnsignedList const&
SgAsmNENameTable::get_ordinals() const {
    return p_ordinals;
}

void
SgAsmNENameTable::set_ordinals(SgUnsignedList const& x) {
    this->p_ordinals = x;
    set_isModified(true);
}

SgAsmNENameTable::~SgAsmNENameTable() {
    destructorHelper();
}

SgAsmNENameTable::SgAsmNENameTable() {}

void
SgAsmNENameTable::initializeProperties() {
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
