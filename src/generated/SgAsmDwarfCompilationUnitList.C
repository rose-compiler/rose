#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmDwarfCompilationUnitList            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmDwarfCompilationUnitPtrList const&
SgAsmDwarfCompilationUnitList::get_cu_list() const {
    return p_cu_list;
}

#line 493 "src/Rosebud/RosettaGenerator.C"
SgAsmDwarfCompilationUnitPtrList&
SgAsmDwarfCompilationUnitList::get_cu_list() {
    return p_cu_list;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmDwarfCompilationUnitList::set_cu_list(SgAsmDwarfCompilationUnitPtrList const& x) {
    this->p_cu_list = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmDwarfCompilationUnitList::~SgAsmDwarfCompilationUnitList() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmDwarfCompilationUnitList::SgAsmDwarfCompilationUnitList() {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmDwarfCompilationUnitList::initializeProperties() {
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
