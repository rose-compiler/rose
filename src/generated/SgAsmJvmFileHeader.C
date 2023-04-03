#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmFileHeader            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
uint16_t const&
SgAsmJvmFileHeader::get_minor_version() const {
    return p_minor_version;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmFileHeader::set_minor_version(uint16_t const& x) {
    this->p_minor_version = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
uint16_t const&
SgAsmJvmFileHeader::get_major_version() const {
    return p_major_version;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmFileHeader::set_major_version(uint16_t const& x) {
    this->p_major_version = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
uint16_t const&
SgAsmJvmFileHeader::get_access_flags() const {
    return p_access_flags;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmFileHeader::set_access_flags(uint16_t const& x) {
    this->p_access_flags = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
uint16_t const&
SgAsmJvmFileHeader::get_this_class() const {
    return p_this_class;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmFileHeader::set_this_class(uint16_t const& x) {
    this->p_this_class = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
uint16_t const&
SgAsmJvmFileHeader::get_super_class() const {
    return p_super_class;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmFileHeader::set_super_class(uint16_t const& x) {
    this->p_super_class = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmConstantPool* const&
SgAsmJvmFileHeader::get_constant_pool() const {
    return p_constant_pool;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmFileHeader::set_constant_pool(SgAsmJvmConstantPool* const& x) {
    this->p_constant_pool = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
std::list<uint16_t> const&
SgAsmJvmFileHeader::get_interfaces() const {
    return p_interfaces;
}

#line 493 "src/Rosebud/RosettaGenerator.C"
std::list<uint16_t>&
SgAsmJvmFileHeader::get_interfaces() {
    return p_interfaces;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmFileHeader::set_interfaces(std::list<uint16_t> const& x) {
    this->p_interfaces = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmFieldTable* const&
SgAsmJvmFileHeader::get_field_table() const {
    return p_field_table;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmFileHeader::set_field_table(SgAsmJvmFieldTable* const& x) {
    this->p_field_table = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmMethodTable* const&
SgAsmJvmFileHeader::get_method_table() const {
    return p_method_table;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmFileHeader::set_method_table(SgAsmJvmMethodTable* const& x) {
    this->p_method_table = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmAttributeTable* const&
SgAsmJvmFileHeader::get_attribute_table() const {
    return p_attribute_table;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmFileHeader::set_attribute_table(SgAsmJvmAttributeTable* const& x) {
    this->p_attribute_table = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmFileHeader::~SgAsmJvmFileHeader() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmFileHeader::SgAsmJvmFileHeader()
    : p_minor_version(0)
    , p_major_version(0)
    , p_access_flags(0)
    , p_this_class(0)
    , p_super_class(0)
    , p_constant_pool(nullptr)
    , p_field_table(nullptr)
    , p_method_table(nullptr)
    , p_attribute_table(nullptr) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmFileHeader::initializeProperties() {
    p_minor_version = 0;
    p_major_version = 0;
    p_access_flags = 0;
    p_this_class = 0;
    p_super_class = 0;
    p_constant_pool = nullptr;
    p_field_table = nullptr;
    p_method_table = nullptr;
    p_attribute_table = nullptr;
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
