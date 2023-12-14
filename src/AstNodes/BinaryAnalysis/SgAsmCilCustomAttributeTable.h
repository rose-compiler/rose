/** CIL CustomAttribute tables. */
class SgAsmCilCustomAttributeTable: public SgAsmCilMetadata {
public:
    [[using Rosebud: rosetta, traverse, mutators(), large]]
    std::vector<SgAsmCilCustomAttribute*> elements;

public:
    using CilMetadataType = SgAsmCilCustomAttribute;
};
