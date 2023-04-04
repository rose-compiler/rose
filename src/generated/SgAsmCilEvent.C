#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCilEvent            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
uint16_t const&
SgAsmCilEvent::get_EventFlags() const {
    return p_EventFlags;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmCilEvent::set_EventFlags(uint16_t const& x) {
    this->p_EventFlags = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
uint32_t const&
SgAsmCilEvent::get_Name() const {
    return p_Name;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmCilEvent::set_Name(uint32_t const& x) {
    this->p_Name = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
uint32_t const&
SgAsmCilEvent::get_EventType() const {
    return p_EventType;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmCilEvent::set_EventType(uint32_t const& x) {
    this->p_EventType = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmCilEvent::~SgAsmCilEvent() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmCilEvent::SgAsmCilEvent() {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmCilEvent::initializeProperties() {
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
