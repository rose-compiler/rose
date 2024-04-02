//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmPEImportItem            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmPEImportItem_IMPL
#include <SgAsmPEImportItem.h>

bool const&
SgAsmPEImportItem::get_by_ordinal() const {
    return p_by_ordinal;
}

void
SgAsmPEImportItem::set_by_ordinal(bool const& x) {
    this->p_by_ordinal = x;
    set_isModified(true);
}

unsigned const&
SgAsmPEImportItem::get_ordinal() const {
    return p_ordinal;
}

void
SgAsmPEImportItem::set_ordinal(unsigned const& x) {
    this->p_ordinal = x;
    set_isModified(true);
}

unsigned const&
SgAsmPEImportItem::get_hint() const {
    return p_hint;
}

void
SgAsmPEImportItem::set_hint(unsigned const& x) {
    this->p_hint = x;
    set_isModified(true);
}

SgAsmGenericString* const&
SgAsmPEImportItem::get_name() const {
    return p_name;
}

void
SgAsmPEImportItem::set_name(SgAsmGenericString* const& x) {
    changeChildPointer(this->p_name, const_cast<SgAsmGenericString*&>(x));
    set_isModified(true);
}

Rose::BinaryAnalysis::RelativeVirtualAddress const&
SgAsmPEImportItem::get_hintname_rva() const {
    return p_hintname_rva;
}

Rose::BinaryAnalysis::RelativeVirtualAddress&
SgAsmPEImportItem::get_hintname_rva() {
    return p_hintname_rva;
}

void
SgAsmPEImportItem::set_hintname_rva(Rose::BinaryAnalysis::RelativeVirtualAddress const& x) {
    this->p_hintname_rva = x;
    set_isModified(true);
}

size_t const&
SgAsmPEImportItem::get_hintname_nalloc() const {
    return p_hintname_nalloc;
}

void
SgAsmPEImportItem::set_hintname_nalloc(size_t const& x) {
    this->p_hintname_nalloc = x;
    set_isModified(true);
}

Rose::BinaryAnalysis::RelativeVirtualAddress const&
SgAsmPEImportItem::get_bound_rva() const {
    return p_bound_rva;
}

Rose::BinaryAnalysis::RelativeVirtualAddress&
SgAsmPEImportItem::get_bound_rva() {
    return p_bound_rva;
}

void
SgAsmPEImportItem::set_bound_rva(Rose::BinaryAnalysis::RelativeVirtualAddress const& x) {
    this->p_bound_rva = x;
    set_isModified(true);
}

bool const&
SgAsmPEImportItem::get_iat_written() const {
    return p_iat_written;
}

void
SgAsmPEImportItem::set_iat_written(bool const& x) {
    this->p_iat_written = x;
    set_isModified(true);
}

SgAsmPEImportItem::~SgAsmPEImportItem() {
    destructorHelper();
}

SgAsmPEImportItem::SgAsmPEImportItem()
    : p_by_ordinal(true)
    , p_ordinal(0)
    , p_hint(0)
    , p_name(createAndParent<SgAsmBasicString>(this))
    , p_hintname_nalloc(0)
    , p_iat_written(false) {}

void
SgAsmPEImportItem::initializeProperties() {
    p_by_ordinal = true;
    p_ordinal = 0;
    p_hint = 0;
    p_name = createAndParent<SgAsmBasicString>(this);
    p_hintname_nalloc = 0;
    p_iat_written = false;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
