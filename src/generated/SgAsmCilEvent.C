#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCilEvent            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
uint16_t const&
SgAsmCilEvent::get_EventFlags() const {
    return p_EventFlags;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmCilEvent::set_EventFlags(uint16_t const& x) {
    this->p_EventFlags = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
uint32_t const&
SgAsmCilEvent::get_Name() const {
    return p_Name;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmCilEvent::set_Name(uint32_t const& x) {
    this->p_Name = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
uint32_t const&
SgAsmCilEvent::get_EventType() const {
    return p_EventType;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmCilEvent::set_EventType(uint32_t const& x) {
    this->p_EventType = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmCilEvent::~SgAsmCilEvent() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmCilEvent::SgAsmCilEvent() {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmCilEvent::initializeProperties() {
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
