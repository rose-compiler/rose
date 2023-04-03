#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmGenericFile            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmDwarfCompilationUnitList* const&
SgAsmGenericFile::get_dwarf_info() const {
    return p_dwarf_info;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmGenericFile::set_dwarf_info(SgAsmDwarfCompilationUnitList* const& x) {
    this->p_dwarf_info = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
std::string const&
SgAsmGenericFile::get_name() const {
    return p_name;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmGenericFile::set_name(std::string const& x) {
    this->p_name = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
int const&
SgAsmGenericFile::get_fd() const {
    return p_fd;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmGenericFile::set_fd(int const& x) {
    this->p_fd = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
struct stat const&
SgAsmGenericFile::get_sb() const {
    return p_sb;
}

#line 487 "src/Rosebud/RosettaGenerator.C"
SgFileContentList const&
SgAsmGenericFile::get_data() const {
    return p_data;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmGenericFile::set_data(SgFileContentList const& x) {
    this->p_data = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericHeaderList* const&
SgAsmGenericFile::get_headers() const {
    return p_headers;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmGenericFile::set_headers(SgAsmGenericHeaderList* const& x) {
    this->p_headers = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericSectionList* const&
SgAsmGenericFile::get_holes() const {
    return p_holes;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmGenericFile::set_holes(SgAsmGenericSectionList* const& x) {
    this->p_holes = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
bool const&
SgAsmGenericFile::get_truncate_zeros() const {
    return p_truncate_zeros;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmGenericFile::set_truncate_zeros(bool const& x) {
    this->p_truncate_zeros = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
bool const&
SgAsmGenericFile::get_tracking_references() const {
    return p_tracking_references;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmGenericFile::set_tracking_references(bool const& x) {
    this->p_tracking_references = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
AddressIntervalSet const&
SgAsmGenericFile::get_referenced_extents() const {
    return p_referenced_extents;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmGenericFile::set_referenced_extents(AddressIntervalSet const& x) {
    this->p_referenced_extents = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
bool const&
SgAsmGenericFile::get_neuter() const {
    return p_neuter;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmGenericFile::set_neuter(bool const& x) {
    this->p_neuter = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericFile::~SgAsmGenericFile() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericFile::SgAsmGenericFile()
    : p_dwarf_info(nullptr)
    , p_fd(-1)
    , p_headers(createAndParent<SgAsmGenericHeaderList>(this))
    , p_holes(createAndParent<SgAsmGenericSectionList>(this))
    , p_truncate_zeros(false)
    , p_tracking_references(true)
    , p_neuter(false) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
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

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
