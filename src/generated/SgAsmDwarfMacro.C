//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmDwarfMacro                -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

std::string const&
SgAsmDwarfMacro::get_macro_string() const {
    return p_macro_string;
}

void
SgAsmDwarfMacro::set_macro_string(std::string const& x) {
    this->p_macro_string = x;
    set_isModified(true);
}

SgAsmDwarfMacro::~SgAsmDwarfMacro() {
    destructorHelper();
}

SgAsmDwarfMacro::SgAsmDwarfMacro() {}

void
SgAsmDwarfMacro::initializeProperties() {
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
