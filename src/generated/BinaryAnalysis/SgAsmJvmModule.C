//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmModule            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmJvmModule_IMPL
#include <SgAsmJvmModule.h>

uint16_t const&
SgAsmJvmModule::get_module_name_index() const {
    return p_module_name_index;
}

void
SgAsmJvmModule::set_module_name_index(uint16_t const& x) {
    this->p_module_name_index = x;
    set_isModified(true);
}

uint16_t const&
SgAsmJvmModule::get_module_flags() const {
    return p_module_flags;
}

void
SgAsmJvmModule::set_module_flags(uint16_t const& x) {
    this->p_module_flags = x;
    set_isModified(true);
}

uint16_t const&
SgAsmJvmModule::get_module_version_index() const {
    return p_module_version_index;
}

void
SgAsmJvmModule::set_module_version_index(uint16_t const& x) {
    this->p_module_version_index = x;
    set_isModified(true);
}

std::vector<SgAsmJvmModule::Requires*> const&
SgAsmJvmModule::get_requires() const {
    return p_requires;
}

std::vector<SgAsmJvmModule::Requires*>&
SgAsmJvmModule::get_requires() {
    return p_requires;
}

void
SgAsmJvmModule::set_requires(std::vector<SgAsmJvmModule::Requires*> const& x) {
    this->p_requires = x;
    set_isModified(true);
}

std::vector<SgAsmJvmModule::Exports*> const&
SgAsmJvmModule::get_exports() const {
    return p_exports;
}

std::vector<SgAsmJvmModule::Exports*>&
SgAsmJvmModule::get_exports() {
    return p_exports;
}

void
SgAsmJvmModule::set_exports(std::vector<SgAsmJvmModule::Exports*> const& x) {
    this->p_exports = x;
    set_isModified(true);
}

std::vector<SgAsmJvmModule::Opens*> const&
SgAsmJvmModule::get_opens() const {
    return p_opens;
}

std::vector<SgAsmJvmModule::Opens*>&
SgAsmJvmModule::get_opens() {
    return p_opens;
}

void
SgAsmJvmModule::set_opens(std::vector<SgAsmJvmModule::Opens*> const& x) {
    this->p_opens = x;
    set_isModified(true);
}

std::vector<uint16_t> const&
SgAsmJvmModule::get_uses_index() const {
    return p_uses_index;
}

std::vector<uint16_t>&
SgAsmJvmModule::get_uses_index() {
    return p_uses_index;
}

void
SgAsmJvmModule::set_uses_index(std::vector<uint16_t> const& x) {
    this->p_uses_index = x;
    set_isModified(true);
}

std::vector<SgAsmJvmModule::Provides*> const&
SgAsmJvmModule::get_provides() const {
    return p_provides;
}

std::vector<SgAsmJvmModule::Provides*>&
SgAsmJvmModule::get_provides() {
    return p_provides;
}

void
SgAsmJvmModule::set_provides(std::vector<SgAsmJvmModule::Provides*> const& x) {
    this->p_provides = x;
    set_isModified(true);
}

SgAsmJvmModule::~SgAsmJvmModule() {
    destructorHelper();
}

SgAsmJvmModule::SgAsmJvmModule()
    : p_module_name_index(0)
    , p_module_flags(0) {}

void
SgAsmJvmModule::initializeProperties() {
    p_module_name_index = 0;
    p_module_flags = 0;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
