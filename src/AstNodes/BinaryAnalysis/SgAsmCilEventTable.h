/** CIL Event tables. */
class SgAsmCilEventTable: public SgAsmCilMetadata {
    [[using Rosebud: rosetta, traverse, mutators(), large]]
    std::vector<SgAsmCilEvent*> elements;

public:
    using CilMetadataType = SgAsmCilEvent;
};
