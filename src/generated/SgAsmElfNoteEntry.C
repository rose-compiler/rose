#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfNoteEntry            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
unsigned const&
SgAsmElfNoteEntry::get_type() const {
    return p_type;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfNoteEntry::set_type(unsigned const& x) {
    this->p_type = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
SgUnsignedCharList const&
SgAsmElfNoteEntry::get_payload() const {
    return p_payload;
}

#line 493 "src/Rosebud/RosettaGenerator.C"
SgUnsignedCharList&
SgAsmElfNoteEntry::get_payload() {
    return p_payload;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfNoteEntry::set_payload(SgUnsignedCharList const& x) {
    this->p_payload = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmElfNoteEntry::~SgAsmElfNoteEntry() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmElfNoteEntry::SgAsmElfNoteEntry()
    : p_type(0)
    , p_name(createAndParent<SgAsmBasicString>(this)) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfNoteEntry::initializeProperties() {
    p_type = 0;
    p_name = createAndParent<SgAsmBasicString>(this);
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
