//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmLineNumberTable            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

SgAsmJvmLineNumberEntryPtrList const&
SgAsmJvmLineNumberTable::get_line_number_table() const {
    return p_line_number_table;
}

SgAsmJvmLineNumberEntryPtrList&
SgAsmJvmLineNumberTable::get_line_number_table() {
    return p_line_number_table;
}

void
SgAsmJvmLineNumberTable::set_line_number_table(SgAsmJvmLineNumberEntryPtrList const& x) {
    this->p_line_number_table = x;
    set_isModified(true);
}

SgAsmJvmLineNumberTable::~SgAsmJvmLineNumberTable() {
    destructorHelper();
}

SgAsmJvmLineNumberTable::SgAsmJvmLineNumberTable() {}

void
SgAsmJvmLineNumberTable::initializeProperties() {
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
