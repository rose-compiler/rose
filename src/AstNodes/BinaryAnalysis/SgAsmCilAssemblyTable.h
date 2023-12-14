/** CIL Assembly tables. */
class SgAsmCilAssemblyTable: public SgAsmCilMetadata {
public:
    [[using Rosebud: rosetta, traverse, mutators(), large]]
    std::vector<SgAsmCilAssembly*> elements;

public:
    using CilMetadataType = SgAsmCilAssembly;
};
