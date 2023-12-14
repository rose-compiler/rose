/** CIL Field tables. */
class SgAsmCilFieldTable: public SgAsmCilMetadata {
public:
    [[using Rosebud: rosetta, traverse, mutators(), large]]
    std::vector<SgAsmCilField*> elements;

public:
    using CilMetadataType = SgAsmCilField;
};
