/** CIL Param tables. */
class SgAsmCilParamTable: public SgAsmCilMetadata {
public:
    [[using Rosebud: rosetta, traverse, mutators(), large]]
    std::vector<SgAsmCilParam*> elements;

public:
    using CilMetadataType = SgAsmCilParam;
};
