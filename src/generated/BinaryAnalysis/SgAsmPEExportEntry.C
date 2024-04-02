//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmPEExportEntry            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmPEExportEntry_IMPL
#include <sage3basic.h>

SgAsmGenericString* const&
SgAsmPEExportEntry::get_name() const {
    return p_name;
}

unsigned const&
SgAsmPEExportEntry::get_ordinal() const {
    return p_ordinal;
}

void
SgAsmPEExportEntry::set_ordinal(unsigned const& x) {
    this->p_ordinal = x;
    set_isModified(true);
}

Rose::BinaryAnalysis::RelativeVirtualAddress const&
SgAsmPEExportEntry::get_exportRva() const {
    return p_exportRva;
}

Rose::BinaryAnalysis::RelativeVirtualAddress&
SgAsmPEExportEntry::get_exportRva() {
    return p_exportRva;
}

void
SgAsmPEExportEntry::set_exportRva(Rose::BinaryAnalysis::RelativeVirtualAddress const& x) {
    this->p_exportRva = x;
    set_isModified(true);
}

SgAsmGenericString* const&
SgAsmPEExportEntry::get_forwarder() const {
    return p_forwarder;
}

SgAsmPEExportEntry::~SgAsmPEExportEntry() {
    destructorHelper();
}

SgAsmPEExportEntry::SgAsmPEExportEntry()
    : p_name(nullptr)
    , p_ordinal(0)
    , p_forwarder(nullptr) {}

void
SgAsmPEExportEntry::initializeProperties() {
    p_name = nullptr;
    p_ordinal = 0;
    p_forwarder = nullptr;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
