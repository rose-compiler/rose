//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmInterpretation            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmInterpretation_IMPL
#include <sage3basic.h>

SgAsmGenericHeaderList* const&
SgAsmInterpretation::get_headers() const {
    return p_headers;
}

void
SgAsmInterpretation::set_headers(SgAsmGenericHeaderList* const& x) {
    this->p_headers = x;
    set_isModified(true);
}

SgAsmBlock* const&
SgAsmInterpretation::get_globalBlock() const {
    return p_globalBlock;
}

void
SgAsmInterpretation::set_globalBlock(SgAsmBlock* const& x) {
    changeChildPointer(this->p_globalBlock, const_cast<SgAsmBlock*&>(x));
    set_isModified(true);
}

Rose::BinaryAnalysis::MemoryMap::Ptr const&
SgAsmInterpretation::get_map() const {
    return p_map;
}

void
SgAsmInterpretation::set_map(Rose::BinaryAnalysis::MemoryMap::Ptr const& x) {
    this->p_map = x;
    set_isModified(true);
}

bool const&
SgAsmInterpretation::get_coverageComputed() const {
    return p_coverageComputed;
}

void
SgAsmInterpretation::set_coverageComputed(bool const& x) {
    this->p_coverageComputed = x;
    set_isModified(true);
}

double const&
SgAsmInterpretation::get_percentageCoverage() const {
    return p_percentageCoverage;
}

void
SgAsmInterpretation::set_percentageCoverage(double const& x) {
    this->p_percentageCoverage = x;
    set_isModified(true);
}

SgAsmInterpretation::~SgAsmInterpretation() {
    destructorHelper();
}

SgAsmInterpretation::SgAsmInterpretation()
    : p_headers(createAndParent<SgAsmGenericHeaderList>(this))
    , p_globalBlock(nullptr)
    , p_coverageComputed(false)
    , p_percentageCoverage(NAN) {}

void
SgAsmInterpretation::initializeProperties() {
    p_headers = createAndParent<SgAsmGenericHeaderList>(this);
    p_globalBlock = nullptr;
    p_coverageComputed = false;
    p_percentageCoverage = NAN;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
