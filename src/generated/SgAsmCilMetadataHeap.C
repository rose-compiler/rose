//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCilMetadataHeap            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

uint32_t const&
SgAsmCilMetadataHeap::get_ReservedAlwaysZero() const {
    return p_ReservedAlwaysZero;
}

void
SgAsmCilMetadataHeap::set_ReservedAlwaysZero(uint32_t const& x) {
    this->p_ReservedAlwaysZero = x;
    set_isModified(true);
}

uint8_t const&
SgAsmCilMetadataHeap::get_MajorVersion() const {
    return p_MajorVersion;
}

void
SgAsmCilMetadataHeap::set_MajorVersion(uint8_t const& x) {
    this->p_MajorVersion = x;
    set_isModified(true);
}

uint8_t const&
SgAsmCilMetadataHeap::get_MinorVersion() const {
    return p_MinorVersion;
}

void
SgAsmCilMetadataHeap::set_MinorVersion(uint8_t const& x) {
    this->p_MinorVersion = x;
    set_isModified(true);
}

uint8_t const&
SgAsmCilMetadataHeap::get_HeapSizes() const {
    return p_HeapSizes;
}

void
SgAsmCilMetadataHeap::set_HeapSizes(uint8_t const& x) {
    this->p_HeapSizes = x;
    set_isModified(true);
}

uint8_t const&
SgAsmCilMetadataHeap::get_ReservedAlwaysOne() const {
    return p_ReservedAlwaysOne;
}

void
SgAsmCilMetadataHeap::set_ReservedAlwaysOne(uint8_t const& x) {
    this->p_ReservedAlwaysOne = x;
    set_isModified(true);
}

uint64_t const&
SgAsmCilMetadataHeap::get_Valid() const {
    return p_Valid;
}

void
SgAsmCilMetadataHeap::set_Valid(uint64_t const& x) {
    this->p_Valid = x;
    set_isModified(true);
}

uint64_t const&
SgAsmCilMetadataHeap::get_Sorted() const {
    return p_Sorted;
}

void
SgAsmCilMetadataHeap::set_Sorted(uint64_t const& x) {
    this->p_Sorted = x;
    set_isModified(true);
}

std::vector<uint32_t> const&
SgAsmCilMetadataHeap::get_NumberOfRows() const {
    return p_NumberOfRows;
}

void
SgAsmCilMetadataHeap::set_NumberOfRows(std::vector<uint32_t> const& x) {
    this->p_NumberOfRows = x;
    set_isModified(true);
}

uint64_t const&
SgAsmCilMetadataHeap::get_DataSizeFlags() const {
    return p_DataSizeFlags;
}

void
SgAsmCilMetadataHeap::set_DataSizeFlags(uint64_t const& x) {
    this->p_DataSizeFlags = x;
    set_isModified(true);
}

SgAsmCilAssemblyTable* const&
SgAsmCilMetadataHeap::get_AssemblyTable() const {
    return p_AssemblyTable;
}

void
SgAsmCilMetadataHeap::set_AssemblyTable(SgAsmCilAssemblyTable* const& x) {
    this->p_AssemblyTable = x;
    set_isModified(true);
}

SgAsmCilAssemblyOSTable* const&
SgAsmCilMetadataHeap::get_AssemblyOSTable() const {
    return p_AssemblyOSTable;
}

void
SgAsmCilMetadataHeap::set_AssemblyOSTable(SgAsmCilAssemblyOSTable* const& x) {
    this->p_AssemblyOSTable = x;
    set_isModified(true);
}

SgAsmCilAssemblyProcessorTable* const&
SgAsmCilMetadataHeap::get_AssemblyProcessorTable() const {
    return p_AssemblyProcessorTable;
}

void
SgAsmCilMetadataHeap::set_AssemblyProcessorTable(SgAsmCilAssemblyProcessorTable* const& x) {
    this->p_AssemblyProcessorTable = x;
    set_isModified(true);
}

SgAsmCilAssemblyRefTable* const&
SgAsmCilMetadataHeap::get_AssemblyRefTable() const {
    return p_AssemblyRefTable;
}

void
SgAsmCilMetadataHeap::set_AssemblyRefTable(SgAsmCilAssemblyRefTable* const& x) {
    this->p_AssemblyRefTable = x;
    set_isModified(true);
}

