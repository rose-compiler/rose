#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmPEImportItem            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
bool const&
SgAsmPEImportItem::get_by_ordinal() const {
    return p_by_ordinal;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPEImportItem::set_by_ordinal(bool const& x) {
    this->p_by_ordinal = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
unsigned const&
SgAsmPEImportItem::get_ordinal() const {
    return p_ordinal;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPEImportItem::set_ordinal(unsigned const& x) {
    this->p_ordinal = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
unsigned const&
SgAsmPEImportItem::get_hint() const {
    return p_hint;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPEImportItem::set_hint(unsigned const& x) {
    this->p_hint = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericString* const&
SgAsmPEImportItem::get_name() const {
    return p_name;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPEImportItem::set_name(SgAsmGenericString* const& x) {
    this->p_name = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
rose_rva_t const&
SgAsmPEImportItem::get_hintname_rva() const {
    return p_hintname_rva;
}

#line 493 "src/Rosebud/RosettaGenerator.C"
rose_rva_t&
SgAsmPEImportItem::get_hintname_rva() {
    return p_hintname_rva;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPEImportItem::set_hintname_rva(rose_rva_t const& x) {
    this->p_hintname_rva = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
size_t const&
SgAsmPEImportItem::get_hintname_nalloc() const {
    return p_hintname_nalloc;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPEImportItem::set_hintname_nalloc(size_t const& x) {
    this->p_hintname_nalloc = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
rose_rva_t const&
SgAsmPEImportItem::get_bound_rva() const {
    return p_bound_rva;
}

#line 493 "src/Rosebud/RosettaGenerator.C"
rose_rva_t&
SgAsmPEImportItem::get_bound_rva() {
    return p_bound_rva;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPEImportItem::set_bound_rva(rose_rva_t const& x) {
    this->p_bound_rva = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
bool const&
SgAsmPEImportItem::get_iat_written() const {
    return p_iat_written;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPEImportItem::set_iat_written(bool const& x) {
    this->p_iat_written = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmPEImportItem::~SgAsmPEImportItem() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmPEImportItem::SgAsmPEImportItem()
    : p_by_ordinal(true)
    , p_ordinal(0)
    , p_hint(0)
    , p_name(createAndParent<SgAsmBasicString>(this))
    , p_hintname_rva(0)
    , p_hintname_nalloc(0)
    , p_bound_rva(0)
    , p_iat_written(false) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
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

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
