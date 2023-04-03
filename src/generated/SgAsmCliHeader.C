#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCliHeader            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
uint32_t const&
SgAsmCliHeader::get_cb() const {
    return p_cb;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmCliHeader::set_cb(uint32_t const& x) {
    this->p_cb = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
uint16_t const&
SgAsmCliHeader::get_majorRuntimeVersion() const {
    return p_majorRuntimeVersion;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmCliHeader::set_majorRuntimeVersion(uint16_t const& x) {
    this->p_majorRuntimeVersion = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
uint16_t const&
SgAsmCliHeader::get_minorRuntimeVersion() const {
    return p_minorRuntimeVersion;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmCliHeader::set_minorRuntimeVersion(uint16_t const& x) {
    this->p_minorRuntimeVersion = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
uint64_t const&
SgAsmCliHeader::get_metaData() const {
    return p_metaData;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmCliHeader::set_metaData(uint64_t const& x) {
    this->p_metaData = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
uint32_t const&
SgAsmCliHeader::get_flags() const {
    return p_flags;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmCliHeader::set_flags(uint32_t const& x) {
    this->p_flags = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
uint32_t const&
SgAsmCliHeader::get_entryPointToken() const {
    return p_entryPointToken;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmCliHeader::set_entryPointToken(uint32_t const& x) {
    this->p_entryPointToken = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
uint64_t const&
SgAsmCliHeader::get_resources() const {
    return p_resources;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmCliHeader::set_resources(uint64_t const& x) {
    this->p_resources = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
uint64_t const&
SgAsmCliHeader::get_strongNameSignature() const {
    return p_strongNameSignature;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmCliHeader::set_strongNameSignature(uint64_t const& x) {
    this->p_strongNameSignature = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
uint64_t const&
SgAsmCliHeader::get_codeManagerTable() const {
    return p_codeManagerTable;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmCliHeader::set_codeManagerTable(uint64_t const& x) {
    this->p_codeManagerTable = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
uint64_t const&
SgAsmCliHeader::get_vTableFixups() const {
    return p_vTableFixups;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmCliHeader::set_vTableFixups(uint64_t const& x) {
    this->p_vTableFixups = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
uint64_t const&
SgAsmCliHeader::get_exportAddressTableJumps() const {
    return p_exportAddressTableJumps;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmCliHeader::set_exportAddressTableJumps(uint64_t const& x) {
    this->p_exportAddressTableJumps = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
uint64_t const&
SgAsmCliHeader::get_managedNativeHeader() const {
    return p_managedNativeHeader;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmCliHeader::set_managedNativeHeader(uint64_t const& x) {
    this->p_managedNativeHeader = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmCilMetadataRoot* const&
SgAsmCliHeader::get_metadataRoot() const {
    return p_metadataRoot;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmCliHeader::set_metadataRoot(SgAsmCilMetadataRoot* const& x) {
    this->p_metadataRoot = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmCliHeader::~SgAsmCliHeader() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmCliHeader::SgAsmCliHeader()
    : p_cb(0)
    , p_majorRuntimeVersion(0)
    , p_minorRuntimeVersion(0)
    , p_metaData(0)
    , p_flags(0)
    , p_entryPointToken(0)
    , p_resources(0)
    , p_strongNameSignature(0)
    , p_codeManagerTable(0)
    , p_vTableFixups(0)
    , p_exportAddressTableJumps(0)
    , p_managedNativeHeader(0)
    , p_metadataRoot(nullptr) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmCliHeader::initializeProperties() {
    p_cb = 0;
    p_majorRuntimeVersion = 0;
    p_minorRuntimeVersion = 0;
    p_metaData = 0;
    p_flags = 0;
    p_entryPointToken = 0;
    p_resources = 0;
    p_strongNameSignature = 0;
    p_codeManagerTable = 0;
    p_vTableFixups = 0;
    p_exportAddressTableJumps = 0;
    p_managedNativeHeader = 0;
    p_metadataRoot = nullptr;
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
