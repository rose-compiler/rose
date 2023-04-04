#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmGenericHeader            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericFormat* const&
SgAsmGenericHeader::get_exec_format() const {
    return p_exec_format;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmGenericHeader::set_exec_format(SgAsmGenericFormat* const& x) {
    this->p_exec_format = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
SgCharList const&
SgAsmGenericHeader::get_magic() const {
    return p_magic;
}

#line 552 "src/Rosebud/RosettaGenerator.C"
SgCharList&
SgAsmGenericHeader::get_magic() {
    return p_magic;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmGenericHeader::set_magic(SgCharList const& x) {
    this->p_magic = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericFormat::InsSetArchitecture const&
SgAsmGenericHeader::get_isa() const {
    return p_isa;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmGenericHeader::set_isa(SgAsmGenericFormat::InsSetArchitecture const& x) {
    this->p_isa = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
rose_addr_t const&
SgAsmGenericHeader::get_base_va() const {
    return p_base_va;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmGenericHeader::set_base_va(rose_addr_t const& x) {
    this->p_base_va = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
SgRVAList const&
SgAsmGenericHeader::get_entry_rvas() const {
    return p_entry_rvas;
}

#line 552 "src/Rosebud/RosettaGenerator.C"
SgRVAList&
SgAsmGenericHeader::get_entry_rvas() {
    return p_entry_rvas;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmGenericHeader::set_entry_rvas(SgRVAList const& x) {
    this->p_entry_rvas = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericDLLList* const&
SgAsmGenericHeader::get_dlls() const {
    return p_dlls;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmGenericHeader::set_dlls(SgAsmGenericDLLList* const& x) {
    this->p_dlls = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericSectionList* const&
SgAsmGenericHeader::get_sections() const {
    return p_sections;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmGenericHeader::set_sections(SgAsmGenericSectionList* const& x) {
    this->p_sections = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericHeader::~SgAsmGenericHeader() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericHeader::SgAsmGenericHeader()
#line 308 "src/Rosebud/RosettaGenerator.C"
    : p_exec_format(createAndParent<SgAsmGenericFormat>(this))
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_isa(SgAsmGenericFormat::ISA_UNSPECIFIED)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_base_va(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_dlls(createAndParent<SgAsmGenericDLLList>(this))
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_sections(createAndParent<SgAsmGenericSectionList>(this)) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmGenericHeader::initializeProperties() {
    p_exec_format = createAndParent<SgAsmGenericFormat>(this);
    p_isa = SgAsmGenericFormat::ISA_UNSPECIFIED;
    p_base_va = 0;
    p_dlls = createAndParent<SgAsmGenericDLLList>(this);
    p_sections = createAndParent<SgAsmGenericSectionList>(this);
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
