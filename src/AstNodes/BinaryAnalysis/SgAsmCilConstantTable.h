/** CIL Constant tables. */
class SgAsmCilConstantTable: public SgAsmCilMetadata {
public:
    [[using Rosebud: rosetta, traverse, mutators(), large]]
    std::vector<SgAsmCilConstant*> elements;

public:
    using CilMetadataType = SgAsmCilConstant;
};
