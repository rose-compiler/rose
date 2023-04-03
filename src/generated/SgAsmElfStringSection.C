#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfStringSection            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmElfStrtab* const&
SgAsmElfStringSection::get_strtab() const {
    return p_strtab;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfStringSection::set_strtab(SgAsmElfStrtab* const& x) {
    this->p_strtab = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmElfStringSection::~SgAsmElfStringSection() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmElfStringSection::SgAsmElfStringSection()
    : p_strtab(nullptr) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfStringSection::initializeProperties() {
    p_strtab = nullptr;
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
