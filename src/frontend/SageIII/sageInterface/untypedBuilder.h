#ifndef UNTYPED_BUILDER_H
#define UNTYPED_BUILDER_H

namespace UntypedBuilder {

void set_language(SgFile::languageOption_enum language);

//! Build an untyped scope including empty lists contained in the scope
SgUntypedScope* buildUntypedScope(const std::string & label);

//! Build an untyped type
SgUntypedType* buildType (SgUntypedType::type_enum type_enum = SgUntypedType::e_unknown, std::string name = "");

//! Build an untyped array type
SgUntypedArrayType* buildArrayType (SgUntypedType::type_enum type_enum, SgUntypedExprListExpression* shape, int rank);

//! Build a null expression, set file info as the default one
ROSE_DLL_API SgUntypedNullExpression* buildUntypedNullExpression();

}  // namespace UntypedBuilder

#endif
