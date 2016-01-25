#ifndef TRAVERSE_SG_UNTYPED_NODES_HPP
#define TRAVERSE_SG_UNTYPED_NODES_HPP

ATbool traverse_SgLocatedNode(ATerm, SgLocatedNode**);
ATbool traverse_SgLocatedNodeSupport(ATerm, SgLocatedNodeSupport**);
ATbool traverse_SgUntypedNode(ATerm, SgUntypedNode**);
ATbool traverse_SgUntypedExpression(ATerm, SgUntypedExpression**);
ATbool traverse_SgUntypedStatement(ATerm, SgUntypedStatement**);
ATbool traverse_SgUntypedType(ATerm, SgUntypedType**);
ATbool traverse_SgUntypedAttribute(ATerm, SgUntypedAttribute**);
ATbool traverse_SgUntypedInitializedName(ATerm, SgUntypedInitializedName**);
ATbool traverse_SgUntypedFile(ATerm, SgUntypedFile**);
ATbool traverse_SgUntypedStatementList(ATerm, SgUntypedStatementList**);
ATbool traverse_SgUntypedDeclarationStatementList(ATerm, SgUntypedDeclarationStatementList**);
ATbool traverse_SgUntypedFunctionDeclarationList(ATerm, SgUntypedFunctionDeclarationList**);
ATbool traverse_SgUntypedInitializedNameList(ATerm, SgUntypedInitializedNameList**);
ATbool traverse_SgUntypedUnaryOperator(ATerm, SgUntypedUnaryOperator**);
ATbool traverse_SgUntypedBinaryOperator(ATerm, SgUntypedBinaryOperator**);
ATbool traverse_SgUntypedValueExpression(ATerm, SgUntypedValueExpression**);
ATbool traverse_SgUntypedArrayReferenceExpression(ATerm, SgUntypedArrayReferenceExpression**);
ATbool traverse_SgUntypedOtherExpression(ATerm, SgUntypedOtherExpression**);
ATbool traverse_SgUntypedFunctionCallOrArrayReferenceExpression(ATerm, SgUntypedFunctionCallOrArrayReferenceExpression**);
ATbool traverse_SgUntypedReferenceExpression(ATerm, SgUntypedReferenceExpression**);
ATbool traverse_SgUntypedDeclarationStatement(ATerm, SgUntypedDeclarationStatement**);
ATbool traverse_SgUntypedAssignmentStatement(ATerm, SgUntypedAssignmentStatement**);
ATbool traverse_SgUntypedFunctionCallStatement(ATerm, SgUntypedFunctionCallStatement**);
ATbool traverse_SgUntypedBlockStatement(ATerm, SgUntypedBlockStatement**);
ATbool traverse_SgUntypedNamedStatement(ATerm, SgUntypedNamedStatement**);
ATbool traverse_SgUntypedOtherStatement(ATerm, SgUntypedOtherStatement**);
ATbool traverse_SgUntypedScope(ATerm, SgUntypedScope**);
ATbool traverse_SgUntypedImplicitDeclaration(ATerm, SgUntypedImplicitDeclaration**);
ATbool traverse_SgUntypedVariableDeclaration(ATerm, SgUntypedVariableDeclaration**);
ATbool traverse_SgUntypedFunctionDeclaration(ATerm, SgUntypedFunctionDeclaration**);
ATbool traverse_SgUntypedModuleDeclaration(ATerm, SgUntypedModuleDeclaration**);
ATbool traverse_SgUntypedProgramHeaderDeclaration(ATerm, SgUntypedProgramHeaderDeclaration**);
ATbool traverse_SgUntypedSubroutineDeclaration(ATerm, SgUntypedSubroutineDeclaration**);
ATbool traverse_SgUntypedFunctionScope(ATerm, SgUntypedFunctionScope**);
ATbool traverse_SgUntypedModuleScope(ATerm, SgUntypedModuleScope**);
ATbool traverse_SgUntypedGlobalScope(ATerm, SgUntypedGlobalScope**);
ATbool traverse_SgUntypedArrayType(ATerm, SgUntypedArrayType**);

#endif // TRAVERSE_SG_UNTYPED_NODES_HPP
