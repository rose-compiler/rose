#ifdef ROSE_IMPL
#include <SgAsmDwarfConstructList.h>
#endif

class SgAsmDwarfStructureType: public SgAsmDwarfConstruct {
public:
    [[using Rosebud: rosetta, traverse]]
    SgAsmDwarfConstructList* body = nullptr;

public:
    SgAsmDwarfConstructList* get_children() override;
};
