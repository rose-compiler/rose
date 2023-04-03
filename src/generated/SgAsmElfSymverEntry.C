#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfSymverEntry            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
size_t const&
SgAsmElfSymverEntry::get_value() const {
    return p_value;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSymverEntry::set_value(size_t const& x) {
    this->p_value = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmElfSymverEntry::~SgAsmElfSymverEntry() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmElfSymverEntry::SgAsmElfSymverEntry()
    : p_value(0) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSymverEntry::initializeProperties() {
    p_value = 0;
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
