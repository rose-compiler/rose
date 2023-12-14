/** CIL Event tables. */
class SgAsmCilEventTable: public SgAsmCilMetadata {
public:
    [[using Rosebud: rosetta, traverse, mutators(), large]]
    std::vector<SgAsmCilEvent*> elements;

public:
    using CilMetadataType = SgAsmCilEvent;
};
