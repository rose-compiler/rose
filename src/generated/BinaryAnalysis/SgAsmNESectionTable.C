//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmNESectionTable            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmNESectionTable_IMPL
#include <SgAsmNESectionTable.h>

unsigned const&
SgAsmNESectionTable::get_flags() const {
    return p_flags;
}

void
SgAsmNESectionTable::set_flags(unsigned const& x) {
    this->p_flags = x;
    set_isModified(true);
}

unsigned const&
SgAsmNESectionTable::get_sector() const {
    return p_sector;
}

void
SgAsmNESectionTable::set_sector(unsigned const& x) {
    this->p_sector = x;
    set_isModified(true);
}

Rose::BinaryAnalysis::Address const&
SgAsmNESectionTable::get_physicalSize() const {
    return p_physicalSize;
}

void
SgAsmNESectionTable::set_physicalSize(Rose::BinaryAnalysis::Address const& x) {
    this->p_physicalSize = x;
    set_isModified(true);
}

Rose::BinaryAnalysis::Address const&
SgAsmNESectionTable::get_virtualSize() const {
    return p_virtualSize;
}

void
SgAsmNESectionTable::set_virtualSize(Rose::BinaryAnalysis::Address const& x) {
    this->p_virtualSize = x;
    set_isModified(true);
}

SgAsmNESectionTable::~SgAsmNESectionTable() {
    destructorHelper();
}

SgAsmNESectionTable::SgAsmNESectionTable()
    : p_flags(0)
    , p_sector(0)
    , p_physicalSize(0)
    , p_virtualSize(0) {}

void
SgAsmNESectionTable::initializeProperties() {
    p_flags = 0;
    p_sector = 0;
    p_physicalSize = 0;
    p_virtualSize = 0;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
