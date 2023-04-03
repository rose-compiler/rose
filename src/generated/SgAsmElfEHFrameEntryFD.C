#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfEHFrameEntryFD            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
rose_rva_t const&
SgAsmElfEHFrameEntryFD::get_begin_rva() const {
    return p_begin_rva;
}

#line 493 "src/Rosebud/RosettaGenerator.C"
rose_rva_t&
SgAsmElfEHFrameEntryFD::get_begin_rva() {
    return p_begin_rva;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfEHFrameEntryFD::set_begin_rva(rose_rva_t const& x) {
    this->p_begin_rva = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
rose_addr_t const&
SgAsmElfEHFrameEntryFD::get_size() const {
    return p_size;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfEHFrameEntryFD::set_size(rose_addr_t const& x) {
    this->p_size = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
SgUnsignedCharList const&
SgAsmElfEHFrameEntryFD::get_augmentation_data() const {
    return p_augmentation_data;
}

#line 493 "src/Rosebud/RosettaGenerator.C"
SgUnsignedCharList&
SgAsmElfEHFrameEntryFD::get_augmentation_data() {
    return p_augmentation_data;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfEHFrameEntryFD::set_augmentation_data(SgUnsignedCharList const& x) {
    this->p_augmentation_data = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
SgUnsignedCharList const&
SgAsmElfEHFrameEntryFD::get_instructions() const {
    return p_instructions;
}

#line 493 "src/Rosebud/RosettaGenerator.C"
SgUnsignedCharList&
SgAsmElfEHFrameEntryFD::get_instructions() {
    return p_instructions;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfEHFrameEntryFD::set_instructions(SgUnsignedCharList const& x) {
    this->p_instructions = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmElfEHFrameEntryFD::~SgAsmElfEHFrameEntryFD() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmElfEHFrameEntryFD::SgAsmElfEHFrameEntryFD()
    : p_begin_rva(0)
    , p_size(0) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfEHFrameEntryFD::initializeProperties() {
    p_begin_rva = 0;
    p_size = 0;
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
