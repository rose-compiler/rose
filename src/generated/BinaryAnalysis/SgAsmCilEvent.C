//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCilEvent            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmCilEvent_IMPL
#include <SgAsmCilEvent.h>

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

SgAsmCilEvent::SgAsmCilEvent()
    : p_EventFlags(0)
    , p_Name(0)
    , p_EventType(0) {}

void
SgAsmCilEvent::initializeProperties() {
    p_EventFlags = 0;
    p_Name = 0;
    p_EventType = 0;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
