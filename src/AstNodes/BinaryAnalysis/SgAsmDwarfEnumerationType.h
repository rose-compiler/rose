#ifdef ROSE_IMPL
#include <SgAsmDwarfConstructList.h>
#endif

class SgAsmDwarfEnumerationType: public SgAsmDwarfConstruct {
public:
    [[using Rosebud: rosetta, traverse]]
    SgAsmDwarfConstructList* body = NULL;

 // DQ (11/21/2025): Added support for Dwarf 4.
    [[using Rosebud: rosetta]]
    bool is_enum_class = false;

public:
    SgAsmDwarfConstructList* get_children() override;
};
