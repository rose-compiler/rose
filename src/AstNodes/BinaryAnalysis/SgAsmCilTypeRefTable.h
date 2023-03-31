/** CIL TypeRef tables. */
class SgAsmCilTypeRefTable: public SgAsmCilMetadata {
    [[using Rosebud: rosetta, traverse, mutators(), large]]
    std::vector<SgAsmCilTypeRef*> elements;

public:
    using CilMetadataType = SgAsmCilTypeRef;
};
