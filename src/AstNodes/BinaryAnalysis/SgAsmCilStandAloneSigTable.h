/** CIL StandAloneSig tables. */
class SgAsmCilStandAloneSigTable: public SgAsmCilMetadata {
    [[using Rosebud: rosetta, traverse, mutators(), large]]
    std::vector<SgAsmCilStandAloneSig*> elements;

public:
    using CilMetadataType = SgAsmCilStandAloneSig;
};
