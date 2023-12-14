/** CIL FieldRVA tables. */
class SgAsmCilFieldRVATable: public SgAsmCilMetadata {
public:
    [[using Rosebud: rosetta, traverse, mutators(), large]]
    std::vector<SgAsmCilFieldRVA*> elements;

public:
    using CilMetadataType = SgAsmCilFieldRVA;
};
