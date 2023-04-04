#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmPEExportEntry            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericString* const&
SgAsmPEExportEntry::get_name() const {
    return p_name;
}

#line 546 "src/Rosebud/RosettaGenerator.C"
unsigned const&
SgAsmPEExportEntry::get_ordinal() const {
    return p_ordinal;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPEExportEntry::set_ordinal(unsigned const& x) {
    this->p_ordinal = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
rose_rva_t const&
SgAsmPEExportEntry::get_export_rva() const {
    return p_export_rva;
}

#line 552 "src/Rosebud/RosettaGenerator.C"
rose_rva_t&
SgAsmPEExportEntry::get_export_rva() {
    return p_export_rva;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPEExportEntry::set_export_rva(rose_rva_t const& x) {
    this->p_export_rva = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericString* const&
SgAsmPEExportEntry::get_forwarder() const {
    return p_forwarder;
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmPEExportEntry::~SgAsmPEExportEntry() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmPEExportEntry::SgAsmPEExportEntry()
#line 308 "src/Rosebud/RosettaGenerator.C"
    : p_name(nullptr)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_ordinal(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_export_rva(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_forwarder(nullptr) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPEExportEntry::initializeProperties() {
    p_name = nullptr;
    p_ordinal = 0;
    p_export_rva = 0;
    p_forwarder = nullptr;
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
