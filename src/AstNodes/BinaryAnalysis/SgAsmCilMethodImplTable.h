/** CIL MethodImpl tables. */
class SgAsmCilMethodImplTable: public SgAsmCilMetadata {
public:
    [[using Rosebud: rosetta, traverse, mutators(), large]]
    std::vector<SgAsmCilMethodImpl*> elements;

public:
    using CilMetadataType = SgAsmCilMethodImpl;
};
