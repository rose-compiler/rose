/** CIL File tables. */
class SgAsmCilFileTable: public SgAsmCilMetadata {
    [[using Rosebud: rosetta, traverse, mutators(), large]]
    std::vector<SgAsmCilFile*> elements;

public:
    using CilMetadataType = SgAsmCilFile;
};
