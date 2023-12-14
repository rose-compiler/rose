/** CIL ClassLayout tables. */
class SgAsmCilClassLayoutTable: public SgAsmCilMetadata {
public:
    [[using Rosebud: rosetta, traverse, mutators(), large]]
    std::vector<SgAsmCilClassLayout*> elements;

public:
    using CilMetadataType = SgAsmCilClassLayout;
};
