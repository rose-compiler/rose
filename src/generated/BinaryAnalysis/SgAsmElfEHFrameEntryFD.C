//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfEHFrameEntryFD            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#include <sage3basic.h>

Rose::BinaryAnalysis::RelativeVirtualAddress const&
SgAsmElfEHFrameEntryFD::get_begin_rva() const {
    return p_begin_rva;
}

Rose::BinaryAnalysis::RelativeVirtualAddress&
SgAsmElfEHFrameEntryFD::get_begin_rva() {
    return p_begin_rva;
}

void
SgAsmElfEHFrameEntryFD::set_begin_rva(Rose::BinaryAnalysis::RelativeVirtualAddress const& x) {
    this->p_begin_rva = x;
    set_isModified(true);
}

rose_addr_t const&
SgAsmElfEHFrameEntryFD::get_size() const {
    return p_size;
}

void
SgAsmElfEHFrameEntryFD::set_size(rose_addr_t const& x) {
    this->p_size = x;
    set_isModified(true);
}

SgUnsignedCharList const&
SgAsmElfEHFrameEntryFD::get_augmentation_data() const {
    return p_augmentation_data;
}

SgUnsignedCharList&
SgAsmElfEHFrameEntryFD::get_augmentation_data() {
    return p_augmentation_data;
}

void
SgAsmElfEHFrameEntryFD::set_augmentation_data(SgUnsignedCharList const& x) {
    this->p_augmentation_data = x;
    set_isModified(true);
}

SgUnsignedCharList const&
SgAsmElfEHFrameEntryFD::get_instructions() const {
    return p_instructions;
}

SgUnsignedCharList&
SgAsmElfEHFrameEntryFD::get_instructions() {
    return p_instructions;
}

void
SgAsmElfEHFrameEntryFD::set_instructions(SgUnsignedCharList const& x) {
    this->p_instructions = x;
    set_isModified(true);
}

SgAsmElfEHFrameEntryFD::~SgAsmElfEHFrameEntryFD() {
    destructorHelper();
}

SgAsmElfEHFrameEntryFD::SgAsmElfEHFrameEntryFD()
    : p_size(0) {}

void
SgAsmElfEHFrameEntryFD::initializeProperties() {
    p_size = 0;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
