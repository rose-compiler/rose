# this is an automatically generated file
top_srcdir=/home/dquinlan/ROSE/NEW_ROSE/projects/compass/../../
srcdir=/home/dquinlan/ROSE/NEW_ROSE/projects/compass/
all: docs

highlight: $(COMPASS_CHECKER_TESTS)
	sed -e '48,52c {\\\small \\\input{assignmentOperatorCheckSelfTest1.tex} }' $(srcdir)/assignmentOperatorCheckSelf/assignmentOperatorCheckSelfDocs.tex > assignmentOperatorCheckSelfDocs.tex
	sed -e '7c Namespace: \\texttt{AssignmentOperatorCheckSelf}' assignmentOperatorCheckSelfDocs.tex > /home/dquinlan/tmp/cvs/tmp.OviQY19218
	mv -f /home/dquinlan/tmp/cvs/tmp.OviQY19218 assignmentOperatorCheckSelfDocs.tex
	highlight -Sc -Ll -f $(srcdir)/assignmentOperatorCheckSelf/assignmentOperatorCheckSelfTest1.C -o assignmentOperatorCheckSelfTest1.tex
	sed -e '54,58c {\\\small \\\input{assignmentReturnConstThisTest1.tex} }' $(srcdir)/assignmentReturnConstThis/assignmentReturnConstThisDocs.tex > assignmentReturnConstThisDocs.tex
	sed -e '7c Namespace: \\texttt{AssignmentReturnConstThis}' assignmentReturnConstThisDocs.tex > /home/dquinlan/tmp/cvs/tmp.OviQY19218
	mv -f /home/dquinlan/tmp/cvs/tmp.OviQY19218 assignmentReturnConstThisDocs.tex
	highlight -Sc -Ll -f $(srcdir)/assignmentReturnConstThis/assignmentReturnConstThisTest1.C -o assignmentReturnConstThisTest1.tex
	sed -e '45,49c {\\\small \\\input{booleanIsHasTest1.tex} }' $(srcdir)/booleanIsHas/booleanIsHasDocs.tex > booleanIsHasDocs.tex
	sed -e '7c Namespace: \\texttt{BooleanIsHas}' booleanIsHasDocs.tex > /home/dquinlan/tmp/cvs/tmp.OviQY19218
	mv -f /home/dquinlan/tmp/cvs/tmp.OviQY19218 booleanIsHasDocs.tex
	highlight -Sc -Ll -f $(srcdir)/booleanIsHas/booleanIsHasTest1.C -o booleanIsHasTest1.tex
	sed -e '34,38c {\\\small \\\input{commaOperatorTest1.tex} }' $(srcdir)/commaOperator/commaOperatorDocs.tex > commaOperatorDocs.tex
	sed -e '7c Namespace: \\texttt{CommaOperator}' commaOperatorDocs.tex > /home/dquinlan/tmp/cvs/tmp.OviQY19218
	mv -f /home/dquinlan/tmp/cvs/tmp.OviQY19218 commaOperatorDocs.tex
	highlight -Sc -Ll -f $(srcdir)/commaOperator/commaOperatorTest1.C -o commaOperatorTest1.tex
	sed -e '58,62c {\\\small \\\input{computationalFunctionsTest1.tex} }' $(srcdir)/computationalFunctions/computationalFunctionsDocs.tex > computationalFunctionsDocs.tex
	sed -e '7c Namespace: \\texttt{ComputationalFunctions}' computationalFunctionsDocs.tex > /home/dquinlan/tmp/cvs/tmp.OviQY19218
	mv -f /home/dquinlan/tmp/cvs/tmp.OviQY19218 computationalFunctionsDocs.tex
	highlight -Sc -Ll -f $(srcdir)/computationalFunctions/computationalFunctionsTest1.C -o computationalFunctionsTest1.tex
	sed -e '32,36c {\\\small \\\input{constCastTest1.tex} }' $(srcdir)/constCast/constCastDocs.tex > constCastDocs.tex
	sed -e '7c Namespace: \\texttt{ConstCast}' constCastDocs.tex > /home/dquinlan/tmp/cvs/tmp.OviQY19218
	mv -f /home/dquinlan/tmp/cvs/tmp.OviQY19218 constCastDocs.tex
	highlight -Sc -Ll -f $(srcdir)/constCast/constCastTest1.C -o constCastTest1.tex
	sed -e '42,46c {\\\small \\\input{constructorDestructorCallsVirtualFunctionTest1.tex} }' $(srcdir)/constructorDestructorCallsVirtualFunction/constructorDestructorCallsVirtualFunctionDocs.tex > constructorDestructorCallsVirtualFunctionDocs.tex
	sed -e '7c Namespace: \\texttt{ConstructorDestructorCallsVirtualFunction}' constructorDestructorCallsVirtualFunctionDocs.tex > /home/dquinlan/tmp/cvs/tmp.OviQY19218
	mv -f /home/dquinlan/tmp/cvs/tmp.OviQY19218 constructorDestructorCallsVirtualFunctionDocs.tex
	highlight -Sc -Ll -f $(srcdir)/constructorDestructorCallsVirtualFunction/constructorDestructorCallsVirtualFunctionTest1.C -o constructorDestructorCallsVirtualFunctionTest1.tex
	sed -e '54,58c {\\\small \\\input{controlVariableTestAgainstFunctionTest1.tex} }' $(srcdir)/controlVariableTestAgainstFunction/controlVariableTestAgainstFunctionDocs.tex > controlVariableTestAgainstFunctionDocs.tex
	sed -e '7c Namespace: \\texttt{ControlVariableTestAgainstFunction}' controlVariableTestAgainstFunctionDocs.tex > /home/dquinlan/tmp/cvs/tmp.OviQY19218
	mv -f /home/dquinlan/tmp/cvs/tmp.OviQY19218 controlVariableTestAgainstFunctionDocs.tex
	highlight -Sc -Ll -f $(srcdir)/controlVariableTestAgainstFunction/controlVariableTestAgainstFunctionTest1.C -o controlVariableTestAgainstFunctionTest1.tex
	sed -e '48,52c {\\\small \\\input{copyConstructorConstArgTest1.tex} }' $(srcdir)/copyConstructorConstArg/copyConstructorConstArgDocs.tex > copyConstructorConstArgDocs.tex
	sed -e '7c Namespace: \\texttt{CopyConstructorConstArg}' copyConstructorConstArgDocs.tex > /home/dquinlan/tmp/cvs/tmp.OviQY19218
	mv -f /home/dquinlan/tmp/cvs/tmp.OviQY19218 copyConstructorConstArgDocs.tex
	highlight -Sc -Ll -f $(srcdir)/copyConstructorConstArg/copyConstructorConstArgTest1.C -o copyConstructorConstArgTest1.tex
	sed -e '55,59c {\\\small \\\input{cppCallsSetjmpLongjmpTest1.tex} }' $(srcdir)/cppCallsSetjmpLongjmp/cppCallsSetjmpLongjmpDocs.tex > cppCallsSetjmpLongjmpDocs.tex
	sed -e '7c Namespace: \\texttt{CppCallsSetjmpLongjmp}' cppCallsSetjmpLongjmpDocs.tex > /home/dquinlan/tmp/cvs/tmp.OviQY19218
	mv -f /home/dquinlan/tmp/cvs/tmp.OviQY19218 cppCallsSetjmpLongjmpDocs.tex
	highlight -Sc -Ll -f $(srcdir)/cppCallsSetjmpLongjmp/cppCallsSetjmpLongjmpTest1.C -o cppCallsSetjmpLongjmpTest1.tex
	sed -e '49,53c {\\\small \\\input{cyclomaticComplexityTest1.tex} }' $(srcdir)/cyclomaticComplexity/cyclomaticComplexityDocs.tex > cyclomaticComplexityDocs.tex
	sed -e '7c Namespace: \\texttt{CyclomaticComplexity}' cyclomaticComplexityDocs.tex > /home/dquinlan/tmp/cvs/tmp.OviQY19218
	mv -f /home/dquinlan/tmp/cvs/tmp.OviQY19218 cyclomaticComplexityDocs.tex
	highlight -Sc -Ll -f $(srcdir)/cyclomaticComplexity/cyclomaticComplexityTest1.C -o cyclomaticComplexityTest1.tex
	sed -e '51,55c {\\\small \\\input{dataMemberAccessTest1.tex} }' $(srcdir)/dataMemberAccess/dataMemberAccessDocs.tex > dataMemberAccessDocs.tex
	sed -e '7c Namespace: \\texttt{DataMemberAccess}' dataMemberAccessDocs.tex > /home/dquinlan/tmp/cvs/tmp.OviQY19218
	mv -f /home/dquinlan/tmp/cvs/tmp.OviQY19218 dataMemberAccessDocs.tex
	highlight -Sc -Ll -f $(srcdir)/dataMemberAccess/dataMemberAccessTest1.C -o dataMemberAccessTest1.tex
	sed -e '38,42c {\\\small \\\input{deepNestingTest1.tex} }' $(srcdir)/deepNesting/deepNestingDocs.tex > deepNestingDocs.tex
	sed -e '7c Namespace: \\texttt{DeepNesting}' deepNestingDocs.tex > /home/dquinlan/tmp/cvs/tmp.OviQY19218
	mv -f /home/dquinlan/tmp/cvs/tmp.OviQY19218 deepNestingDocs.tex
	highlight -Sc -Ll -f $(srcdir)/deepNesting/deepNestingTest1.C -o deepNestingTest1.tex
	sed -e '48,52c {\\\small \\\input{defaultCaseTest1.tex} }' $(srcdir)/defaultCase/defaultCaseDocs.tex > defaultCaseDocs.tex
	sed -e '7c Namespace: \\texttt{DefaultCase}' defaultCaseDocs.tex > /home/dquinlan/tmp/cvs/tmp.OviQY19218
	mv -f /home/dquinlan/tmp/cvs/tmp.OviQY19218 defaultCaseDocs.tex
	highlight -Sc -Ll -f $(srcdir)/defaultCase/defaultCaseTest1.C -o defaultCaseTest1.tex
	sed -e '38,42c {\\\small \\\input{defaultConstructorTest1.tex} }' $(srcdir)/defaultConstructor/defaultConstructorDocs.tex > defaultConstructorDocs.tex
	sed -e '7c Namespace: \\texttt{DefaultConstructor}' defaultConstructorDocs.tex > /home/dquinlan/tmp/cvs/tmp.OviQY19218
	mv -f /home/dquinlan/tmp/cvs/tmp.OviQY19218 defaultConstructorDocs.tex
	highlight -Sc -Ll -f $(srcdir)/defaultConstructor/defaultConstructorTest1.C -o defaultConstructorTest1.tex
	sed -e '37,41c {\\\small \\\input{discardAssignmentTest1.tex} }' $(srcdir)/discardAssignment/discardAssignmentDocs.tex > discardAssignmentDocs.tex
	sed -e '7c Namespace: \\texttt{DiscardAssignment}' discardAssignmentDocs.tex > /home/dquinlan/tmp/cvs/tmp.OviQY19218
	mv -f /home/dquinlan/tmp/cvs/tmp.OviQY19218 discardAssignmentDocs.tex
	highlight -Sc -Ll -f $(srcdir)/discardAssignment/discardAssignmentTest1.C -o discardAssignmentTest1.tex
	sed -e '39,43c {\\\small \\\input{duffsDeviceTest1.tex} }' $(srcdir)/duffsDevice/duffsDeviceDocs.tex > duffsDeviceDocs.tex
	sed -e '7c Namespace: \\texttt{DuffsDevice}' duffsDeviceDocs.tex > /home/dquinlan/tmp/cvs/tmp.OviQY19218
	mv -f /home/dquinlan/tmp/cvs/tmp.OviQY19218 duffsDeviceDocs.tex
	highlight -Sc -Ll -f $(srcdir)/duffsDevice/duffsDeviceTest1.C -o duffsDeviceTest1.tex
	sed -e '39,43c {\\\small \\\input{enumDeclarationNamespaceClassScopeTest1.tex} }' $(srcdir)/enumDeclarationNamespaceClassScope/enumDeclarationNamespaceClassScopeDocs.tex > enumDeclarationNamespaceClassScopeDocs.tex
	sed -e '7c Namespace: \\texttt{EnumDeclarationNamespaceClassScope}' enumDeclarationNamespaceClassScopeDocs.tex > /home/dquinlan/tmp/cvs/tmp.OviQY19218
	mv -f /home/dquinlan/tmp/cvs/tmp.OviQY19218 enumDeclarationNamespaceClassScopeDocs.tex
	highlight -Sc -Ll -f $(srcdir)/enumDeclarationNamespaceClassScope/enumDeclarationNamespaceClassScopeTest1.C -o enumDeclarationNamespaceClassScopeTest1.tex
	sed -e '33,37c {\\\small \\\input{explicitCopyTest1.tex} }' $(srcdir)/explicitCopy/explicitCopyDocs.tex > explicitCopyDocs.tex
	sed -e '7c Namespace: \\texttt{ExplicitCopy}' explicitCopyDocs.tex > /home/dquinlan/tmp/cvs/tmp.OviQY19218
	mv -f /home/dquinlan/tmp/cvs/tmp.OviQY19218 explicitCopyDocs.tex
	highlight -Sc -Ll -f $(srcdir)/explicitCopy/explicitCopyTest1.C -o explicitCopyTest1.tex
	sed -e '56,60c {\\\small \\\input{explicitTestForNonBooleanValueTest1.tex} }' $(srcdir)/explicitTestForNonBooleanValue/explicitTestForNonBooleanValueDocs.tex > explicitTestForNonBooleanValueDocs.tex
	sed -e '7c Namespace: \\texttt{ExplicitTestForNonBooleanValue}' explicitTestForNonBooleanValueDocs.tex > /home/dquinlan/tmp/cvs/tmp.OviQY19218
	mv -f /home/dquinlan/tmp/cvs/tmp.OviQY19218 explicitTestForNonBooleanValueDocs.tex
	highlight -Sc -Ll -f $(srcdir)/explicitTestForNonBooleanValue/explicitTestForNonBooleanValueTest1.C -o explicitTestForNonBooleanValueTest1.tex
	sed -e '40,44c {\\\small \\\input{floatingPointExactComparisonTest1.tex} }' $(srcdir)/floatingPointExactComparison/floatingPointExactComparisonDocs.tex > floatingPointExactComparisonDocs.tex
	sed -e '7c Namespace: \\texttt{FloatingPointExactComparison}' floatingPointExactComparisonDocs.tex > /home/dquinlan/tmp/cvs/tmp.OviQY19218
	mv -f /home/dquinlan/tmp/cvs/tmp.OviQY19218 floatingPointExactComparisonDocs.tex
	highlight -Sc -Ll -f $(srcdir)/floatingPointExactComparison/floatingPointExactComparisonTest1.C -o floatingPointExactComparisonTest1.tex
	sed -e '37,41c {\\\small \\\input{forLoopConstructionControlStmtTest1.tex} }' $(srcdir)/forLoopConstructionControlStmt/forLoopConstructionControlStmtDocs.tex > forLoopConstructionControlStmtDocs.tex
	sed -e '7c Namespace: \\texttt{ForLoopConstructionControlStmt}' forLoopConstructionControlStmtDocs.tex > /home/dquinlan/tmp/cvs/tmp.OviQY19218
	mv -f /home/dquinlan/tmp/cvs/tmp.OviQY19218 forLoopConstructionControlStmtDocs.tex
	highlight -Sc -Ll -f $(srcdir)/forLoopConstructionControlStmt/forLoopConstructionControlStmtTest1.C -o forLoopConstructionControlStmtTest1.tex
	sed -e '41,45c {\\\small \\\input{forLoopCppIndexVariableDeclarationTest1.tex} }' $(srcdir)/forLoopCppIndexVariableDeclaration/forLoopCppIndexVariableDeclarationDocs.tex > forLoopCppIndexVariableDeclarationDocs.tex
	sed -e '7c Namespace: \\texttt{ForLoopCppIndexVariableDeclaration}' forLoopCppIndexVariableDeclarationDocs.tex > /home/dquinlan/tmp/cvs/tmp.OviQY19218
	mv -f /home/dquinlan/tmp/cvs/tmp.OviQY19218 forLoopCppIndexVariableDeclarationDocs.tex
	highlight -Sc -Ll -f $(srcdir)/forLoopCppIndexVariableDeclaration/forLoopCppIndexVariableDeclarationTest1.C -o forLoopCppIndexVariableDeclarationTest1.tex
	sed -e '37,41c {\\\small \\\input{friendDeclarationModifierTest1.tex} }' $(srcdir)/friendDeclarationModifier/friendDeclarationModifierDocs.tex > friendDeclarationModifierDocs.tex
	sed -e '7c Namespace: \\texttt{FriendDeclarationModifier}' friendDeclarationModifierDocs.tex > /home/dquinlan/tmp/cvs/tmp.OviQY19218
	mv -f /home/dquinlan/tmp/cvs/tmp.OviQY19218 friendDeclarationModifierDocs.tex
	highlight -Sc -Ll -f $(srcdir)/friendDeclarationModifier/friendDeclarationModifierTest1.C -o friendDeclarationModifierTest1.tex
	sed -e '39,43c {\\\small \\\input{functionDefinitionPrototypeTest1.tex} }' $(srcdir)/functionDefinitionPrototype/functionDefinitionPrototypeDocs.tex > functionDefinitionPrototypeDocs.tex
	sed -e '7c Namespace: \\texttt{FunctionDefinitionPrototype}' functionDefinitionPrototypeDocs.tex > /home/dquinlan/tmp/cvs/tmp.OviQY19218
	mv -f /home/dquinlan/tmp/cvs/tmp.OviQY19218 functionDefinitionPrototypeDocs.tex
	highlight -Sc -Ll -f $(srcdir)/functionDefinitionPrototype/functionDefinitionPrototypeTest1.C -o functionDefinitionPrototypeTest1.tex
	sed -e '41,45c {\\\small \\\input{functionDocumentationTest1.tex} }' $(srcdir)/functionDocumentation/functionDocumentationDocs.tex > functionDocumentationDocs.tex
	sed -e '7c Namespace: \\texttt{FunctionDocumentation}' functionDocumentationDocs.tex > /home/dquinlan/tmp/cvs/tmp.OviQY19218
	mv -f /home/dquinlan/tmp/cvs/tmp.OviQY19218 functionDocumentationDocs.tex
	highlight -Sc -Ll -f $(srcdir)/functionDocumentation/functionDocumentationTest1.C -o functionDocumentationTest1.tex
	sed -e '41,45c {\\\small \\\input{inductionVariableUpdateTest1.tex} }' $(srcdir)/inductionVariableUpdate/inductionVariableUpdateDocs.tex > inductionVariableUpdateDocs.tex
	sed -e '7c Namespace: \\texttt{InductionVariableUpdate}' inductionVariableUpdateDocs.tex > /home/dquinlan/tmp/cvs/tmp.OviQY19218
	mv -f /home/dquinlan/tmp/cvs/tmp.OviQY19218 inductionVariableUpdateDocs.tex
	highlight -Sc -Ll -f $(srcdir)/inductionVariableUpdate/inductionVariableUpdateTest1.C -o inductionVariableUpdateTest1.tex
	sed -e '45,49c {\\\small \\\input{internalDataSharingTest1.tex} }' $(srcdir)/internalDataSharing/internalDataSharingDocs.tex > internalDataSharingDocs.tex
	sed -e '7c Namespace: \\texttt{InternalDataSharing}' internalDataSharingDocs.tex > /home/dquinlan/tmp/cvs/tmp.OviQY19218
	mv -f /home/dquinlan/tmp/cvs/tmp.OviQY19218 internalDataSharingDocs.tex
	highlight -Sc -Ll -f $(srcdir)/internalDataSharing/internalDataSharingTest1.C -o internalDataSharingTest1.tex
	sed -e '47,51c {\\\small \\\input{localizedVariablesTest1.tex} }' $(srcdir)/localizedVariables/localizedVariablesDocs.tex > localizedVariablesDocs.tex
	sed -e '7c Namespace: \\texttt{LocalizedVariables}' localizedVariablesDocs.tex > /home/dquinlan/tmp/cvs/tmp.OviQY19218
	mv -f /home/dquinlan/tmp/cvs/tmp.OviQY19218 localizedVariablesDocs.tex
	highlight -Sc -Ll -f $(srcdir)/localizedVariables/localizedVariablesTest1.C -o localizedVariablesTest1.tex
	sed -e '58,62c {\\\small \\\input{locPerFunctionTest1.tex} }' $(srcdir)/locPerFunction/locPerFunctionDocs.tex > locPerFunctionDocs.tex
	sed -e '7c Namespace: \\texttt{LocPerFunction}' locPerFunctionDocs.tex > /home/dquinlan/tmp/cvs/tmp.OviQY19218
	mv -f /home/dquinlan/tmp/cvs/tmp.OviQY19218 locPerFunctionDocs.tex
	highlight -Sc -Ll -f $(srcdir)/locPerFunction/locPerFunctionTest1.C -o locPerFunctionTest1.tex
	sed -e '40,44c {\\\small \\\input{mallocReturnValueUsedInIfStmtTest1.tex} }' $(srcdir)/mallocReturnValueUsedInIfStmt/mallocReturnValueUsedInIfStmtDocs.tex > mallocReturnValueUsedInIfStmtDocs.tex
	sed -e '7c Namespace: \\texttt{MallocReturnValueUsedInIfStmt}' mallocReturnValueUsedInIfStmtDocs.tex > /home/dquinlan/tmp/cvs/tmp.OviQY19218
	mv -f /home/dquinlan/tmp/cvs/tmp.OviQY19218 mallocReturnValueUsedInIfStmtDocs.tex
	highlight -Sc -Ll -f $(srcdir)/mallocReturnValueUsedInIfStmt/mallocReturnValueUsedInIfStmtTest1.C -o mallocReturnValueUsedInIfStmtTest1.tex
	sed -e '38,42c {\\\small \\\input{multiplePublicInheritanceTest1.tex} }' $(srcdir)/multiplePublicInheritance/multiplePublicInheritanceDocs.tex > multiplePublicInheritanceDocs.tex
	sed -e '7c Namespace: \\texttt{MultiplePublicInheritance}' multiplePublicInheritanceDocs.tex > /home/dquinlan/tmp/cvs/tmp.OviQY19218
	mv -f /home/dquinlan/tmp/cvs/tmp.OviQY19218 multiplePublicInheritanceDocs.tex
	highlight -Sc -Ll -f $(srcdir)/multiplePublicInheritance/multiplePublicInheritanceTest1.C -o multiplePublicInheritanceTest1.tex
	sed -e '36,40c {\\\small \\\input{nameAllParametersTest1.tex} }' $(srcdir)/nameAllParameters/nameAllParametersDocs.tex > nameAllParametersDocs.tex
	sed -e '7c Namespace: \\texttt{NameAllParameters}' nameAllParametersDocs.tex > /home/dquinlan/tmp/cvs/tmp.OviQY19218
	mv -f /home/dquinlan/tmp/cvs/tmp.OviQY19218 nameAllParametersDocs.tex
	highlight -Sc -Ll -f $(srcdir)/nameAllParameters/nameAllParametersTest1.C -o nameAllParametersTest1.tex
	sed -e '48,52c {\\\small \\\input{noExitInMpiCodeTest1.tex} }' $(srcdir)/noExitInMpiCode/noExitInMpiCodeDocs.tex > noExitInMpiCodeDocs.tex
	sed -e '7c Namespace: \\texttt{NoExitInMpiCode}' noExitInMpiCodeDocs.tex > /home/dquinlan/tmp/cvs/tmp.OviQY19218
	mv -f /home/dquinlan/tmp/cvs/tmp.OviQY19218 noExitInMpiCodeDocs.tex
	highlight -Sc -Ll -f $(srcdir)/noExitInMpiCode/noExitInMpiCodeTest1.C -o noExitInMpiCodeTest1.tex
	sed -e '30,34c {\\\small \\\input{noGotoTest1.tex} }' $(srcdir)/noGoto/noGotoDocs.tex > noGotoDocs.tex
	sed -e '7c Namespace: \\texttt{NoGoto}' noGotoDocs.tex > /home/dquinlan/tmp/cvs/tmp.OviQY19218
	mv -f /home/dquinlan/tmp/cvs/tmp.OviQY19218 noGotoDocs.tex
	highlight -Sc -Ll -f $(srcdir)/noGoto/noGotoTest1.C -o noGotoTest1.tex
	sed -e '40,44c {\\\small \\\input{nonmemberFunctionInterfaceNamespaceTest1.tex} }' $(srcdir)/nonmemberFunctionInterfaceNamespace/nonmemberFunctionInterfaceNamespaceDocs.tex > nonmemberFunctionInterfaceNamespaceDocs.tex
	sed -e '7c Namespace: \\texttt{NonmemberFunctionInterfaceNamespace}' nonmemberFunctionInterfaceNamespaceDocs.tex > /home/dquinlan/tmp/cvs/tmp.OviQY19218
	mv -f /home/dquinlan/tmp/cvs/tmp.OviQY19218 nonmemberFunctionInterfaceNamespaceDocs.tex
	highlight -Sc -Ll -f $(srcdir)/nonmemberFunctionInterfaceNamespace/nonmemberFunctionInterfaceNamespaceTest1.C -o nonmemberFunctionInterfaceNamespaceTest1.tex
	sed -e '44,48c {\\\small \\\input{nonVirtualRedefinitionTest1.tex} }' $(srcdir)/nonVirtualRedefinition/nonVirtualRedefinitionDocs.tex > nonVirtualRedefinitionDocs.tex
	sed -e '7c Namespace: \\texttt{NonVirtualRedefinition}' nonVirtualRedefinitionDocs.tex > /home/dquinlan/tmp/cvs/tmp.OviQY19218
	mv -f /home/dquinlan/tmp/cvs/tmp.OviQY19218 nonVirtualRedefinitionDocs.tex
	highlight -Sc -Ll -f $(srcdir)/nonVirtualRedefinition/nonVirtualRedefinitionTest1.C -o nonVirtualRedefinitionTest1.tex
	sed -e '55,59c {\\\small \\\input{operatorOverloadingTest1.tex} }' $(srcdir)/operatorOverloading/operatorOverloadingDocs.tex > operatorOverloadingDocs.tex
	sed -e '7c Namespace: \\texttt{OperatorOverloading}' operatorOverloadingDocs.tex > /home/dquinlan/tmp/cvs/tmp.OviQY19218
	mv -f /home/dquinlan/tmp/cvs/tmp.OviQY19218 operatorOverloadingDocs.tex
	highlight -Sc -Ll -f $(srcdir)/operatorOverloading/operatorOverloadingTest1.C -o operatorOverloadingTest1.tex
	sed -e '35,39c {\\\small \\\input{otherArgumentTest1.tex} }' $(srcdir)/otherArgument/otherArgumentDocs.tex > otherArgumentDocs.tex
	sed -e '7c Namespace: \\texttt{OtherArgument}' otherArgumentDocs.tex > /home/dquinlan/tmp/cvs/tmp.OviQY19218
	mv -f /home/dquinlan/tmp/cvs/tmp.OviQY19218 otherArgumentDocs.tex
	highlight -Sc -Ll -f $(srcdir)/otherArgument/otherArgumentTest1.C -o otherArgumentTest1.tex
	sed -e '42,46c {\\\small \\\input{placeConstantOnTheLhsTest1.tex} }' $(srcdir)/placeConstantOnTheLhs/placeConstantOnTheLhsDocs.tex > placeConstantOnTheLhsDocs.tex
	sed -e '7c Namespace: \\texttt{PlaceConstantOnTheLhs}' placeConstantOnTheLhsDocs.tex > /home/dquinlan/tmp/cvs/tmp.OviQY19218
	mv -f /home/dquinlan/tmp/cvs/tmp.OviQY19218 placeConstantOnTheLhsDocs.tex
	highlight -Sc -Ll -f $(srcdir)/placeConstantOnTheLhs/placeConstantOnTheLhsTest1.C -o placeConstantOnTheLhsTest1.tex
	sed -e '39,43c {\\\small \\\input{protectVirtualMethodsTest1.tex} }' $(srcdir)/protectVirtualMethods/protectVirtualMethodsDocs.tex > protectVirtualMethodsDocs.tex
	sed -e '7c Namespace: \\texttt{ProtectVirtualMethods}' protectVirtualMethodsDocs.tex > /home/dquinlan/tmp/cvs/tmp.OviQY19218
	mv -f /home/dquinlan/tmp/cvs/tmp.OviQY19218 protectVirtualMethodsDocs.tex
	highlight -Sc -Ll -f $(srcdir)/protectVirtualMethods/protectVirtualMethodsTest1.C -o protectVirtualMethodsTest1.tex
	sed -e '78,82c {\\\small \\\input{pushBackTest1.tex} }' $(srcdir)/pushBack/pushBackDocs.tex > pushBackDocs.tex
	sed -e '7c Namespace: \\texttt{PushBack}' pushBackDocs.tex > /home/dquinlan/tmp/cvs/tmp.OviQY19218
	mv -f /home/dquinlan/tmp/cvs/tmp.OviQY19218 pushBackDocs.tex
	highlight -Sc -Ll -f $(srcdir)/pushBack/pushBackTest1.C -o pushBackTest1.tex
	sed -e '38,42c {\\\small \\\input{singleParameterConstructorExplicitModifierTest1.tex} }' $(srcdir)/singleParameterConstructorExplicitModifier/singleParameterConstructorExplicitModifierDocs.tex > singleParameterConstructorExplicitModifierDocs.tex
	sed -e '7c Namespace: \\texttt{SingleParameterConstructorExplicitModifier}' singleParameterConstructorExplicitModifierDocs.tex > /home/dquinlan/tmp/cvs/tmp.OviQY19218
	mv -f /home/dquinlan/tmp/cvs/tmp.OviQY19218 singleParameterConstructorExplicitModifierDocs.tex
	highlight -Sc -Ll -f $(srcdir)/singleParameterConstructorExplicitModifier/singleParameterConstructorExplicitModifierTest1.C -o singleParameterConstructorExplicitModifierTest1.tex
	sed -e '42,46c {\\\small \\\input{subExpressionEvaluationOrderTest1.tex} }' $(srcdir)/subExpressionEvaluationOrder/subExpressionEvaluationOrderDocs.tex > subExpressionEvaluationOrderDocs.tex
	sed -e '7c Namespace: \\texttt{SubExpressionEvaluationOrder}' subExpressionEvaluationOrderDocs.tex > /home/dquinlan/tmp/cvs/tmp.OviQY19218
	mv -f /home/dquinlan/tmp/cvs/tmp.OviQY19218 subExpressionEvaluationOrderDocs.tex
	highlight -Sc -Ll -f $(srcdir)/subExpressionEvaluationOrder/subExpressionEvaluationOrderTest1.C -o subExpressionEvaluationOrderTest1.tex
	sed -e '40,44c {\\\small \\\input{ternaryOperatorTest1.tex} }' $(srcdir)/ternaryOperator/ternaryOperatorDocs.tex > ternaryOperatorDocs.tex
	sed -e '7c Namespace: \\texttt{TernaryOperator}' ternaryOperatorDocs.tex > /home/dquinlan/tmp/cvs/tmp.OviQY19218
	mv -f /home/dquinlan/tmp/cvs/tmp.OviQY19218 ternaryOperatorDocs.tex
	highlight -Sc -Ll -f $(srcdir)/ternaryOperator/ternaryOperatorTest1.C -o ternaryOperatorTest1.tex
	sed -e '34,38c {\\\small \\\input{unaryMinusTest1.tex} }' $(srcdir)/unaryMinus/unaryMinusDocs.tex > unaryMinusDocs.tex
	sed -e '7c Namespace: \\texttt{UnaryMinus}' unaryMinusDocs.tex > /home/dquinlan/tmp/cvs/tmp.OviQY19218
	mv -f /home/dquinlan/tmp/cvs/tmp.OviQY19218 unaryMinusDocs.tex
	highlight -Sc -Ll -f $(srcdir)/unaryMinus/unaryMinusTest1.C -o unaryMinusTest1.tex
	sed -e '50,54c {\\\small \\\input{uninitializedDefinitionTest1.tex} }' $(srcdir)/uninitializedDefinition/uninitializedDefinitionDocs.tex > uninitializedDefinitionDocs.tex
	sed -e '7c Namespace: \\texttt{UninitializedDefinition}' uninitializedDefinitionDocs.tex > /home/dquinlan/tmp/cvs/tmp.OviQY19218
	mv -f /home/dquinlan/tmp/cvs/tmp.OviQY19218 uninitializedDefinitionDocs.tex
	highlight -Sc -Ll -f $(srcdir)/uninitializedDefinition/uninitializedDefinitionTest1.C -o uninitializedDefinitionTest1.tex
	sed -e '34,38c {\\\small \\\input{voidStarTest1.tex} }' $(srcdir)/voidStar/voidStarDocs.tex > voidStarDocs.tex
	sed -e '7c Namespace: \\texttt{VoidStar}' voidStarDocs.tex > /home/dquinlan/tmp/cvs/tmp.OviQY19218
	mv -f /home/dquinlan/tmp/cvs/tmp.OviQY19218 voidStarDocs.tex
	highlight -Sc -Ll -f $(srcdir)/voidStar/voidStarTest1.C -o voidStarTest1.tex

