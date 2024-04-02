//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmAarch32Coprocessor            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmAarch32Coprocessor_IMPL
#include <sage3basic.h>

int const&
SgAsmAarch32Coprocessor::coprocessor() const {
    return coprocessor_;
}

void
SgAsmAarch32Coprocessor::coprocessor(int const& x) {
    this->coprocessor_ = x;
    set_isModified(true);
}

SgAsmAarch32Coprocessor::~SgAsmAarch32Coprocessor() {
    destructorHelper();
}

SgAsmAarch32Coprocessor::SgAsmAarch32Coprocessor() {}

// The association between constructor arguments and their classes:
//    property=coprocessor      class=SgAsmAarch32Coprocessor
SgAsmAarch32Coprocessor::SgAsmAarch32Coprocessor(int const& coprocessor)
    : coprocessor_(coprocessor) {}

void
SgAsmAarch32Coprocessor::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
