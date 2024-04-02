//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfSymverNeededEntry            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmElfSymverNeededEntry_IMPL
#include <SgAsmElfSymverNeededEntry.h>

size_t const&
SgAsmElfSymverNeededEntry::get_version() const {
    return p_version;
}

void
SgAsmElfSymverNeededEntry::set_version(size_t const& x) {
    this->p_version = x;
    set_isModified(true);
}

SgAsmGenericString* const&
SgAsmElfSymverNeededEntry::get_fileName() const {
    return p_fileName;
}

void
SgAsmElfSymverNeededEntry::set_fileName(SgAsmGenericString* const& x) {
    changeChildPointer(this->p_fileName, const_cast<SgAsmGenericString*&>(x));
    set_isModified(true);
}

SgAsmElfSymverNeededAuxList* const&
SgAsmElfSymverNeededEntry::get_entries() const {
    return p_entries;
}

void
SgAsmElfSymverNeededEntry::set_entries(SgAsmElfSymverNeededAuxList* const& x) {
    changeChildPointer(this->p_entries, const_cast<SgAsmElfSymverNeededAuxList*&>(x));
    set_isModified(true);
}

SgAsmElfSymverNeededEntry::~SgAsmElfSymverNeededEntry() {
    destructorHelper();
}

SgAsmElfSymverNeededEntry::SgAsmElfSymverNeededEntry()
    : p_version(0)
    , p_fileName(nullptr)
    , p_entries(createAndParent<SgAsmElfSymverNeededAuxList>(this)) {}

void
SgAsmElfSymverNeededEntry::initializeProperties() {
    p_version = 0;
    p_fileName = nullptr;
    p_entries = createAndParent<SgAsmElfSymverNeededAuxList>(this);
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
