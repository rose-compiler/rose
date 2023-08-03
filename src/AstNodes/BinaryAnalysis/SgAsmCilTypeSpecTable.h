/** CIL TypeSpec tables. */
class SgAsmCilTypeSpecTable: public SgAsmCilMetadata {
    [[using Rosebud: rosetta, traverse, mutators(), large]]
    std::vector<SgAsmCilTypeSpec*> elements;

public:
    using CilMetadataType = SgAsmCilTypeSpec;
};

