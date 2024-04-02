//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmLENameTable            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmLENameTable_IMPL
#include <sage3basic.h>

SgStringList const&
SgAsmLENameTable::get_names() const {
    return p_names;
}

void
SgAsmLENameTable::set_names(SgStringList const& x) {
    this->p_names = x;
    set_isModified(true);
}

SgUnsignedList const&
SgAsmLENameTable::get_ordinals() const {
    return p_ordinals;
}

void
SgAsmLENameTable::set_ordinals(SgUnsignedList const& x) {
    this->p_ordinals = x;
    set_isModified(true);
}

SgAsmLENameTable::~SgAsmLENameTable() {
    destructorHelper();
}

SgAsmLENameTable::SgAsmLENameTable() {}

void
SgAsmLENameTable::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
