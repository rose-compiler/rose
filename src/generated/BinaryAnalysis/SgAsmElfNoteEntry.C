//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfNoteEntry            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmElfNoteEntry_IMPL
#include <SgAsmElfNoteEntry.h>

unsigned const&
SgAsmElfNoteEntry::get_type() const {
    return p_type;
}

void
SgAsmElfNoteEntry::set_type(unsigned const& x) {
    this->p_type = x;
    set_isModified(true);
}

SgUnsignedCharList const&
SgAsmElfNoteEntry::get_payload() const {
    return p_payload;
}

SgUnsignedCharList&
SgAsmElfNoteEntry::get_payload() {
    return p_payload;
}

void
SgAsmElfNoteEntry::set_payload(SgUnsignedCharList const& x) {
    this->p_payload = x;
    set_isModified(true);
}

SgAsmElfNoteEntry::~SgAsmElfNoteEntry() {
    destructorHelper();
}

SgAsmElfNoteEntry::SgAsmElfNoteEntry()
    : p_type(0)
    , p_name(createAndParent<SgAsmBasicString>(this)) {}

void
SgAsmElfNoteEntry::initializeProperties() {
    p_type = 0;
    p_name = createAndParent<SgAsmBasicString>(this);
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