SgAsmCilAssemblyRefOSTable* const&
SgAsmCilMetadataHeap::get_AssemblyRefOSTable() const {
    return p_AssemblyRefOSTable;
}

void
SgAsmCilMetadataHeap::set_AssemblyRefOSTable(SgAsmCilAssemblyRefOSTable* const& x) {
    this->p_AssemblyRefOSTable = x;
    set_isModified(true);
}

SgAsmCilAssemblyRefProcessorTable* const&
SgAsmCilMetadataHeap::get_AssemblyRefProcessorTable() const {
    return p_AssemblyRefProcessorTable;
}

void
SgAsmCilMetadataHeap::set_AssemblyRefProcessorTable(SgAsmCilAssemblyRefProcessorTable* const& x) {
    this->p_AssemblyRefProcessorTable = x;
    set_isModified(true);
}

SgAsmCilClassLayoutTable* const&
SgAsmCilMetadataHeap::get_ClassLayoutTable() const {
    return p_ClassLayoutTable;
}

void
SgAsmCilMetadataHeap::set_ClassLayoutTable(SgAsmCilClassLayoutTable* const& x) {
    this->p_ClassLayoutTable = x;
    set_isModified(true);
}

SgAsmCilConstantTable* const&
SgAsmCilMetadataHeap::get_ConstantTable() const {
    return p_ConstantTable;
}

void
SgAsmCilMetadataHeap::set_ConstantTable(SgAsmCilConstantTable* const& x) {
    this->p_ConstantTable = x;
    set_isModified(true);
}

SgAsmCilCustomAttributeTable* const&
SgAsmCilMetadataHeap::get_CustomAttributeTable() const {
    return p_CustomAttributeTable;
}

void
SgAsmCilMetadataHeap::set_CustomAttributeTable(SgAsmCilCustomAttributeTable* const& x) {
    this->p_CustomAttributeTable = x;
    set_isModified(true);
}

SgAsmCilDeclSecurityTable* const&
SgAsmCilMetadataHeap::get_DeclSecurityTable() const {
    return p_DeclSecurityTable;
}

void
SgAsmCilMetadataHeap::set_DeclSecurityTable(SgAsmCilDeclSecurityTable* const& x) {
    this->p_DeclSecurityTable = x;
    set_isModified(true);
}

SgAsmCilEventTable* const&
SgAsmCilMetadataHeap::get_EventTable() const {
    return p_EventTable;
}

void
SgAsmCilMetadataHeap::set_EventTable(SgAsmCilEventTable* const& x) {
    this->p_EventTable = x;
    set_isModified(true);
}

SgAsmCilEventMapTable* const&
SgAsmCilMetadataHeap::get_EventMapTable() const {
    return p_EventMapTable;
}

void
SgAsmCilMetadataHeap::set_EventMapTable(SgAsmCilEventMapTable* const& x) {
    this->p_EventMapTable = x;
    set_isModified(true);
}

SgAsmCilExportedTypeTable* const&
SgAsmCilMetadataHeap::get_ExportedTypeTable() const {
    return p_ExportedTypeTable;
}

void
SgAsmCilMetadataHeap::set_ExportedTypeTable(SgAsmCilExportedTypeTable* const& x) {
    this->p_ExportedTypeTable = x;
    set_isModified(true);
}

SgAsmCilFieldTable* const&
SgAsmCilMetadataHeap::get_FieldTable() const {
    return p_FieldTable;
}

void
SgAsmCilMetadataHeap::set_FieldTable(SgAsmCilFieldTable* const& x) {
    this->p_FieldTable = x;
    set_isModified(true);
}

SgAsmCilFieldLayoutTable* const&
SgAsmCilMetadataHeap::get_FieldLayoutTable() const {
    return p_FieldLayoutTable;
}

void
SgAsmCilMetadataHeap::set_FieldLayoutTable(SgAsmCilFieldLayoutTable* const& x) {
    this->p_FieldLayoutTable = x;
    set_isModified(true);
}

SgAsmCilFieldMarshalTable* const&
SgAsmCilMetadataHeap::get_FieldMarshalTable() const {
    return p_FieldMarshalTable;
}

void
SgAsmCilMetadataHeap::set_FieldMarshalTable(SgAsmCilFieldMarshalTable* const& x) {
    this->p_FieldMarshalTable = x;
    set_isModified(true);
}

