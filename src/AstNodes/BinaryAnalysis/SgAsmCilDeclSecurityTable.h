/** CIL DeclSecurity tables. */
class SgAsmCilDeclSecurityTable: public SgAsmCilMetadata {
    [[using Rosebud: rosetta, traverse, mutators(), large]]
    std::vector<SgAsmCilDeclSecurity*> elements;

public:
    using CilMetadataType = SgAsmCilDeclSecurity;
};
