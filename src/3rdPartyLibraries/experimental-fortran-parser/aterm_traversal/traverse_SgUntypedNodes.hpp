#ifndef TRAVERSE_SG_UNTYPED_NODES_HPP
#define TRAVERSE_SG_UNTYPED_NODES_HPP


ATbool traverse_SgLocatedNode(ATerm term, SgLocatedNode** var_SgLocatedNode);
ATbool traverse_SgLocatedNodeSupport(ATerm term, SgLocatedNodeSupport** var_SgLocatedNodeSupport);
ATbool traverse_SgUntypedNode(ATerm term, SgUntypedNode** var_SgUntypedNode);
ATbool traverse_SgUntypedExpression(ATerm term, SgUntypedExpression** var_SgUntypedExpression);
//CER - no C++ type for  SgToken_ROSE_Fortran_Keywords
//CER-FIXME ATbool traverse_SgToken_ROSE_Fortran_Keywords(ATerm term, SgToken::ROSE_Fortran_Keywords** var_SgToken_ROSE_Fortran_Keywords);
ATbool traverse_SgToken_ROSE_Fortran_Keywords(ATerm term, SgToken::ROSE_Fortran_Keywords* var_SgToken_ROSE_Fortran_Keywords);
//CER - no C++ type for  SgToken_ROSE_Fortran_Operators
ATbool traverse_SgToken_ROSE_Fortran_Operators(ATerm term, SgToken::ROSE_Fortran_Operators** var_SgToken_ROSE_Fortran_Operators);
//CER - no C++ type for SgUntypedType_type_enum
ATbool traverse_SgUntypedType_type_enum(ATerm term, SgUntypedType::type_enum** var_SgUntypedType_type_enum);
ATbool traverse_bool(ATerm term, bool** var_bool);
ATbool traverse_SgUntypedUnaryOperator(ATerm term, SgUntypedUnaryOperator** var_SgUntypedUnaryOperator);
ATbool traverse_SgUntypedBinaryOperator(ATerm term, SgUntypedBinaryOperator** var_SgUntypedBinaryOperator);
ATbool traverse_SgUntypedValueExpression(ATerm term, SgUntypedValueExpression** var_SgUntypedValueExpression);
ATbool traverse_SgUntypedArrayReferenceExpression(ATerm term, SgUntypedArrayReferenceExpression** var_SgUntypedArrayReferenceExpression);
ATbool traverse_SgUntypedOtherExpression(ATerm term, SgUntypedOtherExpression** var_SgUntypedOtherExpression);
ATbool traverse_SgUntypedFunctionCallOrArrayReferenceExpression(ATerm term, SgUntypedFunctionCallOrArrayReferenceExpression** var_SgUntypedFunctionCallOrArrayReferenceExpression);
ATbool traverse_SgUntypedReferenceExpression(ATerm term, SgUntypedReferenceExpression** var_SgUntypedReferenceExpression);
ATbool traverse_SgUntypedImplicitDeclaration(ATerm term, SgUntypedImplicitDeclaration** var_SgUntypedImplicitDeclaration);
ATbool traverse_SgUntypedVariableDeclaration(ATerm term, SgUntypedVariableDeclaration** var_SgUntypedVariableDeclaration);
ATbool traverse_SgUntypedFunctionDeclaration(ATerm term, SgUntypedFunctionDeclaration** var_SgUntypedFunctionDeclaration);
ATbool traverse_SgUntypedModuleDeclaration(ATerm term, SgUntypedModuleDeclaration** var_SgUntypedModuleDeclaration);
ATbool traverse_SgUntypedProgramHeaderDeclaration(ATerm term, SgUntypedProgramHeaderDeclaration** var_SgUntypedProgramHeaderDeclaration);
ATbool traverse_SgUntypedSubroutineDeclaration(ATerm term, SgUntypedSubroutineDeclaration** var_SgUntypedSubroutineDeclaration);
ATbool traverse_SgUntypedStatement(ATerm term, SgUntypedStatement** var_SgUntypedStatement);
ATbool traverse_SgUntypedType(ATerm term, SgUntypedType** var_SgUntypedType);
ATbool traverse_SgUntypedAttribute(ATerm term, SgUntypedAttribute** var_SgUntypedAttribute);
ATbool traverse_SgUntypedInitializedName(ATerm term, SgUntypedInitializedName** var_SgUntypedInitializedName);
ATbool traverse_SgUntypedFile(ATerm term, SgUntypedFile** var_SgUntypedFile);
ATbool traverse_SgUntypedStatementList(ATerm term, SgUntypedStatementList** var_SgUntypedStatementList);
ATbool traverse_SgUntypedDeclarationStatementList(ATerm term, SgUntypedDeclarationStatementList** var_SgUntypedDeclarationStatementList);
ATbool traverse_SgUntypedFunctionDeclarationList(ATerm term, SgUntypedFunctionDeclarationList** var_SgUntypedFunctionDeclarationList);
ATbool traverse_SgUntypedInitializedNameList(ATerm term, SgUntypedInitializedNameList** var_SgUntypedInitializedNameList);
ATbool traverse_SgUntypedDeclarationStatement(ATerm term, SgUntypedDeclarationStatement** var_SgUntypedDeclarationStatement);
ATbool traverse_SgUntypedAssignmentStatement(ATerm term, SgUntypedAssignmentStatement** var_SgUntypedAssignmentStatement);
ATbool traverse_SgUntypedFunctionCallStatement(ATerm term, SgUntypedFunctionCallStatement** var_SgUntypedFunctionCallStatement);
ATbool traverse_SgUntypedBlockStatement(ATerm term, SgUntypedBlockStatement** var_SgUntypedBlockStatement);
ATbool traverse_SgUntypedNamedStatement(ATerm term, SgUntypedNamedStatement** var_SgUntypedNamedStatement);
ATbool traverse_SgUntypedOtherStatement(ATerm term, SgUntypedOtherStatement** var_SgUntypedOtherStatement);
ATbool traverse_SgUntypedScope(ATerm term, SgUntypedScope** var_SgUntypedScope);
ATbool traverse_SgUntypedFunctionScope(ATerm term, SgUntypedFunctionScope** var_SgUntypedFunctionScope);
ATbool traverse_SgUntypedModuleScope(ATerm term, SgUntypedModuleScope** var_SgUntypedModuleScope);
ATbool traverse_SgUntypedGlobalScope(ATerm term, SgUntypedGlobalScope** var_SgUntypedGlobalScope);
ATbool traverse_SgUntypedArrayType(ATerm term, SgUntypedArrayType** var_SgUntypedArrayType);

#endif
