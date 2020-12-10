#ifndef UNTYPED_BUILDER_H
#define UNTYPED_BUILDER_H

namespace UntypedBuilder {

void set_language(SgFile::languageOption_enum language);

//! Build an untyped scope including empty lists contained in the scope
template <class ScopeClass>
ScopeClass* buildScope();

//! Build an untyped scope including empty lists contained in the scope
template <class ScopeClass>
ScopeClass* buildScope(const std::string & label);

//! Build an untyped type
SgUntypedType* buildType (SgUntypedType::type_enum type_enum = SgUntypedType::e_unknown, std::string name = "");

//! Build an untyped array type
SgUntypedArrayType* buildArrayType (SgUntypedType::type_enum type_enum, SgUntypedExprListExpression* shape, int rank);

//! Build an untyped Jovial table type from a base type (currently base type can only be an intrinsic type)
SgUntypedTableType* buildJovialTableType (std::string name, SgUntypedType* base_type,
                                          SgUntypedExprListExpression* shape, bool is_anonymous=false);

//! Build an untyped initialized name for the given name and type.  The initializer may be a nullptr.
SgUntypedInitializedName* buildInitializedName(const std::string & name, SgUntypedType* type, SgUntypedExpression* initializer = NULL);

//! Build an untyped initialized name list with potentially one name. The list will be empty if the initialized name parameter is a nullptr.
SgUntypedInitializedNameList* buildInitializedNameList(SgUntypedInitializedName* initialized_name = NULL);

//! Build a variable declaration for only one variable with the given name and type.
SgUntypedVariableDeclaration* buildVariableDeclaration(const std::string & name, SgUntypedType* type,
                                                       SgUntypedExprListExpression* attr_list,
                                                       SgUntypedExpression* initializer = NULL);

 //! Build a variable declaration for only one variable with the given name and base-type declaration.
SgUntypedVariableDeclaration* buildVariableDeclaration(const std::string & name, SgUntypedType* type,
                                                       SgUntypedStructureDeclaration* base_type_decl,
                                                       SgUntypedExprListExpression* attr_list,
                                                       SgUntypedExpression* initializer = NULL);

//! Build an untyped StructureDefinition. This version has a body and thus a scope.
//! Source position for the initializer and modifier lists and table description should be set after construction.
SgUntypedStructureDefinition* buildStructureDefinition();

//! Build an untyped StructureDefinition. This version has a type name and body/scope (default is NULL).
//! If the has_body flag is true and the scope is NULL, a scope will be created.
//! Source position for the initializer and table description should be set after construction.
SgUntypedStructureDefinition* buildStructureDefinition(const std::string type_name, bool has_body=false, SgUntypedScope* scope=NULL);

//! Build an untyped StructureDeclaration. This version builds a contained StructureDefinition with a type name created
//! based on the declared variable name.
SgUntypedStructureDeclaration* buildStructureDeclaration(const std::string struct_name);

//! Build an untyped StructureDeclaration.
//! If the has_body flag is true an untyped StructureDefinition is created.
//! Source position for the initializer and structure definition should be set after construction.
SgUntypedStructureDeclaration* buildStructureDeclaration(const std::string struct_name,
                                                         const std::string struct_type_name, bool has_body=false);

//! Build an untyped JovialTableDescription. This version has a body and thus a scope.
//! Source position for the initializer and modifier lists and table description should be set after construction.
SgUntypedStructureDefinition* buildJovialTableDescription();

//! Build an untyped JovialTableDescription. This version has a type name and body/scope (default is NULL).
//! If the has_body flag is true and the scope is NULL, a scope will be created.
//! Source position for the initializer and table description should be set after construction.
SgUntypedStructureDefinition* buildJovialTableDescription(std::string table_type_name, bool has_body=false, SgUntypedScope* scope=NULL);

//! Build an untyped JovialTableDeclaration with associated JovialTableDescription. This version has a body and thus a scope.
//! Source position for the initializer and modifier lists and table description should be set after construction.
SgUntypedStructureDeclaration* buildJovialTableDeclaration(std::string type_name);

//! Build an untyped directive declaration statement (SgUntypedDirectiveDeclaration)
SgUntypedDirectiveDeclaration* buildDirectiveDeclaration(std::string directive_string);

//! Build a null expression, set file info as the default one
ROSE_DLL_API SgUntypedNullExpression* buildUntypedNullExpression();

}  // namespace UntypedBuilder

#endif
