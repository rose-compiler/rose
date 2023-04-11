//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmGenericFile            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#include <sage3basic.h>

SgAsmDwarfCompilationUnitList* const&
SgAsmGenericFile::get_dwarf_info() const {
    return p_dwarf_info;
}

void
SgAsmGenericFile::set_dwarf_info(SgAsmDwarfCompilationUnitList* const& x) {
    changeChildPointer(this->p_dwarf_info, const_cast<SgAsmDwarfCompilationUnitList*&>(x));
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
SgAsmGenericFile::get_truncate_zeros() const {
    return p_truncate_zeros;
}

void
SgAsmGenericFile::set_truncate_zeros(bool const& x) {
    this->p_truncate_zeros = x;
    set_isModified(true);
}

bool const&
SgAsmGenericFile::get_tracking_references() const {
    return p_tracking_references;
}

void
SgAsmGenericFile::set_tracking_references(bool const& x) {
    this->p_tracking_references = x;
    set_isModified(true);
}

AddressIntervalSet const&
SgAsmGenericFile::get_referenced_extents() const {
    return p_referenced_extents;
}

void
SgAsmGenericFile::set_referenced_extents(AddressIntervalSet const& x) {
    this->p_referenced_extents = x;
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
    : p_dwarf_info(nullptr)
    , p_fd(-1)
    , p_headers(createAndParent<SgAsmGenericHeaderList>(this))
    , p_holes(createAndParent<SgAsmGenericSectionList>(this))
    , p_truncate_zeros(false)
    , p_tracking_references(true)
    , p_neuter(false) {}

void
SgAsmGenericFile::initializeProperties() {
    p_dwarf_info = nullptr;
    p_fd = -1;
    p_headers = createAndParent<SgAsmGenericHeaderList>(this);
    p_holes = createAndParent<SgAsmGenericSectionList>(this);
    p_truncate_zeros = false;
    p_tracking_references = true;
    p_neuter = false;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
