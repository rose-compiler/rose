/** CIL AssemblyRefProcessor tables. */
class SgAsmCilAssemblyRefProcessorTable: public SgAsmCilMetadata {
    [[using Rosebud: rosetta, traverse, mutators(), large]]
    std::vector<SgAsmCilAssemblyRefProcessor*> elements;

public:
    using CilMetadataType = SgAsmCilAssemblyRefProcessor;
};
