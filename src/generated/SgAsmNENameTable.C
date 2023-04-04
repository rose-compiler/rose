#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmNENameTable            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
SgStringList const&
SgAsmNENameTable::get_names() const {
    return p_names;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmNENameTable::set_names(SgStringList const& x) {
    this->p_names = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
SgUnsignedList const&
SgAsmNENameTable::get_ordinals() const {
    return p_ordinals;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmNENameTable::set_ordinals(SgUnsignedList const& x) {
    this->p_ordinals = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmNENameTable::~SgAsmNENameTable() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmNENameTable::SgAsmNENameTable() {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmNENameTable::initializeProperties() {
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
