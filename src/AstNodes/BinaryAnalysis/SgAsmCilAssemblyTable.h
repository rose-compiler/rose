/** CIL Assembly tables. */
class SgAsmCilAssemblyTable: public SgAsmCilMetadata {
    [[using Rosebud: rosetta, traverse, mutators(), large]]
    std::vector<SgAsmCilAssembly*> elements;

public:
    using CilMetadataType = SgAsmCilAssembly;
};
