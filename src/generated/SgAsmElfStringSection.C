#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfStringSection            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmElfStrtab* const&
SgAsmElfStringSection::get_strtab() const {
    return p_strtab;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfStringSection::set_strtab(SgAsmElfStrtab* const& x) {
    this->p_strtab = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmElfStringSection::~SgAsmElfStringSection() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmElfStringSection::SgAsmElfStringSection()
#line 308 "src/Rosebud/RosettaGenerator.C"
    : p_strtab(nullptr) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfStringSection::initializeProperties() {
    p_strtab = nullptr;
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
