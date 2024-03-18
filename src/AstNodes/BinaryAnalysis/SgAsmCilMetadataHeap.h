/** CIL SgAsmCilMetadataHeap node. */
class SgAsmCilMetadataHeap: public SgAsmCilDataStream {

    /** Property: 4 reserved bytes. */
    [[using Rosebud: rosetta]]
    uint32_t ReservedAlwaysZero  = 0;

    /** Property: A 1-byte constant. */
    [[using Rosebud: rosetta]]
    uint8_t MajorVersion  = 0;

    /** Property: A 1-byte constant. */
    [[using Rosebud: rosetta]]
    uint8_t MinorVersion  = 0;

    /** Property: A 1-byte constant indicating the index sizes of streams. */
    [[using Rosebud: rosetta]]
    uint8_t HeapSizes  = 0;

    /** Property: A 1-byte constant. */
    [[using Rosebud: rosetta]]
    uint8_t ReservedAlwaysOne  = 0;

    /** Property: A 4-byte value indicating which metadata tables are present. */
    [[using Rosebud: rosetta]]
    uint64_t Valid  = 0;

    /** Property: A 4-byte value. */
    [[using Rosebud: rosetta]]
    uint64_t Sorted  = 0;

    /** Property: A vector indicating how many rows the present tables contain. */
    [[using Rosebud: rosetta]]
    std::vector<uint32_t> NumberOfRows ;

    /** Property: precomputed flags that determine whether a metadata reference is stored as 2 or 4 byte value in the file. */
    [[using Rosebud: rosetta]]
    uint64_t DataSizeFlags  = 0;

