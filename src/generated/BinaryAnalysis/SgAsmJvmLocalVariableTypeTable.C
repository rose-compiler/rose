//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmLocalVariableTypeTable            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmJvmLocalVariableTypeTable_IMPL
#include <sage3basic.h>

SgAsmJvmLocalVariableTypeEntryPtrList const&
SgAsmJvmLocalVariableTypeTable::get_local_variable_type_table() const {
    return p_local_variable_type_table;
}

SgAsmJvmLocalVariableTypeEntryPtrList&
SgAsmJvmLocalVariableTypeTable::get_local_variable_type_table() {
    return p_local_variable_type_table;
}

void
SgAsmJvmLocalVariableTypeTable::set_local_variable_type_table(SgAsmJvmLocalVariableTypeEntryPtrList const& x) {
    this->p_local_variable_type_table = x;
    set_isModified(true);
}

SgAsmJvmLocalVariableTypeTable::~SgAsmJvmLocalVariableTypeTable() {
    destructorHelper();
}

SgAsmJvmLocalVariableTypeTable::SgAsmJvmLocalVariableTypeTable() {}

void
SgAsmJvmLocalVariableTypeTable::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
