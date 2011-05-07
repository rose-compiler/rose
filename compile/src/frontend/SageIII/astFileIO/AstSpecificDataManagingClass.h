/* #line 1 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarStaticDataManagingClassHeader.macro" */
/* JH (01/01/2006) This file is generated using ROSETTA. It should never be 
   manipulated by hand. The generation is located in buildStorageClasses.C!

    This file contains all declarations for the StorageClasses used for the ast file IO.
*/
class SgProject;


class AstSpecificDataManagingClass 
  {
    private:
     SgProject* rootOfAst;
     int astIndex;
     unsigned long listOfAccumulatedPoolSizes [ 666 + 1 ];
     SgFunctionTypeTable*  SgNode_globalFunctionTypeTable;
     SgTypeTable*  SgNode_globalTypeTable;
     std::map<SgNode*,std::string>  SgNode_globalMangledNameMap;
     std::map<std::string, int>  SgNode_shortMangledNameCache;
     std::map<int, std::string>  Sg_File_Info_fileidtoname_map;
     std::map<std::string, int>  Sg_File_Info_nametofileid_map;
     SgTypePtrList  SgUnparse_Info_structureTagProcessingList;
     bool  SgUnparse_Info_forceDefaultConstructorToTriggerError;
     int  SgGraph_index_counter;
     int  SgGraphNode_index_counter;
     int  SgGraphEdge_index_counter;
     SgTypeUnknown*  SgTypeUnknown_builtin_type;
     SgTypeChar*  SgTypeChar_builtin_type;
     SgTypeSignedChar*  SgTypeSignedChar_builtin_type;
     SgTypeUnsignedChar*  SgTypeUnsignedChar_builtin_type;
     SgTypeShort*  SgTypeShort_builtin_type;
     SgTypeSignedShort*  SgTypeSignedShort_builtin_type;
     SgTypeUnsignedShort*  SgTypeUnsignedShort_builtin_type;
     SgTypeInt*  SgTypeInt_builtin_type;
     SgTypeSignedInt*  SgTypeSignedInt_builtin_type;
     SgTypeUnsignedInt*  SgTypeUnsignedInt_builtin_type;
     SgTypeLong*  SgTypeLong_builtin_type;
     SgTypeSignedLong*  SgTypeSignedLong_builtin_type;
     SgTypeUnsignedLong*  SgTypeUnsignedLong_builtin_type;
     SgTypeVoid*  SgTypeVoid_builtin_type;
     SgTypeGlobalVoid*  SgTypeGlobalVoid_builtin_type;
     SgTypeWchar*  SgTypeWchar_builtin_type;
     SgTypeFloat*  SgTypeFloat_builtin_type;
     SgTypeDouble*  SgTypeDouble_builtin_type;
     SgTypeLongLong*  SgTypeLongLong_builtin_type;
     SgTypeSignedLongLong*  SgTypeSignedLongLong_builtin_type;
     SgTypeUnsignedLongLong*  SgTypeUnsignedLongLong_builtin_type;
     SgTypeLongDouble*  SgTypeLongDouble_builtin_type;
     SgTypeBool*  SgTypeBool_builtin_type;
     SgNamedType*  SgNamedType_builtin_type;
     SgPartialFunctionModifierType*  SgPartialFunctionModifierType_builtin_type;
     SgTypeEllipse*  SgTypeEllipse_builtin_type;
     SgTypeDefault*  SgTypeDefault_builtin_type;
     SgTypeCAFTeam*  SgTypeCAFTeam_builtin_type;
     SgTypeCrayPointer*  SgTypeCrayPointer_builtin_type;
     SgTypeLabel*  SgTypeLabel_builtin_type;
     SgAsmTypeByte*  SgAsmTypeByte_builtin_type;
     SgAsmTypeWord*  SgAsmTypeWord_builtin_type;
     SgAsmTypeDoubleWord*  SgAsmTypeDoubleWord_builtin_type;
     SgAsmTypeQuadWord*  SgAsmTypeQuadWord_builtin_type;
     SgAsmTypeDoubleQuadWord*  SgAsmTypeDoubleQuadWord_builtin_type;
     SgAsmType80bitFloat*  SgAsmType80bitFloat_builtin_type;
     SgAsmType128bitFloat*  SgAsmType128bitFloat_builtin_type;
     SgAsmTypeSingleFloat*  SgAsmTypeSingleFloat_builtin_type;
     SgAsmTypeDoubleFloat*  SgAsmTypeDoubleFloat_builtin_type;

/* #line 16 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarStaticDataManagingClassHeader.macro" */
    public:
     AstSpecificDataManagingClass(SgProject* root);
     AstSpecificDataManagingClass(const AstSpecificDataManagingClassStorageClass& source );
     SgProject* getRootOfAst () const; 
     unsigned long getNumberOfAccumulatedNodes (const int position) const; 
     unsigned long getMemoryPoolSize (const int position) const ;
     unsigned long getTotalNumberOfASTIRNodes () const;
     int get_AstIndex() const ;
     void setStaticDataMembersOfIRNodes ( ) const ;
     SgFunctionTypeTable*  get_SgNode_globalFunctionTypeTable() const;
     SgTypeTable*  get_SgNode_globalTypeTable() const;
     std::map<SgNode*,std::string>  get_SgNode_globalMangledNameMap() const;
     std::map<std::string, int>  get_SgNode_shortMangledNameCache() const;
     std::map<int, std::string>  get_Sg_File_Info_fileidtoname_map() const;
     std::map<std::string, int>  get_Sg_File_Info_nametofileid_map() const;
     SgTypePtrList  get_SgUnparse_Info_structureTagProcessingList() const;
     bool  get_SgUnparse_Info_forceDefaultConstructorToTriggerError() const;
     int  get_SgGraph_index_counter() const;
     int  get_SgGraphNode_index_counter() const;
     int  get_SgGraphEdge_index_counter() const;
     SgTypeUnknown*  get_SgTypeUnknown_builtin_type() const;
     SgTypeChar*  get_SgTypeChar_builtin_type() const;
     SgTypeSignedChar*  get_SgTypeSignedChar_builtin_type() const;
     SgTypeUnsignedChar*  get_SgTypeUnsignedChar_builtin_type() const;
     SgTypeShort*  get_SgTypeShort_builtin_type() const;
     SgTypeSignedShort*  get_SgTypeSignedShort_builtin_type() const;
     SgTypeUnsignedShort*  get_SgTypeUnsignedShort_builtin_type() const;
     SgTypeInt*  get_SgTypeInt_builtin_type() const;
     SgTypeSignedInt*  get_SgTypeSignedInt_builtin_type() const;
     SgTypeUnsignedInt*  get_SgTypeUnsignedInt_builtin_type() const;
     SgTypeLong*  get_SgTypeLong_builtin_type() const;
     SgTypeSignedLong*  get_SgTypeSignedLong_builtin_type() const;
     SgTypeUnsignedLong*  get_SgTypeUnsignedLong_builtin_type() const;
     SgTypeVoid*  get_SgTypeVoid_builtin_type() const;
     SgTypeGlobalVoid*  get_SgTypeGlobalVoid_builtin_type() const;
     SgTypeWchar*  get_SgTypeWchar_builtin_type() const;
     SgTypeFloat*  get_SgTypeFloat_builtin_type() const;
     SgTypeDouble*  get_SgTypeDouble_builtin_type() const;
     SgTypeLongLong*  get_SgTypeLongLong_builtin_type() const;
     SgTypeSignedLongLong*  get_SgTypeSignedLongLong_builtin_type() const;
     SgTypeUnsignedLongLong*  get_SgTypeUnsignedLongLong_builtin_type() const;
     SgTypeLongDouble*  get_SgTypeLongDouble_builtin_type() const;
     SgTypeBool*  get_SgTypeBool_builtin_type() const;
     SgNamedType*  get_SgNamedType_builtin_type() const;
     SgPartialFunctionModifierType*  get_SgPartialFunctionModifierType_builtin_type() const;
     SgTypeEllipse*  get_SgTypeEllipse_builtin_type() const;
     SgTypeDefault*  get_SgTypeDefault_builtin_type() const;
     SgTypeCAFTeam*  get_SgTypeCAFTeam_builtin_type() const;
     SgTypeCrayPointer*  get_SgTypeCrayPointer_builtin_type() const;
     SgTypeLabel*  get_SgTypeLabel_builtin_type() const;
     SgAsmTypeByte*  get_SgAsmTypeByte_builtin_type() const;
     SgAsmTypeWord*  get_SgAsmTypeWord_builtin_type() const;
     SgAsmTypeDoubleWord*  get_SgAsmTypeDoubleWord_builtin_type() const;
     SgAsmTypeQuadWord*  get_SgAsmTypeQuadWord_builtin_type() const;
     SgAsmTypeDoubleQuadWord*  get_SgAsmTypeDoubleQuadWord_builtin_type() const;
     SgAsmType80bitFloat*  get_SgAsmType80bitFloat_builtin_type() const;
     SgAsmType128bitFloat*  get_SgAsmType128bitFloat_builtin_type() const;
     SgAsmTypeSingleFloat*  get_SgAsmTypeSingleFloat_builtin_type() const;
     SgAsmTypeDoubleFloat*  get_SgAsmTypeDoubleFloat_builtin_type() const;

/* #line 26 "/g/g15/bronevet/Compilers/ROSE/src/ROSETTA/Grammar/grammarStaticDataManagingClassHeader.macro" */

    friend class AST_FILE_IO;
    friend class AstSpecificDataManagingClassStorageClass;
  };


