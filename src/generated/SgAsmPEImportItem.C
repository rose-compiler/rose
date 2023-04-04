#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmPEImportItem            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
bool const&
SgAsmPEImportItem::get_by_ordinal() const {
    return p_by_ordinal;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPEImportItem::set_by_ordinal(bool const& x) {
    this->p_by_ordinal = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
unsigned const&
SgAsmPEImportItem::get_ordinal() const {
    return p_ordinal;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPEImportItem::set_ordinal(unsigned const& x) {
    this->p_ordinal = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
unsigned const&
SgAsmPEImportItem::get_hint() const {
    return p_hint;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPEImportItem::set_hint(unsigned const& x) {
    this->p_hint = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericString* const&
SgAsmPEImportItem::get_name() const {
    return p_name;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPEImportItem::set_name(SgAsmGenericString* const& x) {
    this->p_name = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
rose_rva_t const&
SgAsmPEImportItem::get_hintname_rva() const {
    return p_hintname_rva;
}

#line 552 "src/Rosebud/RosettaGenerator.C"
rose_rva_t&
SgAsmPEImportItem::get_hintname_rva() {
    return p_hintname_rva;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPEImportItem::set_hintname_rva(rose_rva_t const& x) {
    this->p_hintname_rva = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
size_t const&
SgAsmPEImportItem::get_hintname_nalloc() const {
    return p_hintname_nalloc;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPEImportItem::set_hintname_nalloc(size_t const& x) {
    this->p_hintname_nalloc = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
rose_rva_t const&
SgAsmPEImportItem::get_bound_rva() const {
    return p_bound_rva;
}

#line 552 "src/Rosebud/RosettaGenerator.C"
rose_rva_t&
SgAsmPEImportItem::get_bound_rva() {
    return p_bound_rva;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPEImportItem::set_bound_rva(rose_rva_t const& x) {
    this->p_bound_rva = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
bool const&
SgAsmPEImportItem::get_iat_written() const {
    return p_iat_written;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPEImportItem::set_iat_written(bool const& x) {
    this->p_iat_written = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmPEImportItem::~SgAsmPEImportItem() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmPEImportItem::SgAsmPEImportItem()
#line 308 "src/Rosebud/RosettaGenerator.C"
    : p_by_ordinal(true)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_ordinal(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_hint(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_name(createAndParent<SgAsmBasicString>(this))
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_hintname_rva(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_hintname_nalloc(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_bound_rva(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_iat_written(false) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPEImportItem::initializeProperties() {
    p_by_ordinal = true;
    p_ordinal = 0;
    p_hint = 0;
    p_name = createAndParent<SgAsmBasicString>(this);
    p_hintname_rva = 0;
    p_hintname_nalloc = 0;
    p_bound_rva = 0;
    p_iat_written = false;
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
