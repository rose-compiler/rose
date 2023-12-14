/** CIL TypeSpec tables. */
class SgAsmCilTypeSpecTable: public SgAsmCilMetadata {
public:
    [[using Rosebud: rosetta, traverse, mutators(), large]]
    std::vector<SgAsmCilTypeSpec*> elements;

public:
    using CilMetadataType = SgAsmCilTypeSpec;
};

