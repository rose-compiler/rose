//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmLocalVariableTable            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmJvmLocalVariableTable_IMPL
#include <sage3basic.h>

SgAsmJvmLocalVariableEntryPtrList const&
SgAsmJvmLocalVariableTable::get_local_variable_table() const {
    return p_local_variable_table;
}

SgAsmJvmLocalVariableEntryPtrList&
SgAsmJvmLocalVariableTable::get_local_variable_table() {
    return p_local_variable_table;
}

void
SgAsmJvmLocalVariableTable::set_local_variable_table(SgAsmJvmLocalVariableEntryPtrList const& x) {
    this->p_local_variable_table = x;
    set_isModified(true);
}

SgAsmJvmLocalVariableTable::~SgAsmJvmLocalVariableTable() {
    destructorHelper();
}

SgAsmJvmLocalVariableTable::SgAsmJvmLocalVariableTable() {}

void
SgAsmJvmLocalVariableTable::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
