class SgAsmDwarfLine: public SgAsmDwarfInformation {
public:
    [[using Rosebud: rosetta, ctor_arg]]
    uint64_t address = 0;

    // FIXME[Robb Matzke 2024-03-14]: Sg_File_Info::NULL_FILE_ID == -2, but since Sg_File_Info's definition is in the
    // huge Cxx_Grammar.h file we don't want to include it here.
    [[using Rosebud: rosetta, ctor_arg]]
    int file_id = -2;

    [[using Rosebud: rosetta, ctor_arg]]
    int line = 0;

    [[using Rosebud: rosetta, ctor_arg]]
    int column = 0;
};
