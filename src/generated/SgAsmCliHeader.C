#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCliHeader            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
uint32_t const&
SgAsmCliHeader::get_cb() const {
    return p_cb;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmCliHeader::set_cb(uint32_t const& x) {
    this->p_cb = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
uint16_t const&
SgAsmCliHeader::get_majorRuntimeVersion() const {
    return p_majorRuntimeVersion;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmCliHeader::set_majorRuntimeVersion(uint16_t const& x) {
    this->p_majorRuntimeVersion = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
uint16_t const&
SgAsmCliHeader::get_minorRuntimeVersion() const {
    return p_minorRuntimeVersion;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmCliHeader::set_minorRuntimeVersion(uint16_t const& x) {
    this->p_minorRuntimeVersion = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
uint64_t const&
SgAsmCliHeader::get_metaData() const {
    return p_metaData;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmCliHeader::set_metaData(uint64_t const& x) {
    this->p_metaData = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
uint32_t const&
SgAsmCliHeader::get_flags() const {
    return p_flags;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmCliHeader::set_flags(uint32_t const& x) {
    this->p_flags = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
uint32_t const&
SgAsmCliHeader::get_entryPointToken() const {
    return p_entryPointToken;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmCliHeader::set_entryPointToken(uint32_t const& x) {
    this->p_entryPointToken = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
uint64_t const&
SgAsmCliHeader::get_resources() const {
    return p_resources;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmCliHeader::set_resources(uint64_t const& x) {
    this->p_resources = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
uint64_t const&
SgAsmCliHeader::get_strongNameSignature() const {
    return p_strongNameSignature;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmCliHeader::set_strongNameSignature(uint64_t const& x) {
    this->p_strongNameSignature = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
uint64_t const&
SgAsmCliHeader::get_codeManagerTable() const {
    return p_codeManagerTable;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmCliHeader::set_codeManagerTable(uint64_t const& x) {
    this->p_codeManagerTable = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
uint64_t const&
SgAsmCliHeader::get_vTableFixups() const {
    return p_vTableFixups;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmCliHeader::set_vTableFixups(uint64_t const& x) {
    this->p_vTableFixups = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
uint64_t const&
SgAsmCliHeader::get_exportAddressTableJumps() const {
    return p_exportAddressTableJumps;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmCliHeader::set_exportAddressTableJumps(uint64_t const& x) {
    this->p_exportAddressTableJumps = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
uint64_t const&
SgAsmCliHeader::get_managedNativeHeader() const {
    return p_managedNativeHeader;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmCliHeader::set_managedNativeHeader(uint64_t const& x) {
    this->p_managedNativeHeader = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmCilMetadataRoot* const&
SgAsmCliHeader::get_metadataRoot() const {
    return p_metadataRoot;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmCliHeader::set_metadataRoot(SgAsmCilMetadataRoot* const& x) {
    this->p_metadataRoot = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmCliHeader::~SgAsmCliHeader() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmCliHeader::SgAsmCliHeader()
#line 308 "src/Rosebud/RosettaGenerator.C"
    : p_cb(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_majorRuntimeVersion(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_minorRuntimeVersion(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_metaData(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_flags(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_entryPointToken(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_resources(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_strongNameSignature(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_codeManagerTable(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_vTableFixups(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_exportAddressTableJumps(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_managedNativeHeader(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_metadataRoot(nullptr) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
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

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
