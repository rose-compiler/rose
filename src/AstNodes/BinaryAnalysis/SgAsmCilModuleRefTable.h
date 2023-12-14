/** CIL ModuleRef tables. */
class SgAsmCilModuleRefTable: public SgAsmCilMetadata {
public:
    [[using Rosebud: rosetta, traverse, mutators(), large]]
    std::vector<SgAsmCilModuleRef*> elements;

public:
    using CilMetadataType = SgAsmCilModuleRef;
};
