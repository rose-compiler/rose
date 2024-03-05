/** CIL FieldRVA tables. */
class SgAsmCilFieldRVATable: public SgAsmCilMetadata {
    [[using Rosebud: rosetta, traverse, mutators(), large]]
    std::vector<SgAsmCilFieldRVA*> elements;

public:
    using CilMetadataType = SgAsmCilFieldRVA;
};
