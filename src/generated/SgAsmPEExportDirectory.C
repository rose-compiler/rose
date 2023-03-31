//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmPEExportDirectory                -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

unsigned const&
SgAsmPEExportDirectory::get_res1() const {
    return p_res1;
}

void
SgAsmPEExportDirectory::set_res1(unsigned const& x) {
    this->p_res1 = x;
    set_isModified(true);
}

time_t const&
SgAsmPEExportDirectory::get_timestamp() const {
    return p_timestamp;
}

void
SgAsmPEExportDirectory::set_timestamp(time_t const& x) {
    this->p_timestamp = x;
    set_isModified(true);
}

unsigned const&
SgAsmPEExportDirectory::get_vmajor() const {
    return p_vmajor;
}

void
SgAsmPEExportDirectory::set_vmajor(unsigned const& x) {
    this->p_vmajor = x;
    set_isModified(true);
}

unsigned const&
SgAsmPEExportDirectory::get_vminor() const {
    return p_vminor;
}

void
SgAsmPEExportDirectory::set_vminor(unsigned const& x) {
    this->p_vminor = x;
    set_isModified(true);
}

rose_rva_t const&
SgAsmPEExportDirectory::get_name_rva() const {
    return p_name_rva;
}

rose_rva_t&
SgAsmPEExportDirectory::get_name_rva() {
    return p_name_rva;
}

void
SgAsmPEExportDirectory::set_name_rva(rose_rva_t const& x) {
    this->p_name_rva = x;
    set_isModified(true);
}

unsigned const&
SgAsmPEExportDirectory::get_ord_base() const {
    return p_ord_base;
}

void
SgAsmPEExportDirectory::set_ord_base(unsigned const& x) {
    this->p_ord_base = x;
    set_isModified(true);
}

size_t const&
SgAsmPEExportDirectory::get_expaddr_n() const {
    return p_expaddr_n;
}

void
SgAsmPEExportDirectory::set_expaddr_n(size_t const& x) {
    this->p_expaddr_n = x;
    set_isModified(true);
}

size_t const&
SgAsmPEExportDirectory::get_nameptr_n() const {
    return p_nameptr_n;
}

void
SgAsmPEExportDirectory::set_nameptr_n(size_t const& x) {
    this->p_nameptr_n = x;
    set_isModified(true);
}

rose_rva_t const&
SgAsmPEExportDirectory::get_expaddr_rva() const {
    return p_expaddr_rva;
}

rose_rva_t&
SgAsmPEExportDirectory::get_expaddr_rva() {
    return p_expaddr_rva;
}

void
SgAsmPEExportDirectory::set_expaddr_rva(rose_rva_t const& x) {
    this->p_expaddr_rva = x;
    set_isModified(true);
}

rose_rva_t const&
SgAsmPEExportDirectory::get_nameptr_rva() const {
    return p_nameptr_rva;
}

rose_rva_t&
SgAsmPEExportDirectory::get_nameptr_rva() {
    return p_nameptr_rva;
}

void
SgAsmPEExportDirectory::set_nameptr_rva(rose_rva_t const& x) {
    this->p_nameptr_rva = x;
    set_isModified(true);
}

rose_rva_t const&
SgAsmPEExportDirectory::get_ordinals_rva() const {
    return p_ordinals_rva;
}

rose_rva_t&
SgAsmPEExportDirectory::get_ordinals_rva() {
    return p_ordinals_rva;
}

void
SgAsmPEExportDirectory::set_ordinals_rva(rose_rva_t const& x) {
    this->p_ordinals_rva = x;
    set_isModified(true);
}

SgAsmGenericString* const&
SgAsmPEExportDirectory::get_name() const {
    return p_name;
}

void
SgAsmPEExportDirectory::set_name(SgAsmGenericString* const& x) {
    this->p_name = x;
    set_isModified(true);
}

SgAsmPEExportDirectory::~SgAsmPEExportDirectory() {
    destructorHelper();
}

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

#endif // ROSE_ENABLE_BINARY_ANALYSIS
