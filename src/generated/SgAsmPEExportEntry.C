#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmPEExportEntry            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericString* const&
SgAsmPEExportEntry::get_name() const {
    return p_name;
}

#line 487 "src/Rosebud/RosettaGenerator.C"
unsigned const&
SgAsmPEExportEntry::get_ordinal() const {
    return p_ordinal;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPEExportEntry::set_ordinal(unsigned const& x) {
    this->p_ordinal = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
rose_rva_t const&
SgAsmPEExportEntry::get_export_rva() const {
    return p_export_rva;
}

#line 493 "src/Rosebud/RosettaGenerator.C"
rose_rva_t&
SgAsmPEExportEntry::get_export_rva() {
    return p_export_rva;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPEExportEntry::set_export_rva(rose_rva_t const& x) {
    this->p_export_rva = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericString* const&
SgAsmPEExportEntry::get_forwarder() const {
    return p_forwarder;
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmPEExportEntry::~SgAsmPEExportEntry() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmPEExportEntry::SgAsmPEExportEntry()
    : p_name(nullptr)
    , p_ordinal(0)
    , p_export_rva(0)
    , p_forwarder(nullptr) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPEExportEntry::initializeProperties() {
    p_name = nullptr;
    p_ordinal = 0;
    p_export_rva = 0;
    p_forwarder = nullptr;
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