    /** Property: AssemblyTable. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmCilAssemblyTable* AssemblyTable;

    /** Property: AssemblyOSTable. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmCilAssemblyOSTable* AssemblyOSTable;

    /** Property: AssemblyProcessorTable. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmCilAssemblyProcessorTable* AssemblyProcessorTable;

    /** Property: AssemblyRefTable. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmCilAssemblyRefTable* AssemblyRefTable;

    /** Property: AssemblyRefOSTable. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmCilAssemblyRefOSTable* AssemblyRefOSTable;

    /** Property: AssemblyRefProcessorTable. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmCilAssemblyRefProcessorTable* AssemblyRefProcessorTable;

    /** Property: ClassLayoutTable. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmCilClassLayoutTable* ClassLayoutTable;

    /** Property: ConstantTable. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmCilConstantTable* ConstantTable;

    /** Property: CustomAttributeTable. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmCilCustomAttributeTable* CustomAttributeTable;

    /** Property: DeclSecurityTable. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmCilDeclSecurityTable* DeclSecurityTable;

    /** Property: EventTable. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmCilEventTable* EventTable;

    /** Property: EventMapTable. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmCilEventMapTable* EventMapTable;

    /** Property: ExportedTypeTable. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmCilExportedTypeTable* ExportedTypeTable;

    /** Property: FieldTable. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmCilFieldTable* FieldTable;

    /** Property: FieldLayoutTable. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmCilFieldLayoutTable* FieldLayoutTable;

    /** Property: FieldMarshalTable. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmCilFieldMarshalTable* FieldMarshalTable;

    /** Property: FieldRVATable. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmCilFieldRVATable* FieldRVATable;

    /** Property: FileTable. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmCilFileTable* FileTable;

    /** Property: GenericParamTable. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmCilGenericParamTable* GenericParamTable;

    /** Property: GenericParamConstraintTable. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmCilGenericParamConstraintTable* GenericParamConstraintTable;

    /** Property: ImplMapTable. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmCilImplMapTable* ImplMapTable;

    /** Property: InterfaceImplTable. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmCilInterfaceImplTable* InterfaceImplTable;

    /** Property: ManifestResourceTable. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmCilManifestResourceTable* ManifestResourceTable;

    /** Property: MemberRefTable. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmCilMemberRefTable* MemberRefTable;

    /** Property: MethodDefTable. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmCilMethodDefTable* MethodDefTable;

    /** Property: MethodImplTable. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmCilMethodImplTable* MethodImplTable;

    /** Property: MethodSemanticsTable. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmCilMethodSemanticsTable* MethodSemanticsTable;

    /** Property: MethodSpecTable. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmCilMethodSpecTable* MethodSpecTable;

    /** Property: ModuleTable. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmCilModuleTable* ModuleTable;

    /** Property: ModuleRefTable. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmCilModuleRefTable* ModuleRefTable;

    /** Property: NestedClassTable. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmCilNestedClassTable* NestedClassTable;

    /** Property: ParamTable. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmCilParamTable* ParamTable;

    /** Property: PropertyTable. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmCilPropertyTable* PropertyTable;

    /** Property: PropertyMapTable. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmCilPropertyMapTable* PropertyMapTable;

    /** Property: StandAloneSigTable. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmCilStandAloneSigTable* StandAloneSigTable;

    /** Property: TypeDefTable. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmCilTypeDefTable* TypeDefTable;

    /** Property: TypeRefTable. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmCilTypeRefTable* TypeRefTable;

    /** Property: TypeSpecTable. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmCilTypeSpecTable* TypeSpecTable;

public:
    /// reference kinds in the metadata tables
    enum ReferenceKind
    {
      // heaps
      e_ref_string_heap           = 1 << 0,
      e_ref_guid_heap             = 1 << 1,
      e_ref_blob_heap             = 1 << 2,
      // single-table references
      e_ref_assembly_ref          = 1 << 3,
      e_ref_type_def              = 1 << 4,
      e_ref_event                 = 1 << 5,
      e_ref_field                 = 1 << 6,
      e_ref_generic_param         = 1 << 7,
      e_ref_module_ref            = 1 << 8,
      e_ref_method_def            = 1 << 9,
      e_ref_param                 = 1 << 10,
      e_ref_property              = 1 << 11,
      // multi-table references
      e_ref_has_constant          = 1 << 12,
      e_ref_has_custom_attribute  = 1 << 13,
      e_ref_has_decl_security     = 1 << 14,
      e_ref_has_field_marshall    = 1 << 15,
      e_ref_has_semantics         = 1 << 16,
      e_ref_method_def_or_ref     = 1 << 17,
      e_ref_type_def_or_ref       = 1 << 18,
      e_ref_implementation        = 1 << 19,
      e_ref_member_forwarded      = 1 << 20,
      e_ref_member_ref_parent     = 1 << 21,
      e_ref_type_or_method_def    = 1 << 22,
      e_ref_custom_attribute_type = 1 << 23,
      e_ref_resolution_scope      = 1 << 24,
      e_ref_last                  = 1 << 25,
    };

    enum TableKind : std::uint8_t
    {

      e_Assembly = 0x20,
      e_AssemblyOS = 0x22,
      e_AssemblyProcessor = 0x21,
      e_AssemblyRef = 0x23,
      e_AssemblyRefOS = 0x25,
      e_AssemblyRefProcessor = 0x24,
      e_ClassLayout = 0x0F,
      e_Constant = 0x0B,
      e_CustomAttribute = 0x0C,
      e_DeclSecurity = 0x0E,
      e_Event = 0x14,
      e_EventMap = 0x12,
      e_ExportedType = 0x27,
      e_Field = 0x04,
      e_FieldLayout = 0x10,
      e_FieldMarshal = 0x0D,
      e_FieldRVA = 0x1D,
      e_File = 0x26,
      e_GenericParam = 0x2A,
      e_GenericParamConstraint = 0x2C,
      e_ImplMap = 0x1C,
      e_InterfaceImpl = 0x09,
      e_ManifestResource = 0x28,
      e_MemberRef = 0x0A,
      e_MethodDef = 0x06,
      e_MethodImpl = 0x19,
      e_MethodSemantics = 0x18,
      e_MethodSpec = 0x2B,
      e_Module = 0x00,
      e_ModuleRef = 0x1A,
      e_NestedClass = 0x29,
      e_Param = 0x08,
      e_Property = 0x17,
      e_PropertyMap = 0x15,
      e_StandAloneSig = 0x11,
      e_TypeDef = 0x02,
      e_TypeRef = 0x01,
      e_TypeSpec = 0x1B,
      e_Unknown_table_kind = 0xFF
    };

    /** parses the metadata objects from the buffer. */
    void parse(const std::vector<uint8_t>& buf, size_t startOfMetaData) override;

    /** unparses the metadata objects into the buffer */
    void unparse(std::vector<uint8_t>& buf, size_t startOfMetaData) const override;

    /** Prints the object's data in a human readable form. */
    void dump(std::ostream& os) const override;

    /** Look up node by index.
     *
     *  Looks up the node associated with the coded index @p idx in the metadata table associated with @p tblcode. */
    SgAsmCilMetadata* get_MetadataNode(std::uint32_t idx, TableKind tblcode) const;

    /** Look up node by index.
     *
     *  Looks up the node associated with the coded index @p refcode in the metadata tables under the assumption that the value is
     *  of kind @p knd. */
    SgAsmCilMetadata* get_CodedMetadataNode(std::uint32_t refcode, ReferenceKind knd) const;

    /** Look up node by index.
     *
     *  Looks up the node associated with the coded index @p refcode in the metadata tables under the assumption the value is of
     *  kind @p knd. */
    SgAsmCilMetadata* get_MetadataNode(std::uint32_t refcode, ReferenceKind knd) const;
};
