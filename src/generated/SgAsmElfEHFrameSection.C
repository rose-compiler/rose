#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfEHFrameSection            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmElfEHFrameEntryCIList* const&
SgAsmElfEHFrameSection::get_ci_entries() const {
    return p_ci_entries;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfEHFrameSection::set_ci_entries(SgAsmElfEHFrameEntryCIList* const& x) {
    this->p_ci_entries = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmElfEHFrameSection::~SgAsmElfEHFrameSection() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmElfEHFrameSection::SgAsmElfEHFrameSection()
#line 308 "src/Rosebud/RosettaGenerator.C"
    : p_ci_entries(createAndParent<SgAsmElfEHFrameEntryCIList>(this)) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfEHFrameSection::initializeProperties() {
    p_ci_entries = createAndParent<SgAsmElfEHFrameEntryCIList>(this);
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
