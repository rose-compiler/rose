/** CIL TypeDef tables. */
class SgAsmCilTypeDefTable: public SgAsmCilMetadata {
    [[using Rosebud: rosetta, traverse, mutators(), large]]
    std::vector<SgAsmCilTypeDef*> elements;

public:
    using CilMetadataType = SgAsmCilTypeDef;
};
