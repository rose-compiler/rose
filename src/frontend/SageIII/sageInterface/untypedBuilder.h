#ifndef UNTYPED_BUILDER_H
#define UNTYPED_BUILDER_H

namespace UntypedBuilder {

void set_language(SgFile::languageOption_enum language);

SgUntypedType*      buildType       (SgUntypedType::type_enum type_enum = SgUntypedType::e_unknown);
SgUntypedArrayType* buildArrayType  (SgUntypedType::type_enum type_enum, SgUntypedExprListExpression* shape, int rank);

}  // namespace UntypedBuilder

#endif
