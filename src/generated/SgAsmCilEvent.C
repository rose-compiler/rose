//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCilEvent            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#include <sage3basic.h>

uint16_t const&
SgAsmCilEvent::get_EventFlags() const {
    return p_EventFlags;
}

void
SgAsmCilEvent::set_EventFlags(uint16_t const& x) {
    this->p_EventFlags = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilEvent::get_Name() const {
    return p_Name;
}

void
SgAsmCilEvent::set_Name(uint32_t const& x) {
    this->p_Name = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilEvent::get_EventType() const {
    return p_EventType;
}

void
SgAsmCilEvent::set_EventType(uint32_t const& x) {
    this->p_EventType = x;
    set_isModified(true);
}

SgAsmCilEvent::~SgAsmCilEvent() {
    destructorHelper();
}

SgAsmCilEvent::SgAsmCilEvent() {}

void
SgAsmCilEvent::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
