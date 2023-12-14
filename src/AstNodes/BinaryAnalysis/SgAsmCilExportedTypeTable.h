/** CIL ExportedType tables. */
class SgAsmCilExportedTypeTable: public SgAsmCilMetadata {
public:
    [[using Rosebud: rosetta, traverse, mutators(), large]]
    std::vector<SgAsmCilExportedType*> elements;

public:
    using CilMetadataType = SgAsmCilExportedType;
};
