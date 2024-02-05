//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmGenericFile            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#include <sage3basic.h>

SgAsmDwarfCompilationUnitList* const&
SgAsmGenericFile::get_dwarfInfo() const {
    return p_dwarfInfo;
}

void
SgAsmGenericFile::set_dwarfInfo(SgAsmDwarfCompilationUnitList* const& x) {
    changeChildPointer(this->p_dwarfInfo, const_cast<SgAsmDwarfCompilationUnitList*&>(x));
    set_isModified(true);
}

std::string const&
SgAsmGenericFile::get_name() const {
    return p_name;
}

void
SgAsmGenericFile::set_name(std::string const& x) {
    this->p_name = x;
    set_isModified(true);
}

int const&
SgAsmGenericFile::get_fd() const {
    return p_fd;
}

void
SgAsmGenericFile::set_fd(int const& x) {
    this->p_fd = x;
    set_isModified(true);
}

struct stat const&
SgAsmGenericFile::get_sb() const {
    return p_sb;
}

SgFileContentList const&
SgAsmGenericFile::get_data() const {
    return p_data;
}

void
SgAsmGenericFile::set_data(SgFileContentList const& x) {
    this->p_data = x;
    set_isModified(true);
}

SgAsmGenericHeaderList* const&
SgAsmGenericFile::get_headers() const {
    return p_headers;
}

void
SgAsmGenericFile::set_headers(SgAsmGenericHeaderList* const& x) {
    changeChildPointer(this->p_headers, const_cast<SgAsmGenericHeaderList*&>(x));
    set_isModified(true);
}

SgAsmGenericSectionList* const&
SgAsmGenericFile::get_holes() const {
    return p_holes;
}

void
SgAsmGenericFile::set_holes(SgAsmGenericSectionList* const& x) {
    changeChildPointer(this->p_holes, const_cast<SgAsmGenericSectionList*&>(x));
    set_isModified(true);
}

bool const&
SgAsmGenericFile::get_truncateZeros() const {
    return p_truncateZeros;
}

void
SgAsmGenericFile::set_truncateZeros(bool const& x) {
    this->p_truncateZeros = x;
    set_isModified(true);
}

bool const&
SgAsmGenericFile::get_trackingReferences() const {
    return p_trackingReferences;
}

void
SgAsmGenericFile::set_trackingReferences(bool const& x) {
    this->p_trackingReferences = x;
    set_isModified(true);
}

Rose::BinaryAnalysis::AddressIntervalSet const&
SgAsmGenericFile::get_referencedExtents() const {
    return p_referencedExtents;
}

void
SgAsmGenericFile::set_referencedExtents(Rose::BinaryAnalysis::AddressIntervalSet const& x) {
    this->p_referencedExtents = x;
    set_isModified(true);
}

bool const&
SgAsmGenericFile::get_neuter() const {
    return p_neuter;
}

void
SgAsmGenericFile::set_neuter(bool const& x) {
    this->p_neuter = x;
    set_isModified(true);
}

SgAsmGenericFile::~SgAsmGenericFile() {
    destructorHelper();
}

SgAsmGenericFile::SgAsmGenericFile()
    : p_dwarfInfo(nullptr)
    , p_fd(-1)
    , p_headers(createAndParent<SgAsmGenericHeaderList>(this))
    , p_holes(createAndParent<SgAsmGenericSectionList>(this))
    , p_truncateZeros(false)
    , p_trackingReferences(true)
    , p_neuter(false) {}

void
SgAsmGenericFile::initializeProperties() {
    p_dwarfInfo = nullptr;
    p_fd = -1;
    p_headers = createAndParent<SgAsmGenericHeaderList>(this);
    p_holes = createAndParent<SgAsmGenericSectionList>(this);
    p_truncateZeros = false;
    p_trackingReferences = true;
    p_neuter = false;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
