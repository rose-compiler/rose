/** CIL Property tables. */
class SgAsmCilPropertyTable: public SgAsmCilMetadata {
public:
    [[using Rosebud: rosetta, traverse, mutators(), large]]
    std::vector<SgAsmCilProperty*> elements;

public:
    using CilMetadataType = SgAsmCilProperty;
};
