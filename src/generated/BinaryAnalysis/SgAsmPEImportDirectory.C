//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmPEImportDirectory            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmPEImportDirectory_IMPL
#include <SgAsmPEImportDirectory.h>

SgAsmGenericString* const&
SgAsmPEImportDirectory::get_dllName() const {
    return p_dllName;
}

void
SgAsmPEImportDirectory::set_dllName(SgAsmGenericString* const& x) {
    changeChildPointer(this->p_dllName, const_cast<SgAsmGenericString*&>(x));
    set_isModified(true);
}

Rose::BinaryAnalysis::RelativeVirtualAddress const&
SgAsmPEImportDirectory::get_dllNameRva() const {
    return p_dllNameRva;
}

Rose::BinaryAnalysis::RelativeVirtualAddress&
SgAsmPEImportDirectory::get_dllNameRva() {
    return p_dllNameRva;
}

void
SgAsmPEImportDirectory::set_dllNameRva(Rose::BinaryAnalysis::RelativeVirtualAddress const& x) {
    this->p_dllNameRva = x;
    set_isModified(true);
}

size_t const&
SgAsmPEImportDirectory::get_dll_name_nalloc() const {
    return p_dll_name_nalloc;
}

void
SgAsmPEImportDirectory::set_dll_name_nalloc(size_t const& x) {
    this->p_dll_name_nalloc = x;
    set_isModified(true);
}

time_t const&
SgAsmPEImportDirectory::get_time() const {
    return p_time;
}

void
SgAsmPEImportDirectory::set_time(time_t const& x) {
    this->p_time = x;
    set_isModified(true);
}

unsigned const&
SgAsmPEImportDirectory::get_forwarder_chain() const {
    return p_forwarder_chain;
}

void
SgAsmPEImportDirectory::set_forwarder_chain(unsigned const& x) {
    this->p_forwarder_chain = x;
    set_isModified(true);
}

Rose::BinaryAnalysis::RelativeVirtualAddress const&
SgAsmPEImportDirectory::get_ilt_rva() const {
    return p_ilt_rva;
}

Rose::BinaryAnalysis::RelativeVirtualAddress&
SgAsmPEImportDirectory::get_ilt_rva() {
    return p_ilt_rva;
}

void
SgAsmPEImportDirectory::set_ilt_rva(Rose::BinaryAnalysis::RelativeVirtualAddress const& x) {
    this->p_ilt_rva = x;
    set_isModified(true);
}

size_t const&
SgAsmPEImportDirectory::get_ilt_nalloc() const {
    return p_ilt_nalloc;
}

void
SgAsmPEImportDirectory::set_ilt_nalloc(size_t const& x) {
    this->p_ilt_nalloc = x;
    set_isModified(true);
}

Rose::BinaryAnalysis::RelativeVirtualAddress const&
SgAsmPEImportDirectory::get_iat_rva() const {
    return p_iat_rva;
}

Rose::BinaryAnalysis::RelativeVirtualAddress&
SgAsmPEImportDirectory::get_iat_rva() {
    return p_iat_rva;
}

void
SgAsmPEImportDirectory::set_iat_rva(Rose::BinaryAnalysis::RelativeVirtualAddress const& x) {
    this->p_iat_rva = x;
    set_isModified(true);
}

size_t const&
SgAsmPEImportDirectory::get_iat_nalloc() const {
    return p_iat_nalloc;
}

void
SgAsmPEImportDirectory::set_iat_nalloc(size_t const& x) {
    this->p_iat_nalloc = x;
    set_isModified(true);
}

SgAsmPEImportItemList* const&
SgAsmPEImportDirectory::get_imports() const {
    return p_imports;
}

void
SgAsmPEImportDirectory::set_imports(SgAsmPEImportItemList* const& x) {
    changeChildPointer(this->p_imports, const_cast<SgAsmPEImportItemList*&>(x));
    set_isModified(true);
}

SgAsmPEImportDirectory::~SgAsmPEImportDirectory() {
    destructorHelper();
}

SgAsmPEImportDirectory::SgAsmPEImportDirectory()
    : p_dllName(nullptr)
    , p_dll_name_nalloc(0)
    , p_time(0)
    , p_forwarder_chain(0)
    , p_ilt_nalloc(0)
    , p_iat_nalloc(0)
    , p_imports(createAndParent<SgAsmPEImportItemList>(this)) {}

void
SgAsmPEImportDirectory::initializeProperties() {
    p_dllName = nullptr;
    p_dll_name_nalloc = 0;
    p_time = 0;
    p_forwarder_chain = 0;
    p_ilt_nalloc = 0;
    p_iat_nalloc = 0;
    p_imports = createAndParent<SgAsmPEImportItemList>(this);
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
