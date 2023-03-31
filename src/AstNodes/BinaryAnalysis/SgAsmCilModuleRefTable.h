/** CIL ModuleRef tables. */
class SgAsmCilModuleRefTable: public SgAsmCilMetadata {
    [[using Rosebud: rosetta, traverse, mutators(), large]]
    std::vector<SgAsmCilModuleRef*> elements;

public:
    using CilMetadataType = SgAsmCilModuleRef;
};
