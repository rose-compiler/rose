class SgAsmDwarfLine: public SgAsmDwarfInformation {
public:
    [[using Rosebud: rosetta, ctor_arg]]
    uint64_t address = 0;

    [[using Rosebud: rosetta, ctor_arg]]
    int file_id = Sg_File_Info::NULL_FILE_ID;

    [[using Rosebud: rosetta, ctor_arg]]
    int line = 0;

    [[using Rosebud: rosetta, ctor_arg]]
    int column = 0;
};
