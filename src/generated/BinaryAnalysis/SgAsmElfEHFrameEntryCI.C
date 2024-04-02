//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfEHFrameEntryCI            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmElfEHFrameEntryCI_IMPL
#include <sage3basic.h>

int const&
SgAsmElfEHFrameEntryCI::get_version() const {
    return p_version;
}

void
SgAsmElfEHFrameEntryCI::set_version(int const& x) {
    this->p_version = x;
    set_isModified(true);
}

std::string const&
SgAsmElfEHFrameEntryCI::get_augmentation_string() const {
    return p_augmentation_string;
}

void
SgAsmElfEHFrameEntryCI::set_augmentation_string(std::string const& x) {
    this->p_augmentation_string = x;
    set_isModified(true);
}

uint64_t const&
SgAsmElfEHFrameEntryCI::get_eh_data() const {
    return p_eh_data;
}

void
SgAsmElfEHFrameEntryCI::set_eh_data(uint64_t const& x) {
    this->p_eh_data = x;
    set_isModified(true);
}

uint64_t const&
SgAsmElfEHFrameEntryCI::get_code_alignment_factor() const {
    return p_code_alignment_factor;
}

void
SgAsmElfEHFrameEntryCI::set_code_alignment_factor(uint64_t const& x) {
    this->p_code_alignment_factor = x;
    set_isModified(true);
}

int64_t const&
SgAsmElfEHFrameEntryCI::get_data_alignment_factor() const {
    return p_data_alignment_factor;
}

void
SgAsmElfEHFrameEntryCI::set_data_alignment_factor(int64_t const& x) {
    this->p_data_alignment_factor = x;
    set_isModified(true);
}

uint64_t const&
SgAsmElfEHFrameEntryCI::get_augmentation_data_length() const {
    return p_augmentation_data_length;
}

void
SgAsmElfEHFrameEntryCI::set_augmentation_data_length(uint64_t const& x) {
    this->p_augmentation_data_length = x;
    set_isModified(true);
}

int const&
SgAsmElfEHFrameEntryCI::get_lsda_encoding() const {
    return p_lsda_encoding;
}

void
SgAsmElfEHFrameEntryCI::set_lsda_encoding(int const& x) {
    this->p_lsda_encoding = x;
    set_isModified(true);
}

int const&
SgAsmElfEHFrameEntryCI::get_prh_encoding() const {
    return p_prh_encoding;
}

void
SgAsmElfEHFrameEntryCI::set_prh_encoding(int const& x) {
    this->p_prh_encoding = x;
    set_isModified(true);
}

unsigned const&
SgAsmElfEHFrameEntryCI::get_prh_arg() const {
    return p_prh_arg;
}

void
SgAsmElfEHFrameEntryCI::set_prh_arg(unsigned const& x) {
    this->p_prh_arg = x;
    set_isModified(true);
}

rose_addr_t const&
SgAsmElfEHFrameEntryCI::get_prh_addr() const {
    return p_prh_addr;
}

void
SgAsmElfEHFrameEntryCI::set_prh_addr(rose_addr_t const& x) {
    this->p_prh_addr = x;
    set_isModified(true);
}

int const&
SgAsmElfEHFrameEntryCI::get_addr_encoding() const {
    return p_addr_encoding;
}

void
SgAsmElfEHFrameEntryCI::set_addr_encoding(int const& x) {
    this->p_addr_encoding = x;
    set_isModified(true);
}

bool const&
SgAsmElfEHFrameEntryCI::get_sig_frame() const {
    return p_sig_frame;
}

void
SgAsmElfEHFrameEntryCI::set_sig_frame(bool const& x) {
    this->p_sig_frame = x;
    set_isModified(true);
}

SgUnsignedCharList const&
SgAsmElfEHFrameEntryCI::get_instructions() const {
    return p_instructions;
}

SgUnsignedCharList&
SgAsmElfEHFrameEntryCI::get_instructions() {
    return p_instructions;
}

void
SgAsmElfEHFrameEntryCI::set_instructions(SgUnsignedCharList const& x) {
    this->p_instructions = x;
    set_isModified(true);
}

SgAsmElfEHFrameEntryFDList* const&
SgAsmElfEHFrameEntryCI::get_fd_entries() const {
    return p_fd_entries;
}

void
SgAsmElfEHFrameEntryCI::set_fd_entries(SgAsmElfEHFrameEntryFDList* const& x) {
    changeChildPointer(this->p_fd_entries, const_cast<SgAsmElfEHFrameEntryFDList*&>(x));
    set_isModified(true);
}

SgAsmElfEHFrameEntryCI::~SgAsmElfEHFrameEntryCI() {
    destructorHelper();
}

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

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
