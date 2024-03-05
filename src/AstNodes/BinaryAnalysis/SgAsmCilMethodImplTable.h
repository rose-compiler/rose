/** CIL MethodImpl tables. */
class SgAsmCilMethodImplTable: public SgAsmCilMetadata {
    [[using Rosebud: rosetta, traverse, mutators(), large]]
    std::vector<SgAsmCilMethodImpl*> elements;

public:
    using CilMetadataType = SgAsmCilMethodImpl;
};
