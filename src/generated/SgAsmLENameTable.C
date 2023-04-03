#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmLENameTable            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
SgStringList const&
SgAsmLENameTable::get_names() const {
    return p_names;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmLENameTable::set_names(SgStringList const& x) {
    this->p_names = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
SgUnsignedList const&
SgAsmLENameTable::get_ordinals() const {
    return p_ordinals;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmLENameTable::set_ordinals(SgUnsignedList const& x) {
    this->p_ordinals = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmLENameTable::~SgAsmLENameTable() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmLENameTable::SgAsmLENameTable() {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmLENameTable::initializeProperties() {
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
