#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfEHFrameEntryCI            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
int const&
SgAsmElfEHFrameEntryCI::get_version() const {
    return p_version;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfEHFrameEntryCI::set_version(int const& x) {
    this->p_version = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
std::string const&
SgAsmElfEHFrameEntryCI::get_augmentation_string() const {
    return p_augmentation_string;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfEHFrameEntryCI::set_augmentation_string(std::string const& x) {
    this->p_augmentation_string = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
uint64_t const&
SgAsmElfEHFrameEntryCI::get_eh_data() const {
    return p_eh_data;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfEHFrameEntryCI::set_eh_data(uint64_t const& x) {
    this->p_eh_data = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
uint64_t const&
SgAsmElfEHFrameEntryCI::get_code_alignment_factor() const {
    return p_code_alignment_factor;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfEHFrameEntryCI::set_code_alignment_factor(uint64_t const& x) {
    this->p_code_alignment_factor = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
int64_t const&
SgAsmElfEHFrameEntryCI::get_data_alignment_factor() const {
    return p_data_alignment_factor;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfEHFrameEntryCI::set_data_alignment_factor(int64_t const& x) {
    this->p_data_alignment_factor = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
uint64_t const&
SgAsmElfEHFrameEntryCI::get_augmentation_data_length() const {
    return p_augmentation_data_length;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfEHFrameEntryCI::set_augmentation_data_length(uint64_t const& x) {
    this->p_augmentation_data_length = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
int const&
SgAsmElfEHFrameEntryCI::get_lsda_encoding() const {
    return p_lsda_encoding;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfEHFrameEntryCI::set_lsda_encoding(int const& x) {
    this->p_lsda_encoding = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
int const&
SgAsmElfEHFrameEntryCI::get_prh_encoding() const {
    return p_prh_encoding;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfEHFrameEntryCI::set_prh_encoding(int const& x) {
    this->p_prh_encoding = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
unsigned const&
SgAsmElfEHFrameEntryCI::get_prh_arg() const {
    return p_prh_arg;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfEHFrameEntryCI::set_prh_arg(unsigned const& x) {
    this->p_prh_arg = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
rose_addr_t const&
SgAsmElfEHFrameEntryCI::get_prh_addr() const {
    return p_prh_addr;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfEHFrameEntryCI::set_prh_addr(rose_addr_t const& x) {
    this->p_prh_addr = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
int const&
SgAsmElfEHFrameEntryCI::get_addr_encoding() const {
    return p_addr_encoding;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfEHFrameEntryCI::set_addr_encoding(int const& x) {
    this->p_addr_encoding = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
bool const&
SgAsmElfEHFrameEntryCI::get_sig_frame() const {
    return p_sig_frame;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfEHFrameEntryCI::set_sig_frame(bool const& x) {
    this->p_sig_frame = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
SgUnsignedCharList const&
SgAsmElfEHFrameEntryCI::get_instructions() const {
    return p_instructions;
}

#line 493 "src/Rosebud/RosettaGenerator.C"
SgUnsignedCharList&
SgAsmElfEHFrameEntryCI::get_instructions() {
    return p_instructions;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfEHFrameEntryCI::set_instructions(SgUnsignedCharList const& x) {
    this->p_instructions = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmElfEHFrameEntryFDList* const&
SgAsmElfEHFrameEntryCI::get_fd_entries() const {
    return p_fd_entries;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfEHFrameEntryCI::set_fd_entries(SgAsmElfEHFrameEntryFDList* const& x) {
    this->p_fd_entries = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmElfEHFrameEntryCI::~SgAsmElfEHFrameEntryCI() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmElfEHFrameEntryCI::SgAsmElfEHFrameEntryCI()
    : p_version(0)
    , p_eh_data(0)
    , p_code_alignment_factor(0)
    , p_data_alignment_factor(0)
    , p_augmentation_data_length(0)
    , p_lsda_encoding(-1)
    , p_prh_encoding(-1)
    , p_prh_arg(0)
    , p_prh_addr(0)
    , p_addr_encoding(-1)
    , p_sig_frame(false)
    , p_fd_entries(createAndParent<SgAsmElfEHFrameEntryFDList>(this)) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfEHFrameEntryCI::initializeProperties() {
    p_version = 0;
    p_eh_data = 0;
    p_code_alignment_factor = 0;
    p_data_alignment_factor = 0;
    p_augmentation_data_length = 0;
    p_lsda_encoding = -1;
    p_prh_encoding = -1;
    p_prh_arg = 0;
    p_prh_addr = 0;
    p_addr_encoding = -1;
    p_sig_frame = false;
    p_fd_entries = createAndParent<SgAsmElfEHFrameEntryFDList>(this);
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
