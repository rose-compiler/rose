#ifdef ROSE_IMPL
#include <SgAsmDwarfConstructList.h>
#endif

class SgAsmDwarfSubprogram: public SgAsmDwarfConstruct {
public:
    [[using Rosebud: rosetta, traverse]]
    SgAsmDwarfConstructList* body = NULL;

public:
    SgAsmDwarfConstructList* get_children() override;
};
