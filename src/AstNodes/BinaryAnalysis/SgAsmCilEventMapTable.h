/** CIL EventMap tables. */
class SgAsmCilEventMapTable: public SgAsmCilMetadata {
    [[using Rosebud: rosetta, traverse, mutators(), large]]
    std::vector<SgAsmCilEventMap*> elements;

public:
    using CilMetadataType = SgAsmCilEventMap;
};
