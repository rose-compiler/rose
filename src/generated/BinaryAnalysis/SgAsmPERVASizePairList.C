//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmPERVASizePairList            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmPERVASizePairList_IMPL
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
    changeChildPointer(this->p_pairs, const_cast<SgAsmPERVASizePairPtrList&>(x));
    set_isModified(true);
}

SgAsmPERVASizePairList::~SgAsmPERVASizePairList() {
    destructorHelper();
}

SgAsmPERVASizePairList::SgAsmPERVASizePairList() {}

void
SgAsmPERVASizePairList::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
