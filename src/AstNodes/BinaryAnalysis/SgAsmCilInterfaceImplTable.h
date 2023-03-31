/** CIL InterfaceImpl tables. */
class SgAsmCilInterfaceImplTable: public SgAsmCilMetadata {
    [[using Rosebud: rosetta, traverse, mutators(), large]]
    std::vector<SgAsmCilInterfaceImpl*> elements;

public:
    using CilMetadataType = SgAsmCilInterfaceImpl;
};
