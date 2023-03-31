//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmLESection                -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

SgAsmLESectionTableEntry* const&
SgAsmLESection::get_st_entry() const {
    return p_st_entry;
}

void
SgAsmLESection::set_st_entry(SgAsmLESectionTableEntry* const& x) {
    this->p_st_entry = x;
    set_isModified(true);
}

SgAsmLESection::~SgAsmLESection() {
    destructorHelper();
}

SgAsmLESection::SgAsmLESection()
    : p_st_entry(NULL) {}

void
SgAsmLESection::initializeProperties() {
    p_st_entry = NULL;
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
