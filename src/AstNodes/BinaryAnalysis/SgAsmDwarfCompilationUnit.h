#ifdef ROSE_IMPL
#include <SgAsmDwarfConstructList.h>
#include <SgAsmDwarfLineList.h>
#include <SgAsmDwarfMacroList.h>
#endif

class SgAsmDwarfCompilationUnit: public SgAsmDwarfConstruct {
public:
    [[using Rosebud: rosetta]]
    std::string producer;

    [[using Rosebud: rosetta]]
    std::string language;

    [[using Rosebud: rosetta]]
    uint64_t low_pc = 0;

    [[using Rosebud: rosetta]]
    uint64_t hi_pc = 0;

    [[using Rosebud: rosetta]]
    int version_stamp = 0;

    [[using Rosebud: rosetta]]
    uint64_t abbrev_offset = 0;

    [[using Rosebud: rosetta]]
    uint64_t address_size = 0;

    [[using Rosebud: rosetta]]
    uint64_t offset_length = 0;

    [[using Rosebud: rosetta, traverse]]
    SgAsmDwarfLineList* line_info = nullptr;

    [[using Rosebud: rosetta, traverse]]
    SgAsmDwarfConstructList* language_constructs = nullptr;

    [[using Rosebud: rosetta, traverse]]
    SgAsmDwarfMacroList* macro_info = nullptr;

public:
    SgAsmDwarfConstructList* get_children() override;
};
