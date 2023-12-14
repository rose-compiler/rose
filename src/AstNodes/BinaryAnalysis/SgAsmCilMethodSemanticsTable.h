/** CIL MethodSemantics tables. */
class SgAsmCilMethodSemanticsTable: public SgAsmCilMetadata {
public:
    [[using Rosebud: rosetta, traverse, mutators(), large]]
    std::vector<SgAsmCilMethodSemantics*> elements;

public:
    using CilMetadataType = SgAsmCilMethodSemantics;
};
