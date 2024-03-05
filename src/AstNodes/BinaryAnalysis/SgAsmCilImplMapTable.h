/** CIL ImplMap tables. */
class SgAsmCilImplMapTable: public SgAsmCilMetadata {
    [[using Rosebud: rosetta, traverse, mutators(), large]]
    std::vector<SgAsmCilImplMap*> elements;

public:
    using CilMetadataType = SgAsmCilImplMap;
};
