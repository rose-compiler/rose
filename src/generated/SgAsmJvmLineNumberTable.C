#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmLineNumberTable            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmLineNumberEntryPtrList const&
SgAsmJvmLineNumberTable::get_line_number_table() const {
    return p_line_number_table;
}

#line 493 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmLineNumberEntryPtrList&
SgAsmJvmLineNumberTable::get_line_number_table() {
    return p_line_number_table;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmLineNumberTable::set_line_number_table(SgAsmJvmLineNumberEntryPtrList const& x) {
    this->p_line_number_table = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmLineNumberTable::~SgAsmJvmLineNumberTable() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmLineNumberTable::SgAsmJvmLineNumberTable() {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmLineNumberTable::initializeProperties() {
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
