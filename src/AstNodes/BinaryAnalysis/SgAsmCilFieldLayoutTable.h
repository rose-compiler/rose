/** CIL FieldLayout tables. */
class SgAsmCilFieldLayoutTable: public SgAsmCilMetadata {
    [[using Rosebud: rosetta, traverse, mutators(), large]]
    std::vector<SgAsmCilFieldLayout*> elements;

public:
    using CilMetadataType = SgAsmCilFieldLayout;
};
