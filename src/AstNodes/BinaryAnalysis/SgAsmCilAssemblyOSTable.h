/** CIL AssemblyOS tables. */
class SgAsmCilAssemblyOSTable: public SgAsmCilMetadata {
    [[using Rosebud: rosetta, traverse, mutators(), large]]
    std::vector<SgAsmCilAssemblyOS*> elements;

public:
    using CilMetadataType = SgAsmCilAssemblyOS;
};
