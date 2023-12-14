/** CIL FieldMarshal tables. */
class SgAsmCilFieldMarshalTable: public SgAsmCilMetadata {
public:
    [[using Rosebud: rosetta, traverse, mutators(), large]]
    std::vector<SgAsmCilFieldMarshal*> elements;

public:
    using CilMetadataType = SgAsmCilFieldMarshal;
};
