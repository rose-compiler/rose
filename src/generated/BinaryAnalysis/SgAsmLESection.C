//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmLESection            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmLESection_IMPL
#include <sage3basic.h>

SgAsmLESectionTableEntry* const&
SgAsmLESection::get_sectionTableEntry() const {
    return p_sectionTableEntry;
}

void
SgAsmLESection::set_sectionTableEntry(SgAsmLESectionTableEntry* const& x) {
    changeChildPointer(this->p_sectionTableEntry, const_cast<SgAsmLESectionTableEntry*&>(x));
    set_isModified(true);
}

SgAsmLESection::~SgAsmLESection() {
    destructorHelper();
}

SgAsmLESection::SgAsmLESection()
    : p_sectionTableEntry(nullptr) {}

void
SgAsmLESection::initializeProperties() {
    p_sectionTableEntry = nullptr;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
