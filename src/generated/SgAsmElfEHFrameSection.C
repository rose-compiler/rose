#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfEHFrameSection            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmElfEHFrameEntryCIList* const&
SgAsmElfEHFrameSection::get_ci_entries() const {
    return p_ci_entries;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfEHFrameSection::set_ci_entries(SgAsmElfEHFrameEntryCIList* const& x) {
    this->p_ci_entries = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmElfEHFrameSection::~SgAsmElfEHFrameSection() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmElfEHFrameSection::SgAsmElfEHFrameSection()
    : p_ci_entries(createAndParent<SgAsmElfEHFrameEntryCIList>(this)) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfEHFrameSection::initializeProperties() {
    p_ci_entries = createAndParent<SgAsmElfEHFrameEntryCIList>(this);
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