SgAsmCilFieldRVATable* const&
SgAsmCilMetadataHeap::get_FieldRVATable() const {
    return p_FieldRVATable;
}

void
SgAsmCilMetadataHeap::set_FieldRVATable(SgAsmCilFieldRVATable* const& x) {
    this->p_FieldRVATable = x;
    set_isModified(true);
}

SgAsmCilFileTable* const&
SgAsmCilMetadataHeap::get_FileTable() const {
    return p_FileTable;
}

void
SgAsmCilMetadataHeap::set_FileTable(SgAsmCilFileTable* const& x) {
    this->p_FileTable = x;
    set_isModified(true);
}

SgAsmCilGenericParamTable* const&
SgAsmCilMetadataHeap::get_GenericParamTable() const {
    return p_GenericParamTable;
}

void
SgAsmCilMetadataHeap::set_GenericParamTable(SgAsmCilGenericParamTable* const& x) {
    this->p_GenericParamTable = x;
    set_isModified(true);
}

SgAsmCilGenericParamConstraintTable* const&
SgAsmCilMetadataHeap::get_GenericParamConstraintTable() const {
    return p_GenericParamConstraintTable;
}

void
SgAsmCilMetadataHeap::set_GenericParamConstraintTable(SgAsmCilGenericParamConstraintTable* const& x) {
    this->p_GenericParamConstraintTable = x;
    set_isModified(true);
}

SgAsmCilImplMapTable* const&
SgAsmCilMetadataHeap::get_ImplMapTable() const {
    return p_ImplMapTable;
}

void
SgAsmCilMetadataHeap::set_ImplMapTable(SgAsmCilImplMapTable* const& x) {
    this->p_ImplMapTable = x;
    set_isModified(true);
}

SgAsmCilInterfaceImplTable* const&
SgAsmCilMetadataHeap::get_InterfaceImplTable() const {
    return p_InterfaceImplTable;
}

void
SgAsmCilMetadataHeap::set_InterfaceImplTable(SgAsmCilInterfaceImplTable* const& x) {
    this->p_InterfaceImplTable = x;
    set_isModified(true);
}

SgAsmCilManifestResourceTable* const&
SgAsmCilMetadataHeap::get_ManifestResourceTable() const {
    return p_ManifestResourceTable;
}

void
SgAsmCilMetadataHeap::set_ManifestResourceTable(SgAsmCilManifestResourceTable* const& x) {
    this->p_ManifestResourceTable = x;
    set_isModified(true);
}

SgAsmCilMemberRefTable* const&
SgAsmCilMetadataHeap::get_MemberRefTable() const {
    return p_MemberRefTable;
}

void
SgAsmCilMetadataHeap::set_MemberRefTable(SgAsmCilMemberRefTable* const& x) {
    this->p_MemberRefTable = x;
    set_isModified(true);
}

SgAsmCilMethodDefTable* const&
SgAsmCilMetadataHeap::get_MethodDefTable() const {
    return p_MethodDefTable;
}

void
SgAsmCilMetadataHeap::set_MethodDefTable(SgAsmCilMethodDefTable* const& x) {
    this->p_MethodDefTable = x;
    set_isModified(true);
}

SgAsmCilMethodImplTable* const&
SgAsmCilMetadataHeap::get_MethodImplTable() const {
    return p_MethodImplTable;
}

void
SgAsmCilMetadataHeap::set_MethodImplTable(SgAsmCilMethodImplTable* const& x) {
    this->p_MethodImplTable = x;
    set_isModified(true);
}

SgAsmCilMethodSemanticsTable* const&
SgAsmCilMetadataHeap::get_MethodSemanticsTable() const {
    return p_MethodSemanticsTable;
}

void
SgAsmCilMetadataHeap::set_MethodSemanticsTable(SgAsmCilMethodSemanticsTable* const& x) {
    this->p_MethodSemanticsTable = x;
    set_isModified(true);
}

SgAsmCilMethodSpecTable* const&
SgAsmCilMetadataHeap::get_MethodSpecTable() const {
    return p_MethodSpecTable;
}

void
SgAsmCilMetadataHeap::set_MethodSpecTable(SgAsmCilMethodSpecTable* const& x) {
    this->p_MethodSpecTable = x;
    set_isModified(true);
}

