//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmLineNumberTable            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmJvmLineNumberTable_IMPL
#include <SgAsmJvmLineNumberTable.h>

std::vector<SgAsmJvmLineNumberTable::Entry*> const&
SgAsmJvmLineNumberTable::get_line_number_table() const {
    return p_line_number_table;
}

std::vector<SgAsmJvmLineNumberTable::Entry*>&
SgAsmJvmLineNumberTable::get_line_number_table() {
    return p_line_number_table;
}

SgAsmJvmLineNumberTable::~SgAsmJvmLineNumberTable() {
    destructorHelper();
}

SgAsmJvmLineNumberTable::SgAsmJvmLineNumberTable() {}

void
SgAsmJvmLineNumberTable::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
