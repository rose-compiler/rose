/** CIL GenericParamConstraint tables. */
class SgAsmCilGenericParamConstraintTable: public SgAsmCilMetadata {
    [[using Rosebud: rosetta, traverse, mutators(), large]]
    std::vector<SgAsmCilGenericParamConstraint*> elements;

public:
    using CilMetadataType = SgAsmCilGenericParamConstraint;
};
