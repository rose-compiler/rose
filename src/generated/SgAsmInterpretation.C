#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmInterpretation            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericHeaderList* const&
SgAsmInterpretation::get_headers() const {
    return p_headers;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmInterpretation::set_headers(SgAsmGenericHeaderList* const& x) {
    this->p_headers = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmBlock* const&
SgAsmInterpretation::get_global_block() const {
    return p_global_block;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmInterpretation::set_global_block(SgAsmBlock* const& x) {
    this->p_global_block = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
Rose::BinaryAnalysis::MemoryMap::Ptr const&
SgAsmInterpretation::get_map() const {
    return p_map;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmInterpretation::set_map(Rose::BinaryAnalysis::MemoryMap::Ptr const& x) {
    this->p_map = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
bool const&
SgAsmInterpretation::get_coverageComputed() const {
    return p_coverageComputed;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmInterpretation::set_coverageComputed(bool const& x) {
    this->p_coverageComputed = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
double const&
SgAsmInterpretation::get_percentageCoverage() const {
    return p_percentageCoverage;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmInterpretation::set_percentageCoverage(double const& x) {
    this->p_percentageCoverage = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmInterpretation::~SgAsmInterpretation() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmInterpretation::SgAsmInterpretation()
#line 308 "src/Rosebud/RosettaGenerator.C"
    : p_headers(createAndParent<SgAsmGenericHeaderList>(this))
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_global_block(nullptr)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_coverageComputed(false)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_percentageCoverage(NAN) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmInterpretation::initializeProperties() {
    p_headers = createAndParent<SgAsmGenericHeaderList>(this);
    p_global_block = nullptr;
    p_coverageComputed = false;
    p_percentageCoverage = NAN;
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
