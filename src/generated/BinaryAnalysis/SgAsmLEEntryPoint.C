//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmLEEntryPoint            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmLEEntryPoint_IMPL
#include <SgAsmLEEntryPoint.h>

SgAsmLEEntryPointPtrList const&
SgAsmLEEntryPoint::get_entries() const {
    return p_entries;
}

SgAsmLEEntryPointPtrList&
SgAsmLEEntryPoint::get_entries() {
    return p_entries;
}

void
SgAsmLEEntryPoint::set_entries(SgAsmLEEntryPointPtrList const& x) {
    changeChildPointer(this->p_entries, const_cast<SgAsmLEEntryPointPtrList&>(x));
    set_isModified(true);
}

unsigned const&
SgAsmLEEntryPoint::get_flags() const {
    return p_flags;
}

void
SgAsmLEEntryPoint::set_flags(unsigned const& x) {
    this->p_flags = x;
    set_isModified(true);
}

unsigned const&
SgAsmLEEntryPoint::get_objnum() const {
    return p_objnum;
}

void
SgAsmLEEntryPoint::set_objnum(unsigned const& x) {
    this->p_objnum = x;
    set_isModified(true);
}

unsigned const&
SgAsmLEEntryPoint::get_entry_type() const {
    return p_entry_type;
}

void
SgAsmLEEntryPoint::set_entry_type(unsigned const& x) {
    this->p_entry_type = x;
    set_isModified(true);
}

unsigned const&
SgAsmLEEntryPoint::get_res1() const {
    return p_res1;
}

void
SgAsmLEEntryPoint::set_res1(unsigned const& x) {
    this->p_res1 = x;
    set_isModified(true);
}

Rose::BinaryAnalysis::Address const&
SgAsmLEEntryPoint::get_entry_offset() const {
    return p_entry_offset;
}

void
SgAsmLEEntryPoint::set_entry_offset(Rose::BinaryAnalysis::Address const& x) {
    this->p_entry_offset = x;
    set_isModified(true);
}

SgAsmLEEntryPoint::~SgAsmLEEntryPoint() {
    destructorHelper();
}

SgAsmLEEntryPoint::SgAsmLEEntryPoint()
    : p_flags(0)
    , p_objnum(0)
    , p_entry_type(0)
    , p_res1(0)
    , p_entry_offset(0) {}

void
SgAsmLEEntryPoint::initializeProperties() {
    p_flags = 0;
    p_objnum = 0;
    p_entry_type = 0;
    p_res1 = 0;
    p_entry_offset = 0;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
