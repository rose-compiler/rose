//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmIndexTableAttr            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmJvmIndexTableAttr_IMPL
#include <SgAsmJvmIndexTableAttr.h>

unsigned const&
SgAsmJvmIndexTableAttr::get_attribute_type() const {
    return p_attribute_type;
}

void
SgAsmJvmIndexTableAttr::set_attribute_type(unsigned const& x) {
    this->p_attribute_type = x;
    set_isModified(true);
}

std::vector<uint16_t> const&
SgAsmJvmIndexTableAttr::get_table() const {
    return p_table;
}

std::vector<uint16_t>&
SgAsmJvmIndexTableAttr::get_table() {
    return p_table;
}

SgAsmJvmIndexTableAttr::~SgAsmJvmIndexTableAttr() {
    destructorHelper();
}

SgAsmJvmIndexTableAttr::SgAsmJvmIndexTableAttr()
    : p_attribute_type(SgAsmJvmIndexTableAttr::ATTR_NONE) {}

void
SgAsmJvmIndexTableAttr::initializeProperties() {
    p_attribute_type = SgAsmJvmIndexTableAttr::ATTR_NONE;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
