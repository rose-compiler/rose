/** CIL Module tables. */
class SgAsmCilModuleTable: public SgAsmCilMetadata {
public:
    [[using Rosebud: rosetta, traverse, mutators(), large]]
    std::vector<SgAsmCilModule*> elements;

public:
    using CilMetadataType = SgAsmCilModule;
};
