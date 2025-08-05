//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCilMetadataRoot            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmCilMetadataRoot_IMPL
#include <SgAsmCilMetadataRoot.h>

uint32_t const&
SgAsmCilMetadataRoot::get_Signature() const {
    return p_Signature;
}

void
SgAsmCilMetadataRoot::set_Signature(uint32_t const& x) {
    this->p_Signature = x;
    set_isModified(true);
}

uint16_t const&
SgAsmCilMetadataRoot::get_MajorVersion() const {
    return p_MajorVersion;
}

void
SgAsmCilMetadataRoot::set_MajorVersion(uint16_t const& x) {
    this->p_MajorVersion = x;
    set_isModified(true);
}

uint16_t const&
SgAsmCilMetadataRoot::get_MinorVersion() const {
    return p_MinorVersion;
}

void
SgAsmCilMetadataRoot::set_MinorVersion(uint16_t const& x) {
    this->p_MinorVersion = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilMetadataRoot::get_Reserved0() const {
    return p_Reserved0;
}

void
SgAsmCilMetadataRoot::set_Reserved0(uint32_t const& x) {
    this->p_Reserved0 = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilMetadataRoot::get_Length() const {
    return p_Length;
}

void
SgAsmCilMetadataRoot::set_Length(uint32_t const& x) {
    this->p_Length = x;
    set_isModified(true);
}

std::string const&
SgAsmCilMetadataRoot::get_Version() const {
    return p_Version;
}

void
SgAsmCilMetadataRoot::set_Version(std::string const& x) {
    this->p_Version = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilMetadataRoot::get_VersionPadding() const {
    return p_VersionPadding;
}

void
SgAsmCilMetadataRoot::set_VersionPadding(uint32_t const& x) {
    this->p_VersionPadding = x;
    set_isModified(true);
}

uint16_t const&
SgAsmCilMetadataRoot::get_Flags() const {
    return p_Flags;
}

void
SgAsmCilMetadataRoot::set_Flags(uint16_t const& x) {
    this->p_Flags = x;
    set_isModified(true);
}

uint16_t const&
SgAsmCilMetadataRoot::get_NumberOfStreams() const {
    return p_NumberOfStreams;
}

void
SgAsmCilMetadataRoot::set_NumberOfStreams(uint16_t const& x) {
    this->p_NumberOfStreams = x;
    set_isModified(true);
}

std::vector<SgAsmCilDataStream*> const&
SgAsmCilMetadataRoot::get_Streams() const {
    return p_Streams;
}

std::vector<SgAsmCilDataStream*>&
SgAsmCilMetadataRoot::get_Streams() {
    return p_Streams;
}

bool const&
SgAsmCilMetadataRoot::get_ErrorFree() const {
    return p_ErrorFree;
}

void
SgAsmCilMetadataRoot::set_ErrorFree(bool const& x) {
    this->p_ErrorFree = x;
    set_isModified(true);
}

SgAsmCilMetadataRoot::~SgAsmCilMetadataRoot() {
    destructorHelper();
}

SgAsmCilMetadataRoot::SgAsmCilMetadataRoot()
    : p_Signature(0)
    , p_MajorVersion(0)
    , p_MinorVersion(0)
    , p_Reserved0(0)
    , p_Length(0)
    , p_VersionPadding(0)
    , p_Flags(0)
    , p_NumberOfStreams(0)
    , p_ErrorFree(true) {}

void
SgAsmCilMetadataRoot::initializeProperties() {
    p_Signature = 0;
    p_MajorVersion = 0;
    p_MinorVersion = 0;
    p_Reserved0 = 0;
    p_Length = 0;
    p_VersionPadding = 0;
    p_Flags = 0;
    p_NumberOfStreams = 0;
    p_ErrorFree = true;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
