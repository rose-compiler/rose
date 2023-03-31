//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmPERVASizePairList                -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

SgAsmPERVASizePairPtrList const&
SgAsmPERVASizePairList::get_pairs() const {
    return p_pairs;
}

SgAsmPERVASizePairPtrList&
SgAsmPERVASizePairList::get_pairs() {
    return p_pairs;
}

void
SgAsmPERVASizePairList::set_pairs(SgAsmPERVASizePairPtrList const& x) {
    this->p_pairs = x;
    set_isModified(true);
}

SgAsmPERVASizePairList::~SgAsmPERVASizePairList() {
    destructorHelper();
}

SgAsmPERVASizePairList::SgAsmPERVASizePairList() {}

void
SgAsmPERVASizePairList::initializeProperties() {
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
