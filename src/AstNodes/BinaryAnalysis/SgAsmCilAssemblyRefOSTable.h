/** CIL AssemblyRefOS tables. */
class SgAsmCilAssemblyRefOSTable: public SgAsmCilMetadata {
public:
    [[using Rosebud: rosetta, traverse, mutators(), large]]
    std::vector<SgAsmCilAssemblyRefOS*> elements;

public:
    using CilMetadataType = SgAsmCilAssemblyRefOS;
};
