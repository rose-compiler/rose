/** CIL MethodDef tables. */
class SgAsmCilMethodDefTable: public SgAsmCilMetadata {
    [[using Rosebud: rosetta, traverse, mutators(), large]]
    std::vector<SgAsmCilMethodDef*> elements;

public:
    using CilMetadataType = SgAsmCilMethodDef;
};
