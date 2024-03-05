/** CIL MemberRef tables. */
class SgAsmCilMemberRefTable: public SgAsmCilMetadata {
    [[using Rosebud: rosetta, traverse, mutators(), large]]
    std::vector<SgAsmCilMemberRef*> elements;

public:
    using CilMetadataType = SgAsmCilMemberRef;
};
