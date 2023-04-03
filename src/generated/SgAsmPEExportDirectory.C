#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmPEExportDirectory            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
unsigned const&
SgAsmPEExportDirectory::get_res1() const {
    return p_res1;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPEExportDirectory::set_res1(unsigned const& x) {
    this->p_res1 = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
time_t const&
SgAsmPEExportDirectory::get_timestamp() const {
    return p_timestamp;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPEExportDirectory::set_timestamp(time_t const& x) {
    this->p_timestamp = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
unsigned const&
SgAsmPEExportDirectory::get_vmajor() const {
    return p_vmajor;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPEExportDirectory::set_vmajor(unsigned const& x) {
    this->p_vmajor = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
unsigned const&
SgAsmPEExportDirectory::get_vminor() const {
    return p_vminor;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPEExportDirectory::set_vminor(unsigned const& x) {
    this->p_vminor = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
rose_rva_t const&
SgAsmPEExportDirectory::get_name_rva() const {
    return p_name_rva;
}

#line 493 "src/Rosebud/RosettaGenerator.C"
rose_rva_t&
SgAsmPEExportDirectory::get_name_rva() {
    return p_name_rva;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPEExportDirectory::set_name_rva(rose_rva_t const& x) {
    this->p_name_rva = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
unsigned const&
SgAsmPEExportDirectory::get_ord_base() const {
    return p_ord_base;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPEExportDirectory::set_ord_base(unsigned const& x) {
    this->p_ord_base = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
size_t const&
SgAsmPEExportDirectory::get_expaddr_n() const {
    return p_expaddr_n;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPEExportDirectory::set_expaddr_n(size_t const& x) {
    this->p_expaddr_n = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
size_t const&
SgAsmPEExportDirectory::get_nameptr_n() const {
    return p_nameptr_n;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPEExportDirectory::set_nameptr_n(size_t const& x) {
    this->p_nameptr_n = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
rose_rva_t const&
SgAsmPEExportDirectory::get_expaddr_rva() const {
    return p_expaddr_rva;
}

#line 493 "src/Rosebud/RosettaGenerator.C"
rose_rva_t&
SgAsmPEExportDirectory::get_expaddr_rva() {
    return p_expaddr_rva;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPEExportDirectory::set_expaddr_rva(rose_rva_t const& x) {
    this->p_expaddr_rva = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
rose_rva_t const&
SgAsmPEExportDirectory::get_nameptr_rva() const {
    return p_nameptr_rva;
}

#line 493 "src/Rosebud/RosettaGenerator.C"
rose_rva_t&
SgAsmPEExportDirectory::get_nameptr_rva() {
    return p_nameptr_rva;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPEExportDirectory::set_nameptr_rva(rose_rva_t const& x) {
    this->p_nameptr_rva = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
rose_rva_t const&
SgAsmPEExportDirectory::get_ordinals_rva() const {
    return p_ordinals_rva;
}

#line 493 "src/Rosebud/RosettaGenerator.C"
rose_rva_t&
SgAsmPEExportDirectory::get_ordinals_rva() {
    return p_ordinals_rva;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPEExportDirectory::set_ordinals_rva(rose_rva_t const& x) {
    this->p_ordinals_rva = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericString* const&
SgAsmPEExportDirectory::get_name() const {
    return p_name;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPEExportDirectory::set_name(SgAsmGenericString* const& x) {
    this->p_name = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmPEExportDirectory::~SgAsmPEExportDirectory() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmPEExportDirectory::SgAsmPEExportDirectory()
    : p_res1(0)
    , p_timestamp(0)
    , p_vmajor(0)
    , p_vminor(0)
    , p_name_rva(0)
    , p_ord_base(0)
    , p_expaddr_n(0)
    , p_nameptr_n(0)
    , p_expaddr_rva(0)
    , p_nameptr_rva(0)
    , p_ordinals_rva(0)
    , p_name(NULL) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPEExportDirectory::initializeProperties() {
    p_res1 = 0;
    p_timestamp = 0;
    p_vmajor = 0;
    p_vminor = 0;
    p_name_rva = 0;
    p_ord_base = 0;
    p_expaddr_n = 0;
    p_nameptr_n = 0;
    p_expaddr_rva = 0;
    p_nameptr_rva = 0;
    p_ordinals_rva = 0;
    p_name = NULL;
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