docs: compass.tex highlight
	test -f fixme.sty || ln -s $(top_srcdir)/docs/Rose/fixme.sty fixme.sty
	test -f compassCheckerDocs.tex || ln -s $(srcdir)/compassCheckerDocs.tex compassCheckerDocs.tex
	test -f acknowledgments.tex || ln -s $(srcdir)/acknowledgments.tex acknowledgments.tex
	test -f introduction.tex || ln -s $(srcdir)/introduction.tex introduction.tex
	test -f usingCompass.tex || ln -s $(srcdir)/usingCompass.tex usingCompass.tex
	test -f appendix.tex || ln -s $(srcdir)/appendix.tex appendix.tex
	test -f compassDocs.tex || ln -s $(srcdir)/compassSupport/compassDocs.tex compassDocs.tex
	test -f emacs_screenshot.jpg || ln -s $(srcdir)/emacs_screenshot.jpg emacs_screenshot.jpg
	pdflatex compass.tex
	pdflatex compass.tex

clean:
	find . -maxdepth 1 -type l -exec rm {} \;
	rm -f compass.aux compass.toc compass.log compass.pdf compass.out
	rm -f assignmentOperatorCheckSelfTest1.tex
	rm -f assignmentOperatorCheckSelfDocs.tex
	rm -f assignmentReturnConstThisTest1.tex
	rm -f assignmentReturnConstThisDocs.tex
	rm -f booleanIsHasTest1.tex
	rm -f booleanIsHasDocs.tex
	rm -f commaOperatorTest1.tex
	rm -f commaOperatorDocs.tex
	rm -f computationalFunctionsTest1.tex
	rm -f computationalFunctionsDocs.tex
	rm -f constCastTest1.tex
	rm -f constCastDocs.tex
	rm -f constructorDestructorCallsVirtualFunctionTest1.tex
	rm -f constructorDestructorCallsVirtualFunctionDocs.tex
	rm -f controlVariableTestAgainstFunctionTest1.tex
	rm -f controlVariableTestAgainstFunctionDocs.tex
	rm -f copyConstructorConstArgTest1.tex
	rm -f copyConstructorConstArgDocs.tex
	rm -f cppCallsSetjmpLongjmpTest1.tex
	rm -f cppCallsSetjmpLongjmpDocs.tex
	rm -f cyclomaticComplexityTest1.tex
	rm -f cyclomaticComplexityDocs.tex
	rm -f dataMemberAccessTest1.tex
	rm -f dataMemberAccessDocs.tex
	rm -f deepNestingTest1.tex
	rm -f deepNestingDocs.tex
	rm -f defaultCaseTest1.tex
	rm -f defaultCaseDocs.tex
	rm -f defaultConstructorTest1.tex
	rm -f defaultConstructorDocs.tex
	rm -f discardAssignmentTest1.tex
	rm -f discardAssignmentDocs.tex
	rm -f duffsDeviceTest1.tex
	rm -f duffsDeviceDocs.tex
	rm -f enumDeclarationNamespaceClassScopeTest1.tex
	rm -f enumDeclarationNamespaceClassScopeDocs.tex
	rm -f explicitCopyTest1.tex
	rm -f explicitCopyDocs.tex
	rm -f explicitTestForNonBooleanValueTest1.tex
	rm -f explicitTestForNonBooleanValueDocs.tex
	rm -f floatingPointExactComparisonTest1.tex
	rm -f floatingPointExactComparisonDocs.tex
	rm -f forLoopConstructionControlStmtTest1.tex
	rm -f forLoopConstructionControlStmtDocs.tex
	rm -f forLoopCppIndexVariableDeclarationTest1.tex
	rm -f forLoopCppIndexVariableDeclarationDocs.tex
	rm -f friendDeclarationModifierTest1.tex
	rm -f friendDeclarationModifierDocs.tex
	rm -f functionDefinitionPrototypeTest1.tex
	rm -f functionDefinitionPrototypeDocs.tex
	rm -f functionDocumentationTest1.tex
	rm -f functionDocumentationDocs.tex
	rm -f inductionVariableUpdateTest1.tex
	rm -f inductionVariableUpdateDocs.tex
	rm -f internalDataSharingTest1.tex
	rm -f internalDataSharingDocs.tex
	rm -f localizedVariablesTest1.tex
	rm -f localizedVariablesDocs.tex
	rm -f locPerFunctionTest1.tex
	rm -f locPerFunctionDocs.tex
	rm -f mallocReturnValueUsedInIfStmtTest1.tex
	rm -f mallocReturnValueUsedInIfStmtDocs.tex
	rm -f multiplePublicInheritanceTest1.tex
	rm -f multiplePublicInheritanceDocs.tex
	rm -f nameAllParametersTest1.tex
	rm -f nameAllParametersDocs.tex
	rm -f noExitInMpiCodeTest1.tex
	rm -f noExitInMpiCodeDocs.tex
	rm -f noGotoTest1.tex
	rm -f noGotoDocs.tex
	rm -f nonmemberFunctionInterfaceNamespaceTest1.tex
	rm -f nonmemberFunctionInterfaceNamespaceDocs.tex
	rm -f nonVirtualRedefinitionTest1.tex
	rm -f nonVirtualRedefinitionDocs.tex
	rm -f operatorOverloadingTest1.tex
	rm -f operatorOverloadingDocs.tex
	rm -f otherArgumentTest1.tex
	rm -f otherArgumentDocs.tex
	rm -f placeConstantOnTheLhsTest1.tex
	rm -f placeConstantOnTheLhsDocs.tex
	rm -f protectVirtualMethodsTest1.tex
	rm -f protectVirtualMethodsDocs.tex
	rm -f pushBackTest1.tex
	rm -f pushBackDocs.tex
	rm -f singleParameterConstructorExplicitModifierTest1.tex
	rm -f singleParameterConstructorExplicitModifierDocs.tex
	rm -f subExpressionEvaluationOrderTest1.tex
	rm -f subExpressionEvaluationOrderDocs.tex
	rm -f ternaryOperatorTest1.tex
	rm -f ternaryOperatorDocs.tex
	rm -f unaryMinusTest1.tex
	rm -f unaryMinusDocs.tex
	rm -f uninitializedDefinitionTest1.tex
	rm -f uninitializedDefinitionDocs.tex
	rm -f voidStarTest1.tex
	rm -f voidStarDocs.tex
