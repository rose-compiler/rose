#ifdef ROSE_IMPL
#include <SgAsmDwarfConstructList.h>
#endif

class SgAsmDwarfSubprogram: public SgAsmDwarfConstruct {
public:
    [[using Rosebud: rosetta, traverse]]
    SgAsmDwarfConstructList* body = NULL;

 // DQ (11/21/2025): Added support for Dwarf 4.
    [[using Rosebud: rosetta]]
    std::string linkage_name;

 // DQ (11/21/2025): Added support for Dwarf 4.
    [[using Rosebud: rosetta]]
    bool is_main_subprogram = false;

 // DQ (11/21/2025): Added support for Dwarf 4.
    [[using Rosebud: rosetta]]  
    bool is_const_expr = false;
  
public:
    SgAsmDwarfConstructList* get_children() override;
};
