#include <sageContainer.h>

class SgAsmDwarfCompilationUnitList: public SgAsmDwarfInformation {
public:
    [[using Rosebud: rosetta, traverse, large]]
    SgAsmDwarfCompilationUnitPtrList cu_list;
};
