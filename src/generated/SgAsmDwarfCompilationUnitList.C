#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmDwarfCompilationUnitList            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmDwarfCompilationUnitPtrList const&
SgAsmDwarfCompilationUnitList::get_cu_list() const {
    return p_cu_list;
}

#line 552 "src/Rosebud/RosettaGenerator.C"
SgAsmDwarfCompilationUnitPtrList&
SgAsmDwarfCompilationUnitList::get_cu_list() {
    return p_cu_list;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmDwarfCompilationUnitList::set_cu_list(SgAsmDwarfCompilationUnitPtrList const& x) {
    this->p_cu_list = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmDwarfCompilationUnitList::~SgAsmDwarfCompilationUnitList() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmDwarfCompilationUnitList::SgAsmDwarfCompilationUnitList() {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmDwarfCompilationUnitList::initializeProperties() {
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
