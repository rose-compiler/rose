/** CIL ManifestResource tables. */
class SgAsmCilManifestResourceTable: public SgAsmCilMetadata {
public:
    [[using Rosebud: rosetta, traverse, mutators(), large]]
    std::vector<SgAsmCilManifestResource*> elements;

public:
    using CilMetadataType = SgAsmCilManifestResource;
};
