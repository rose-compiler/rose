#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmLineNumberTable            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmLineNumberEntryPtrList const&
SgAsmJvmLineNumberTable::get_line_number_table() const {
    return p_line_number_table;
}

#line 552 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmLineNumberEntryPtrList&
SgAsmJvmLineNumberTable::get_line_number_table() {
    return p_line_number_table;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmLineNumberTable::set_line_number_table(SgAsmJvmLineNumberEntryPtrList const& x) {
    this->p_line_number_table = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmLineNumberTable::~SgAsmJvmLineNumberTable() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmLineNumberTable::SgAsmJvmLineNumberTable() {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmLineNumberTable::initializeProperties() {
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
