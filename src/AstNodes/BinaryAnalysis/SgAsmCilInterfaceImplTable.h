/** CIL InterfaceImpl tables. */
class SgAsmCilInterfaceImplTable: public SgAsmCilMetadata {
public:
    [[using Rosebud: rosetta, traverse, mutators(), large]]
    std::vector<SgAsmCilInterfaceImpl*> elements;

public:
    using CilMetadataType = SgAsmCilInterfaceImpl;
};
