#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmPEImportDirectory            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericString* const&
SgAsmPEImportDirectory::get_dll_name() const {
    return p_dll_name;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPEImportDirectory::set_dll_name(SgAsmGenericString* const& x) {
    this->p_dll_name = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
rose_rva_t const&
SgAsmPEImportDirectory::get_dll_name_rva() const {
    return p_dll_name_rva;
}

#line 552 "src/Rosebud/RosettaGenerator.C"
rose_rva_t&
SgAsmPEImportDirectory::get_dll_name_rva() {
    return p_dll_name_rva;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPEImportDirectory::set_dll_name_rva(rose_rva_t const& x) {
    this->p_dll_name_rva = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
size_t const&
SgAsmPEImportDirectory::get_dll_name_nalloc() const {
    return p_dll_name_nalloc;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPEImportDirectory::set_dll_name_nalloc(size_t const& x) {
    this->p_dll_name_nalloc = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
time_t const&
SgAsmPEImportDirectory::get_time() const {
    return p_time;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPEImportDirectory::set_time(time_t const& x) {
    this->p_time = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
unsigned const&
SgAsmPEImportDirectory::get_forwarder_chain() const {
    return p_forwarder_chain;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPEImportDirectory::set_forwarder_chain(unsigned const& x) {
    this->p_forwarder_chain = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
rose_rva_t const&
SgAsmPEImportDirectory::get_ilt_rva() const {
    return p_ilt_rva;
}

#line 552 "src/Rosebud/RosettaGenerator.C"
rose_rva_t&
SgAsmPEImportDirectory::get_ilt_rva() {
    return p_ilt_rva;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPEImportDirectory::set_ilt_rva(rose_rva_t const& x) {
    this->p_ilt_rva = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
size_t const&
SgAsmPEImportDirectory::get_ilt_nalloc() const {
    return p_ilt_nalloc;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPEImportDirectory::set_ilt_nalloc(size_t const& x) {
    this->p_ilt_nalloc = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
rose_rva_t const&
SgAsmPEImportDirectory::get_iat_rva() const {
    return p_iat_rva;
}

#line 552 "src/Rosebud/RosettaGenerator.C"
rose_rva_t&
SgAsmPEImportDirectory::get_iat_rva() {
    return p_iat_rva;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPEImportDirectory::set_iat_rva(rose_rva_t const& x) {
    this->p_iat_rva = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
size_t const&
SgAsmPEImportDirectory::get_iat_nalloc() const {
    return p_iat_nalloc;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPEImportDirectory::set_iat_nalloc(size_t const& x) {
    this->p_iat_nalloc = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmPEImportItemList* const&
SgAsmPEImportDirectory::get_imports() const {
    return p_imports;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPEImportDirectory::set_imports(SgAsmPEImportItemList* const& x) {
    this->p_imports = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmPEImportDirectory::~SgAsmPEImportDirectory() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmPEImportDirectory::SgAsmPEImportDirectory()
#line 308 "src/Rosebud/RosettaGenerator.C"
    : p_dll_name(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_dll_name_rva(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_dll_name_nalloc(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_time(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_forwarder_chain(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_ilt_rva(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_ilt_nalloc(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_iat_rva(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_iat_nalloc(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_imports(createAndParent<SgAsmPEImportItemList>(this)) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPEImportDirectory::initializeProperties() {
    p_dll_name = 0;
    p_dll_name_rva = 0;
    p_dll_name_nalloc = 0;
    p_time = 0;
    p_forwarder_chain = 0;
    p_ilt_rva = 0;
    p_ilt_nalloc = 0;
    p_iat_rva = 0;
    p_iat_nalloc = 0;
    p_imports = createAndParent<SgAsmPEImportItemList>(this);
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
