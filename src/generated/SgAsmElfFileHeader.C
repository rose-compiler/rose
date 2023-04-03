#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfFileHeader            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
unsigned char const&
SgAsmElfFileHeader::get_e_ident_file_class() const {
    return p_e_ident_file_class;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfFileHeader::set_e_ident_file_class(unsigned char const& x) {
    this->p_e_ident_file_class = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
unsigned char const&
SgAsmElfFileHeader::get_e_ident_data_encoding() const {
    return p_e_ident_data_encoding;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfFileHeader::set_e_ident_data_encoding(unsigned char const& x) {
    this->p_e_ident_data_encoding = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
unsigned char const&
SgAsmElfFileHeader::get_e_ident_file_version() const {
    return p_e_ident_file_version;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfFileHeader::set_e_ident_file_version(unsigned char const& x) {
    this->p_e_ident_file_version = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
SgUnsignedCharList const&
SgAsmElfFileHeader::get_e_ident_padding() const {
    return p_e_ident_padding;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfFileHeader::set_e_ident_padding(SgUnsignedCharList const& x) {
    this->p_e_ident_padding = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
unsigned long const&
SgAsmElfFileHeader::get_e_type() const {
    return p_e_type;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfFileHeader::set_e_type(unsigned long const& x) {
    this->p_e_type = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
unsigned long const&
SgAsmElfFileHeader::get_e_machine() const {
    return p_e_machine;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfFileHeader::set_e_machine(unsigned long const& x) {
    this->p_e_machine = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
unsigned long const&
SgAsmElfFileHeader::get_e_flags() const {
    return p_e_flags;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfFileHeader::set_e_flags(unsigned long const& x) {
    this->p_e_flags = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
unsigned long const&
SgAsmElfFileHeader::get_e_ehsize() const {
    return p_e_ehsize;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfFileHeader::set_e_ehsize(unsigned long const& x) {
    this->p_e_ehsize = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
unsigned long const&
SgAsmElfFileHeader::get_phextrasz() const {
    return p_phextrasz;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfFileHeader::set_phextrasz(unsigned long const& x) {
    this->p_phextrasz = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
unsigned long const&
SgAsmElfFileHeader::get_e_phnum() const {
    return p_e_phnum;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfFileHeader::set_e_phnum(unsigned long const& x) {
    this->p_e_phnum = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
unsigned long const&
SgAsmElfFileHeader::get_shextrasz() const {
    return p_shextrasz;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfFileHeader::set_shextrasz(unsigned long const& x) {
    this->p_shextrasz = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
unsigned long const&
SgAsmElfFileHeader::get_e_shnum() const {
    return p_e_shnum;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfFileHeader::set_e_shnum(unsigned long const& x) {
    this->p_e_shnum = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
unsigned long const&
SgAsmElfFileHeader::get_e_shstrndx() const {
    return p_e_shstrndx;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfFileHeader::set_e_shstrndx(unsigned long const& x) {
    this->p_e_shstrndx = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmElfSectionTable* const&
SgAsmElfFileHeader::get_section_table() const {
    return p_section_table;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfFileHeader::set_section_table(SgAsmElfSectionTable* const& x) {
    this->p_section_table = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmElfSegmentTable* const&
SgAsmElfFileHeader::get_segment_table() const {
    return p_segment_table;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfFileHeader::set_segment_table(SgAsmElfSegmentTable* const& x) {
    this->p_segment_table = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmElfFileHeader::~SgAsmElfFileHeader() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmElfFileHeader::SgAsmElfFileHeader()
    : p_e_ident_file_class(0)
    , p_e_ident_data_encoding(0)
    , p_e_ident_file_version(0)
    , p_e_type(0)
    , p_e_machine(0)
    , p_e_flags(0)
    , p_e_ehsize(0)
    , p_phextrasz(0)
    , p_e_phnum(0)
    , p_shextrasz(0)
    , p_e_shnum(0)
    , p_e_shstrndx(0)
    , p_section_table(nullptr)
    , p_segment_table(nullptr) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfFileHeader::initializeProperties() {
    p_e_ident_file_class = 0;
    p_e_ident_data_encoding = 0;
    p_e_ident_file_version = 0;
    p_e_type = 0;
    p_e_machine = 0;
    p_e_flags = 0;
    p_e_ehsize = 0;
    p_phextrasz = 0;
    p_e_phnum = 0;
    p_shextrasz = 0;
    p_e_shnum = 0;
    p_e_shstrndx = 0;
    p_section_table = nullptr;
    p_segment_table = nullptr;
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
