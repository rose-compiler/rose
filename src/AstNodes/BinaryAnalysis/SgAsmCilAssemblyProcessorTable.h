/** CIL AssemblyProcessor tables. */
class SgAsmCilAssemblyProcessorTable: public SgAsmCilMetadata {
public:
    [[using Rosebud: rosetta, traverse, mutators(), large]]
    std::vector<SgAsmCilAssemblyProcessor*> elements;

public:
    using CilMetadataType = SgAsmCilAssemblyProcessor;
};