SgAsmCilModuleTable* const&
SgAsmCilMetadataHeap::get_ModuleTable() const {
    return p_ModuleTable;
}

void
SgAsmCilMetadataHeap::set_ModuleTable(SgAsmCilModuleTable* const& x) {
    this->p_ModuleTable = x;
    set_isModified(true);
}

SgAsmCilModuleRefTable* const&
SgAsmCilMetadataHeap::get_ModuleRefTable() const {
    return p_ModuleRefTable;
}

void
SgAsmCilMetadataHeap::set_ModuleRefTable(SgAsmCilModuleRefTable* const& x) {
    this->p_ModuleRefTable = x;
    set_isModified(true);
}

SgAsmCilNestedClassTable* const&
SgAsmCilMetadataHeap::get_NestedClassTable() const {
    return p_NestedClassTable;
}

void
SgAsmCilMetadataHeap::set_NestedClassTable(SgAsmCilNestedClassTable* const& x) {
    this->p_NestedClassTable = x;
    set_isModified(true);
}

SgAsmCilParamTable* const&
SgAsmCilMetadataHeap::get_ParamTable() const {
    return p_ParamTable;
}

void
SgAsmCilMetadataHeap::set_ParamTable(SgAsmCilParamTable* const& x) {
    this->p_ParamTable = x;
    set_isModified(true);
}

SgAsmCilPropertyTable* const&
SgAsmCilMetadataHeap::get_PropertyTable() const {
    return p_PropertyTable;
}

void
SgAsmCilMetadataHeap::set_PropertyTable(SgAsmCilPropertyTable* const& x) {
    this->p_PropertyTable = x;
    set_isModified(true);
}

SgAsmCilPropertyMapTable* const&
SgAsmCilMetadataHeap::get_PropertyMapTable() const {
    return p_PropertyMapTable;
}

void
SgAsmCilMetadataHeap::set_PropertyMapTable(SgAsmCilPropertyMapTable* const& x) {
    this->p_PropertyMapTable = x;
    set_isModified(true);
}

SgAsmCilStandAloneSigTable* const&
SgAsmCilMetadataHeap::get_StandAloneSigTable() const {
    return p_StandAloneSigTable;
}

void
SgAsmCilMetadataHeap::set_StandAloneSigTable(SgAsmCilStandAloneSigTable* const& x) {
    this->p_StandAloneSigTable = x;
    set_isModified(true);
}

SgAsmCilTypeDefTable* const&
SgAsmCilMetadataHeap::get_TypeDefTable() const {
    return p_TypeDefTable;
}

void
SgAsmCilMetadataHeap::set_TypeDefTable(SgAsmCilTypeDefTable* const& x) {
    this->p_TypeDefTable = x;
    set_isModified(true);
}

SgAsmCilTypeRefTable* const&
SgAsmCilMetadataHeap::get_TypeRefTable() const {
    return p_TypeRefTable;
}

void
SgAsmCilMetadataHeap::set_TypeRefTable(SgAsmCilTypeRefTable* const& x) {
    this->p_TypeRefTable = x;
    set_isModified(true);
}

SgAsmCilTypeSpecTable* const&
SgAsmCilMetadataHeap::get_TypeSpecTable() const {
    return p_TypeSpecTable;
}

void
SgAsmCilMetadataHeap::set_TypeSpecTable(SgAsmCilTypeSpecTable* const& x) {
    this->p_TypeSpecTable = x;
    set_isModified(true);
}

SgAsmCilMetadataHeap::~SgAsmCilMetadataHeap() {
    destructorHelper();
}

SgAsmCilMetadataHeap::SgAsmCilMetadataHeap() {}

// The association between constructor arguments and their classes:
//    property=Offset           class=SgAsmCilDataStream
//    property=Size             class=SgAsmCilDataStream
//    property=Name             class=SgAsmCilDataStream
//    property=NamePadding      class=SgAsmCilDataStream
SgAsmCilMetadataHeap::SgAsmCilMetadataHeap(uint32_t const& Offset,
                                           uint32_t const& Size,
                                           std::string const& Name,
                                           uint32_t const& NamePadding)
    : SgAsmCilDataStream(Offset, Size, Name, NamePadding) {}

void
SgAsmCilMetadataHeap::initializeProperties() {
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
