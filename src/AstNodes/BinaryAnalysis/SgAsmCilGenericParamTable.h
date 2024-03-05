/** CIL GenericParam tables. */
class SgAsmCilGenericParamTable: public SgAsmCilMetadata {
    [[using Rosebud: rosetta, traverse, mutators(), large]]
    std::vector<SgAsmCilGenericParam*> elements;

public:
    using CilMetadataType = SgAsmCilGenericParam;
};
