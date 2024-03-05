/** CIL Property tables. */
class SgAsmCilPropertyTable: public SgAsmCilMetadata {
    [[using Rosebud: rosetta, traverse, mutators(), large]]
    std::vector<SgAsmCilProperty*> elements;

public:
    using CilMetadataType = SgAsmCilProperty;
};
