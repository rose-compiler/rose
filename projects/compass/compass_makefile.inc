# This is a generated file

COMPASS_CHECKER_DIRECTORIES = \
	allocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstraction \
	assignmentOperatorCheckSelf \
	assignmentReturnConstThis \
	asynchronousSignalHandler \
	avoidUsingTheSameHandlerForMultipleSignals \
	booleanIsHas \
	bufferOverflowFunctions \
	byteByByteStructureComparison \
	charStarForString \
	commaOperator \
	computationalFunctions \
	constCast \
	constructorDestructorCallsVirtualFunction \
	constStringLiterals \
	controlVariableTestAgainstFunction \
	copyConstructorConstArg \
	cppCallsSetjmpLongjmp \
	cyclomaticComplexity \
	dataMemberAccess \
	deepNesting \
	defaultCase \
	defaultConstructor \
	discardAssignment \
	doNotCallPutenvWithAutoVar \
	doNotDeleteThis \
	doNotUseCstyleCasts \
	duffsDevice \
	dynamicCast \
	emptyInsteadOfSize \
	enumDeclarationNamespaceClassScope \
	explicitCharSign \
	explicitCopy \
	explicitTestForNonBooleanValue \
	floatForLoopCounter \
	floatingPointExactComparison \
	fopenFormatParameter \
	forbiddenFunctions \
	forLoopConstructionControlStmt \
	forLoopCppIndexVariableDeclaration \
	friendDeclarationModifier \
	functionCallAllocatesMultipleResources \
	functionDefinitionPrototype \
	functionDocumentation \
	inductionVariableUpdate \
	internalDataSharing \
	localizedVariables \
	locPerFunction \
	lowerRangeLimit \
	magicNumber \
	mallocReturnValueUsedInIfStmt \
	multiplePublicInheritance \
	nameAllParameters \
	nameConsistency \
	newDelete \
	noExceptions \
	noExitInMpiCode \
	noGoto \
	nonAssociativeRelationalOperators \
	nonmemberFunctionInterfaceNamespace \
	nonStandardTypeRefArgs \
	nonStandardTypeRefReturns \
	nonVirtualRedefinition \
	noOverloadAmpersand \
	noRand \
	noSecondTermSideEffects \
	noSideEffectInSizeof \
	noTemplateUsage \
	noVfork \
	nullDeref \
	oneLinePerDeclaration \
	operatorOverloading \
	otherArgument \
	placeConstantOnTheLhs \
	preferAlgorithms \
	preferFseekToRewind \
	preferSetvbufToSetbuf \
	protectVirtualMethods \
	pushBack \
	rightShiftMask \
	setPointersToNull \
	singleParameterConstructorExplicitModifier \
	sizeOfPointer \
	stringTokenToIntegerConverter \
	subExpressionEvaluationOrder \
	ternaryOperator \
	time_tDirectManipulation \
	typeTypedef \
	unaryMinus \
	uninitializedDefinition \
	upperRangeLimit \
	variableNameEqualsDatabaseName \
	voidStar

COMPASS_INCLUDES = \
	-I$(srcdir)/allocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstraction \
	-I$(srcdir)/assignmentOperatorCheckSelf \
	-I$(srcdir)/assignmentReturnConstThis \
	-I$(srcdir)/asynchronousSignalHandler \
	-I$(srcdir)/avoidUsingTheSameHandlerForMultipleSignals \
	-I$(srcdir)/booleanIsHas \
	-I$(srcdir)/bufferOverflowFunctions \
	-I$(srcdir)/byteByByteStructureComparison \
	-I$(srcdir)/charStarForString \
	-I$(srcdir)/commaOperator \
	-I$(srcdir)/computationalFunctions \
	-I$(srcdir)/constCast \
	-I$(srcdir)/constructorDestructorCallsVirtualFunction \
	-I$(srcdir)/constStringLiterals \
	-I$(srcdir)/controlVariableTestAgainstFunction \
	-I$(srcdir)/copyConstructorConstArg \
	-I$(srcdir)/cppCallsSetjmpLongjmp \
	-I$(srcdir)/cyclomaticComplexity \
	-I$(srcdir)/dataMemberAccess \
	-I$(srcdir)/deepNesting \
	-I$(srcdir)/defaultCase \
	-I$(srcdir)/defaultConstructor \
	-I$(srcdir)/discardAssignment \
	-I$(srcdir)/doNotCallPutenvWithAutoVar \
	-I$(srcdir)/doNotDeleteThis \
	-I$(srcdir)/doNotUseCstyleCasts \
	-I$(srcdir)/duffsDevice \
	-I$(srcdir)/dynamicCast \
	-I$(srcdir)/emptyInsteadOfSize \
	-I$(srcdir)/enumDeclarationNamespaceClassScope \
	-I$(srcdir)/explicitCharSign \
	-I$(srcdir)/explicitCopy \
	-I$(srcdir)/explicitTestForNonBooleanValue \
	-I$(srcdir)/floatForLoopCounter \
	-I$(srcdir)/floatingPointExactComparison \
	-I$(srcdir)/fopenFormatParameter \
	-I$(srcdir)/forbiddenFunctions \
	-I$(srcdir)/forLoopConstructionControlStmt \
	-I$(srcdir)/forLoopCppIndexVariableDeclaration \
	-I$(srcdir)/friendDeclarationModifier \
	-I$(srcdir)/functionCallAllocatesMultipleResources \
	-I$(srcdir)/functionDefinitionPrototype \
	-I$(srcdir)/functionDocumentation \
	-I$(srcdir)/inductionVariableUpdate \
	-I$(srcdir)/internalDataSharing \
	-I$(srcdir)/localizedVariables \
	-I$(srcdir)/locPerFunction \
	-I$(srcdir)/lowerRangeLimit \
	-I$(srcdir)/magicNumber \
	-I$(srcdir)/mallocReturnValueUsedInIfStmt \
	-I$(srcdir)/multiplePublicInheritance \
	-I$(srcdir)/nameAllParameters \
	-I$(srcdir)/nameConsistency \
	-I$(srcdir)/newDelete \
	-I$(srcdir)/noExceptions \
	-I$(srcdir)/noExitInMpiCode \
	-I$(srcdir)/noGoto \
	-I$(srcdir)/nonAssociativeRelationalOperators \
	-I$(srcdir)/nonmemberFunctionInterfaceNamespace \
	-I$(srcdir)/nonStandardTypeRefArgs \
	-I$(srcdir)/nonStandardTypeRefReturns \
	-I$(srcdir)/nonVirtualRedefinition \
	-I$(srcdir)/noOverloadAmpersand \
	-I$(srcdir)/noRand \
	-I$(srcdir)/noSecondTermSideEffects \
	-I$(srcdir)/noSideEffectInSizeof \
	-I$(srcdir)/noTemplateUsage \
	-I$(srcdir)/noVfork \
	-I$(srcdir)/nullDeref \
	-I$(srcdir)/oneLinePerDeclaration \
	-I$(srcdir)/operatorOverloading \
	-I$(srcdir)/otherArgument \
	-I$(srcdir)/placeConstantOnTheLhs \
	-I$(srcdir)/preferAlgorithms \
	-I$(srcdir)/preferFseekToRewind \
	-I$(srcdir)/preferSetvbufToSetbuf \
	-I$(srcdir)/protectVirtualMethods \
	-I$(srcdir)/pushBack \
	-I$(srcdir)/rightShiftMask \
	-I$(srcdir)/setPointersToNull \
	-I$(srcdir)/singleParameterConstructorExplicitModifier \
	-I$(srcdir)/sizeOfPointer \
	-I$(srcdir)/stringTokenToIntegerConverter \
	-I$(srcdir)/subExpressionEvaluationOrder \
	-I$(srcdir)/ternaryOperator \
	-I$(srcdir)/time_tDirectManipulation \
	-I$(srcdir)/typeTypedef \
	-I$(srcdir)/unaryMinus \
	-I$(srcdir)/uninitializedDefinition \
	-I$(srcdir)/upperRangeLimit \
	-I$(srcdir)/variableNameEqualsDatabaseName \
	-I$(srcdir)/voidStar

COMPASS_CHECKER_SRCS = \
	$(srcdir)/allocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstraction/allocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstraction.C \
	$(srcdir)/assignmentOperatorCheckSelf/assignmentOperatorCheckSelf.C \
	$(srcdir)/assignmentReturnConstThis/assignmentReturnConstThis.C \
	$(srcdir)/asynchronousSignalHandler/asynchronousSignalHandler.C \
	$(srcdir)/avoidUsingTheSameHandlerForMultipleSignals/avoidUsingTheSameHandlerForMultipleSignals.C \
	$(srcdir)/booleanIsHas/booleanIsHas.C \
	$(srcdir)/bufferOverflowFunctions/bufferOverflowFunctions.C \
	$(srcdir)/byteByByteStructureComparison/byteByByteStructureComparison.C \
	$(srcdir)/charStarForString/charStarForString.C \
	$(srcdir)/commaOperator/commaOperator.C \
	$(srcdir)/computationalFunctions/computationalFunctions.C \
	$(srcdir)/constCast/constCast.C \
	$(srcdir)/constructorDestructorCallsVirtualFunction/constructorDestructorCallsVirtualFunction.C \
	$(srcdir)/constStringLiterals/constStringLiterals.C \
	$(srcdir)/controlVariableTestAgainstFunction/controlVariableTestAgainstFunction.C \
	$(srcdir)/copyConstructorConstArg/copyConstructorConstArg.C \
	$(srcdir)/cppCallsSetjmpLongjmp/cppCallsSetjmpLongjmp.C \
	$(srcdir)/cyclomaticComplexity/cyclomaticComplexity.C \
	$(srcdir)/dataMemberAccess/dataMemberAccess.C \
	$(srcdir)/deepNesting/deepNesting.C \
	$(srcdir)/defaultCase/defaultCase.C \
	$(srcdir)/defaultConstructor/defaultConstructor.C \
	$(srcdir)/discardAssignment/discardAssignment.C \
	$(srcdir)/doNotCallPutenvWithAutoVar/doNotCallPutenvWithAutoVar.C \
	$(srcdir)/doNotDeleteThis/doNotDeleteThis.C \
	$(srcdir)/doNotUseCstyleCasts/doNotUseCstyleCasts.C \
	$(srcdir)/duffsDevice/duffsDevice.C \
	$(srcdir)/dynamicCast/dynamicCast.C \
	$(srcdir)/emptyInsteadOfSize/emptyInsteadOfSize.C \
	$(srcdir)/enumDeclarationNamespaceClassScope/enumDeclarationNamespaceClassScope.C \
	$(srcdir)/explicitCharSign/explicitCharSign.C \
	$(srcdir)/explicitCopy/explicitCopy.C \
	$(srcdir)/explicitTestForNonBooleanValue/explicitTestForNonBooleanValue.C \
	$(srcdir)/floatForLoopCounter/floatForLoopCounter.C \
	$(srcdir)/floatingPointExactComparison/floatingPointExactComparison.C \
	$(srcdir)/fopenFormatParameter/fopenFormatParameter.C \
	$(srcdir)/forbiddenFunctions/forbiddenFunctions.C \
	$(srcdir)/forLoopConstructionControlStmt/forLoopConstructionControlStmt.C \
	$(srcdir)/forLoopCppIndexVariableDeclaration/forLoopCppIndexVariableDeclaration.C \
	$(srcdir)/friendDeclarationModifier/friendDeclarationModifier.C \
	$(srcdir)/functionCallAllocatesMultipleResources/functionCallAllocatesMultipleResources.C \
	$(srcdir)/functionDefinitionPrototype/functionDefinitionPrototype.C \
	$(srcdir)/functionDocumentation/functionDocumentation.C \
	$(srcdir)/inductionVariableUpdate/inductionVariableUpdate.C \
	$(srcdir)/internalDataSharing/internalDataSharing.C \
	$(srcdir)/localizedVariables/localizedVariables.C \
	$(srcdir)/locPerFunction/locPerFunction.C \
	$(srcdir)/lowerRangeLimit/lowerRangeLimit.C \
	$(srcdir)/magicNumber/magicNumber.C \
	$(srcdir)/mallocReturnValueUsedInIfStmt/mallocReturnValueUsedInIfStmt.C \
	$(srcdir)/multiplePublicInheritance/multiplePublicInheritance.C \
	$(srcdir)/nameAllParameters/nameAllParameters.C \
	$(srcdir)/nameConsistency/nameConsistency.C \
	$(srcdir)/newDelete/newDelete.C \
	$(srcdir)/noExceptions/noExceptions.C \
	$(srcdir)/noExitInMpiCode/noExitInMpiCode.C \
	$(srcdir)/noGoto/noGoto.C \
	$(srcdir)/nonAssociativeRelationalOperators/nonAssociativeRelationalOperators.C \
	$(srcdir)/nonmemberFunctionInterfaceNamespace/nonmemberFunctionInterfaceNamespace.C \
	$(srcdir)/nonStandardTypeRefArgs/nonStandardTypeRefArgs.C \
	$(srcdir)/nonStandardTypeRefReturns/nonStandardTypeRefReturns.C \
	$(srcdir)/nonVirtualRedefinition/nonVirtualRedefinition.C \
	$(srcdir)/noOverloadAmpersand/noOverloadAmpersand.C \
	$(srcdir)/noRand/noRand.C \
	$(srcdir)/noSecondTermSideEffects/noSecondTermSideEffects.C \
	$(srcdir)/noSideEffectInSizeof/noSideEffectInSizeof.C \
	$(srcdir)/noTemplateUsage/noTemplateUsage.C \
	$(srcdir)/noVfork/noVfork.C \
	$(srcdir)/nullDeref/nullDeref.C \
	$(srcdir)/oneLinePerDeclaration/oneLinePerDeclaration.C \
	$(srcdir)/operatorOverloading/operatorOverloading.C \
	$(srcdir)/otherArgument/otherArgument.C \
	$(srcdir)/placeConstantOnTheLhs/placeConstantOnTheLhs.C \
	$(srcdir)/preferAlgorithms/preferAlgorithms.C \
	$(srcdir)/preferFseekToRewind/preferFseekToRewind.C \
	$(srcdir)/preferSetvbufToSetbuf/preferSetvbufToSetbuf.C \
	$(srcdir)/protectVirtualMethods/protectVirtualMethods.C \
	$(srcdir)/pushBack/pushBack.C \
	$(srcdir)/rightShiftMask/rightShiftMask.C \
	$(srcdir)/setPointersToNull/setPointersToNull.C \
	$(srcdir)/singleParameterConstructorExplicitModifier/singleParameterConstructorExplicitModifier.C \
	$(srcdir)/sizeOfPointer/sizeOfPointer.C \
	$(srcdir)/stringTokenToIntegerConverter/stringTokenToIntegerConverter.C \
	$(srcdir)/subExpressionEvaluationOrder/subExpressionEvaluationOrder.C \
	$(srcdir)/ternaryOperator/ternaryOperator.C \
	$(srcdir)/time_tDirectManipulation/time_tDirectManipulation.C \
	$(srcdir)/typeTypedef/typeTypedef.C \
	$(srcdir)/unaryMinus/unaryMinus.C \
	$(srcdir)/uninitializedDefinition/uninitializedDefinition.C \
	$(srcdir)/upperRangeLimit/upperRangeLimit.C \
	$(srcdir)/variableNameEqualsDatabaseName/variableNameEqualsDatabaseName.C \
	$(srcdir)/voidStar/voidStar.C

COMPASS_CHECKER_TESTS = \
	 $(srcdir)/allocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstraction/allocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstractionTest1.C \
	 $(srcdir)/assignmentOperatorCheckSelf/assignmentOperatorCheckSelfTest1.C \
	 $(srcdir)/assignmentReturnConstThis/assignmentReturnConstThisTest1.C \
	 $(srcdir)/asynchronousSignalHandler/asynchronousSignalHandlerTest1.C \
	 $(srcdir)/avoidUsingTheSameHandlerForMultipleSignals/avoidUsingTheSameHandlerForMultipleSignalsTest1.C \
	 $(srcdir)/booleanIsHas/booleanIsHasTest1.C \
	 $(srcdir)/bufferOverflowFunctions/bufferOverflowFunctionsTest1.C \
	 $(srcdir)/byteByByteStructureComparison/byteByByteStructureComparisonTest1.C \
	 $(srcdir)/charStarForString/charStarForStringTest1.C \
	 $(srcdir)/commaOperator/commaOperatorTest1.C \
	 $(srcdir)/computationalFunctions/computationalFunctionsTest1.C \
	 $(srcdir)/constCast/constCastTest1.C \
	 $(srcdir)/constructorDestructorCallsVirtualFunction/constructorDestructorCallsVirtualFunctionTest1.C \
	 $(srcdir)/constStringLiterals/constStringLiteralsTest1.C \
	 $(srcdir)/controlVariableTestAgainstFunction/controlVariableTestAgainstFunctionTest1.C \
	 $(srcdir)/copyConstructorConstArg/copyConstructorConstArgTest1.C \
	 $(srcdir)/cppCallsSetjmpLongjmp/cppCallsSetjmpLongjmpTest1.C \
	 $(srcdir)/cyclomaticComplexity/cyclomaticComplexityTest1.C \
	 $(srcdir)/dataMemberAccess/dataMemberAccessTest1.C \
	 $(srcdir)/deepNesting/deepNestingTest1.C \
	 $(srcdir)/defaultCase/defaultCaseTest1.C \
	 $(srcdir)/defaultConstructor/defaultConstructorTest1.C \
	 $(srcdir)/discardAssignment/discardAssignmentTest1.C \
	 $(srcdir)/doNotCallPutenvWithAutoVar/doNotCallPutenvWithAutoVarTest1.C \
	 $(srcdir)/doNotDeleteThis/doNotDeleteThisTest1.C \
	 $(srcdir)/doNotUseCstyleCasts/doNotUseCstyleCastsTest1.C \
	 $(srcdir)/duffsDevice/duffsDeviceTest1.C \
	 $(srcdir)/dynamicCast/dynamicCastTest1.C \
	 $(srcdir)/emptyInsteadOfSize/emptyInsteadOfSizeTest1.C \
	 $(srcdir)/enumDeclarationNamespaceClassScope/enumDeclarationNamespaceClassScopeTest1.C \
	 $(srcdir)/explicitCharSign/explicitCharSignTest1.C \
	 $(srcdir)/explicitCopy/explicitCopyTest1.C \
	 $(srcdir)/explicitTestForNonBooleanValue/explicitTestForNonBooleanValueTest1.C \
	 $(srcdir)/floatForLoopCounter/floatForLoopCounterTest1.C \
	 $(srcdir)/floatingPointExactComparison/floatingPointExactComparisonTest1.C \
	 $(srcdir)/fopenFormatParameter/fopenFormatParameterTest1.C \
	 $(srcdir)/forbiddenFunctions/forbiddenFunctionsTest1.C \
	 $(srcdir)/forLoopConstructionControlStmt/forLoopConstructionControlStmtTest1.C \
	 $(srcdir)/forLoopCppIndexVariableDeclaration/forLoopCppIndexVariableDeclarationTest1.C \
	 $(srcdir)/friendDeclarationModifier/friendDeclarationModifierTest1.C \
	 $(srcdir)/functionCallAllocatesMultipleResources/functionCallAllocatesMultipleResourcesTest1.C \
	 $(srcdir)/functionDefinitionPrototype/functionDefinitionPrototypeTest1.C \
	 $(srcdir)/functionDocumentation/functionDocumentationTest1.C \
	 $(srcdir)/inductionVariableUpdate/inductionVariableUpdateTest1.C \
	 $(srcdir)/internalDataSharing/internalDataSharingTest1.C \
	 $(srcdir)/localizedVariables/localizedVariablesTest1.C \
	 $(srcdir)/locPerFunction/locPerFunctionTest1.C \
	 $(srcdir)/lowerRangeLimit/lowerRangeLimitTest1.C \
	 $(srcdir)/magicNumber/magicNumberTest1.C \
	 $(srcdir)/mallocReturnValueUsedInIfStmt/mallocReturnValueUsedInIfStmtTest1.C \
	 $(srcdir)/multiplePublicInheritance/multiplePublicInheritanceTest1.C \
	 $(srcdir)/nameAllParameters/nameAllParametersTest1.C \
	 $(srcdir)/nameConsistency/nameConsistencyTest1.C \
	 $(srcdir)/newDelete/newDeleteTest1.C \
	 $(srcdir)/noExceptions/noExceptionsTest1.C \
	 $(srcdir)/noExitInMpiCode/noExitInMpiCodeTest1.C \
	 $(srcdir)/noGoto/noGotoTest1.C \
	 $(srcdir)/nonAssociativeRelationalOperators/nonAssociativeRelationalOperatorsTest1.C \
	 $(srcdir)/nonmemberFunctionInterfaceNamespace/nonmemberFunctionInterfaceNamespaceTest1.C \
	 $(srcdir)/nonStandardTypeRefArgs/nonStandardTypeRefArgsTest1.C \
	 $(srcdir)/nonStandardTypeRefReturns/nonStandardTypeRefReturnsTest1.C \
	 $(srcdir)/nonVirtualRedefinition/nonVirtualRedefinitionTest1.C \
	 $(srcdir)/noOverloadAmpersand/noOverloadAmpersandTest1.C \
	 $(srcdir)/noRand/noRandTest1.C \
	 $(srcdir)/noSecondTermSideEffects/noSecondTermSideEffectsTest1.C \
	 $(srcdir)/noSideEffectInSizeof/noSideEffectInSizeofTest1.C \
	 $(srcdir)/noTemplateUsage/noTemplateUsageTest1.C \
	 $(srcdir)/noVfork/noVforkTest1.C \
	 $(srcdir)/nullDeref/nullDerefTest1.C \
	 $(srcdir)/oneLinePerDeclaration/oneLinePerDeclarationTest1.C \
	 $(srcdir)/operatorOverloading/operatorOverloadingTest1.C \
	 $(srcdir)/otherArgument/otherArgumentTest1.C \
	 $(srcdir)/placeConstantOnTheLhs/placeConstantOnTheLhsTest1.C \
	 $(srcdir)/preferAlgorithms/preferAlgorithmsTest1.C \
	 $(srcdir)/preferFseekToRewind/preferFseekToRewindTest1.C \
	 $(srcdir)/preferSetvbufToSetbuf/preferSetvbufToSetbufTest1.C \
	 $(srcdir)/protectVirtualMethods/protectVirtualMethodsTest1.C \
	 $(srcdir)/pushBack/pushBackTest1.C \
	 $(srcdir)/rightShiftMask/rightShiftMaskTest1.C \
	 $(srcdir)/setPointersToNull/setPointersToNullTest1.C \
	 $(srcdir)/singleParameterConstructorExplicitModifier/singleParameterConstructorExplicitModifierTest1.C \
	 $(srcdir)/sizeOfPointer/sizeOfPointerTest1.C \
	 $(srcdir)/stringTokenToIntegerConverter/stringTokenToIntegerConverterTest1.C \
	 $(srcdir)/subExpressionEvaluationOrder/subExpressionEvaluationOrderTest1.C \
	 $(srcdir)/ternaryOperator/ternaryOperatorTest1.C \
	 $(srcdir)/time_tDirectManipulation/time_tDirectManipulationTest1.C \
	 $(srcdir)/typeTypedef/typeTypedefTest1.C \
	 $(srcdir)/unaryMinus/unaryMinusTest1.C \
	 $(srcdir)/uninitializedDefinition/uninitializedDefinitionTest1.C \
	 $(srcdir)/upperRangeLimit/upperRangeLimitTest1.C \
	 $(srcdir)/variableNameEqualsDatabaseName/variableNameEqualsDatabaseNameTest1.C \
	 $(srcdir)/voidStar/voidStarTest1.C

COMPASS_CHECKER_TEST_LABELS = \
	testAllocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstraction \
	testAssignmentOperatorCheckSelf \
	testAssignmentReturnConstThis \
	testAsynchronousSignalHandler \
	testAvoidUsingTheSameHandlerForMultipleSignals \
	testBooleanIsHas \
	testBufferOverflowFunctions \
	testByteByByteStructureComparison \
	testCharStarForString \
	testCommaOperator \
	testComputationalFunctions \
	testConstCast \
	testConstructorDestructorCallsVirtualFunction \
	testConstStringLiterals \
	testControlVariableTestAgainstFunction \
	testCopyConstructorConstArg \
	testCppCallsSetjmpLongjmp \
	testCyclomaticComplexity \
	testDataMemberAccess \
	testDeepNesting \
	testDefaultCase \
	testDefaultConstructor \
	testDiscardAssignment \
	testDoNotCallPutenvWithAutoVar \
	testDoNotDeleteThis \
	testDoNotUseCstyleCasts \
	testDuffsDevice \
	testDynamicCast \
	testEmptyInsteadOfSize \
	testEnumDeclarationNamespaceClassScope \
	testExplicitCharSign \
	testExplicitCopy \
	testExplicitTestForNonBooleanValue \
	testFloatForLoopCounter \
	testFloatingPointExactComparison \
	testFopenFormatParameter \
	testForbiddenFunctions \
	testForLoopConstructionControlStmt \
	testForLoopCppIndexVariableDeclaration \
	testFriendDeclarationModifier \
	testFunctionCallAllocatesMultipleResources \
	testFunctionDefinitionPrototype \
	testFunctionDocumentation \
	testInductionVariableUpdate \
	testInternalDataSharing \
	testLocalizedVariables \
	testLocPerFunction \
	testLowerRangeLimit \
	testMagicNumber \
	testMallocReturnValueUsedInIfStmt \
	testMultiplePublicInheritance \
	testNameAllParameters \
	testNameConsistency \
	testNewDelete \
	testNoExceptions \
	testNoExitInMpiCode \
	testNoGoto \
	testNonAssociativeRelationalOperators \
	testNonmemberFunctionInterfaceNamespace \
	testNonStandardTypeRefArgs \
	testNonStandardTypeRefReturns \
	testNonVirtualRedefinition \
	testNoOverloadAmpersand \
	testNoRand \
	testNoSecondTermSideEffects \
	testNoSideEffectInSizeof \
	testNoTemplateUsage \
	testNoVfork \
	testNullDeref \
	testOneLinePerDeclaration \
	testOperatorOverloading \
	testOtherArgument \
	testPlaceConstantOnTheLhs \
	testPreferAlgorithms \
	testPreferFseekToRewind \
	testPreferSetvbufToSetbuf \
	testProtectVirtualMethods \
	testPushBack \
	testRightShiftMask \
	testSetPointersToNull \
	testSingleParameterConstructorExplicitModifier \
	testSizeOfPointer \
	testStringTokenToIntegerConverter \
	testSubExpressionEvaluationOrder \
	testTernaryOperator \
	testTime_tDirectManipulation \
	testTypeTypedef \
	testUnaryMinus \
	testUninitializedDefinition \
	testUpperRangeLimit \
	testVariableNameEqualsDatabaseName \
	testVoidStar

COMPASS_CHECKER_TEX_FILES = \
	allocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstractionDocs.tex \
	assignmentOperatorCheckSelfDocs.tex \
	assignmentReturnConstThisDocs.tex \
	asynchronousSignalHandlerDocs.tex \
	avoidUsingTheSameHandlerForMultipleSignalsDocs.tex \
	booleanIsHasDocs.tex \
	bufferOverflowFunctionsDocs.tex \
	byteByByteStructureComparisonDocs.tex \
	charStarForStringDocs.tex \
	commaOperatorDocs.tex \
	computationalFunctionsDocs.tex \
	constCastDocs.tex \
	constructorDestructorCallsVirtualFunctionDocs.tex \
	constStringLiteralsDocs.tex \
	controlVariableTestAgainstFunctionDocs.tex \
	copyConstructorConstArgDocs.tex \
	cppCallsSetjmpLongjmpDocs.tex \
	cyclomaticComplexityDocs.tex \
	dataMemberAccessDocs.tex \
	deepNestingDocs.tex \
	defaultCaseDocs.tex \
	defaultConstructorDocs.tex \
	discardAssignmentDocs.tex \
	doNotCallPutenvWithAutoVarDocs.tex \
	doNotDeleteThisDocs.tex \
	doNotUseCstyleCastsDocs.tex \
	duffsDeviceDocs.tex \
	dynamicCastDocs.tex \
	emptyInsteadOfSizeDocs.tex \
	enumDeclarationNamespaceClassScopeDocs.tex \
	explicitCharSignDocs.tex \
	explicitCopyDocs.tex \
	explicitTestForNonBooleanValueDocs.tex \
	floatForLoopCounterDocs.tex \
	floatingPointExactComparisonDocs.tex \
	fopenFormatParameterDocs.tex \
	forbiddenFunctionsDocs.tex \
	forLoopConstructionControlStmtDocs.tex \
	forLoopCppIndexVariableDeclarationDocs.tex \
	friendDeclarationModifierDocs.tex \
	functionCallAllocatesMultipleResourcesDocs.tex \
	functionDefinitionPrototypeDocs.tex \
	functionDocumentationDocs.tex \
	inductionVariableUpdateDocs.tex \
	internalDataSharingDocs.tex \
	localizedVariablesDocs.tex \
	locPerFunctionDocs.tex \
	lowerRangeLimitDocs.tex \
	magicNumberDocs.tex \
	mallocReturnValueUsedInIfStmtDocs.tex \
	multiplePublicInheritanceDocs.tex \
	nameAllParametersDocs.tex \
	nameConsistencyDocs.tex \
	newDeleteDocs.tex \
	noExceptionsDocs.tex \
	noExitInMpiCodeDocs.tex \
	noGotoDocs.tex \
	nonAssociativeRelationalOperatorsDocs.tex \
	nonmemberFunctionInterfaceNamespaceDocs.tex \
	nonStandardTypeRefArgsDocs.tex \
	nonStandardTypeRefReturnsDocs.tex \
	nonVirtualRedefinitionDocs.tex \
	noOverloadAmpersandDocs.tex \
	noRandDocs.tex \
	noSecondTermSideEffectsDocs.tex \
	noSideEffectInSizeofDocs.tex \
	noTemplateUsageDocs.tex \
	noVforkDocs.tex \
	nullDerefDocs.tex \
	oneLinePerDeclarationDocs.tex \
	operatorOverloadingDocs.tex \
	otherArgumentDocs.tex \
	placeConstantOnTheLhsDocs.tex \
	preferAlgorithmsDocs.tex \
	preferFseekToRewindDocs.tex \
	preferSetvbufToSetbufDocs.tex \
	protectVirtualMethodsDocs.tex \
	pushBackDocs.tex \
	rightShiftMaskDocs.tex \
	setPointersToNullDocs.tex \
	singleParameterConstructorExplicitModifierDocs.tex \
	sizeOfPointerDocs.tex \
	stringTokenToIntegerConverterDocs.tex \
	subExpressionEvaluationOrderDocs.tex \
	ternaryOperatorDocs.tex \
	time_tDirectManipulationDocs.tex \
	typeTypedefDocs.tex \
	unaryMinusDocs.tex \
	uninitializedDefinitionDocs.tex \
	upperRangeLimitDocs.tex \
	variableNameEqualsDatabaseNameDocs.tex \
	voidStarDocs.tex

# Liao, 3/5/2008, added to generate compassCheckerDocs.tex
compassCheckerDocs.tex:
	$(MAKE) regenerate_skip_compass_makefile_inc
docs: compass.tex compassCheckerDocs.tex
	test -f fixme.sty || ln -s $(top_srcdir)/docs/Rose/fixme.sty fixme.sty
	test -f acknowledgments.tex || ln -s $(srcdir)/acknowledgments.tex acknowledgments.tex
	test -f introduction.tex || ln -s $(srcdir)/introduction.tex introduction.tex
	test -f usingCompass.tex || ln -s $(srcdir)/usingCompass.tex usingCompass.tex
	test -f emacs_screenshot.jpg || ln -s $(srcdir)/emacs_screenshot.jpg emacs_screenshot.jpg
	test -f CompassScreenshot.pdf || ln -s $(srcdir)/CompassScreenshot.pdf CompassScreenshot.pdf
	test -f ToolGear_gui_compass_01.pdf || ln -s $(srcdir)/ToolGear_gui_compass_01.pdf ToolGear_gui_compass_01.pdf
	test -f appendix.tex || ln -s $(srcdir)/appendix.tex appendix.tex
	test -f allocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstractionDocs.new.tex || ln -s $(srcdir)/allocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstraction/allocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstractionDocs.new.tex allocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstractionDocs.new.tex
	test -f assignmentOperatorCheckSelfDocs.new.tex || ln -s $(srcdir)/assignmentOperatorCheckSelf/assignmentOperatorCheckSelfDocs.new.tex assignmentOperatorCheckSelfDocs.new.tex
	test -f assignmentReturnConstThisDocs.new.tex || ln -s $(srcdir)/assignmentReturnConstThis/assignmentReturnConstThisDocs.new.tex assignmentReturnConstThisDocs.new.tex
	test -f asynchronousSignalHandlerDocs.new.tex || ln -s $(srcdir)/asynchronousSignalHandler/asynchronousSignalHandlerDocs.new.tex asynchronousSignalHandlerDocs.new.tex
	test -f avoidUsingTheSameHandlerForMultipleSignalsDocs.new.tex || ln -s $(srcdir)/avoidUsingTheSameHandlerForMultipleSignals/avoidUsingTheSameHandlerForMultipleSignalsDocs.new.tex avoidUsingTheSameHandlerForMultipleSignalsDocs.new.tex
	test -f booleanIsHasDocs.new.tex || ln -s $(srcdir)/booleanIsHas/booleanIsHasDocs.new.tex booleanIsHasDocs.new.tex
	test -f bufferOverflowFunctionsDocs.new.tex || ln -s $(srcdir)/bufferOverflowFunctions/bufferOverflowFunctionsDocs.new.tex bufferOverflowFunctionsDocs.new.tex
	test -f byteByByteStructureComparisonDocs.new.tex || ln -s $(srcdir)/byteByByteStructureComparison/byteByByteStructureComparisonDocs.new.tex byteByByteStructureComparisonDocs.new.tex
	test -f charStarForStringDocs.new.tex || ln -s $(srcdir)/charStarForString/charStarForStringDocs.new.tex charStarForStringDocs.new.tex
	test -f commaOperatorDocs.new.tex || ln -s $(srcdir)/commaOperator/commaOperatorDocs.new.tex commaOperatorDocs.new.tex
	test -f computationalFunctionsDocs.new.tex || ln -s $(srcdir)/computationalFunctions/computationalFunctionsDocs.new.tex computationalFunctionsDocs.new.tex
	test -f constCastDocs.new.tex || ln -s $(srcdir)/constCast/constCastDocs.new.tex constCastDocs.new.tex
	test -f constructorDestructorCallsVirtualFunctionDocs.new.tex || ln -s $(srcdir)/constructorDestructorCallsVirtualFunction/constructorDestructorCallsVirtualFunctionDocs.new.tex constructorDestructorCallsVirtualFunctionDocs.new.tex
	test -f constStringLiteralsDocs.new.tex || ln -s $(srcdir)/constStringLiterals/constStringLiteralsDocs.new.tex constStringLiteralsDocs.new.tex
	test -f controlVariableTestAgainstFunctionDocs.new.tex || ln -s $(srcdir)/controlVariableTestAgainstFunction/controlVariableTestAgainstFunctionDocs.new.tex controlVariableTestAgainstFunctionDocs.new.tex
	test -f copyConstructorConstArgDocs.new.tex || ln -s $(srcdir)/copyConstructorConstArg/copyConstructorConstArgDocs.new.tex copyConstructorConstArgDocs.new.tex
	test -f cppCallsSetjmpLongjmpDocs.new.tex || ln -s $(srcdir)/cppCallsSetjmpLongjmp/cppCallsSetjmpLongjmpDocs.new.tex cppCallsSetjmpLongjmpDocs.new.tex
	test -f cyclomaticComplexityDocs.new.tex || ln -s $(srcdir)/cyclomaticComplexity/cyclomaticComplexityDocs.new.tex cyclomaticComplexityDocs.new.tex
	test -f dataMemberAccessDocs.new.tex || ln -s $(srcdir)/dataMemberAccess/dataMemberAccessDocs.new.tex dataMemberAccessDocs.new.tex
	test -f deepNestingDocs.new.tex || ln -s $(srcdir)/deepNesting/deepNestingDocs.new.tex deepNestingDocs.new.tex
	test -f defaultCaseDocs.new.tex || ln -s $(srcdir)/defaultCase/defaultCaseDocs.new.tex defaultCaseDocs.new.tex
	test -f defaultConstructorDocs.new.tex || ln -s $(srcdir)/defaultConstructor/defaultConstructorDocs.new.tex defaultConstructorDocs.new.tex
	test -f discardAssignmentDocs.new.tex || ln -s $(srcdir)/discardAssignment/discardAssignmentDocs.new.tex discardAssignmentDocs.new.tex
	test -f doNotCallPutenvWithAutoVarDocs.new.tex || ln -s $(srcdir)/doNotCallPutenvWithAutoVar/doNotCallPutenvWithAutoVarDocs.new.tex doNotCallPutenvWithAutoVarDocs.new.tex
	test -f doNotDeleteThisDocs.new.tex || ln -s $(srcdir)/doNotDeleteThis/doNotDeleteThisDocs.new.tex doNotDeleteThisDocs.new.tex
	test -f doNotUseCstyleCastsDocs.new.tex || ln -s $(srcdir)/doNotUseCstyleCasts/doNotUseCstyleCastsDocs.new.tex doNotUseCstyleCastsDocs.new.tex
	test -f duffsDeviceDocs.new.tex || ln -s $(srcdir)/duffsDevice/duffsDeviceDocs.new.tex duffsDeviceDocs.new.tex
	test -f dynamicCastDocs.new.tex || ln -s $(srcdir)/dynamicCast/dynamicCastDocs.new.tex dynamicCastDocs.new.tex
	test -f emptyInsteadOfSizeDocs.new.tex || ln -s $(srcdir)/emptyInsteadOfSize/emptyInsteadOfSizeDocs.new.tex emptyInsteadOfSizeDocs.new.tex
	test -f enumDeclarationNamespaceClassScopeDocs.new.tex || ln -s $(srcdir)/enumDeclarationNamespaceClassScope/enumDeclarationNamespaceClassScopeDocs.new.tex enumDeclarationNamespaceClassScopeDocs.new.tex
	test -f explicitCharSignDocs.new.tex || ln -s $(srcdir)/explicitCharSign/explicitCharSignDocs.new.tex explicitCharSignDocs.new.tex
	test -f explicitCopyDocs.new.tex || ln -s $(srcdir)/explicitCopy/explicitCopyDocs.new.tex explicitCopyDocs.new.tex
	test -f explicitTestForNonBooleanValueDocs.new.tex || ln -s $(srcdir)/explicitTestForNonBooleanValue/explicitTestForNonBooleanValueDocs.new.tex explicitTestForNonBooleanValueDocs.new.tex
	test -f floatForLoopCounterDocs.new.tex || ln -s $(srcdir)/floatForLoopCounter/floatForLoopCounterDocs.new.tex floatForLoopCounterDocs.new.tex
	test -f floatingPointExactComparisonDocs.new.tex || ln -s $(srcdir)/floatingPointExactComparison/floatingPointExactComparisonDocs.new.tex floatingPointExactComparisonDocs.new.tex
	test -f fopenFormatParameterDocs.new.tex || ln -s $(srcdir)/fopenFormatParameter/fopenFormatParameterDocs.new.tex fopenFormatParameterDocs.new.tex
	test -f forbiddenFunctionsDocs.new.tex || ln -s $(srcdir)/forbiddenFunctions/forbiddenFunctionsDocs.new.tex forbiddenFunctionsDocs.new.tex
	test -f forLoopConstructionControlStmtDocs.new.tex || ln -s $(srcdir)/forLoopConstructionControlStmt/forLoopConstructionControlStmtDocs.new.tex forLoopConstructionControlStmtDocs.new.tex
	test -f forLoopCppIndexVariableDeclarationDocs.new.tex || ln -s $(srcdir)/forLoopCppIndexVariableDeclaration/forLoopCppIndexVariableDeclarationDocs.new.tex forLoopCppIndexVariableDeclarationDocs.new.tex
	test -f friendDeclarationModifierDocs.new.tex || ln -s $(srcdir)/friendDeclarationModifier/friendDeclarationModifierDocs.new.tex friendDeclarationModifierDocs.new.tex
	test -f functionCallAllocatesMultipleResourcesDocs.new.tex || ln -s $(srcdir)/functionCallAllocatesMultipleResources/functionCallAllocatesMultipleResourcesDocs.new.tex functionCallAllocatesMultipleResourcesDocs.new.tex
	test -f functionDefinitionPrototypeDocs.new.tex || ln -s $(srcdir)/functionDefinitionPrototype/functionDefinitionPrototypeDocs.new.tex functionDefinitionPrototypeDocs.new.tex
	test -f functionDocumentationDocs.new.tex || ln -s $(srcdir)/functionDocumentation/functionDocumentationDocs.new.tex functionDocumentationDocs.new.tex
	test -f inductionVariableUpdateDocs.new.tex || ln -s $(srcdir)/inductionVariableUpdate/inductionVariableUpdateDocs.new.tex inductionVariableUpdateDocs.new.tex
	test -f internalDataSharingDocs.new.tex || ln -s $(srcdir)/internalDataSharing/internalDataSharingDocs.new.tex internalDataSharingDocs.new.tex
	test -f localizedVariablesDocs.new.tex || ln -s $(srcdir)/localizedVariables/localizedVariablesDocs.new.tex localizedVariablesDocs.new.tex
	test -f locPerFunctionDocs.new.tex || ln -s $(srcdir)/locPerFunction/locPerFunctionDocs.new.tex locPerFunctionDocs.new.tex
	test -f lowerRangeLimitDocs.new.tex || ln -s $(srcdir)/lowerRangeLimit/lowerRangeLimitDocs.new.tex lowerRangeLimitDocs.new.tex
	test -f magicNumberDocs.new.tex || ln -s $(srcdir)/magicNumber/magicNumberDocs.new.tex magicNumberDocs.new.tex
	test -f mallocReturnValueUsedInIfStmtDocs.new.tex || ln -s $(srcdir)/mallocReturnValueUsedInIfStmt/mallocReturnValueUsedInIfStmtDocs.new.tex mallocReturnValueUsedInIfStmtDocs.new.tex
	test -f multiplePublicInheritanceDocs.new.tex || ln -s $(srcdir)/multiplePublicInheritance/multiplePublicInheritanceDocs.new.tex multiplePublicInheritanceDocs.new.tex
	test -f nameAllParametersDocs.new.tex || ln -s $(srcdir)/nameAllParameters/nameAllParametersDocs.new.tex nameAllParametersDocs.new.tex
	test -f nameConsistencyDocs.new.tex || ln -s $(srcdir)/nameConsistency/nameConsistencyDocs.new.tex nameConsistencyDocs.new.tex
	test -f newDeleteDocs.new.tex || ln -s $(srcdir)/newDelete/newDeleteDocs.new.tex newDeleteDocs.new.tex
	test -f noExceptionsDocs.new.tex || ln -s $(srcdir)/noExceptions/noExceptionsDocs.new.tex noExceptionsDocs.new.tex
	test -f noExitInMpiCodeDocs.new.tex || ln -s $(srcdir)/noExitInMpiCode/noExitInMpiCodeDocs.new.tex noExitInMpiCodeDocs.new.tex
	test -f noGotoDocs.new.tex || ln -s $(srcdir)/noGoto/noGotoDocs.new.tex noGotoDocs.new.tex
	test -f nonAssociativeRelationalOperatorsDocs.new.tex || ln -s $(srcdir)/nonAssociativeRelationalOperators/nonAssociativeRelationalOperatorsDocs.new.tex nonAssociativeRelationalOperatorsDocs.new.tex
	test -f nonmemberFunctionInterfaceNamespaceDocs.new.tex || ln -s $(srcdir)/nonmemberFunctionInterfaceNamespace/nonmemberFunctionInterfaceNamespaceDocs.new.tex nonmemberFunctionInterfaceNamespaceDocs.new.tex
	test -f nonStandardTypeRefArgsDocs.new.tex || ln -s $(srcdir)/nonStandardTypeRefArgs/nonStandardTypeRefArgsDocs.new.tex nonStandardTypeRefArgsDocs.new.tex
	test -f nonStandardTypeRefReturnsDocs.new.tex || ln -s $(srcdir)/nonStandardTypeRefReturns/nonStandardTypeRefReturnsDocs.new.tex nonStandardTypeRefReturnsDocs.new.tex
	test -f nonVirtualRedefinitionDocs.new.tex || ln -s $(srcdir)/nonVirtualRedefinition/nonVirtualRedefinitionDocs.new.tex nonVirtualRedefinitionDocs.new.tex
	test -f noOverloadAmpersandDocs.new.tex || ln -s $(srcdir)/noOverloadAmpersand/noOverloadAmpersandDocs.new.tex noOverloadAmpersandDocs.new.tex
	test -f noRandDocs.new.tex || ln -s $(srcdir)/noRand/noRandDocs.new.tex noRandDocs.new.tex
	test -f noSecondTermSideEffectsDocs.new.tex || ln -s $(srcdir)/noSecondTermSideEffects/noSecondTermSideEffectsDocs.new.tex noSecondTermSideEffectsDocs.new.tex
	test -f noSideEffectInSizeofDocs.new.tex || ln -s $(srcdir)/noSideEffectInSizeof/noSideEffectInSizeofDocs.new.tex noSideEffectInSizeofDocs.new.tex
	test -f noTemplateUsageDocs.new.tex || ln -s $(srcdir)/noTemplateUsage/noTemplateUsageDocs.new.tex noTemplateUsageDocs.new.tex
	test -f noVforkDocs.new.tex || ln -s $(srcdir)/noVfork/noVforkDocs.new.tex noVforkDocs.new.tex
	test -f nullDerefDocs.new.tex || ln -s $(srcdir)/nullDeref/nullDerefDocs.new.tex nullDerefDocs.new.tex
	test -f oneLinePerDeclarationDocs.new.tex || ln -s $(srcdir)/oneLinePerDeclaration/oneLinePerDeclarationDocs.new.tex oneLinePerDeclarationDocs.new.tex
	test -f operatorOverloadingDocs.new.tex || ln -s $(srcdir)/operatorOverloading/operatorOverloadingDocs.new.tex operatorOverloadingDocs.new.tex
	test -f otherArgumentDocs.new.tex || ln -s $(srcdir)/otherArgument/otherArgumentDocs.new.tex otherArgumentDocs.new.tex
	test -f placeConstantOnTheLhsDocs.new.tex || ln -s $(srcdir)/placeConstantOnTheLhs/placeConstantOnTheLhsDocs.new.tex placeConstantOnTheLhsDocs.new.tex
	test -f preferAlgorithmsDocs.new.tex || ln -s $(srcdir)/preferAlgorithms/preferAlgorithmsDocs.new.tex preferAlgorithmsDocs.new.tex
	test -f preferFseekToRewindDocs.new.tex || ln -s $(srcdir)/preferFseekToRewind/preferFseekToRewindDocs.new.tex preferFseekToRewindDocs.new.tex
	test -f preferSetvbufToSetbufDocs.new.tex || ln -s $(srcdir)/preferSetvbufToSetbuf/preferSetvbufToSetbufDocs.new.tex preferSetvbufToSetbufDocs.new.tex
	test -f protectVirtualMethodsDocs.new.tex || ln -s $(srcdir)/protectVirtualMethods/protectVirtualMethodsDocs.new.tex protectVirtualMethodsDocs.new.tex
	test -f pushBackDocs.new.tex || ln -s $(srcdir)/pushBack/pushBackDocs.new.tex pushBackDocs.new.tex
	test -f rightShiftMaskDocs.new.tex || ln -s $(srcdir)/rightShiftMask/rightShiftMaskDocs.new.tex rightShiftMaskDocs.new.tex
	test -f setPointersToNullDocs.new.tex || ln -s $(srcdir)/setPointersToNull/setPointersToNullDocs.new.tex setPointersToNullDocs.new.tex
	test -f singleParameterConstructorExplicitModifierDocs.new.tex || ln -s $(srcdir)/singleParameterConstructorExplicitModifier/singleParameterConstructorExplicitModifierDocs.new.tex singleParameterConstructorExplicitModifierDocs.new.tex
	test -f sizeOfPointerDocs.new.tex || ln -s $(srcdir)/sizeOfPointer/sizeOfPointerDocs.new.tex sizeOfPointerDocs.new.tex
	test -f stringTokenToIntegerConverterDocs.new.tex || ln -s $(srcdir)/stringTokenToIntegerConverter/stringTokenToIntegerConverterDocs.new.tex stringTokenToIntegerConverterDocs.new.tex
	test -f subExpressionEvaluationOrderDocs.new.tex || ln -s $(srcdir)/subExpressionEvaluationOrder/subExpressionEvaluationOrderDocs.new.tex subExpressionEvaluationOrderDocs.new.tex
	test -f ternaryOperatorDocs.new.tex || ln -s $(srcdir)/ternaryOperator/ternaryOperatorDocs.new.tex ternaryOperatorDocs.new.tex
	test -f time_tDirectManipulationDocs.new.tex || ln -s $(srcdir)/time_tDirectManipulation/time_tDirectManipulationDocs.new.tex time_tDirectManipulationDocs.new.tex
	test -f typeTypedefDocs.new.tex || ln -s $(srcdir)/typeTypedef/typeTypedefDocs.new.tex typeTypedefDocs.new.tex
	test -f unaryMinusDocs.new.tex || ln -s $(srcdir)/unaryMinus/unaryMinusDocs.new.tex unaryMinusDocs.new.tex
	test -f uninitializedDefinitionDocs.new.tex || ln -s $(srcdir)/uninitializedDefinition/uninitializedDefinitionDocs.new.tex uninitializedDefinitionDocs.new.tex
	test -f upperRangeLimitDocs.new.tex || ln -s $(srcdir)/upperRangeLimit/upperRangeLimitDocs.new.tex upperRangeLimitDocs.new.tex
	test -f variableNameEqualsDatabaseNameDocs.new.tex || ln -s $(srcdir)/variableNameEqualsDatabaseName/variableNameEqualsDatabaseNameDocs.new.tex variableNameEqualsDatabaseNameDocs.new.tex
	test -f voidStarDocs.new.tex || ln -s $(srcdir)/voidStar/voidStarDocs.new.tex voidStarDocs.new.tex
	test -f compassDocs.tex || ln -s $(srcdir)/compassSupport/compassDocs.tex compassDocs.tex
	pdflatex compass.tex
	pdflatex compass.tex

TGUI=

testAllocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstraction: compassMain
	@echo -e "\nRunning testAllocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstraction"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/allocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstraction/allocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstractionTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/allocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstraction/allocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstractionTestOutput.txt; then \
		echo "creating $(srcdir)/allocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstraction/allocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstractionTestOutput.txt" ;\
		./compassMain $(srcdir)/allocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstraction/allocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstractionTest1.C $(TGUI) &> $(srcdir)/allocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstraction/allocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstractionTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/allocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstraction/allocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstractionTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/allocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstraction/allocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstractionTestOutput.txt $(top_builddir)/projects/compass/allocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstractionTestOutput.txt.00 ;\
		grep "AllocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstraction:" $(top_builddir)/projects/compass/allocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstractionTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/allocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstraction/allocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstractionTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/allocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstractionTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/allocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstraction/allocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstractionTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/allocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstraction/allocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstractionTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/allocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstraction/allocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstractionTestOutput.txt; then \
			if test -e $(srcdir)/allocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstraction/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/allocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstraction/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/allocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstraction/allocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstractionTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/allocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstraction/allocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstractionTest1.C $(TGUI) &> $(top_builddir)/projects/compass/allocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstractionTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/allocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstractionTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/allocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstractionTestOutput.txt $(top_builddir)/projects/compass/allocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstractionTestOutput.txt.00 ;\
		grep "AllocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstraction:" $(top_builddir)/projects/compass/allocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstractionTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/allocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstractionTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/allocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstractionTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/allocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstractionTestOutput.txt || ! test -s $(srcdir)/allocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstraction/allocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstractionTestOutput.txt ; then \
			if test -e $(srcdir)/allocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstraction/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/allocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstraction/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/allocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstractionTestOutput.txt or $(srcdir)/allocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstraction/allocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstractionTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/allocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstractionTestOutput.txt $(srcdir)/allocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstraction/allocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstractionTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/allocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstractionTestOutput.txt and $(srcdir)/allocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstraction/allocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstractionTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/allocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstractionTestOutput.txt ;\
	fi
	@echo "done testallocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstraction"

testAssignmentOperatorCheckSelf: compassMain
	@echo -e "\nRunning testAssignmentOperatorCheckSelf"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/assignmentOperatorCheckSelf/assignmentOperatorCheckSelfTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/assignmentOperatorCheckSelf/assignmentOperatorCheckSelfTestOutput.txt; then \
		echo "creating $(srcdir)/assignmentOperatorCheckSelf/assignmentOperatorCheckSelfTestOutput.txt" ;\
		./compassMain $(srcdir)/assignmentOperatorCheckSelf/assignmentOperatorCheckSelfTest1.C $(TGUI) &> $(srcdir)/assignmentOperatorCheckSelf/assignmentOperatorCheckSelfTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/assignmentOperatorCheckSelf/assignmentOperatorCheckSelfTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/assignmentOperatorCheckSelf/assignmentOperatorCheckSelfTestOutput.txt $(top_builddir)/projects/compass/assignmentOperatorCheckSelfTestOutput.txt.00 ;\
		grep "AssignmentOperatorCheckSelf:" $(top_builddir)/projects/compass/assignmentOperatorCheckSelfTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/assignmentOperatorCheckSelf/assignmentOperatorCheckSelfTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/assignmentOperatorCheckSelfTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/assignmentOperatorCheckSelf/assignmentOperatorCheckSelfTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/assignmentOperatorCheckSelf/assignmentOperatorCheckSelfTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/assignmentOperatorCheckSelf/assignmentOperatorCheckSelfTestOutput.txt; then \
			if test -e $(srcdir)/assignmentOperatorCheckSelf/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/assignmentOperatorCheckSelf/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/assignmentOperatorCheckSelf/assignmentOperatorCheckSelfTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/assignmentOperatorCheckSelf/assignmentOperatorCheckSelfTest1.C $(TGUI) &> $(top_builddir)/projects/compass/assignmentOperatorCheckSelfTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/assignmentOperatorCheckSelfTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/assignmentOperatorCheckSelfTestOutput.txt $(top_builddir)/projects/compass/assignmentOperatorCheckSelfTestOutput.txt.00 ;\
		grep "AssignmentOperatorCheckSelf:" $(top_builddir)/projects/compass/assignmentOperatorCheckSelfTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/assignmentOperatorCheckSelfTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/assignmentOperatorCheckSelfTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/assignmentOperatorCheckSelfTestOutput.txt || ! test -s $(srcdir)/assignmentOperatorCheckSelf/assignmentOperatorCheckSelfTestOutput.txt ; then \
			if test -e $(srcdir)/assignmentOperatorCheckSelf/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/assignmentOperatorCheckSelf/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/assignmentOperatorCheckSelfTestOutput.txt or $(srcdir)/assignmentOperatorCheckSelf/assignmentOperatorCheckSelfTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/assignmentOperatorCheckSelfTestOutput.txt $(srcdir)/assignmentOperatorCheckSelf/assignmentOperatorCheckSelfTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/assignmentOperatorCheckSelfTestOutput.txt and $(srcdir)/assignmentOperatorCheckSelf/assignmentOperatorCheckSelfTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/assignmentOperatorCheckSelfTestOutput.txt ;\
	fi
	@echo "done testassignmentOperatorCheckSelf"

testAssignmentReturnConstThis: compassMain
	@echo -e "\nRunning testAssignmentReturnConstThis"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/assignmentReturnConstThis/assignmentReturnConstThisTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/assignmentReturnConstThis/assignmentReturnConstThisTestOutput.txt; then \
		echo "creating $(srcdir)/assignmentReturnConstThis/assignmentReturnConstThisTestOutput.txt" ;\
		./compassMain $(srcdir)/assignmentReturnConstThis/assignmentReturnConstThisTest1.C $(TGUI) &> $(srcdir)/assignmentReturnConstThis/assignmentReturnConstThisTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/assignmentReturnConstThis/assignmentReturnConstThisTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/assignmentReturnConstThis/assignmentReturnConstThisTestOutput.txt $(top_builddir)/projects/compass/assignmentReturnConstThisTestOutput.txt.00 ;\
		grep "AssignmentReturnConstThis:" $(top_builddir)/projects/compass/assignmentReturnConstThisTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/assignmentReturnConstThis/assignmentReturnConstThisTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/assignmentReturnConstThisTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/assignmentReturnConstThis/assignmentReturnConstThisTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/assignmentReturnConstThis/assignmentReturnConstThisTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/assignmentReturnConstThis/assignmentReturnConstThisTestOutput.txt; then \
			if test -e $(srcdir)/assignmentReturnConstThis/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/assignmentReturnConstThis/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/assignmentReturnConstThis/assignmentReturnConstThisTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/assignmentReturnConstThis/assignmentReturnConstThisTest1.C $(TGUI) &> $(top_builddir)/projects/compass/assignmentReturnConstThisTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/assignmentReturnConstThisTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/assignmentReturnConstThisTestOutput.txt $(top_builddir)/projects/compass/assignmentReturnConstThisTestOutput.txt.00 ;\
		grep "AssignmentReturnConstThis:" $(top_builddir)/projects/compass/assignmentReturnConstThisTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/assignmentReturnConstThisTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/assignmentReturnConstThisTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/assignmentReturnConstThisTestOutput.txt || ! test -s $(srcdir)/assignmentReturnConstThis/assignmentReturnConstThisTestOutput.txt ; then \
			if test -e $(srcdir)/assignmentReturnConstThis/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/assignmentReturnConstThis/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/assignmentReturnConstThisTestOutput.txt or $(srcdir)/assignmentReturnConstThis/assignmentReturnConstThisTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/assignmentReturnConstThisTestOutput.txt $(srcdir)/assignmentReturnConstThis/assignmentReturnConstThisTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/assignmentReturnConstThisTestOutput.txt and $(srcdir)/assignmentReturnConstThis/assignmentReturnConstThisTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/assignmentReturnConstThisTestOutput.txt ;\
	fi
	@echo "done testassignmentReturnConstThis"

testAsynchronousSignalHandler: compassMain
	@echo -e "\nRunning testAsynchronousSignalHandler"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/asynchronousSignalHandler/asynchronousSignalHandlerTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/asynchronousSignalHandler/asynchronousSignalHandlerTestOutput.txt; then \
		echo "creating $(srcdir)/asynchronousSignalHandler/asynchronousSignalHandlerTestOutput.txt" ;\
		./compassMain $(srcdir)/asynchronousSignalHandler/asynchronousSignalHandlerTest1.C $(TGUI) &> $(srcdir)/asynchronousSignalHandler/asynchronousSignalHandlerTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/asynchronousSignalHandler/asynchronousSignalHandlerTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/asynchronousSignalHandler/asynchronousSignalHandlerTestOutput.txt $(top_builddir)/projects/compass/asynchronousSignalHandlerTestOutput.txt.00 ;\
		grep "AsynchronousSignalHandler:" $(top_builddir)/projects/compass/asynchronousSignalHandlerTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/asynchronousSignalHandler/asynchronousSignalHandlerTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/asynchronousSignalHandlerTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/asynchronousSignalHandler/asynchronousSignalHandlerTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/asynchronousSignalHandler/asynchronousSignalHandlerTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/asynchronousSignalHandler/asynchronousSignalHandlerTestOutput.txt; then \
			if test -e $(srcdir)/asynchronousSignalHandler/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/asynchronousSignalHandler/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/asynchronousSignalHandler/asynchronousSignalHandlerTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/asynchronousSignalHandler/asynchronousSignalHandlerTest1.C $(TGUI) &> $(top_builddir)/projects/compass/asynchronousSignalHandlerTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/asynchronousSignalHandlerTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/asynchronousSignalHandlerTestOutput.txt $(top_builddir)/projects/compass/asynchronousSignalHandlerTestOutput.txt.00 ;\
		grep "AsynchronousSignalHandler:" $(top_builddir)/projects/compass/asynchronousSignalHandlerTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/asynchronousSignalHandlerTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/asynchronousSignalHandlerTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/asynchronousSignalHandlerTestOutput.txt || ! test -s $(srcdir)/asynchronousSignalHandler/asynchronousSignalHandlerTestOutput.txt ; then \
			if test -e $(srcdir)/asynchronousSignalHandler/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/asynchronousSignalHandler/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/asynchronousSignalHandlerTestOutput.txt or $(srcdir)/asynchronousSignalHandler/asynchronousSignalHandlerTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/asynchronousSignalHandlerTestOutput.txt $(srcdir)/asynchronousSignalHandler/asynchronousSignalHandlerTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/asynchronousSignalHandlerTestOutput.txt and $(srcdir)/asynchronousSignalHandler/asynchronousSignalHandlerTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/asynchronousSignalHandlerTestOutput.txt ;\
	fi
	@echo "done testasynchronousSignalHandler"

testAvoidUsingTheSameHandlerForMultipleSignals: compassMain
	@echo -e "\nRunning testAvoidUsingTheSameHandlerForMultipleSignals"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/avoidUsingTheSameHandlerForMultipleSignals/avoidUsingTheSameHandlerForMultipleSignalsTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/avoidUsingTheSameHandlerForMultipleSignals/avoidUsingTheSameHandlerForMultipleSignalsTestOutput.txt; then \
		echo "creating $(srcdir)/avoidUsingTheSameHandlerForMultipleSignals/avoidUsingTheSameHandlerForMultipleSignalsTestOutput.txt" ;\
		./compassMain $(srcdir)/avoidUsingTheSameHandlerForMultipleSignals/avoidUsingTheSameHandlerForMultipleSignalsTest1.C $(TGUI) &> $(srcdir)/avoidUsingTheSameHandlerForMultipleSignals/avoidUsingTheSameHandlerForMultipleSignalsTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/avoidUsingTheSameHandlerForMultipleSignals/avoidUsingTheSameHandlerForMultipleSignalsTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/avoidUsingTheSameHandlerForMultipleSignals/avoidUsingTheSameHandlerForMultipleSignalsTestOutput.txt $(top_builddir)/projects/compass/avoidUsingTheSameHandlerForMultipleSignalsTestOutput.txt.00 ;\
		grep "AvoidUsingTheSameHandlerForMultipleSignals:" $(top_builddir)/projects/compass/avoidUsingTheSameHandlerForMultipleSignalsTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/avoidUsingTheSameHandlerForMultipleSignals/avoidUsingTheSameHandlerForMultipleSignalsTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/avoidUsingTheSameHandlerForMultipleSignalsTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/avoidUsingTheSameHandlerForMultipleSignals/avoidUsingTheSameHandlerForMultipleSignalsTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/avoidUsingTheSameHandlerForMultipleSignals/avoidUsingTheSameHandlerForMultipleSignalsTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/avoidUsingTheSameHandlerForMultipleSignals/avoidUsingTheSameHandlerForMultipleSignalsTestOutput.txt; then \
			if test -e $(srcdir)/avoidUsingTheSameHandlerForMultipleSignals/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/avoidUsingTheSameHandlerForMultipleSignals/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/avoidUsingTheSameHandlerForMultipleSignals/avoidUsingTheSameHandlerForMultipleSignalsTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/avoidUsingTheSameHandlerForMultipleSignals/avoidUsingTheSameHandlerForMultipleSignalsTest1.C $(TGUI) &> $(top_builddir)/projects/compass/avoidUsingTheSameHandlerForMultipleSignalsTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/avoidUsingTheSameHandlerForMultipleSignalsTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/avoidUsingTheSameHandlerForMultipleSignalsTestOutput.txt $(top_builddir)/projects/compass/avoidUsingTheSameHandlerForMultipleSignalsTestOutput.txt.00 ;\
		grep "AvoidUsingTheSameHandlerForMultipleSignals:" $(top_builddir)/projects/compass/avoidUsingTheSameHandlerForMultipleSignalsTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/avoidUsingTheSameHandlerForMultipleSignalsTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/avoidUsingTheSameHandlerForMultipleSignalsTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/avoidUsingTheSameHandlerForMultipleSignalsTestOutput.txt || ! test -s $(srcdir)/avoidUsingTheSameHandlerForMultipleSignals/avoidUsingTheSameHandlerForMultipleSignalsTestOutput.txt ; then \
			if test -e $(srcdir)/avoidUsingTheSameHandlerForMultipleSignals/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/avoidUsingTheSameHandlerForMultipleSignals/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/avoidUsingTheSameHandlerForMultipleSignalsTestOutput.txt or $(srcdir)/avoidUsingTheSameHandlerForMultipleSignals/avoidUsingTheSameHandlerForMultipleSignalsTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/avoidUsingTheSameHandlerForMultipleSignalsTestOutput.txt $(srcdir)/avoidUsingTheSameHandlerForMultipleSignals/avoidUsingTheSameHandlerForMultipleSignalsTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/avoidUsingTheSameHandlerForMultipleSignalsTestOutput.txt and $(srcdir)/avoidUsingTheSameHandlerForMultipleSignals/avoidUsingTheSameHandlerForMultipleSignalsTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/avoidUsingTheSameHandlerForMultipleSignalsTestOutput.txt ;\
	fi
	@echo "done testavoidUsingTheSameHandlerForMultipleSignals"

testBooleanIsHas: compassMain
	@echo -e "\nRunning testBooleanIsHas"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/booleanIsHas/booleanIsHasTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/booleanIsHas/booleanIsHasTestOutput.txt; then \
		echo "creating $(srcdir)/booleanIsHas/booleanIsHasTestOutput.txt" ;\
		./compassMain $(srcdir)/booleanIsHas/booleanIsHasTest1.C $(TGUI) &> $(srcdir)/booleanIsHas/booleanIsHasTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/booleanIsHas/booleanIsHasTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/booleanIsHas/booleanIsHasTestOutput.txt $(top_builddir)/projects/compass/booleanIsHasTestOutput.txt.00 ;\
		grep "BooleanIsHas:" $(top_builddir)/projects/compass/booleanIsHasTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/booleanIsHas/booleanIsHasTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/booleanIsHasTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/booleanIsHas/booleanIsHasTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/booleanIsHas/booleanIsHasTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/booleanIsHas/booleanIsHasTestOutput.txt; then \
			if test -e $(srcdir)/booleanIsHas/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/booleanIsHas/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/booleanIsHas/booleanIsHasTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/booleanIsHas/booleanIsHasTest1.C $(TGUI) &> $(top_builddir)/projects/compass/booleanIsHasTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/booleanIsHasTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/booleanIsHasTestOutput.txt $(top_builddir)/projects/compass/booleanIsHasTestOutput.txt.00 ;\
		grep "BooleanIsHas:" $(top_builddir)/projects/compass/booleanIsHasTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/booleanIsHasTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/booleanIsHasTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/booleanIsHasTestOutput.txt || ! test -s $(srcdir)/booleanIsHas/booleanIsHasTestOutput.txt ; then \
			if test -e $(srcdir)/booleanIsHas/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/booleanIsHas/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/booleanIsHasTestOutput.txt or $(srcdir)/booleanIsHas/booleanIsHasTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/booleanIsHasTestOutput.txt $(srcdir)/booleanIsHas/booleanIsHasTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/booleanIsHasTestOutput.txt and $(srcdir)/booleanIsHas/booleanIsHasTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/booleanIsHasTestOutput.txt ;\
	fi
	@echo "done testbooleanIsHas"

testBufferOverflowFunctions: compassMain
	@echo -e "\nRunning testBufferOverflowFunctions"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/bufferOverflowFunctions/bufferOverflowFunctionsTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/bufferOverflowFunctions/bufferOverflowFunctionsTestOutput.txt; then \
		echo "creating $(srcdir)/bufferOverflowFunctions/bufferOverflowFunctionsTestOutput.txt" ;\
		./compassMain $(srcdir)/bufferOverflowFunctions/bufferOverflowFunctionsTest1.C $(TGUI) &> $(srcdir)/bufferOverflowFunctions/bufferOverflowFunctionsTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/bufferOverflowFunctions/bufferOverflowFunctionsTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/bufferOverflowFunctions/bufferOverflowFunctionsTestOutput.txt $(top_builddir)/projects/compass/bufferOverflowFunctionsTestOutput.txt.00 ;\
		grep "BufferOverflowFunctions:" $(top_builddir)/projects/compass/bufferOverflowFunctionsTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/bufferOverflowFunctions/bufferOverflowFunctionsTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/bufferOverflowFunctionsTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/bufferOverflowFunctions/bufferOverflowFunctionsTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/bufferOverflowFunctions/bufferOverflowFunctionsTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/bufferOverflowFunctions/bufferOverflowFunctionsTestOutput.txt; then \
			if test -e $(srcdir)/bufferOverflowFunctions/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/bufferOverflowFunctions/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/bufferOverflowFunctions/bufferOverflowFunctionsTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/bufferOverflowFunctions/bufferOverflowFunctionsTest1.C $(TGUI) &> $(top_builddir)/projects/compass/bufferOverflowFunctionsTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/bufferOverflowFunctionsTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/bufferOverflowFunctionsTestOutput.txt $(top_builddir)/projects/compass/bufferOverflowFunctionsTestOutput.txt.00 ;\
		grep "BufferOverflowFunctions:" $(top_builddir)/projects/compass/bufferOverflowFunctionsTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/bufferOverflowFunctionsTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/bufferOverflowFunctionsTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/bufferOverflowFunctionsTestOutput.txt || ! test -s $(srcdir)/bufferOverflowFunctions/bufferOverflowFunctionsTestOutput.txt ; then \
			if test -e $(srcdir)/bufferOverflowFunctions/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/bufferOverflowFunctions/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/bufferOverflowFunctionsTestOutput.txt or $(srcdir)/bufferOverflowFunctions/bufferOverflowFunctionsTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/bufferOverflowFunctionsTestOutput.txt $(srcdir)/bufferOverflowFunctions/bufferOverflowFunctionsTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/bufferOverflowFunctionsTestOutput.txt and $(srcdir)/bufferOverflowFunctions/bufferOverflowFunctionsTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/bufferOverflowFunctionsTestOutput.txt ;\
	fi
	@echo "done testbufferOverflowFunctions"

testByteByByteStructureComparison: compassMain
	@echo -e "\nRunning testByteByByteStructureComparison"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/byteByByteStructureComparison/byteByByteStructureComparisonTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/byteByByteStructureComparison/byteByByteStructureComparisonTestOutput.txt; then \
		echo "creating $(srcdir)/byteByByteStructureComparison/byteByByteStructureComparisonTestOutput.txt" ;\
		./compassMain $(srcdir)/byteByByteStructureComparison/byteByByteStructureComparisonTest1.C $(TGUI) &> $(srcdir)/byteByByteStructureComparison/byteByByteStructureComparisonTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/byteByByteStructureComparison/byteByByteStructureComparisonTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/byteByByteStructureComparison/byteByByteStructureComparisonTestOutput.txt $(top_builddir)/projects/compass/byteByByteStructureComparisonTestOutput.txt.00 ;\
		grep "ByteByByteStructureComparison:" $(top_builddir)/projects/compass/byteByByteStructureComparisonTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/byteByByteStructureComparison/byteByByteStructureComparisonTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/byteByByteStructureComparisonTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/byteByByteStructureComparison/byteByByteStructureComparisonTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/byteByByteStructureComparison/byteByByteStructureComparisonTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/byteByByteStructureComparison/byteByByteStructureComparisonTestOutput.txt; then \
			if test -e $(srcdir)/byteByByteStructureComparison/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/byteByByteStructureComparison/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/byteByByteStructureComparison/byteByByteStructureComparisonTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/byteByByteStructureComparison/byteByByteStructureComparisonTest1.C $(TGUI) &> $(top_builddir)/projects/compass/byteByByteStructureComparisonTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/byteByByteStructureComparisonTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/byteByByteStructureComparisonTestOutput.txt $(top_builddir)/projects/compass/byteByByteStructureComparisonTestOutput.txt.00 ;\
		grep "ByteByByteStructureComparison:" $(top_builddir)/projects/compass/byteByByteStructureComparisonTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/byteByByteStructureComparisonTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/byteByByteStructureComparisonTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/byteByByteStructureComparisonTestOutput.txt || ! test -s $(srcdir)/byteByByteStructureComparison/byteByByteStructureComparisonTestOutput.txt ; then \
			if test -e $(srcdir)/byteByByteStructureComparison/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/byteByByteStructureComparison/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/byteByByteStructureComparisonTestOutput.txt or $(srcdir)/byteByByteStructureComparison/byteByByteStructureComparisonTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/byteByByteStructureComparisonTestOutput.txt $(srcdir)/byteByByteStructureComparison/byteByByteStructureComparisonTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/byteByByteStructureComparisonTestOutput.txt and $(srcdir)/byteByByteStructureComparison/byteByByteStructureComparisonTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/byteByByteStructureComparisonTestOutput.txt ;\
	fi
	@echo "done testbyteByByteStructureComparison"

testCharStarForString: compassMain
	@echo -e "\nRunning testCharStarForString"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/charStarForString/charStarForStringTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/charStarForString/charStarForStringTestOutput.txt; then \
		echo "creating $(srcdir)/charStarForString/charStarForStringTestOutput.txt" ;\
		./compassMain $(srcdir)/charStarForString/charStarForStringTest1.C $(TGUI) &> $(srcdir)/charStarForString/charStarForStringTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/charStarForString/charStarForStringTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/charStarForString/charStarForStringTestOutput.txt $(top_builddir)/projects/compass/charStarForStringTestOutput.txt.00 ;\
		grep "CharStarForString:" $(top_builddir)/projects/compass/charStarForStringTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/charStarForString/charStarForStringTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/charStarForStringTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/charStarForString/charStarForStringTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/charStarForString/charStarForStringTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/charStarForString/charStarForStringTestOutput.txt; then \
			if test -e $(srcdir)/charStarForString/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/charStarForString/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/charStarForString/charStarForStringTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/charStarForString/charStarForStringTest1.C $(TGUI) &> $(top_builddir)/projects/compass/charStarForStringTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/charStarForStringTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/charStarForStringTestOutput.txt $(top_builddir)/projects/compass/charStarForStringTestOutput.txt.00 ;\
		grep "CharStarForString:" $(top_builddir)/projects/compass/charStarForStringTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/charStarForStringTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/charStarForStringTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/charStarForStringTestOutput.txt || ! test -s $(srcdir)/charStarForString/charStarForStringTestOutput.txt ; then \
			if test -e $(srcdir)/charStarForString/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/charStarForString/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/charStarForStringTestOutput.txt or $(srcdir)/charStarForString/charStarForStringTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/charStarForStringTestOutput.txt $(srcdir)/charStarForString/charStarForStringTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/charStarForStringTestOutput.txt and $(srcdir)/charStarForString/charStarForStringTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/charStarForStringTestOutput.txt ;\
	fi
	@echo "done testcharStarForString"

testCommaOperator: compassMain
	@echo -e "\nRunning testCommaOperator"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/commaOperator/commaOperatorTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/commaOperator/commaOperatorTestOutput.txt; then \
		echo "creating $(srcdir)/commaOperator/commaOperatorTestOutput.txt" ;\
		./compassMain $(srcdir)/commaOperator/commaOperatorTest1.C $(TGUI) &> $(srcdir)/commaOperator/commaOperatorTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/commaOperator/commaOperatorTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/commaOperator/commaOperatorTestOutput.txt $(top_builddir)/projects/compass/commaOperatorTestOutput.txt.00 ;\
		grep "CommaOperator:" $(top_builddir)/projects/compass/commaOperatorTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/commaOperator/commaOperatorTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/commaOperatorTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/commaOperator/commaOperatorTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/commaOperator/commaOperatorTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/commaOperator/commaOperatorTestOutput.txt; then \
			if test -e $(srcdir)/commaOperator/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/commaOperator/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/commaOperator/commaOperatorTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/commaOperator/commaOperatorTest1.C $(TGUI) &> $(top_builddir)/projects/compass/commaOperatorTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/commaOperatorTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/commaOperatorTestOutput.txt $(top_builddir)/projects/compass/commaOperatorTestOutput.txt.00 ;\
		grep "CommaOperator:" $(top_builddir)/projects/compass/commaOperatorTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/commaOperatorTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/commaOperatorTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/commaOperatorTestOutput.txt || ! test -s $(srcdir)/commaOperator/commaOperatorTestOutput.txt ; then \
			if test -e $(srcdir)/commaOperator/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/commaOperator/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/commaOperatorTestOutput.txt or $(srcdir)/commaOperator/commaOperatorTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/commaOperatorTestOutput.txt $(srcdir)/commaOperator/commaOperatorTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/commaOperatorTestOutput.txt and $(srcdir)/commaOperator/commaOperatorTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/commaOperatorTestOutput.txt ;\
	fi
	@echo "done testcommaOperator"

testComputationalFunctions: compassMain
	@echo -e "\nRunning testComputationalFunctions"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/computationalFunctions/computationalFunctionsTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/computationalFunctions/computationalFunctionsTestOutput.txt; then \
		echo "creating $(srcdir)/computationalFunctions/computationalFunctionsTestOutput.txt" ;\
		./compassMain $(srcdir)/computationalFunctions/computationalFunctionsTest1.C $(TGUI) &> $(srcdir)/computationalFunctions/computationalFunctionsTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/computationalFunctions/computationalFunctionsTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/computationalFunctions/computationalFunctionsTestOutput.txt $(top_builddir)/projects/compass/computationalFunctionsTestOutput.txt.00 ;\
		grep "ComputationalFunctions:" $(top_builddir)/projects/compass/computationalFunctionsTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/computationalFunctions/computationalFunctionsTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/computationalFunctionsTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/computationalFunctions/computationalFunctionsTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/computationalFunctions/computationalFunctionsTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/computationalFunctions/computationalFunctionsTestOutput.txt; then \
			if test -e $(srcdir)/computationalFunctions/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/computationalFunctions/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/computationalFunctions/computationalFunctionsTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/computationalFunctions/computationalFunctionsTest1.C $(TGUI) &> $(top_builddir)/projects/compass/computationalFunctionsTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/computationalFunctionsTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/computationalFunctionsTestOutput.txt $(top_builddir)/projects/compass/computationalFunctionsTestOutput.txt.00 ;\
		grep "ComputationalFunctions:" $(top_builddir)/projects/compass/computationalFunctionsTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/computationalFunctionsTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/computationalFunctionsTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/computationalFunctionsTestOutput.txt || ! test -s $(srcdir)/computationalFunctions/computationalFunctionsTestOutput.txt ; then \
			if test -e $(srcdir)/computationalFunctions/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/computationalFunctions/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/computationalFunctionsTestOutput.txt or $(srcdir)/computationalFunctions/computationalFunctionsTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/computationalFunctionsTestOutput.txt $(srcdir)/computationalFunctions/computationalFunctionsTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/computationalFunctionsTestOutput.txt and $(srcdir)/computationalFunctions/computationalFunctionsTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/computationalFunctionsTestOutput.txt ;\
	fi
	@echo "done testcomputationalFunctions"

testConstCast: compassMain
	@echo -e "\nRunning testConstCast"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/constCast/constCastTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/constCast/constCastTestOutput.txt; then \
		echo "creating $(srcdir)/constCast/constCastTestOutput.txt" ;\
		./compassMain $(srcdir)/constCast/constCastTest1.C $(TGUI) &> $(srcdir)/constCast/constCastTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/constCast/constCastTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/constCast/constCastTestOutput.txt $(top_builddir)/projects/compass/constCastTestOutput.txt.00 ;\
		grep "ConstCast:" $(top_builddir)/projects/compass/constCastTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/constCast/constCastTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/constCastTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/constCast/constCastTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/constCast/constCastTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/constCast/constCastTestOutput.txt; then \
			if test -e $(srcdir)/constCast/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/constCast/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/constCast/constCastTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/constCast/constCastTest1.C $(TGUI) &> $(top_builddir)/projects/compass/constCastTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/constCastTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/constCastTestOutput.txt $(top_builddir)/projects/compass/constCastTestOutput.txt.00 ;\
		grep "ConstCast:" $(top_builddir)/projects/compass/constCastTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/constCastTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/constCastTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/constCastTestOutput.txt || ! test -s $(srcdir)/constCast/constCastTestOutput.txt ; then \
			if test -e $(srcdir)/constCast/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/constCast/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/constCastTestOutput.txt or $(srcdir)/constCast/constCastTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/constCastTestOutput.txt $(srcdir)/constCast/constCastTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/constCastTestOutput.txt and $(srcdir)/constCast/constCastTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/constCastTestOutput.txt ;\
	fi
	@echo "done testconstCast"

testConstructorDestructorCallsVirtualFunction: compassMain
	@echo -e "\nRunning testConstructorDestructorCallsVirtualFunction"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/constructorDestructorCallsVirtualFunction/constructorDestructorCallsVirtualFunctionTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/constructorDestructorCallsVirtualFunction/constructorDestructorCallsVirtualFunctionTestOutput.txt; then \
		echo "creating $(srcdir)/constructorDestructorCallsVirtualFunction/constructorDestructorCallsVirtualFunctionTestOutput.txt" ;\
		./compassMain $(srcdir)/constructorDestructorCallsVirtualFunction/constructorDestructorCallsVirtualFunctionTest1.C $(TGUI) &> $(srcdir)/constructorDestructorCallsVirtualFunction/constructorDestructorCallsVirtualFunctionTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/constructorDestructorCallsVirtualFunction/constructorDestructorCallsVirtualFunctionTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/constructorDestructorCallsVirtualFunction/constructorDestructorCallsVirtualFunctionTestOutput.txt $(top_builddir)/projects/compass/constructorDestructorCallsVirtualFunctionTestOutput.txt.00 ;\
		grep "ConstructorDestructorCallsVirtualFunction:" $(top_builddir)/projects/compass/constructorDestructorCallsVirtualFunctionTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/constructorDestructorCallsVirtualFunction/constructorDestructorCallsVirtualFunctionTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/constructorDestructorCallsVirtualFunctionTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/constructorDestructorCallsVirtualFunction/constructorDestructorCallsVirtualFunctionTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/constructorDestructorCallsVirtualFunction/constructorDestructorCallsVirtualFunctionTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/constructorDestructorCallsVirtualFunction/constructorDestructorCallsVirtualFunctionTestOutput.txt; then \
			if test -e $(srcdir)/constructorDestructorCallsVirtualFunction/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/constructorDestructorCallsVirtualFunction/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/constructorDestructorCallsVirtualFunction/constructorDestructorCallsVirtualFunctionTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/constructorDestructorCallsVirtualFunction/constructorDestructorCallsVirtualFunctionTest1.C $(TGUI) &> $(top_builddir)/projects/compass/constructorDestructorCallsVirtualFunctionTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/constructorDestructorCallsVirtualFunctionTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/constructorDestructorCallsVirtualFunctionTestOutput.txt $(top_builddir)/projects/compass/constructorDestructorCallsVirtualFunctionTestOutput.txt.00 ;\
		grep "ConstructorDestructorCallsVirtualFunction:" $(top_builddir)/projects/compass/constructorDestructorCallsVirtualFunctionTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/constructorDestructorCallsVirtualFunctionTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/constructorDestructorCallsVirtualFunctionTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/constructorDestructorCallsVirtualFunctionTestOutput.txt || ! test -s $(srcdir)/constructorDestructorCallsVirtualFunction/constructorDestructorCallsVirtualFunctionTestOutput.txt ; then \
			if test -e $(srcdir)/constructorDestructorCallsVirtualFunction/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/constructorDestructorCallsVirtualFunction/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/constructorDestructorCallsVirtualFunctionTestOutput.txt or $(srcdir)/constructorDestructorCallsVirtualFunction/constructorDestructorCallsVirtualFunctionTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/constructorDestructorCallsVirtualFunctionTestOutput.txt $(srcdir)/constructorDestructorCallsVirtualFunction/constructorDestructorCallsVirtualFunctionTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/constructorDestructorCallsVirtualFunctionTestOutput.txt and $(srcdir)/constructorDestructorCallsVirtualFunction/constructorDestructorCallsVirtualFunctionTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/constructorDestructorCallsVirtualFunctionTestOutput.txt ;\
	fi
	@echo "done testconstructorDestructorCallsVirtualFunction"

testConstStringLiterals: compassMain
	@echo -e "\nRunning testConstStringLiterals"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/constStringLiterals/constStringLiteralsTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/constStringLiterals/constStringLiteralsTestOutput.txt; then \
		echo "creating $(srcdir)/constStringLiterals/constStringLiteralsTestOutput.txt" ;\
		./compassMain $(srcdir)/constStringLiterals/constStringLiteralsTest1.C $(TGUI) &> $(srcdir)/constStringLiterals/constStringLiteralsTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/constStringLiterals/constStringLiteralsTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/constStringLiterals/constStringLiteralsTestOutput.txt $(top_builddir)/projects/compass/constStringLiteralsTestOutput.txt.00 ;\
		grep "ConstStringLiterals:" $(top_builddir)/projects/compass/constStringLiteralsTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/constStringLiterals/constStringLiteralsTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/constStringLiteralsTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/constStringLiterals/constStringLiteralsTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/constStringLiterals/constStringLiteralsTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/constStringLiterals/constStringLiteralsTestOutput.txt; then \
			if test -e $(srcdir)/constStringLiterals/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/constStringLiterals/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/constStringLiterals/constStringLiteralsTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/constStringLiterals/constStringLiteralsTest1.C $(TGUI) &> $(top_builddir)/projects/compass/constStringLiteralsTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/constStringLiteralsTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/constStringLiteralsTestOutput.txt $(top_builddir)/projects/compass/constStringLiteralsTestOutput.txt.00 ;\
		grep "ConstStringLiterals:" $(top_builddir)/projects/compass/constStringLiteralsTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/constStringLiteralsTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/constStringLiteralsTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/constStringLiteralsTestOutput.txt || ! test -s $(srcdir)/constStringLiterals/constStringLiteralsTestOutput.txt ; then \
			if test -e $(srcdir)/constStringLiterals/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/constStringLiterals/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/constStringLiteralsTestOutput.txt or $(srcdir)/constStringLiterals/constStringLiteralsTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/constStringLiteralsTestOutput.txt $(srcdir)/constStringLiterals/constStringLiteralsTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/constStringLiteralsTestOutput.txt and $(srcdir)/constStringLiterals/constStringLiteralsTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/constStringLiteralsTestOutput.txt ;\
	fi
	@echo "done testconstStringLiterals"

testControlVariableTestAgainstFunction: compassMain
	@echo -e "\nRunning testControlVariableTestAgainstFunction"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/controlVariableTestAgainstFunction/controlVariableTestAgainstFunctionTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/controlVariableTestAgainstFunction/controlVariableTestAgainstFunctionTestOutput.txt; then \
		echo "creating $(srcdir)/controlVariableTestAgainstFunction/controlVariableTestAgainstFunctionTestOutput.txt" ;\
		./compassMain $(srcdir)/controlVariableTestAgainstFunction/controlVariableTestAgainstFunctionTest1.C $(TGUI) &> $(srcdir)/controlVariableTestAgainstFunction/controlVariableTestAgainstFunctionTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/controlVariableTestAgainstFunction/controlVariableTestAgainstFunctionTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/controlVariableTestAgainstFunction/controlVariableTestAgainstFunctionTestOutput.txt $(top_builddir)/projects/compass/controlVariableTestAgainstFunctionTestOutput.txt.00 ;\
		grep "ControlVariableTestAgainstFunction:" $(top_builddir)/projects/compass/controlVariableTestAgainstFunctionTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/controlVariableTestAgainstFunction/controlVariableTestAgainstFunctionTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/controlVariableTestAgainstFunctionTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/controlVariableTestAgainstFunction/controlVariableTestAgainstFunctionTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/controlVariableTestAgainstFunction/controlVariableTestAgainstFunctionTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/controlVariableTestAgainstFunction/controlVariableTestAgainstFunctionTestOutput.txt; then \
			if test -e $(srcdir)/controlVariableTestAgainstFunction/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/controlVariableTestAgainstFunction/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/controlVariableTestAgainstFunction/controlVariableTestAgainstFunctionTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/controlVariableTestAgainstFunction/controlVariableTestAgainstFunctionTest1.C $(TGUI) &> $(top_builddir)/projects/compass/controlVariableTestAgainstFunctionTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/controlVariableTestAgainstFunctionTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/controlVariableTestAgainstFunctionTestOutput.txt $(top_builddir)/projects/compass/controlVariableTestAgainstFunctionTestOutput.txt.00 ;\
		grep "ControlVariableTestAgainstFunction:" $(top_builddir)/projects/compass/controlVariableTestAgainstFunctionTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/controlVariableTestAgainstFunctionTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/controlVariableTestAgainstFunctionTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/controlVariableTestAgainstFunctionTestOutput.txt || ! test -s $(srcdir)/controlVariableTestAgainstFunction/controlVariableTestAgainstFunctionTestOutput.txt ; then \
			if test -e $(srcdir)/controlVariableTestAgainstFunction/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/controlVariableTestAgainstFunction/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/controlVariableTestAgainstFunctionTestOutput.txt or $(srcdir)/controlVariableTestAgainstFunction/controlVariableTestAgainstFunctionTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/controlVariableTestAgainstFunctionTestOutput.txt $(srcdir)/controlVariableTestAgainstFunction/controlVariableTestAgainstFunctionTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/controlVariableTestAgainstFunctionTestOutput.txt and $(srcdir)/controlVariableTestAgainstFunction/controlVariableTestAgainstFunctionTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/controlVariableTestAgainstFunctionTestOutput.txt ;\
	fi
	@echo "done testcontrolVariableTestAgainstFunction"

testCopyConstructorConstArg: compassMain
	@echo -e "\nRunning testCopyConstructorConstArg"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/copyConstructorConstArg/copyConstructorConstArgTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/copyConstructorConstArg/copyConstructorConstArgTestOutput.txt; then \
		echo "creating $(srcdir)/copyConstructorConstArg/copyConstructorConstArgTestOutput.txt" ;\
		./compassMain $(srcdir)/copyConstructorConstArg/copyConstructorConstArgTest1.C $(TGUI) &> $(srcdir)/copyConstructorConstArg/copyConstructorConstArgTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/copyConstructorConstArg/copyConstructorConstArgTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/copyConstructorConstArg/copyConstructorConstArgTestOutput.txt $(top_builddir)/projects/compass/copyConstructorConstArgTestOutput.txt.00 ;\
		grep "CopyConstructorConstArg:" $(top_builddir)/projects/compass/copyConstructorConstArgTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/copyConstructorConstArg/copyConstructorConstArgTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/copyConstructorConstArgTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/copyConstructorConstArg/copyConstructorConstArgTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/copyConstructorConstArg/copyConstructorConstArgTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/copyConstructorConstArg/copyConstructorConstArgTestOutput.txt; then \
			if test -e $(srcdir)/copyConstructorConstArg/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/copyConstructorConstArg/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/copyConstructorConstArg/copyConstructorConstArgTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/copyConstructorConstArg/copyConstructorConstArgTest1.C $(TGUI) &> $(top_builddir)/projects/compass/copyConstructorConstArgTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/copyConstructorConstArgTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/copyConstructorConstArgTestOutput.txt $(top_builddir)/projects/compass/copyConstructorConstArgTestOutput.txt.00 ;\
		grep "CopyConstructorConstArg:" $(top_builddir)/projects/compass/copyConstructorConstArgTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/copyConstructorConstArgTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/copyConstructorConstArgTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/copyConstructorConstArgTestOutput.txt || ! test -s $(srcdir)/copyConstructorConstArg/copyConstructorConstArgTestOutput.txt ; then \
			if test -e $(srcdir)/copyConstructorConstArg/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/copyConstructorConstArg/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/copyConstructorConstArgTestOutput.txt or $(srcdir)/copyConstructorConstArg/copyConstructorConstArgTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/copyConstructorConstArgTestOutput.txt $(srcdir)/copyConstructorConstArg/copyConstructorConstArgTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/copyConstructorConstArgTestOutput.txt and $(srcdir)/copyConstructorConstArg/copyConstructorConstArgTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/copyConstructorConstArgTestOutput.txt ;\
	fi
	@echo "done testcopyConstructorConstArg"

testCppCallsSetjmpLongjmp: compassMain
	@echo -e "\nRunning testCppCallsSetjmpLongjmp"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/cppCallsSetjmpLongjmp/cppCallsSetjmpLongjmpTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/cppCallsSetjmpLongjmp/cppCallsSetjmpLongjmpTestOutput.txt; then \
		echo "creating $(srcdir)/cppCallsSetjmpLongjmp/cppCallsSetjmpLongjmpTestOutput.txt" ;\
		./compassMain $(srcdir)/cppCallsSetjmpLongjmp/cppCallsSetjmpLongjmpTest1.C $(TGUI) &> $(srcdir)/cppCallsSetjmpLongjmp/cppCallsSetjmpLongjmpTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/cppCallsSetjmpLongjmp/cppCallsSetjmpLongjmpTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/cppCallsSetjmpLongjmp/cppCallsSetjmpLongjmpTestOutput.txt $(top_builddir)/projects/compass/cppCallsSetjmpLongjmpTestOutput.txt.00 ;\
		grep "CppCallsSetjmpLongjmp:" $(top_builddir)/projects/compass/cppCallsSetjmpLongjmpTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/cppCallsSetjmpLongjmp/cppCallsSetjmpLongjmpTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/cppCallsSetjmpLongjmpTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/cppCallsSetjmpLongjmp/cppCallsSetjmpLongjmpTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/cppCallsSetjmpLongjmp/cppCallsSetjmpLongjmpTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/cppCallsSetjmpLongjmp/cppCallsSetjmpLongjmpTestOutput.txt; then \
			if test -e $(srcdir)/cppCallsSetjmpLongjmp/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/cppCallsSetjmpLongjmp/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/cppCallsSetjmpLongjmp/cppCallsSetjmpLongjmpTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/cppCallsSetjmpLongjmp/cppCallsSetjmpLongjmpTest1.C $(TGUI) &> $(top_builddir)/projects/compass/cppCallsSetjmpLongjmpTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/cppCallsSetjmpLongjmpTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/cppCallsSetjmpLongjmpTestOutput.txt $(top_builddir)/projects/compass/cppCallsSetjmpLongjmpTestOutput.txt.00 ;\
		grep "CppCallsSetjmpLongjmp:" $(top_builddir)/projects/compass/cppCallsSetjmpLongjmpTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/cppCallsSetjmpLongjmpTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/cppCallsSetjmpLongjmpTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/cppCallsSetjmpLongjmpTestOutput.txt || ! test -s $(srcdir)/cppCallsSetjmpLongjmp/cppCallsSetjmpLongjmpTestOutput.txt ; then \
			if test -e $(srcdir)/cppCallsSetjmpLongjmp/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/cppCallsSetjmpLongjmp/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/cppCallsSetjmpLongjmpTestOutput.txt or $(srcdir)/cppCallsSetjmpLongjmp/cppCallsSetjmpLongjmpTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/cppCallsSetjmpLongjmpTestOutput.txt $(srcdir)/cppCallsSetjmpLongjmp/cppCallsSetjmpLongjmpTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/cppCallsSetjmpLongjmpTestOutput.txt and $(srcdir)/cppCallsSetjmpLongjmp/cppCallsSetjmpLongjmpTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/cppCallsSetjmpLongjmpTestOutput.txt ;\
	fi
	@echo "done testcppCallsSetjmpLongjmp"

testCyclomaticComplexity: compassMain
	@echo -e "\nRunning testCyclomaticComplexity"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/cyclomaticComplexity/cyclomaticComplexityTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/cyclomaticComplexity/cyclomaticComplexityTestOutput.txt; then \
		echo "creating $(srcdir)/cyclomaticComplexity/cyclomaticComplexityTestOutput.txt" ;\
		./compassMain $(srcdir)/cyclomaticComplexity/cyclomaticComplexityTest1.C $(TGUI) &> $(srcdir)/cyclomaticComplexity/cyclomaticComplexityTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/cyclomaticComplexity/cyclomaticComplexityTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/cyclomaticComplexity/cyclomaticComplexityTestOutput.txt $(top_builddir)/projects/compass/cyclomaticComplexityTestOutput.txt.00 ;\
		grep "CyclomaticComplexity:" $(top_builddir)/projects/compass/cyclomaticComplexityTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/cyclomaticComplexity/cyclomaticComplexityTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/cyclomaticComplexityTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/cyclomaticComplexity/cyclomaticComplexityTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/cyclomaticComplexity/cyclomaticComplexityTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/cyclomaticComplexity/cyclomaticComplexityTestOutput.txt; then \
			if test -e $(srcdir)/cyclomaticComplexity/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/cyclomaticComplexity/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/cyclomaticComplexity/cyclomaticComplexityTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/cyclomaticComplexity/cyclomaticComplexityTest1.C $(TGUI) &> $(top_builddir)/projects/compass/cyclomaticComplexityTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/cyclomaticComplexityTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/cyclomaticComplexityTestOutput.txt $(top_builddir)/projects/compass/cyclomaticComplexityTestOutput.txt.00 ;\
		grep "CyclomaticComplexity:" $(top_builddir)/projects/compass/cyclomaticComplexityTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/cyclomaticComplexityTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/cyclomaticComplexityTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/cyclomaticComplexityTestOutput.txt || ! test -s $(srcdir)/cyclomaticComplexity/cyclomaticComplexityTestOutput.txt ; then \
			if test -e $(srcdir)/cyclomaticComplexity/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/cyclomaticComplexity/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/cyclomaticComplexityTestOutput.txt or $(srcdir)/cyclomaticComplexity/cyclomaticComplexityTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/cyclomaticComplexityTestOutput.txt $(srcdir)/cyclomaticComplexity/cyclomaticComplexityTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/cyclomaticComplexityTestOutput.txt and $(srcdir)/cyclomaticComplexity/cyclomaticComplexityTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/cyclomaticComplexityTestOutput.txt ;\
	fi
	@echo "done testcyclomaticComplexity"

testDataMemberAccess: compassMain
	@echo -e "\nRunning testDataMemberAccess"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/dataMemberAccess/dataMemberAccessTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/dataMemberAccess/dataMemberAccessTestOutput.txt; then \
		echo "creating $(srcdir)/dataMemberAccess/dataMemberAccessTestOutput.txt" ;\
		./compassMain $(srcdir)/dataMemberAccess/dataMemberAccessTest1.C $(TGUI) &> $(srcdir)/dataMemberAccess/dataMemberAccessTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/dataMemberAccess/dataMemberAccessTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/dataMemberAccess/dataMemberAccessTestOutput.txt $(top_builddir)/projects/compass/dataMemberAccessTestOutput.txt.00 ;\
		grep "DataMemberAccess:" $(top_builddir)/projects/compass/dataMemberAccessTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/dataMemberAccess/dataMemberAccessTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/dataMemberAccessTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/dataMemberAccess/dataMemberAccessTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/dataMemberAccess/dataMemberAccessTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/dataMemberAccess/dataMemberAccessTestOutput.txt; then \
			if test -e $(srcdir)/dataMemberAccess/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/dataMemberAccess/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/dataMemberAccess/dataMemberAccessTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/dataMemberAccess/dataMemberAccessTest1.C $(TGUI) &> $(top_builddir)/projects/compass/dataMemberAccessTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/dataMemberAccessTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/dataMemberAccessTestOutput.txt $(top_builddir)/projects/compass/dataMemberAccessTestOutput.txt.00 ;\
		grep "DataMemberAccess:" $(top_builddir)/projects/compass/dataMemberAccessTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/dataMemberAccessTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/dataMemberAccessTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/dataMemberAccessTestOutput.txt || ! test -s $(srcdir)/dataMemberAccess/dataMemberAccessTestOutput.txt ; then \
			if test -e $(srcdir)/dataMemberAccess/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/dataMemberAccess/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/dataMemberAccessTestOutput.txt or $(srcdir)/dataMemberAccess/dataMemberAccessTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/dataMemberAccessTestOutput.txt $(srcdir)/dataMemberAccess/dataMemberAccessTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/dataMemberAccessTestOutput.txt and $(srcdir)/dataMemberAccess/dataMemberAccessTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/dataMemberAccessTestOutput.txt ;\
	fi
	@echo "done testdataMemberAccess"

testDeepNesting: compassMain
	@echo -e "\nRunning testDeepNesting"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/deepNesting/deepNestingTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/deepNesting/deepNestingTestOutput.txt; then \
		echo "creating $(srcdir)/deepNesting/deepNestingTestOutput.txt" ;\
		./compassMain $(srcdir)/deepNesting/deepNestingTest1.C $(TGUI) &> $(srcdir)/deepNesting/deepNestingTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/deepNesting/deepNestingTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/deepNesting/deepNestingTestOutput.txt $(top_builddir)/projects/compass/deepNestingTestOutput.txt.00 ;\
		grep "DeepNesting:" $(top_builddir)/projects/compass/deepNestingTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/deepNesting/deepNestingTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/deepNestingTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/deepNesting/deepNestingTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/deepNesting/deepNestingTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/deepNesting/deepNestingTestOutput.txt; then \
			if test -e $(srcdir)/deepNesting/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/deepNesting/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/deepNesting/deepNestingTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/deepNesting/deepNestingTest1.C $(TGUI) &> $(top_builddir)/projects/compass/deepNestingTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/deepNestingTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/deepNestingTestOutput.txt $(top_builddir)/projects/compass/deepNestingTestOutput.txt.00 ;\
		grep "DeepNesting:" $(top_builddir)/projects/compass/deepNestingTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/deepNestingTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/deepNestingTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/deepNestingTestOutput.txt || ! test -s $(srcdir)/deepNesting/deepNestingTestOutput.txt ; then \
			if test -e $(srcdir)/deepNesting/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/deepNesting/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/deepNestingTestOutput.txt or $(srcdir)/deepNesting/deepNestingTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/deepNestingTestOutput.txt $(srcdir)/deepNesting/deepNestingTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/deepNestingTestOutput.txt and $(srcdir)/deepNesting/deepNestingTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/deepNestingTestOutput.txt ;\
	fi
	@echo "done testdeepNesting"

testDefaultCase: compassMain
	@echo -e "\nRunning testDefaultCase"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/defaultCase/defaultCaseTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/defaultCase/defaultCaseTestOutput.txt; then \
		echo "creating $(srcdir)/defaultCase/defaultCaseTestOutput.txt" ;\
		./compassMain $(srcdir)/defaultCase/defaultCaseTest1.C $(TGUI) &> $(srcdir)/defaultCase/defaultCaseTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/defaultCase/defaultCaseTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/defaultCase/defaultCaseTestOutput.txt $(top_builddir)/projects/compass/defaultCaseTestOutput.txt.00 ;\
		grep "DefaultCase:" $(top_builddir)/projects/compass/defaultCaseTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/defaultCase/defaultCaseTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/defaultCaseTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/defaultCase/defaultCaseTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/defaultCase/defaultCaseTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/defaultCase/defaultCaseTestOutput.txt; then \
			if test -e $(srcdir)/defaultCase/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/defaultCase/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/defaultCase/defaultCaseTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/defaultCase/defaultCaseTest1.C $(TGUI) &> $(top_builddir)/projects/compass/defaultCaseTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/defaultCaseTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/defaultCaseTestOutput.txt $(top_builddir)/projects/compass/defaultCaseTestOutput.txt.00 ;\
		grep "DefaultCase:" $(top_builddir)/projects/compass/defaultCaseTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/defaultCaseTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/defaultCaseTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/defaultCaseTestOutput.txt || ! test -s $(srcdir)/defaultCase/defaultCaseTestOutput.txt ; then \
			if test -e $(srcdir)/defaultCase/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/defaultCase/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/defaultCaseTestOutput.txt or $(srcdir)/defaultCase/defaultCaseTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/defaultCaseTestOutput.txt $(srcdir)/defaultCase/defaultCaseTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/defaultCaseTestOutput.txt and $(srcdir)/defaultCase/defaultCaseTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/defaultCaseTestOutput.txt ;\
	fi
	@echo "done testdefaultCase"

testDefaultConstructor: compassMain
	@echo -e "\nRunning testDefaultConstructor"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/defaultConstructor/defaultConstructorTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/defaultConstructor/defaultConstructorTestOutput.txt; then \
		echo "creating $(srcdir)/defaultConstructor/defaultConstructorTestOutput.txt" ;\
		./compassMain $(srcdir)/defaultConstructor/defaultConstructorTest1.C $(TGUI) &> $(srcdir)/defaultConstructor/defaultConstructorTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/defaultConstructor/defaultConstructorTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/defaultConstructor/defaultConstructorTestOutput.txt $(top_builddir)/projects/compass/defaultConstructorTestOutput.txt.00 ;\
		grep "DefaultConstructor:" $(top_builddir)/projects/compass/defaultConstructorTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/defaultConstructor/defaultConstructorTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/defaultConstructorTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/defaultConstructor/defaultConstructorTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/defaultConstructor/defaultConstructorTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/defaultConstructor/defaultConstructorTestOutput.txt; then \
			if test -e $(srcdir)/defaultConstructor/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/defaultConstructor/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/defaultConstructor/defaultConstructorTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/defaultConstructor/defaultConstructorTest1.C $(TGUI) &> $(top_builddir)/projects/compass/defaultConstructorTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/defaultConstructorTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/defaultConstructorTestOutput.txt $(top_builddir)/projects/compass/defaultConstructorTestOutput.txt.00 ;\
		grep "DefaultConstructor:" $(top_builddir)/projects/compass/defaultConstructorTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/defaultConstructorTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/defaultConstructorTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/defaultConstructorTestOutput.txt || ! test -s $(srcdir)/defaultConstructor/defaultConstructorTestOutput.txt ; then \
			if test -e $(srcdir)/defaultConstructor/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/defaultConstructor/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/defaultConstructorTestOutput.txt or $(srcdir)/defaultConstructor/defaultConstructorTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/defaultConstructorTestOutput.txt $(srcdir)/defaultConstructor/defaultConstructorTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/defaultConstructorTestOutput.txt and $(srcdir)/defaultConstructor/defaultConstructorTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/defaultConstructorTestOutput.txt ;\
	fi
	@echo "done testdefaultConstructor"

testDiscardAssignment: compassMain
	@echo -e "\nRunning testDiscardAssignment"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/discardAssignment/discardAssignmentTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/discardAssignment/discardAssignmentTestOutput.txt; then \
		echo "creating $(srcdir)/discardAssignment/discardAssignmentTestOutput.txt" ;\
		./compassMain $(srcdir)/discardAssignment/discardAssignmentTest1.C $(TGUI) &> $(srcdir)/discardAssignment/discardAssignmentTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/discardAssignment/discardAssignmentTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/discardAssignment/discardAssignmentTestOutput.txt $(top_builddir)/projects/compass/discardAssignmentTestOutput.txt.00 ;\
		grep "DiscardAssignment:" $(top_builddir)/projects/compass/discardAssignmentTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/discardAssignment/discardAssignmentTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/discardAssignmentTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/discardAssignment/discardAssignmentTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/discardAssignment/discardAssignmentTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/discardAssignment/discardAssignmentTestOutput.txt; then \
			if test -e $(srcdir)/discardAssignment/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/discardAssignment/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/discardAssignment/discardAssignmentTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/discardAssignment/discardAssignmentTest1.C $(TGUI) &> $(top_builddir)/projects/compass/discardAssignmentTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/discardAssignmentTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/discardAssignmentTestOutput.txt $(top_builddir)/projects/compass/discardAssignmentTestOutput.txt.00 ;\
		grep "DiscardAssignment:" $(top_builddir)/projects/compass/discardAssignmentTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/discardAssignmentTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/discardAssignmentTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/discardAssignmentTestOutput.txt || ! test -s $(srcdir)/discardAssignment/discardAssignmentTestOutput.txt ; then \
			if test -e $(srcdir)/discardAssignment/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/discardAssignment/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/discardAssignmentTestOutput.txt or $(srcdir)/discardAssignment/discardAssignmentTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/discardAssignmentTestOutput.txt $(srcdir)/discardAssignment/discardAssignmentTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/discardAssignmentTestOutput.txt and $(srcdir)/discardAssignment/discardAssignmentTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/discardAssignmentTestOutput.txt ;\
	fi
	@echo "done testdiscardAssignment"

testDoNotCallPutenvWithAutoVar: compassMain
	@echo -e "\nRunning testDoNotCallPutenvWithAutoVar"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/doNotCallPutenvWithAutoVar/doNotCallPutenvWithAutoVarTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/doNotCallPutenvWithAutoVar/doNotCallPutenvWithAutoVarTestOutput.txt; then \
		echo "creating $(srcdir)/doNotCallPutenvWithAutoVar/doNotCallPutenvWithAutoVarTestOutput.txt" ;\
		./compassMain $(srcdir)/doNotCallPutenvWithAutoVar/doNotCallPutenvWithAutoVarTest1.C $(TGUI) &> $(srcdir)/doNotCallPutenvWithAutoVar/doNotCallPutenvWithAutoVarTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/doNotCallPutenvWithAutoVar/doNotCallPutenvWithAutoVarTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/doNotCallPutenvWithAutoVar/doNotCallPutenvWithAutoVarTestOutput.txt $(top_builddir)/projects/compass/doNotCallPutenvWithAutoVarTestOutput.txt.00 ;\
		grep "DoNotCallPutenvWithAutoVar:" $(top_builddir)/projects/compass/doNotCallPutenvWithAutoVarTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/doNotCallPutenvWithAutoVar/doNotCallPutenvWithAutoVarTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/doNotCallPutenvWithAutoVarTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/doNotCallPutenvWithAutoVar/doNotCallPutenvWithAutoVarTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/doNotCallPutenvWithAutoVar/doNotCallPutenvWithAutoVarTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/doNotCallPutenvWithAutoVar/doNotCallPutenvWithAutoVarTestOutput.txt; then \
			if test -e $(srcdir)/doNotCallPutenvWithAutoVar/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/doNotCallPutenvWithAutoVar/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/doNotCallPutenvWithAutoVar/doNotCallPutenvWithAutoVarTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/doNotCallPutenvWithAutoVar/doNotCallPutenvWithAutoVarTest1.C $(TGUI) &> $(top_builddir)/projects/compass/doNotCallPutenvWithAutoVarTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/doNotCallPutenvWithAutoVarTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/doNotCallPutenvWithAutoVarTestOutput.txt $(top_builddir)/projects/compass/doNotCallPutenvWithAutoVarTestOutput.txt.00 ;\
		grep "DoNotCallPutenvWithAutoVar:" $(top_builddir)/projects/compass/doNotCallPutenvWithAutoVarTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/doNotCallPutenvWithAutoVarTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/doNotCallPutenvWithAutoVarTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/doNotCallPutenvWithAutoVarTestOutput.txt || ! test -s $(srcdir)/doNotCallPutenvWithAutoVar/doNotCallPutenvWithAutoVarTestOutput.txt ; then \
			if test -e $(srcdir)/doNotCallPutenvWithAutoVar/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/doNotCallPutenvWithAutoVar/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/doNotCallPutenvWithAutoVarTestOutput.txt or $(srcdir)/doNotCallPutenvWithAutoVar/doNotCallPutenvWithAutoVarTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/doNotCallPutenvWithAutoVarTestOutput.txt $(srcdir)/doNotCallPutenvWithAutoVar/doNotCallPutenvWithAutoVarTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/doNotCallPutenvWithAutoVarTestOutput.txt and $(srcdir)/doNotCallPutenvWithAutoVar/doNotCallPutenvWithAutoVarTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/doNotCallPutenvWithAutoVarTestOutput.txt ;\
	fi
	@echo "done testdoNotCallPutenvWithAutoVar"

testDoNotDeleteThis: compassMain
	@echo -e "\nRunning testDoNotDeleteThis"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/doNotDeleteThis/doNotDeleteThisTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/doNotDeleteThis/doNotDeleteThisTestOutput.txt; then \
		echo "creating $(srcdir)/doNotDeleteThis/doNotDeleteThisTestOutput.txt" ;\
		./compassMain $(srcdir)/doNotDeleteThis/doNotDeleteThisTest1.C $(TGUI) &> $(srcdir)/doNotDeleteThis/doNotDeleteThisTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/doNotDeleteThis/doNotDeleteThisTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/doNotDeleteThis/doNotDeleteThisTestOutput.txt $(top_builddir)/projects/compass/doNotDeleteThisTestOutput.txt.00 ;\
		grep "DoNotDeleteThis:" $(top_builddir)/projects/compass/doNotDeleteThisTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/doNotDeleteThis/doNotDeleteThisTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/doNotDeleteThisTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/doNotDeleteThis/doNotDeleteThisTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/doNotDeleteThis/doNotDeleteThisTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/doNotDeleteThis/doNotDeleteThisTestOutput.txt; then \
			if test -e $(srcdir)/doNotDeleteThis/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/doNotDeleteThis/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/doNotDeleteThis/doNotDeleteThisTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/doNotDeleteThis/doNotDeleteThisTest1.C $(TGUI) &> $(top_builddir)/projects/compass/doNotDeleteThisTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/doNotDeleteThisTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/doNotDeleteThisTestOutput.txt $(top_builddir)/projects/compass/doNotDeleteThisTestOutput.txt.00 ;\
		grep "DoNotDeleteThis:" $(top_builddir)/projects/compass/doNotDeleteThisTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/doNotDeleteThisTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/doNotDeleteThisTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/doNotDeleteThisTestOutput.txt || ! test -s $(srcdir)/doNotDeleteThis/doNotDeleteThisTestOutput.txt ; then \
			if test -e $(srcdir)/doNotDeleteThis/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/doNotDeleteThis/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/doNotDeleteThisTestOutput.txt or $(srcdir)/doNotDeleteThis/doNotDeleteThisTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/doNotDeleteThisTestOutput.txt $(srcdir)/doNotDeleteThis/doNotDeleteThisTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/doNotDeleteThisTestOutput.txt and $(srcdir)/doNotDeleteThis/doNotDeleteThisTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/doNotDeleteThisTestOutput.txt ;\
	fi
	@echo "done testdoNotDeleteThis"

testDoNotUseCstyleCasts: compassMain
	@echo -e "\nRunning testDoNotUseCstyleCasts"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/doNotUseCstyleCasts/doNotUseCstyleCastsTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/doNotUseCstyleCasts/doNotUseCstyleCastsTestOutput.txt; then \
		echo "creating $(srcdir)/doNotUseCstyleCasts/doNotUseCstyleCastsTestOutput.txt" ;\
		./compassMain $(srcdir)/doNotUseCstyleCasts/doNotUseCstyleCastsTest1.C $(TGUI) &> $(srcdir)/doNotUseCstyleCasts/doNotUseCstyleCastsTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/doNotUseCstyleCasts/doNotUseCstyleCastsTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/doNotUseCstyleCasts/doNotUseCstyleCastsTestOutput.txt $(top_builddir)/projects/compass/doNotUseCstyleCastsTestOutput.txt.00 ;\
		grep "DoNotUseCstyleCasts:" $(top_builddir)/projects/compass/doNotUseCstyleCastsTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/doNotUseCstyleCasts/doNotUseCstyleCastsTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/doNotUseCstyleCastsTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/doNotUseCstyleCasts/doNotUseCstyleCastsTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/doNotUseCstyleCasts/doNotUseCstyleCastsTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/doNotUseCstyleCasts/doNotUseCstyleCastsTestOutput.txt; then \
			if test -e $(srcdir)/doNotUseCstyleCasts/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/doNotUseCstyleCasts/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/doNotUseCstyleCasts/doNotUseCstyleCastsTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/doNotUseCstyleCasts/doNotUseCstyleCastsTest1.C $(TGUI) &> $(top_builddir)/projects/compass/doNotUseCstyleCastsTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/doNotUseCstyleCastsTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/doNotUseCstyleCastsTestOutput.txt $(top_builddir)/projects/compass/doNotUseCstyleCastsTestOutput.txt.00 ;\
		grep "DoNotUseCstyleCasts:" $(top_builddir)/projects/compass/doNotUseCstyleCastsTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/doNotUseCstyleCastsTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/doNotUseCstyleCastsTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/doNotUseCstyleCastsTestOutput.txt || ! test -s $(srcdir)/doNotUseCstyleCasts/doNotUseCstyleCastsTestOutput.txt ; then \
			if test -e $(srcdir)/doNotUseCstyleCasts/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/doNotUseCstyleCasts/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/doNotUseCstyleCastsTestOutput.txt or $(srcdir)/doNotUseCstyleCasts/doNotUseCstyleCastsTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/doNotUseCstyleCastsTestOutput.txt $(srcdir)/doNotUseCstyleCasts/doNotUseCstyleCastsTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/doNotUseCstyleCastsTestOutput.txt and $(srcdir)/doNotUseCstyleCasts/doNotUseCstyleCastsTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/doNotUseCstyleCastsTestOutput.txt ;\
	fi
	@echo "done testdoNotUseCstyleCasts"

testDuffsDevice: compassMain
	@echo -e "\nRunning testDuffsDevice"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/duffsDevice/duffsDeviceTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/duffsDevice/duffsDeviceTestOutput.txt; then \
		echo "creating $(srcdir)/duffsDevice/duffsDeviceTestOutput.txt" ;\
		./compassMain $(srcdir)/duffsDevice/duffsDeviceTest1.C $(TGUI) &> $(srcdir)/duffsDevice/duffsDeviceTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/duffsDevice/duffsDeviceTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/duffsDevice/duffsDeviceTestOutput.txt $(top_builddir)/projects/compass/duffsDeviceTestOutput.txt.00 ;\
		grep "DuffsDevice:" $(top_builddir)/projects/compass/duffsDeviceTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/duffsDevice/duffsDeviceTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/duffsDeviceTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/duffsDevice/duffsDeviceTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/duffsDevice/duffsDeviceTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/duffsDevice/duffsDeviceTestOutput.txt; then \
			if test -e $(srcdir)/duffsDevice/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/duffsDevice/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/duffsDevice/duffsDeviceTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/duffsDevice/duffsDeviceTest1.C $(TGUI) &> $(top_builddir)/projects/compass/duffsDeviceTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/duffsDeviceTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/duffsDeviceTestOutput.txt $(top_builddir)/projects/compass/duffsDeviceTestOutput.txt.00 ;\
		grep "DuffsDevice:" $(top_builddir)/projects/compass/duffsDeviceTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/duffsDeviceTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/duffsDeviceTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/duffsDeviceTestOutput.txt || ! test -s $(srcdir)/duffsDevice/duffsDeviceTestOutput.txt ; then \
			if test -e $(srcdir)/duffsDevice/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/duffsDevice/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/duffsDeviceTestOutput.txt or $(srcdir)/duffsDevice/duffsDeviceTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/duffsDeviceTestOutput.txt $(srcdir)/duffsDevice/duffsDeviceTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/duffsDeviceTestOutput.txt and $(srcdir)/duffsDevice/duffsDeviceTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/duffsDeviceTestOutput.txt ;\
	fi
	@echo "done testduffsDevice"

testDynamicCast: compassMain
	@echo -e "\nRunning testDynamicCast"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/dynamicCast/dynamicCastTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/dynamicCast/dynamicCastTestOutput.txt; then \
		echo "creating $(srcdir)/dynamicCast/dynamicCastTestOutput.txt" ;\
		./compassMain $(srcdir)/dynamicCast/dynamicCastTest1.C $(TGUI) &> $(srcdir)/dynamicCast/dynamicCastTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/dynamicCast/dynamicCastTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/dynamicCast/dynamicCastTestOutput.txt $(top_builddir)/projects/compass/dynamicCastTestOutput.txt.00 ;\
		grep "DynamicCast:" $(top_builddir)/projects/compass/dynamicCastTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/dynamicCast/dynamicCastTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/dynamicCastTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/dynamicCast/dynamicCastTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/dynamicCast/dynamicCastTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/dynamicCast/dynamicCastTestOutput.txt; then \
			if test -e $(srcdir)/dynamicCast/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/dynamicCast/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/dynamicCast/dynamicCastTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/dynamicCast/dynamicCastTest1.C $(TGUI) &> $(top_builddir)/projects/compass/dynamicCastTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/dynamicCastTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/dynamicCastTestOutput.txt $(top_builddir)/projects/compass/dynamicCastTestOutput.txt.00 ;\
		grep "DynamicCast:" $(top_builddir)/projects/compass/dynamicCastTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/dynamicCastTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/dynamicCastTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/dynamicCastTestOutput.txt || ! test -s $(srcdir)/dynamicCast/dynamicCastTestOutput.txt ; then \
			if test -e $(srcdir)/dynamicCast/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/dynamicCast/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/dynamicCastTestOutput.txt or $(srcdir)/dynamicCast/dynamicCastTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/dynamicCastTestOutput.txt $(srcdir)/dynamicCast/dynamicCastTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/dynamicCastTestOutput.txt and $(srcdir)/dynamicCast/dynamicCastTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/dynamicCastTestOutput.txt ;\
	fi
	@echo "done testdynamicCast"

testEmptyInsteadOfSize: compassMain
	@echo -e "\nRunning testEmptyInsteadOfSize"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/emptyInsteadOfSize/emptyInsteadOfSizeTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/emptyInsteadOfSize/emptyInsteadOfSizeTestOutput.txt; then \
		echo "creating $(srcdir)/emptyInsteadOfSize/emptyInsteadOfSizeTestOutput.txt" ;\
		./compassMain $(srcdir)/emptyInsteadOfSize/emptyInsteadOfSizeTest1.C $(TGUI) &> $(srcdir)/emptyInsteadOfSize/emptyInsteadOfSizeTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/emptyInsteadOfSize/emptyInsteadOfSizeTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/emptyInsteadOfSize/emptyInsteadOfSizeTestOutput.txt $(top_builddir)/projects/compass/emptyInsteadOfSizeTestOutput.txt.00 ;\
		grep "EmptyInsteadOfSize:" $(top_builddir)/projects/compass/emptyInsteadOfSizeTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/emptyInsteadOfSize/emptyInsteadOfSizeTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/emptyInsteadOfSizeTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/emptyInsteadOfSize/emptyInsteadOfSizeTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/emptyInsteadOfSize/emptyInsteadOfSizeTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/emptyInsteadOfSize/emptyInsteadOfSizeTestOutput.txt; then \
			if test -e $(srcdir)/emptyInsteadOfSize/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/emptyInsteadOfSize/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/emptyInsteadOfSize/emptyInsteadOfSizeTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/emptyInsteadOfSize/emptyInsteadOfSizeTest1.C $(TGUI) &> $(top_builddir)/projects/compass/emptyInsteadOfSizeTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/emptyInsteadOfSizeTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/emptyInsteadOfSizeTestOutput.txt $(top_builddir)/projects/compass/emptyInsteadOfSizeTestOutput.txt.00 ;\
		grep "EmptyInsteadOfSize:" $(top_builddir)/projects/compass/emptyInsteadOfSizeTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/emptyInsteadOfSizeTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/emptyInsteadOfSizeTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/emptyInsteadOfSizeTestOutput.txt || ! test -s $(srcdir)/emptyInsteadOfSize/emptyInsteadOfSizeTestOutput.txt ; then \
			if test -e $(srcdir)/emptyInsteadOfSize/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/emptyInsteadOfSize/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/emptyInsteadOfSizeTestOutput.txt or $(srcdir)/emptyInsteadOfSize/emptyInsteadOfSizeTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/emptyInsteadOfSizeTestOutput.txt $(srcdir)/emptyInsteadOfSize/emptyInsteadOfSizeTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/emptyInsteadOfSizeTestOutput.txt and $(srcdir)/emptyInsteadOfSize/emptyInsteadOfSizeTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/emptyInsteadOfSizeTestOutput.txt ;\
	fi
	@echo "done testemptyInsteadOfSize"

testEnumDeclarationNamespaceClassScope: compassMain
	@echo -e "\nRunning testEnumDeclarationNamespaceClassScope"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/enumDeclarationNamespaceClassScope/enumDeclarationNamespaceClassScopeTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/enumDeclarationNamespaceClassScope/enumDeclarationNamespaceClassScopeTestOutput.txt; then \
		echo "creating $(srcdir)/enumDeclarationNamespaceClassScope/enumDeclarationNamespaceClassScopeTestOutput.txt" ;\
		./compassMain $(srcdir)/enumDeclarationNamespaceClassScope/enumDeclarationNamespaceClassScopeTest1.C $(TGUI) &> $(srcdir)/enumDeclarationNamespaceClassScope/enumDeclarationNamespaceClassScopeTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/enumDeclarationNamespaceClassScope/enumDeclarationNamespaceClassScopeTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/enumDeclarationNamespaceClassScope/enumDeclarationNamespaceClassScopeTestOutput.txt $(top_builddir)/projects/compass/enumDeclarationNamespaceClassScopeTestOutput.txt.00 ;\
		grep "EnumDeclarationNamespaceClassScope:" $(top_builddir)/projects/compass/enumDeclarationNamespaceClassScopeTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/enumDeclarationNamespaceClassScope/enumDeclarationNamespaceClassScopeTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/enumDeclarationNamespaceClassScopeTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/enumDeclarationNamespaceClassScope/enumDeclarationNamespaceClassScopeTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/enumDeclarationNamespaceClassScope/enumDeclarationNamespaceClassScopeTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/enumDeclarationNamespaceClassScope/enumDeclarationNamespaceClassScopeTestOutput.txt; then \
			if test -e $(srcdir)/enumDeclarationNamespaceClassScope/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/enumDeclarationNamespaceClassScope/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/enumDeclarationNamespaceClassScope/enumDeclarationNamespaceClassScopeTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/enumDeclarationNamespaceClassScope/enumDeclarationNamespaceClassScopeTest1.C $(TGUI) &> $(top_builddir)/projects/compass/enumDeclarationNamespaceClassScopeTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/enumDeclarationNamespaceClassScopeTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/enumDeclarationNamespaceClassScopeTestOutput.txt $(top_builddir)/projects/compass/enumDeclarationNamespaceClassScopeTestOutput.txt.00 ;\
		grep "EnumDeclarationNamespaceClassScope:" $(top_builddir)/projects/compass/enumDeclarationNamespaceClassScopeTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/enumDeclarationNamespaceClassScopeTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/enumDeclarationNamespaceClassScopeTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/enumDeclarationNamespaceClassScopeTestOutput.txt || ! test -s $(srcdir)/enumDeclarationNamespaceClassScope/enumDeclarationNamespaceClassScopeTestOutput.txt ; then \
			if test -e $(srcdir)/enumDeclarationNamespaceClassScope/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/enumDeclarationNamespaceClassScope/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/enumDeclarationNamespaceClassScopeTestOutput.txt or $(srcdir)/enumDeclarationNamespaceClassScope/enumDeclarationNamespaceClassScopeTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/enumDeclarationNamespaceClassScopeTestOutput.txt $(srcdir)/enumDeclarationNamespaceClassScope/enumDeclarationNamespaceClassScopeTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/enumDeclarationNamespaceClassScopeTestOutput.txt and $(srcdir)/enumDeclarationNamespaceClassScope/enumDeclarationNamespaceClassScopeTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/enumDeclarationNamespaceClassScopeTestOutput.txt ;\
	fi
	@echo "done testenumDeclarationNamespaceClassScope"

testExplicitCharSign: compassMain
	@echo -e "\nRunning testExplicitCharSign"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/explicitCharSign/explicitCharSignTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/explicitCharSign/explicitCharSignTestOutput.txt; then \
		echo "creating $(srcdir)/explicitCharSign/explicitCharSignTestOutput.txt" ;\
		./compassMain $(srcdir)/explicitCharSign/explicitCharSignTest1.C $(TGUI) &> $(srcdir)/explicitCharSign/explicitCharSignTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/explicitCharSign/explicitCharSignTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/explicitCharSign/explicitCharSignTestOutput.txt $(top_builddir)/projects/compass/explicitCharSignTestOutput.txt.00 ;\
		grep "ExplicitCharSign:" $(top_builddir)/projects/compass/explicitCharSignTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/explicitCharSign/explicitCharSignTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/explicitCharSignTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/explicitCharSign/explicitCharSignTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/explicitCharSign/explicitCharSignTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/explicitCharSign/explicitCharSignTestOutput.txt; then \
			if test -e $(srcdir)/explicitCharSign/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/explicitCharSign/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/explicitCharSign/explicitCharSignTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/explicitCharSign/explicitCharSignTest1.C $(TGUI) &> $(top_builddir)/projects/compass/explicitCharSignTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/explicitCharSignTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/explicitCharSignTestOutput.txt $(top_builddir)/projects/compass/explicitCharSignTestOutput.txt.00 ;\
		grep "ExplicitCharSign:" $(top_builddir)/projects/compass/explicitCharSignTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/explicitCharSignTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/explicitCharSignTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/explicitCharSignTestOutput.txt || ! test -s $(srcdir)/explicitCharSign/explicitCharSignTestOutput.txt ; then \
			if test -e $(srcdir)/explicitCharSign/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/explicitCharSign/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/explicitCharSignTestOutput.txt or $(srcdir)/explicitCharSign/explicitCharSignTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/explicitCharSignTestOutput.txt $(srcdir)/explicitCharSign/explicitCharSignTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/explicitCharSignTestOutput.txt and $(srcdir)/explicitCharSign/explicitCharSignTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/explicitCharSignTestOutput.txt ;\
	fi
	@echo "done testexplicitCharSign"

testExplicitCopy: compassMain
	@echo -e "\nRunning testExplicitCopy"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/explicitCopy/explicitCopyTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/explicitCopy/explicitCopyTestOutput.txt; then \
		echo "creating $(srcdir)/explicitCopy/explicitCopyTestOutput.txt" ;\
		./compassMain $(srcdir)/explicitCopy/explicitCopyTest1.C $(TGUI) &> $(srcdir)/explicitCopy/explicitCopyTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/explicitCopy/explicitCopyTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/explicitCopy/explicitCopyTestOutput.txt $(top_builddir)/projects/compass/explicitCopyTestOutput.txt.00 ;\
		grep "ExplicitCopy:" $(top_builddir)/projects/compass/explicitCopyTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/explicitCopy/explicitCopyTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/explicitCopyTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/explicitCopy/explicitCopyTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/explicitCopy/explicitCopyTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/explicitCopy/explicitCopyTestOutput.txt; then \
			if test -e $(srcdir)/explicitCopy/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/explicitCopy/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/explicitCopy/explicitCopyTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/explicitCopy/explicitCopyTest1.C $(TGUI) &> $(top_builddir)/projects/compass/explicitCopyTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/explicitCopyTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/explicitCopyTestOutput.txt $(top_builddir)/projects/compass/explicitCopyTestOutput.txt.00 ;\
		grep "ExplicitCopy:" $(top_builddir)/projects/compass/explicitCopyTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/explicitCopyTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/explicitCopyTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/explicitCopyTestOutput.txt || ! test -s $(srcdir)/explicitCopy/explicitCopyTestOutput.txt ; then \
			if test -e $(srcdir)/explicitCopy/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/explicitCopy/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/explicitCopyTestOutput.txt or $(srcdir)/explicitCopy/explicitCopyTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/explicitCopyTestOutput.txt $(srcdir)/explicitCopy/explicitCopyTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/explicitCopyTestOutput.txt and $(srcdir)/explicitCopy/explicitCopyTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/explicitCopyTestOutput.txt ;\
	fi
	@echo "done testexplicitCopy"

testExplicitTestForNonBooleanValue: compassMain
	@echo -e "\nRunning testExplicitTestForNonBooleanValue"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/explicitTestForNonBooleanValue/explicitTestForNonBooleanValueTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/explicitTestForNonBooleanValue/explicitTestForNonBooleanValueTestOutput.txt; then \
		echo "creating $(srcdir)/explicitTestForNonBooleanValue/explicitTestForNonBooleanValueTestOutput.txt" ;\
		./compassMain $(srcdir)/explicitTestForNonBooleanValue/explicitTestForNonBooleanValueTest1.C $(TGUI) &> $(srcdir)/explicitTestForNonBooleanValue/explicitTestForNonBooleanValueTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/explicitTestForNonBooleanValue/explicitTestForNonBooleanValueTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/explicitTestForNonBooleanValue/explicitTestForNonBooleanValueTestOutput.txt $(top_builddir)/projects/compass/explicitTestForNonBooleanValueTestOutput.txt.00 ;\
		grep "ExplicitTestForNonBooleanValue:" $(top_builddir)/projects/compass/explicitTestForNonBooleanValueTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/explicitTestForNonBooleanValue/explicitTestForNonBooleanValueTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/explicitTestForNonBooleanValueTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/explicitTestForNonBooleanValue/explicitTestForNonBooleanValueTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/explicitTestForNonBooleanValue/explicitTestForNonBooleanValueTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/explicitTestForNonBooleanValue/explicitTestForNonBooleanValueTestOutput.txt; then \
			if test -e $(srcdir)/explicitTestForNonBooleanValue/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/explicitTestForNonBooleanValue/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/explicitTestForNonBooleanValue/explicitTestForNonBooleanValueTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/explicitTestForNonBooleanValue/explicitTestForNonBooleanValueTest1.C $(TGUI) &> $(top_builddir)/projects/compass/explicitTestForNonBooleanValueTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/explicitTestForNonBooleanValueTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/explicitTestForNonBooleanValueTestOutput.txt $(top_builddir)/projects/compass/explicitTestForNonBooleanValueTestOutput.txt.00 ;\
		grep "ExplicitTestForNonBooleanValue:" $(top_builddir)/projects/compass/explicitTestForNonBooleanValueTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/explicitTestForNonBooleanValueTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/explicitTestForNonBooleanValueTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/explicitTestForNonBooleanValueTestOutput.txt || ! test -s $(srcdir)/explicitTestForNonBooleanValue/explicitTestForNonBooleanValueTestOutput.txt ; then \
			if test -e $(srcdir)/explicitTestForNonBooleanValue/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/explicitTestForNonBooleanValue/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/explicitTestForNonBooleanValueTestOutput.txt or $(srcdir)/explicitTestForNonBooleanValue/explicitTestForNonBooleanValueTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/explicitTestForNonBooleanValueTestOutput.txt $(srcdir)/explicitTestForNonBooleanValue/explicitTestForNonBooleanValueTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/explicitTestForNonBooleanValueTestOutput.txt and $(srcdir)/explicitTestForNonBooleanValue/explicitTestForNonBooleanValueTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/explicitTestForNonBooleanValueTestOutput.txt ;\
	fi
	@echo "done testexplicitTestForNonBooleanValue"

testFloatForLoopCounter: compassMain
	@echo -e "\nRunning testFloatForLoopCounter"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/floatForLoopCounter/floatForLoopCounterTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/floatForLoopCounter/floatForLoopCounterTestOutput.txt; then \
		echo "creating $(srcdir)/floatForLoopCounter/floatForLoopCounterTestOutput.txt" ;\
		./compassMain $(srcdir)/floatForLoopCounter/floatForLoopCounterTest1.C $(TGUI) &> $(srcdir)/floatForLoopCounter/floatForLoopCounterTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/floatForLoopCounter/floatForLoopCounterTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/floatForLoopCounter/floatForLoopCounterTestOutput.txt $(top_builddir)/projects/compass/floatForLoopCounterTestOutput.txt.00 ;\
		grep "FloatForLoopCounter:" $(top_builddir)/projects/compass/floatForLoopCounterTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/floatForLoopCounter/floatForLoopCounterTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/floatForLoopCounterTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/floatForLoopCounter/floatForLoopCounterTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/floatForLoopCounter/floatForLoopCounterTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/floatForLoopCounter/floatForLoopCounterTestOutput.txt; then \
			if test -e $(srcdir)/floatForLoopCounter/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/floatForLoopCounter/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/floatForLoopCounter/floatForLoopCounterTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/floatForLoopCounter/floatForLoopCounterTest1.C $(TGUI) &> $(top_builddir)/projects/compass/floatForLoopCounterTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/floatForLoopCounterTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/floatForLoopCounterTestOutput.txt $(top_builddir)/projects/compass/floatForLoopCounterTestOutput.txt.00 ;\
		grep "FloatForLoopCounter:" $(top_builddir)/projects/compass/floatForLoopCounterTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/floatForLoopCounterTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/floatForLoopCounterTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/floatForLoopCounterTestOutput.txt || ! test -s $(srcdir)/floatForLoopCounter/floatForLoopCounterTestOutput.txt ; then \
			if test -e $(srcdir)/floatForLoopCounter/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/floatForLoopCounter/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/floatForLoopCounterTestOutput.txt or $(srcdir)/floatForLoopCounter/floatForLoopCounterTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/floatForLoopCounterTestOutput.txt $(srcdir)/floatForLoopCounter/floatForLoopCounterTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/floatForLoopCounterTestOutput.txt and $(srcdir)/floatForLoopCounter/floatForLoopCounterTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/floatForLoopCounterTestOutput.txt ;\
	fi
	@echo "done testfloatForLoopCounter"

testFloatingPointExactComparison: compassMain
	@echo -e "\nRunning testFloatingPointExactComparison"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/floatingPointExactComparison/floatingPointExactComparisonTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/floatingPointExactComparison/floatingPointExactComparisonTestOutput.txt; then \
		echo "creating $(srcdir)/floatingPointExactComparison/floatingPointExactComparisonTestOutput.txt" ;\
		./compassMain $(srcdir)/floatingPointExactComparison/floatingPointExactComparisonTest1.C $(TGUI) &> $(srcdir)/floatingPointExactComparison/floatingPointExactComparisonTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/floatingPointExactComparison/floatingPointExactComparisonTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/floatingPointExactComparison/floatingPointExactComparisonTestOutput.txt $(top_builddir)/projects/compass/floatingPointExactComparisonTestOutput.txt.00 ;\
		grep "FloatingPointExactComparison:" $(top_builddir)/projects/compass/floatingPointExactComparisonTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/floatingPointExactComparison/floatingPointExactComparisonTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/floatingPointExactComparisonTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/floatingPointExactComparison/floatingPointExactComparisonTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/floatingPointExactComparison/floatingPointExactComparisonTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/floatingPointExactComparison/floatingPointExactComparisonTestOutput.txt; then \
			if test -e $(srcdir)/floatingPointExactComparison/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/floatingPointExactComparison/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/floatingPointExactComparison/floatingPointExactComparisonTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/floatingPointExactComparison/floatingPointExactComparisonTest1.C $(TGUI) &> $(top_builddir)/projects/compass/floatingPointExactComparisonTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/floatingPointExactComparisonTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/floatingPointExactComparisonTestOutput.txt $(top_builddir)/projects/compass/floatingPointExactComparisonTestOutput.txt.00 ;\
		grep "FloatingPointExactComparison:" $(top_builddir)/projects/compass/floatingPointExactComparisonTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/floatingPointExactComparisonTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/floatingPointExactComparisonTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/floatingPointExactComparisonTestOutput.txt || ! test -s $(srcdir)/floatingPointExactComparison/floatingPointExactComparisonTestOutput.txt ; then \
			if test -e $(srcdir)/floatingPointExactComparison/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/floatingPointExactComparison/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/floatingPointExactComparisonTestOutput.txt or $(srcdir)/floatingPointExactComparison/floatingPointExactComparisonTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/floatingPointExactComparisonTestOutput.txt $(srcdir)/floatingPointExactComparison/floatingPointExactComparisonTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/floatingPointExactComparisonTestOutput.txt and $(srcdir)/floatingPointExactComparison/floatingPointExactComparisonTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/floatingPointExactComparisonTestOutput.txt ;\
	fi
	@echo "done testfloatingPointExactComparison"

testFopenFormatParameter: compassMain
	@echo -e "\nRunning testFopenFormatParameter"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/fopenFormatParameter/fopenFormatParameterTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/fopenFormatParameter/fopenFormatParameterTestOutput.txt; then \
		echo "creating $(srcdir)/fopenFormatParameter/fopenFormatParameterTestOutput.txt" ;\
		./compassMain $(srcdir)/fopenFormatParameter/fopenFormatParameterTest1.C $(TGUI) &> $(srcdir)/fopenFormatParameter/fopenFormatParameterTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/fopenFormatParameter/fopenFormatParameterTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/fopenFormatParameter/fopenFormatParameterTestOutput.txt $(top_builddir)/projects/compass/fopenFormatParameterTestOutput.txt.00 ;\
		grep "FopenFormatParameter:" $(top_builddir)/projects/compass/fopenFormatParameterTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/fopenFormatParameter/fopenFormatParameterTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/fopenFormatParameterTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/fopenFormatParameter/fopenFormatParameterTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/fopenFormatParameter/fopenFormatParameterTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/fopenFormatParameter/fopenFormatParameterTestOutput.txt; then \
			if test -e $(srcdir)/fopenFormatParameter/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/fopenFormatParameter/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/fopenFormatParameter/fopenFormatParameterTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/fopenFormatParameter/fopenFormatParameterTest1.C $(TGUI) &> $(top_builddir)/projects/compass/fopenFormatParameterTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/fopenFormatParameterTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/fopenFormatParameterTestOutput.txt $(top_builddir)/projects/compass/fopenFormatParameterTestOutput.txt.00 ;\
		grep "FopenFormatParameter:" $(top_builddir)/projects/compass/fopenFormatParameterTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/fopenFormatParameterTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/fopenFormatParameterTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/fopenFormatParameterTestOutput.txt || ! test -s $(srcdir)/fopenFormatParameter/fopenFormatParameterTestOutput.txt ; then \
			if test -e $(srcdir)/fopenFormatParameter/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/fopenFormatParameter/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/fopenFormatParameterTestOutput.txt or $(srcdir)/fopenFormatParameter/fopenFormatParameterTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/fopenFormatParameterTestOutput.txt $(srcdir)/fopenFormatParameter/fopenFormatParameterTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/fopenFormatParameterTestOutput.txt and $(srcdir)/fopenFormatParameter/fopenFormatParameterTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/fopenFormatParameterTestOutput.txt ;\
	fi
	@echo "done testfopenFormatParameter"

testForbiddenFunctions: compassMain
	@echo -e "\nRunning testForbiddenFunctions"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/forbiddenFunctions/forbiddenFunctionsTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/forbiddenFunctions/forbiddenFunctionsTestOutput.txt; then \
		echo "creating $(srcdir)/forbiddenFunctions/forbiddenFunctionsTestOutput.txt" ;\
		./compassMain $(srcdir)/forbiddenFunctions/forbiddenFunctionsTest1.C $(TGUI) &> $(srcdir)/forbiddenFunctions/forbiddenFunctionsTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/forbiddenFunctions/forbiddenFunctionsTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/forbiddenFunctions/forbiddenFunctionsTestOutput.txt $(top_builddir)/projects/compass/forbiddenFunctionsTestOutput.txt.00 ;\
		grep "ForbiddenFunctions:" $(top_builddir)/projects/compass/forbiddenFunctionsTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/forbiddenFunctions/forbiddenFunctionsTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/forbiddenFunctionsTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/forbiddenFunctions/forbiddenFunctionsTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/forbiddenFunctions/forbiddenFunctionsTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/forbiddenFunctions/forbiddenFunctionsTestOutput.txt; then \
			if test -e $(srcdir)/forbiddenFunctions/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/forbiddenFunctions/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/forbiddenFunctions/forbiddenFunctionsTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/forbiddenFunctions/forbiddenFunctionsTest1.C $(TGUI) &> $(top_builddir)/projects/compass/forbiddenFunctionsTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/forbiddenFunctionsTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/forbiddenFunctionsTestOutput.txt $(top_builddir)/projects/compass/forbiddenFunctionsTestOutput.txt.00 ;\
		grep "ForbiddenFunctions:" $(top_builddir)/projects/compass/forbiddenFunctionsTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/forbiddenFunctionsTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/forbiddenFunctionsTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/forbiddenFunctionsTestOutput.txt || ! test -s $(srcdir)/forbiddenFunctions/forbiddenFunctionsTestOutput.txt ; then \
			if test -e $(srcdir)/forbiddenFunctions/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/forbiddenFunctions/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/forbiddenFunctionsTestOutput.txt or $(srcdir)/forbiddenFunctions/forbiddenFunctionsTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/forbiddenFunctionsTestOutput.txt $(srcdir)/forbiddenFunctions/forbiddenFunctionsTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/forbiddenFunctionsTestOutput.txt and $(srcdir)/forbiddenFunctions/forbiddenFunctionsTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/forbiddenFunctionsTestOutput.txt ;\
	fi
	@echo "done testforbiddenFunctions"

testForLoopConstructionControlStmt: compassMain
	@echo -e "\nRunning testForLoopConstructionControlStmt"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/forLoopConstructionControlStmt/forLoopConstructionControlStmtTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/forLoopConstructionControlStmt/forLoopConstructionControlStmtTestOutput.txt; then \
		echo "creating $(srcdir)/forLoopConstructionControlStmt/forLoopConstructionControlStmtTestOutput.txt" ;\
		./compassMain $(srcdir)/forLoopConstructionControlStmt/forLoopConstructionControlStmtTest1.C $(TGUI) &> $(srcdir)/forLoopConstructionControlStmt/forLoopConstructionControlStmtTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/forLoopConstructionControlStmt/forLoopConstructionControlStmtTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/forLoopConstructionControlStmt/forLoopConstructionControlStmtTestOutput.txt $(top_builddir)/projects/compass/forLoopConstructionControlStmtTestOutput.txt.00 ;\
		grep "ForLoopConstructionControlStmt:" $(top_builddir)/projects/compass/forLoopConstructionControlStmtTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/forLoopConstructionControlStmt/forLoopConstructionControlStmtTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/forLoopConstructionControlStmtTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/forLoopConstructionControlStmt/forLoopConstructionControlStmtTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/forLoopConstructionControlStmt/forLoopConstructionControlStmtTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/forLoopConstructionControlStmt/forLoopConstructionControlStmtTestOutput.txt; then \
			if test -e $(srcdir)/forLoopConstructionControlStmt/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/forLoopConstructionControlStmt/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/forLoopConstructionControlStmt/forLoopConstructionControlStmtTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/forLoopConstructionControlStmt/forLoopConstructionControlStmtTest1.C $(TGUI) &> $(top_builddir)/projects/compass/forLoopConstructionControlStmtTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/forLoopConstructionControlStmtTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/forLoopConstructionControlStmtTestOutput.txt $(top_builddir)/projects/compass/forLoopConstructionControlStmtTestOutput.txt.00 ;\
		grep "ForLoopConstructionControlStmt:" $(top_builddir)/projects/compass/forLoopConstructionControlStmtTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/forLoopConstructionControlStmtTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/forLoopConstructionControlStmtTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/forLoopConstructionControlStmtTestOutput.txt || ! test -s $(srcdir)/forLoopConstructionControlStmt/forLoopConstructionControlStmtTestOutput.txt ; then \
			if test -e $(srcdir)/forLoopConstructionControlStmt/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/forLoopConstructionControlStmt/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/forLoopConstructionControlStmtTestOutput.txt or $(srcdir)/forLoopConstructionControlStmt/forLoopConstructionControlStmtTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/forLoopConstructionControlStmtTestOutput.txt $(srcdir)/forLoopConstructionControlStmt/forLoopConstructionControlStmtTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/forLoopConstructionControlStmtTestOutput.txt and $(srcdir)/forLoopConstructionControlStmt/forLoopConstructionControlStmtTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/forLoopConstructionControlStmtTestOutput.txt ;\
	fi
	@echo "done testforLoopConstructionControlStmt"

testForLoopCppIndexVariableDeclaration: compassMain
	@echo -e "\nRunning testForLoopCppIndexVariableDeclaration"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/forLoopCppIndexVariableDeclaration/forLoopCppIndexVariableDeclarationTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/forLoopCppIndexVariableDeclaration/forLoopCppIndexVariableDeclarationTestOutput.txt; then \
		echo "creating $(srcdir)/forLoopCppIndexVariableDeclaration/forLoopCppIndexVariableDeclarationTestOutput.txt" ;\
		./compassMain $(srcdir)/forLoopCppIndexVariableDeclaration/forLoopCppIndexVariableDeclarationTest1.C $(TGUI) &> $(srcdir)/forLoopCppIndexVariableDeclaration/forLoopCppIndexVariableDeclarationTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/forLoopCppIndexVariableDeclaration/forLoopCppIndexVariableDeclarationTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/forLoopCppIndexVariableDeclaration/forLoopCppIndexVariableDeclarationTestOutput.txt $(top_builddir)/projects/compass/forLoopCppIndexVariableDeclarationTestOutput.txt.00 ;\
		grep "ForLoopCppIndexVariableDeclaration:" $(top_builddir)/projects/compass/forLoopCppIndexVariableDeclarationTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/forLoopCppIndexVariableDeclaration/forLoopCppIndexVariableDeclarationTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/forLoopCppIndexVariableDeclarationTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/forLoopCppIndexVariableDeclaration/forLoopCppIndexVariableDeclarationTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/forLoopCppIndexVariableDeclaration/forLoopCppIndexVariableDeclarationTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/forLoopCppIndexVariableDeclaration/forLoopCppIndexVariableDeclarationTestOutput.txt; then \
			if test -e $(srcdir)/forLoopCppIndexVariableDeclaration/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/forLoopCppIndexVariableDeclaration/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/forLoopCppIndexVariableDeclaration/forLoopCppIndexVariableDeclarationTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/forLoopCppIndexVariableDeclaration/forLoopCppIndexVariableDeclarationTest1.C $(TGUI) &> $(top_builddir)/projects/compass/forLoopCppIndexVariableDeclarationTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/forLoopCppIndexVariableDeclarationTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/forLoopCppIndexVariableDeclarationTestOutput.txt $(top_builddir)/projects/compass/forLoopCppIndexVariableDeclarationTestOutput.txt.00 ;\
		grep "ForLoopCppIndexVariableDeclaration:" $(top_builddir)/projects/compass/forLoopCppIndexVariableDeclarationTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/forLoopCppIndexVariableDeclarationTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/forLoopCppIndexVariableDeclarationTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/forLoopCppIndexVariableDeclarationTestOutput.txt || ! test -s $(srcdir)/forLoopCppIndexVariableDeclaration/forLoopCppIndexVariableDeclarationTestOutput.txt ; then \
			if test -e $(srcdir)/forLoopCppIndexVariableDeclaration/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/forLoopCppIndexVariableDeclaration/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/forLoopCppIndexVariableDeclarationTestOutput.txt or $(srcdir)/forLoopCppIndexVariableDeclaration/forLoopCppIndexVariableDeclarationTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/forLoopCppIndexVariableDeclarationTestOutput.txt $(srcdir)/forLoopCppIndexVariableDeclaration/forLoopCppIndexVariableDeclarationTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/forLoopCppIndexVariableDeclarationTestOutput.txt and $(srcdir)/forLoopCppIndexVariableDeclaration/forLoopCppIndexVariableDeclarationTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/forLoopCppIndexVariableDeclarationTestOutput.txt ;\
	fi
	@echo "done testforLoopCppIndexVariableDeclaration"

testFriendDeclarationModifier: compassMain
	@echo -e "\nRunning testFriendDeclarationModifier"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/friendDeclarationModifier/friendDeclarationModifierTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/friendDeclarationModifier/friendDeclarationModifierTestOutput.txt; then \
		echo "creating $(srcdir)/friendDeclarationModifier/friendDeclarationModifierTestOutput.txt" ;\
		./compassMain $(srcdir)/friendDeclarationModifier/friendDeclarationModifierTest1.C $(TGUI) &> $(srcdir)/friendDeclarationModifier/friendDeclarationModifierTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/friendDeclarationModifier/friendDeclarationModifierTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/friendDeclarationModifier/friendDeclarationModifierTestOutput.txt $(top_builddir)/projects/compass/friendDeclarationModifierTestOutput.txt.00 ;\
		grep "FriendDeclarationModifier:" $(top_builddir)/projects/compass/friendDeclarationModifierTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/friendDeclarationModifier/friendDeclarationModifierTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/friendDeclarationModifierTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/friendDeclarationModifier/friendDeclarationModifierTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/friendDeclarationModifier/friendDeclarationModifierTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/friendDeclarationModifier/friendDeclarationModifierTestOutput.txt; then \
			if test -e $(srcdir)/friendDeclarationModifier/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/friendDeclarationModifier/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/friendDeclarationModifier/friendDeclarationModifierTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/friendDeclarationModifier/friendDeclarationModifierTest1.C $(TGUI) &> $(top_builddir)/projects/compass/friendDeclarationModifierTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/friendDeclarationModifierTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/friendDeclarationModifierTestOutput.txt $(top_builddir)/projects/compass/friendDeclarationModifierTestOutput.txt.00 ;\
		grep "FriendDeclarationModifier:" $(top_builddir)/projects/compass/friendDeclarationModifierTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/friendDeclarationModifierTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/friendDeclarationModifierTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/friendDeclarationModifierTestOutput.txt || ! test -s $(srcdir)/friendDeclarationModifier/friendDeclarationModifierTestOutput.txt ; then \
			if test -e $(srcdir)/friendDeclarationModifier/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/friendDeclarationModifier/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/friendDeclarationModifierTestOutput.txt or $(srcdir)/friendDeclarationModifier/friendDeclarationModifierTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/friendDeclarationModifierTestOutput.txt $(srcdir)/friendDeclarationModifier/friendDeclarationModifierTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/friendDeclarationModifierTestOutput.txt and $(srcdir)/friendDeclarationModifier/friendDeclarationModifierTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/friendDeclarationModifierTestOutput.txt ;\
	fi
	@echo "done testfriendDeclarationModifier"

testFunctionCallAllocatesMultipleResources: compassMain
	@echo -e "\nRunning testFunctionCallAllocatesMultipleResources"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/functionCallAllocatesMultipleResources/functionCallAllocatesMultipleResourcesTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/functionCallAllocatesMultipleResources/functionCallAllocatesMultipleResourcesTestOutput.txt; then \
		echo "creating $(srcdir)/functionCallAllocatesMultipleResources/functionCallAllocatesMultipleResourcesTestOutput.txt" ;\
		./compassMain $(srcdir)/functionCallAllocatesMultipleResources/functionCallAllocatesMultipleResourcesTest1.C $(TGUI) &> $(srcdir)/functionCallAllocatesMultipleResources/functionCallAllocatesMultipleResourcesTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/functionCallAllocatesMultipleResources/functionCallAllocatesMultipleResourcesTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/functionCallAllocatesMultipleResources/functionCallAllocatesMultipleResourcesTestOutput.txt $(top_builddir)/projects/compass/functionCallAllocatesMultipleResourcesTestOutput.txt.00 ;\
		grep "FunctionCallAllocatesMultipleResources:" $(top_builddir)/projects/compass/functionCallAllocatesMultipleResourcesTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/functionCallAllocatesMultipleResources/functionCallAllocatesMultipleResourcesTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/functionCallAllocatesMultipleResourcesTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/functionCallAllocatesMultipleResources/functionCallAllocatesMultipleResourcesTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/functionCallAllocatesMultipleResources/functionCallAllocatesMultipleResourcesTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/functionCallAllocatesMultipleResources/functionCallAllocatesMultipleResourcesTestOutput.txt; then \
			if test -e $(srcdir)/functionCallAllocatesMultipleResources/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/functionCallAllocatesMultipleResources/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/functionCallAllocatesMultipleResources/functionCallAllocatesMultipleResourcesTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/functionCallAllocatesMultipleResources/functionCallAllocatesMultipleResourcesTest1.C $(TGUI) &> $(top_builddir)/projects/compass/functionCallAllocatesMultipleResourcesTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/functionCallAllocatesMultipleResourcesTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/functionCallAllocatesMultipleResourcesTestOutput.txt $(top_builddir)/projects/compass/functionCallAllocatesMultipleResourcesTestOutput.txt.00 ;\
		grep "FunctionCallAllocatesMultipleResources:" $(top_builddir)/projects/compass/functionCallAllocatesMultipleResourcesTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/functionCallAllocatesMultipleResourcesTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/functionCallAllocatesMultipleResourcesTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/functionCallAllocatesMultipleResourcesTestOutput.txt || ! test -s $(srcdir)/functionCallAllocatesMultipleResources/functionCallAllocatesMultipleResourcesTestOutput.txt ; then \
			if test -e $(srcdir)/functionCallAllocatesMultipleResources/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/functionCallAllocatesMultipleResources/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/functionCallAllocatesMultipleResourcesTestOutput.txt or $(srcdir)/functionCallAllocatesMultipleResources/functionCallAllocatesMultipleResourcesTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/functionCallAllocatesMultipleResourcesTestOutput.txt $(srcdir)/functionCallAllocatesMultipleResources/functionCallAllocatesMultipleResourcesTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/functionCallAllocatesMultipleResourcesTestOutput.txt and $(srcdir)/functionCallAllocatesMultipleResources/functionCallAllocatesMultipleResourcesTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/functionCallAllocatesMultipleResourcesTestOutput.txt ;\
	fi
	@echo "done testfunctionCallAllocatesMultipleResources"

testFunctionDefinitionPrototype: compassMain
	@echo -e "\nRunning testFunctionDefinitionPrototype"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/functionDefinitionPrototype/functionDefinitionPrototypeTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/functionDefinitionPrototype/functionDefinitionPrototypeTestOutput.txt; then \
		echo "creating $(srcdir)/functionDefinitionPrototype/functionDefinitionPrototypeTestOutput.txt" ;\
		./compassMain $(srcdir)/functionDefinitionPrototype/functionDefinitionPrototypeTest1.C $(TGUI) &> $(srcdir)/functionDefinitionPrototype/functionDefinitionPrototypeTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/functionDefinitionPrototype/functionDefinitionPrototypeTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/functionDefinitionPrototype/functionDefinitionPrototypeTestOutput.txt $(top_builddir)/projects/compass/functionDefinitionPrototypeTestOutput.txt.00 ;\
		grep "FunctionDefinitionPrototype:" $(top_builddir)/projects/compass/functionDefinitionPrototypeTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/functionDefinitionPrototype/functionDefinitionPrototypeTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/functionDefinitionPrototypeTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/functionDefinitionPrototype/functionDefinitionPrototypeTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/functionDefinitionPrototype/functionDefinitionPrototypeTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/functionDefinitionPrototype/functionDefinitionPrototypeTestOutput.txt; then \
			if test -e $(srcdir)/functionDefinitionPrototype/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/functionDefinitionPrototype/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/functionDefinitionPrototype/functionDefinitionPrototypeTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/functionDefinitionPrototype/functionDefinitionPrototypeTest1.C $(TGUI) &> $(top_builddir)/projects/compass/functionDefinitionPrototypeTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/functionDefinitionPrototypeTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/functionDefinitionPrototypeTestOutput.txt $(top_builddir)/projects/compass/functionDefinitionPrototypeTestOutput.txt.00 ;\
		grep "FunctionDefinitionPrototype:" $(top_builddir)/projects/compass/functionDefinitionPrototypeTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/functionDefinitionPrototypeTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/functionDefinitionPrototypeTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/functionDefinitionPrototypeTestOutput.txt || ! test -s $(srcdir)/functionDefinitionPrototype/functionDefinitionPrototypeTestOutput.txt ; then \
			if test -e $(srcdir)/functionDefinitionPrototype/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/functionDefinitionPrototype/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/functionDefinitionPrototypeTestOutput.txt or $(srcdir)/functionDefinitionPrototype/functionDefinitionPrototypeTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/functionDefinitionPrototypeTestOutput.txt $(srcdir)/functionDefinitionPrototype/functionDefinitionPrototypeTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/functionDefinitionPrototypeTestOutput.txt and $(srcdir)/functionDefinitionPrototype/functionDefinitionPrototypeTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/functionDefinitionPrototypeTestOutput.txt ;\
	fi
	@echo "done testfunctionDefinitionPrototype"

testFunctionDocumentation: compassMain
	@echo -e "\nRunning testFunctionDocumentation"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/functionDocumentation/functionDocumentationTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/functionDocumentation/functionDocumentationTestOutput.txt; then \
		echo "creating $(srcdir)/functionDocumentation/functionDocumentationTestOutput.txt" ;\
		./compassMain $(srcdir)/functionDocumentation/functionDocumentationTest1.C $(TGUI) &> $(srcdir)/functionDocumentation/functionDocumentationTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/functionDocumentation/functionDocumentationTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/functionDocumentation/functionDocumentationTestOutput.txt $(top_builddir)/projects/compass/functionDocumentationTestOutput.txt.00 ;\
		grep "FunctionDocumentation:" $(top_builddir)/projects/compass/functionDocumentationTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/functionDocumentation/functionDocumentationTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/functionDocumentationTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/functionDocumentation/functionDocumentationTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/functionDocumentation/functionDocumentationTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/functionDocumentation/functionDocumentationTestOutput.txt; then \
			if test -e $(srcdir)/functionDocumentation/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/functionDocumentation/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/functionDocumentation/functionDocumentationTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/functionDocumentation/functionDocumentationTest1.C $(TGUI) &> $(top_builddir)/projects/compass/functionDocumentationTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/functionDocumentationTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/functionDocumentationTestOutput.txt $(top_builddir)/projects/compass/functionDocumentationTestOutput.txt.00 ;\
		grep "FunctionDocumentation:" $(top_builddir)/projects/compass/functionDocumentationTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/functionDocumentationTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/functionDocumentationTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/functionDocumentationTestOutput.txt || ! test -s $(srcdir)/functionDocumentation/functionDocumentationTestOutput.txt ; then \
			if test -e $(srcdir)/functionDocumentation/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/functionDocumentation/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/functionDocumentationTestOutput.txt or $(srcdir)/functionDocumentation/functionDocumentationTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/functionDocumentationTestOutput.txt $(srcdir)/functionDocumentation/functionDocumentationTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/functionDocumentationTestOutput.txt and $(srcdir)/functionDocumentation/functionDocumentationTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/functionDocumentationTestOutput.txt ;\
	fi
	@echo "done testfunctionDocumentation"

testInductionVariableUpdate: compassMain
	@echo -e "\nRunning testInductionVariableUpdate"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/inductionVariableUpdate/inductionVariableUpdateTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/inductionVariableUpdate/inductionVariableUpdateTestOutput.txt; then \
		echo "creating $(srcdir)/inductionVariableUpdate/inductionVariableUpdateTestOutput.txt" ;\
		./compassMain $(srcdir)/inductionVariableUpdate/inductionVariableUpdateTest1.C $(TGUI) &> $(srcdir)/inductionVariableUpdate/inductionVariableUpdateTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/inductionVariableUpdate/inductionVariableUpdateTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/inductionVariableUpdate/inductionVariableUpdateTestOutput.txt $(top_builddir)/projects/compass/inductionVariableUpdateTestOutput.txt.00 ;\
		grep "InductionVariableUpdate:" $(top_builddir)/projects/compass/inductionVariableUpdateTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/inductionVariableUpdate/inductionVariableUpdateTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/inductionVariableUpdateTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/inductionVariableUpdate/inductionVariableUpdateTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/inductionVariableUpdate/inductionVariableUpdateTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/inductionVariableUpdate/inductionVariableUpdateTestOutput.txt; then \
			if test -e $(srcdir)/inductionVariableUpdate/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/inductionVariableUpdate/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/inductionVariableUpdate/inductionVariableUpdateTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/inductionVariableUpdate/inductionVariableUpdateTest1.C $(TGUI) &> $(top_builddir)/projects/compass/inductionVariableUpdateTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/inductionVariableUpdateTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/inductionVariableUpdateTestOutput.txt $(top_builddir)/projects/compass/inductionVariableUpdateTestOutput.txt.00 ;\
		grep "InductionVariableUpdate:" $(top_builddir)/projects/compass/inductionVariableUpdateTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/inductionVariableUpdateTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/inductionVariableUpdateTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/inductionVariableUpdateTestOutput.txt || ! test -s $(srcdir)/inductionVariableUpdate/inductionVariableUpdateTestOutput.txt ; then \
			if test -e $(srcdir)/inductionVariableUpdate/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/inductionVariableUpdate/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/inductionVariableUpdateTestOutput.txt or $(srcdir)/inductionVariableUpdate/inductionVariableUpdateTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/inductionVariableUpdateTestOutput.txt $(srcdir)/inductionVariableUpdate/inductionVariableUpdateTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/inductionVariableUpdateTestOutput.txt and $(srcdir)/inductionVariableUpdate/inductionVariableUpdateTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/inductionVariableUpdateTestOutput.txt ;\
	fi
	@echo "done testinductionVariableUpdate"

testInternalDataSharing: compassMain
	@echo -e "\nRunning testInternalDataSharing"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/internalDataSharing/internalDataSharingTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/internalDataSharing/internalDataSharingTestOutput.txt; then \
		echo "creating $(srcdir)/internalDataSharing/internalDataSharingTestOutput.txt" ;\
		./compassMain $(srcdir)/internalDataSharing/internalDataSharingTest1.C $(TGUI) &> $(srcdir)/internalDataSharing/internalDataSharingTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/internalDataSharing/internalDataSharingTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/internalDataSharing/internalDataSharingTestOutput.txt $(top_builddir)/projects/compass/internalDataSharingTestOutput.txt.00 ;\
		grep "InternalDataSharing:" $(top_builddir)/projects/compass/internalDataSharingTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/internalDataSharing/internalDataSharingTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/internalDataSharingTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/internalDataSharing/internalDataSharingTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/internalDataSharing/internalDataSharingTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/internalDataSharing/internalDataSharingTestOutput.txt; then \
			if test -e $(srcdir)/internalDataSharing/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/internalDataSharing/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/internalDataSharing/internalDataSharingTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/internalDataSharing/internalDataSharingTest1.C $(TGUI) &> $(top_builddir)/projects/compass/internalDataSharingTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/internalDataSharingTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/internalDataSharingTestOutput.txt $(top_builddir)/projects/compass/internalDataSharingTestOutput.txt.00 ;\
		grep "InternalDataSharing:" $(top_builddir)/projects/compass/internalDataSharingTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/internalDataSharingTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/internalDataSharingTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/internalDataSharingTestOutput.txt || ! test -s $(srcdir)/internalDataSharing/internalDataSharingTestOutput.txt ; then \
			if test -e $(srcdir)/internalDataSharing/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/internalDataSharing/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/internalDataSharingTestOutput.txt or $(srcdir)/internalDataSharing/internalDataSharingTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/internalDataSharingTestOutput.txt $(srcdir)/internalDataSharing/internalDataSharingTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/internalDataSharingTestOutput.txt and $(srcdir)/internalDataSharing/internalDataSharingTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/internalDataSharingTestOutput.txt ;\
	fi
	@echo "done testinternalDataSharing"

testLocalizedVariables: compassMain
	@echo -e "\nRunning testLocalizedVariables"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/localizedVariables/localizedVariablesTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/localizedVariables/localizedVariablesTestOutput.txt; then \
		echo "creating $(srcdir)/localizedVariables/localizedVariablesTestOutput.txt" ;\
		./compassMain $(srcdir)/localizedVariables/localizedVariablesTest1.C $(TGUI) &> $(srcdir)/localizedVariables/localizedVariablesTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/localizedVariables/localizedVariablesTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/localizedVariables/localizedVariablesTestOutput.txt $(top_builddir)/projects/compass/localizedVariablesTestOutput.txt.00 ;\
		grep "LocalizedVariables:" $(top_builddir)/projects/compass/localizedVariablesTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/localizedVariables/localizedVariablesTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/localizedVariablesTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/localizedVariables/localizedVariablesTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/localizedVariables/localizedVariablesTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/localizedVariables/localizedVariablesTestOutput.txt; then \
			if test -e $(srcdir)/localizedVariables/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/localizedVariables/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/localizedVariables/localizedVariablesTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/localizedVariables/localizedVariablesTest1.C $(TGUI) &> $(top_builddir)/projects/compass/localizedVariablesTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/localizedVariablesTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/localizedVariablesTestOutput.txt $(top_builddir)/projects/compass/localizedVariablesTestOutput.txt.00 ;\
		grep "LocalizedVariables:" $(top_builddir)/projects/compass/localizedVariablesTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/localizedVariablesTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/localizedVariablesTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/localizedVariablesTestOutput.txt || ! test -s $(srcdir)/localizedVariables/localizedVariablesTestOutput.txt ; then \
			if test -e $(srcdir)/localizedVariables/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/localizedVariables/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/localizedVariablesTestOutput.txt or $(srcdir)/localizedVariables/localizedVariablesTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/localizedVariablesTestOutput.txt $(srcdir)/localizedVariables/localizedVariablesTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/localizedVariablesTestOutput.txt and $(srcdir)/localizedVariables/localizedVariablesTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/localizedVariablesTestOutput.txt ;\
	fi
	@echo "done testlocalizedVariables"

testLocPerFunction: compassMain
	@echo -e "\nRunning testLocPerFunction"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/locPerFunction/locPerFunctionTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/locPerFunction/locPerFunctionTestOutput.txt; then \
		echo "creating $(srcdir)/locPerFunction/locPerFunctionTestOutput.txt" ;\
		./compassMain $(srcdir)/locPerFunction/locPerFunctionTest1.C $(TGUI) &> $(srcdir)/locPerFunction/locPerFunctionTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/locPerFunction/locPerFunctionTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/locPerFunction/locPerFunctionTestOutput.txt $(top_builddir)/projects/compass/locPerFunctionTestOutput.txt.00 ;\
		grep "LocPerFunction:" $(top_builddir)/projects/compass/locPerFunctionTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/locPerFunction/locPerFunctionTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/locPerFunctionTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/locPerFunction/locPerFunctionTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/locPerFunction/locPerFunctionTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/locPerFunction/locPerFunctionTestOutput.txt; then \
			if test -e $(srcdir)/locPerFunction/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/locPerFunction/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/locPerFunction/locPerFunctionTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/locPerFunction/locPerFunctionTest1.C $(TGUI) &> $(top_builddir)/projects/compass/locPerFunctionTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/locPerFunctionTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/locPerFunctionTestOutput.txt $(top_builddir)/projects/compass/locPerFunctionTestOutput.txt.00 ;\
		grep "LocPerFunction:" $(top_builddir)/projects/compass/locPerFunctionTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/locPerFunctionTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/locPerFunctionTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/locPerFunctionTestOutput.txt || ! test -s $(srcdir)/locPerFunction/locPerFunctionTestOutput.txt ; then \
			if test -e $(srcdir)/locPerFunction/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/locPerFunction/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/locPerFunctionTestOutput.txt or $(srcdir)/locPerFunction/locPerFunctionTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/locPerFunctionTestOutput.txt $(srcdir)/locPerFunction/locPerFunctionTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/locPerFunctionTestOutput.txt and $(srcdir)/locPerFunction/locPerFunctionTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/locPerFunctionTestOutput.txt ;\
	fi
	@echo "done testlocPerFunction"

testLowerRangeLimit: compassMain
	@echo -e "\nRunning testLowerRangeLimit"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/lowerRangeLimit/lowerRangeLimitTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/lowerRangeLimit/lowerRangeLimitTestOutput.txt; then \
		echo "creating $(srcdir)/lowerRangeLimit/lowerRangeLimitTestOutput.txt" ;\
		./compassMain $(srcdir)/lowerRangeLimit/lowerRangeLimitTest1.C $(TGUI) &> $(srcdir)/lowerRangeLimit/lowerRangeLimitTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/lowerRangeLimit/lowerRangeLimitTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/lowerRangeLimit/lowerRangeLimitTestOutput.txt $(top_builddir)/projects/compass/lowerRangeLimitTestOutput.txt.00 ;\
		grep "LowerRangeLimit:" $(top_builddir)/projects/compass/lowerRangeLimitTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/lowerRangeLimit/lowerRangeLimitTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/lowerRangeLimitTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/lowerRangeLimit/lowerRangeLimitTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/lowerRangeLimit/lowerRangeLimitTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/lowerRangeLimit/lowerRangeLimitTestOutput.txt; then \
			if test -e $(srcdir)/lowerRangeLimit/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/lowerRangeLimit/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/lowerRangeLimit/lowerRangeLimitTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/lowerRangeLimit/lowerRangeLimitTest1.C $(TGUI) &> $(top_builddir)/projects/compass/lowerRangeLimitTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/lowerRangeLimitTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/lowerRangeLimitTestOutput.txt $(top_builddir)/projects/compass/lowerRangeLimitTestOutput.txt.00 ;\
		grep "LowerRangeLimit:" $(top_builddir)/projects/compass/lowerRangeLimitTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/lowerRangeLimitTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/lowerRangeLimitTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/lowerRangeLimitTestOutput.txt || ! test -s $(srcdir)/lowerRangeLimit/lowerRangeLimitTestOutput.txt ; then \
			if test -e $(srcdir)/lowerRangeLimit/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/lowerRangeLimit/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/lowerRangeLimitTestOutput.txt or $(srcdir)/lowerRangeLimit/lowerRangeLimitTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/lowerRangeLimitTestOutput.txt $(srcdir)/lowerRangeLimit/lowerRangeLimitTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/lowerRangeLimitTestOutput.txt and $(srcdir)/lowerRangeLimit/lowerRangeLimitTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/lowerRangeLimitTestOutput.txt ;\
	fi
	@echo "done testlowerRangeLimit"

testMagicNumber: compassMain
	@echo -e "\nRunning testMagicNumber"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/magicNumber/magicNumberTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/magicNumber/magicNumberTestOutput.txt; then \
		echo "creating $(srcdir)/magicNumber/magicNumberTestOutput.txt" ;\
		./compassMain $(srcdir)/magicNumber/magicNumberTest1.C $(TGUI) &> $(srcdir)/magicNumber/magicNumberTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/magicNumber/magicNumberTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/magicNumber/magicNumberTestOutput.txt $(top_builddir)/projects/compass/magicNumberTestOutput.txt.00 ;\
		grep "MagicNumber:" $(top_builddir)/projects/compass/magicNumberTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/magicNumber/magicNumberTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/magicNumberTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/magicNumber/magicNumberTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/magicNumber/magicNumberTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/magicNumber/magicNumberTestOutput.txt; then \
			if test -e $(srcdir)/magicNumber/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/magicNumber/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/magicNumber/magicNumberTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/magicNumber/magicNumberTest1.C $(TGUI) &> $(top_builddir)/projects/compass/magicNumberTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/magicNumberTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/magicNumberTestOutput.txt $(top_builddir)/projects/compass/magicNumberTestOutput.txt.00 ;\
		grep "MagicNumber:" $(top_builddir)/projects/compass/magicNumberTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/magicNumberTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/magicNumberTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/magicNumberTestOutput.txt || ! test -s $(srcdir)/magicNumber/magicNumberTestOutput.txt ; then \
			if test -e $(srcdir)/magicNumber/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/magicNumber/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/magicNumberTestOutput.txt or $(srcdir)/magicNumber/magicNumberTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/magicNumberTestOutput.txt $(srcdir)/magicNumber/magicNumberTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/magicNumberTestOutput.txt and $(srcdir)/magicNumber/magicNumberTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/magicNumberTestOutput.txt ;\
	fi
	@echo "done testmagicNumber"

testMallocReturnValueUsedInIfStmt: compassMain
	@echo -e "\nRunning testMallocReturnValueUsedInIfStmt"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/mallocReturnValueUsedInIfStmt/mallocReturnValueUsedInIfStmtTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/mallocReturnValueUsedInIfStmt/mallocReturnValueUsedInIfStmtTestOutput.txt; then \
		echo "creating $(srcdir)/mallocReturnValueUsedInIfStmt/mallocReturnValueUsedInIfStmtTestOutput.txt" ;\
		./compassMain $(srcdir)/mallocReturnValueUsedInIfStmt/mallocReturnValueUsedInIfStmtTest1.C $(TGUI) &> $(srcdir)/mallocReturnValueUsedInIfStmt/mallocReturnValueUsedInIfStmtTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/mallocReturnValueUsedInIfStmt/mallocReturnValueUsedInIfStmtTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/mallocReturnValueUsedInIfStmt/mallocReturnValueUsedInIfStmtTestOutput.txt $(top_builddir)/projects/compass/mallocReturnValueUsedInIfStmtTestOutput.txt.00 ;\
		grep "MallocReturnValueUsedInIfStmt:" $(top_builddir)/projects/compass/mallocReturnValueUsedInIfStmtTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/mallocReturnValueUsedInIfStmt/mallocReturnValueUsedInIfStmtTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/mallocReturnValueUsedInIfStmtTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/mallocReturnValueUsedInIfStmt/mallocReturnValueUsedInIfStmtTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/mallocReturnValueUsedInIfStmt/mallocReturnValueUsedInIfStmtTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/mallocReturnValueUsedInIfStmt/mallocReturnValueUsedInIfStmtTestOutput.txt; then \
			if test -e $(srcdir)/mallocReturnValueUsedInIfStmt/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/mallocReturnValueUsedInIfStmt/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/mallocReturnValueUsedInIfStmt/mallocReturnValueUsedInIfStmtTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/mallocReturnValueUsedInIfStmt/mallocReturnValueUsedInIfStmtTest1.C $(TGUI) &> $(top_builddir)/projects/compass/mallocReturnValueUsedInIfStmtTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/mallocReturnValueUsedInIfStmtTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/mallocReturnValueUsedInIfStmtTestOutput.txt $(top_builddir)/projects/compass/mallocReturnValueUsedInIfStmtTestOutput.txt.00 ;\
		grep "MallocReturnValueUsedInIfStmt:" $(top_builddir)/projects/compass/mallocReturnValueUsedInIfStmtTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/mallocReturnValueUsedInIfStmtTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/mallocReturnValueUsedInIfStmtTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/mallocReturnValueUsedInIfStmtTestOutput.txt || ! test -s $(srcdir)/mallocReturnValueUsedInIfStmt/mallocReturnValueUsedInIfStmtTestOutput.txt ; then \
			if test -e $(srcdir)/mallocReturnValueUsedInIfStmt/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/mallocReturnValueUsedInIfStmt/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/mallocReturnValueUsedInIfStmtTestOutput.txt or $(srcdir)/mallocReturnValueUsedInIfStmt/mallocReturnValueUsedInIfStmtTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/mallocReturnValueUsedInIfStmtTestOutput.txt $(srcdir)/mallocReturnValueUsedInIfStmt/mallocReturnValueUsedInIfStmtTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/mallocReturnValueUsedInIfStmtTestOutput.txt and $(srcdir)/mallocReturnValueUsedInIfStmt/mallocReturnValueUsedInIfStmtTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/mallocReturnValueUsedInIfStmtTestOutput.txt ;\
	fi
	@echo "done testmallocReturnValueUsedInIfStmt"

testMultiplePublicInheritance: compassMain
	@echo -e "\nRunning testMultiplePublicInheritance"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/multiplePublicInheritance/multiplePublicInheritanceTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/multiplePublicInheritance/multiplePublicInheritanceTestOutput.txt; then \
		echo "creating $(srcdir)/multiplePublicInheritance/multiplePublicInheritanceTestOutput.txt" ;\
		./compassMain $(srcdir)/multiplePublicInheritance/multiplePublicInheritanceTest1.C $(TGUI) &> $(srcdir)/multiplePublicInheritance/multiplePublicInheritanceTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/multiplePublicInheritance/multiplePublicInheritanceTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/multiplePublicInheritance/multiplePublicInheritanceTestOutput.txt $(top_builddir)/projects/compass/multiplePublicInheritanceTestOutput.txt.00 ;\
		grep "MultiplePublicInheritance:" $(top_builddir)/projects/compass/multiplePublicInheritanceTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/multiplePublicInheritance/multiplePublicInheritanceTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/multiplePublicInheritanceTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/multiplePublicInheritance/multiplePublicInheritanceTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/multiplePublicInheritance/multiplePublicInheritanceTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/multiplePublicInheritance/multiplePublicInheritanceTestOutput.txt; then \
			if test -e $(srcdir)/multiplePublicInheritance/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/multiplePublicInheritance/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/multiplePublicInheritance/multiplePublicInheritanceTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/multiplePublicInheritance/multiplePublicInheritanceTest1.C $(TGUI) &> $(top_builddir)/projects/compass/multiplePublicInheritanceTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/multiplePublicInheritanceTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/multiplePublicInheritanceTestOutput.txt $(top_builddir)/projects/compass/multiplePublicInheritanceTestOutput.txt.00 ;\
		grep "MultiplePublicInheritance:" $(top_builddir)/projects/compass/multiplePublicInheritanceTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/multiplePublicInheritanceTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/multiplePublicInheritanceTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/multiplePublicInheritanceTestOutput.txt || ! test -s $(srcdir)/multiplePublicInheritance/multiplePublicInheritanceTestOutput.txt ; then \
			if test -e $(srcdir)/multiplePublicInheritance/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/multiplePublicInheritance/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/multiplePublicInheritanceTestOutput.txt or $(srcdir)/multiplePublicInheritance/multiplePublicInheritanceTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/multiplePublicInheritanceTestOutput.txt $(srcdir)/multiplePublicInheritance/multiplePublicInheritanceTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/multiplePublicInheritanceTestOutput.txt and $(srcdir)/multiplePublicInheritance/multiplePublicInheritanceTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/multiplePublicInheritanceTestOutput.txt ;\
	fi
	@echo "done testmultiplePublicInheritance"

testNameAllParameters: compassMain
	@echo -e "\nRunning testNameAllParameters"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/nameAllParameters/nameAllParametersTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/nameAllParameters/nameAllParametersTestOutput.txt; then \
		echo "creating $(srcdir)/nameAllParameters/nameAllParametersTestOutput.txt" ;\
		./compassMain $(srcdir)/nameAllParameters/nameAllParametersTest1.C $(TGUI) &> $(srcdir)/nameAllParameters/nameAllParametersTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/nameAllParameters/nameAllParametersTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/nameAllParameters/nameAllParametersTestOutput.txt $(top_builddir)/projects/compass/nameAllParametersTestOutput.txt.00 ;\
		grep "NameAllParameters:" $(top_builddir)/projects/compass/nameAllParametersTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/nameAllParameters/nameAllParametersTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/nameAllParametersTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/nameAllParameters/nameAllParametersTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/nameAllParameters/nameAllParametersTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/nameAllParameters/nameAllParametersTestOutput.txt; then \
			if test -e $(srcdir)/nameAllParameters/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/nameAllParameters/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/nameAllParameters/nameAllParametersTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/nameAllParameters/nameAllParametersTest1.C $(TGUI) &> $(top_builddir)/projects/compass/nameAllParametersTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/nameAllParametersTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/nameAllParametersTestOutput.txt $(top_builddir)/projects/compass/nameAllParametersTestOutput.txt.00 ;\
		grep "NameAllParameters:" $(top_builddir)/projects/compass/nameAllParametersTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/nameAllParametersTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/nameAllParametersTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/nameAllParametersTestOutput.txt || ! test -s $(srcdir)/nameAllParameters/nameAllParametersTestOutput.txt ; then \
			if test -e $(srcdir)/nameAllParameters/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/nameAllParameters/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/nameAllParametersTestOutput.txt or $(srcdir)/nameAllParameters/nameAllParametersTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/nameAllParametersTestOutput.txt $(srcdir)/nameAllParameters/nameAllParametersTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/nameAllParametersTestOutput.txt and $(srcdir)/nameAllParameters/nameAllParametersTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/nameAllParametersTestOutput.txt ;\
	fi
	@echo "done testnameAllParameters"

testNameConsistency: compassMain
	@echo -e "\nRunning testNameConsistency"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/nameConsistency/nameConsistencyTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/nameConsistency/nameConsistencyTestOutput.txt; then \
		echo "creating $(srcdir)/nameConsistency/nameConsistencyTestOutput.txt" ;\
		./compassMain $(srcdir)/nameConsistency/nameConsistencyTest1.C $(TGUI) &> $(srcdir)/nameConsistency/nameConsistencyTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/nameConsistency/nameConsistencyTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/nameConsistency/nameConsistencyTestOutput.txt $(top_builddir)/projects/compass/nameConsistencyTestOutput.txt.00 ;\
		grep "NameConsistency:" $(top_builddir)/projects/compass/nameConsistencyTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/nameConsistency/nameConsistencyTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/nameConsistencyTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/nameConsistency/nameConsistencyTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/nameConsistency/nameConsistencyTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/nameConsistency/nameConsistencyTestOutput.txt; then \
			if test -e $(srcdir)/nameConsistency/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/nameConsistency/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/nameConsistency/nameConsistencyTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/nameConsistency/nameConsistencyTest1.C $(TGUI) &> $(top_builddir)/projects/compass/nameConsistencyTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/nameConsistencyTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/nameConsistencyTestOutput.txt $(top_builddir)/projects/compass/nameConsistencyTestOutput.txt.00 ;\
		grep "NameConsistency:" $(top_builddir)/projects/compass/nameConsistencyTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/nameConsistencyTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/nameConsistencyTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/nameConsistencyTestOutput.txt || ! test -s $(srcdir)/nameConsistency/nameConsistencyTestOutput.txt ; then \
			if test -e $(srcdir)/nameConsistency/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/nameConsistency/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/nameConsistencyTestOutput.txt or $(srcdir)/nameConsistency/nameConsistencyTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/nameConsistencyTestOutput.txt $(srcdir)/nameConsistency/nameConsistencyTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/nameConsistencyTestOutput.txt and $(srcdir)/nameConsistency/nameConsistencyTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/nameConsistencyTestOutput.txt ;\
	fi
	@echo "done testnameConsistency"

testNewDelete: compassMain
	@echo -e "\nRunning testNewDelete"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/newDelete/newDeleteTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/newDelete/newDeleteTestOutput.txt; then \
		echo "creating $(srcdir)/newDelete/newDeleteTestOutput.txt" ;\
		./compassMain $(srcdir)/newDelete/newDeleteTest1.C $(TGUI) &> $(srcdir)/newDelete/newDeleteTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/newDelete/newDeleteTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/newDelete/newDeleteTestOutput.txt $(top_builddir)/projects/compass/newDeleteTestOutput.txt.00 ;\
		grep "NewDelete:" $(top_builddir)/projects/compass/newDeleteTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/newDelete/newDeleteTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/newDeleteTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/newDelete/newDeleteTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/newDelete/newDeleteTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/newDelete/newDeleteTestOutput.txt; then \
			if test -e $(srcdir)/newDelete/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/newDelete/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/newDelete/newDeleteTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/newDelete/newDeleteTest1.C $(TGUI) &> $(top_builddir)/projects/compass/newDeleteTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/newDeleteTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/newDeleteTestOutput.txt $(top_builddir)/projects/compass/newDeleteTestOutput.txt.00 ;\
		grep "NewDelete:" $(top_builddir)/projects/compass/newDeleteTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/newDeleteTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/newDeleteTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/newDeleteTestOutput.txt || ! test -s $(srcdir)/newDelete/newDeleteTestOutput.txt ; then \
			if test -e $(srcdir)/newDelete/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/newDelete/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/newDeleteTestOutput.txt or $(srcdir)/newDelete/newDeleteTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/newDeleteTestOutput.txt $(srcdir)/newDelete/newDeleteTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/newDeleteTestOutput.txt and $(srcdir)/newDelete/newDeleteTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/newDeleteTestOutput.txt ;\
	fi
	@echo "done testnewDelete"

testNoExceptions: compassMain
	@echo -e "\nRunning testNoExceptions"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/noExceptions/noExceptionsTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/noExceptions/noExceptionsTestOutput.txt; then \
		echo "creating $(srcdir)/noExceptions/noExceptionsTestOutput.txt" ;\
		./compassMain $(srcdir)/noExceptions/noExceptionsTest1.C $(TGUI) &> $(srcdir)/noExceptions/noExceptionsTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/noExceptions/noExceptionsTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/noExceptions/noExceptionsTestOutput.txt $(top_builddir)/projects/compass/noExceptionsTestOutput.txt.00 ;\
		grep "NoExceptions:" $(top_builddir)/projects/compass/noExceptionsTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/noExceptions/noExceptionsTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/noExceptionsTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/noExceptions/noExceptionsTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/noExceptions/noExceptionsTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/noExceptions/noExceptionsTestOutput.txt; then \
			if test -e $(srcdir)/noExceptions/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/noExceptions/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/noExceptions/noExceptionsTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/noExceptions/noExceptionsTest1.C $(TGUI) &> $(top_builddir)/projects/compass/noExceptionsTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/noExceptionsTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/noExceptionsTestOutput.txt $(top_builddir)/projects/compass/noExceptionsTestOutput.txt.00 ;\
		grep "NoExceptions:" $(top_builddir)/projects/compass/noExceptionsTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/noExceptionsTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/noExceptionsTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/noExceptionsTestOutput.txt || ! test -s $(srcdir)/noExceptions/noExceptionsTestOutput.txt ; then \
			if test -e $(srcdir)/noExceptions/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/noExceptions/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/noExceptionsTestOutput.txt or $(srcdir)/noExceptions/noExceptionsTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/noExceptionsTestOutput.txt $(srcdir)/noExceptions/noExceptionsTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/noExceptionsTestOutput.txt and $(srcdir)/noExceptions/noExceptionsTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/noExceptionsTestOutput.txt ;\
	fi
	@echo "done testnoExceptions"

testNoExitInMpiCode: compassMain
	@echo -e "\nRunning testNoExitInMpiCode"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/noExitInMpiCode/noExitInMpiCodeTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/noExitInMpiCode/noExitInMpiCodeTestOutput.txt; then \
		echo "creating $(srcdir)/noExitInMpiCode/noExitInMpiCodeTestOutput.txt" ;\
		./compassMain $(srcdir)/noExitInMpiCode/noExitInMpiCodeTest1.C $(TGUI) &> $(srcdir)/noExitInMpiCode/noExitInMpiCodeTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/noExitInMpiCode/noExitInMpiCodeTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/noExitInMpiCode/noExitInMpiCodeTestOutput.txt $(top_builddir)/projects/compass/noExitInMpiCodeTestOutput.txt.00 ;\
		grep "NoExitInMpiCode:" $(top_builddir)/projects/compass/noExitInMpiCodeTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/noExitInMpiCode/noExitInMpiCodeTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/noExitInMpiCodeTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/noExitInMpiCode/noExitInMpiCodeTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/noExitInMpiCode/noExitInMpiCodeTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/noExitInMpiCode/noExitInMpiCodeTestOutput.txt; then \
			if test -e $(srcdir)/noExitInMpiCode/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/noExitInMpiCode/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/noExitInMpiCode/noExitInMpiCodeTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/noExitInMpiCode/noExitInMpiCodeTest1.C $(TGUI) &> $(top_builddir)/projects/compass/noExitInMpiCodeTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/noExitInMpiCodeTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/noExitInMpiCodeTestOutput.txt $(top_builddir)/projects/compass/noExitInMpiCodeTestOutput.txt.00 ;\
		grep "NoExitInMpiCode:" $(top_builddir)/projects/compass/noExitInMpiCodeTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/noExitInMpiCodeTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/noExitInMpiCodeTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/noExitInMpiCodeTestOutput.txt || ! test -s $(srcdir)/noExitInMpiCode/noExitInMpiCodeTestOutput.txt ; then \
			if test -e $(srcdir)/noExitInMpiCode/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/noExitInMpiCode/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/noExitInMpiCodeTestOutput.txt or $(srcdir)/noExitInMpiCode/noExitInMpiCodeTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/noExitInMpiCodeTestOutput.txt $(srcdir)/noExitInMpiCode/noExitInMpiCodeTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/noExitInMpiCodeTestOutput.txt and $(srcdir)/noExitInMpiCode/noExitInMpiCodeTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/noExitInMpiCodeTestOutput.txt ;\
	fi
	@echo "done testnoExitInMpiCode"

testNoGoto: compassMain
	@echo -e "\nRunning testNoGoto"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/noGoto/noGotoTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/noGoto/noGotoTestOutput.txt; then \
		echo "creating $(srcdir)/noGoto/noGotoTestOutput.txt" ;\
		./compassMain $(srcdir)/noGoto/noGotoTest1.C $(TGUI) &> $(srcdir)/noGoto/noGotoTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/noGoto/noGotoTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/noGoto/noGotoTestOutput.txt $(top_builddir)/projects/compass/noGotoTestOutput.txt.00 ;\
		grep "NoGoto:" $(top_builddir)/projects/compass/noGotoTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/noGoto/noGotoTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/noGotoTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/noGoto/noGotoTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/noGoto/noGotoTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/noGoto/noGotoTestOutput.txt; then \
			if test -e $(srcdir)/noGoto/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/noGoto/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/noGoto/noGotoTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/noGoto/noGotoTest1.C $(TGUI) &> $(top_builddir)/projects/compass/noGotoTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/noGotoTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/noGotoTestOutput.txt $(top_builddir)/projects/compass/noGotoTestOutput.txt.00 ;\
		grep "NoGoto:" $(top_builddir)/projects/compass/noGotoTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/noGotoTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/noGotoTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/noGotoTestOutput.txt || ! test -s $(srcdir)/noGoto/noGotoTestOutput.txt ; then \
			if test -e $(srcdir)/noGoto/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/noGoto/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/noGotoTestOutput.txt or $(srcdir)/noGoto/noGotoTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/noGotoTestOutput.txt $(srcdir)/noGoto/noGotoTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/noGotoTestOutput.txt and $(srcdir)/noGoto/noGotoTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/noGotoTestOutput.txt ;\
	fi
	@echo "done testnoGoto"

testNonAssociativeRelationalOperators: compassMain
	@echo -e "\nRunning testNonAssociativeRelationalOperators"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/nonAssociativeRelationalOperators/nonAssociativeRelationalOperatorsTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/nonAssociativeRelationalOperators/nonAssociativeRelationalOperatorsTestOutput.txt; then \
		echo "creating $(srcdir)/nonAssociativeRelationalOperators/nonAssociativeRelationalOperatorsTestOutput.txt" ;\
		./compassMain $(srcdir)/nonAssociativeRelationalOperators/nonAssociativeRelationalOperatorsTest1.C $(TGUI) &> $(srcdir)/nonAssociativeRelationalOperators/nonAssociativeRelationalOperatorsTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/nonAssociativeRelationalOperators/nonAssociativeRelationalOperatorsTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/nonAssociativeRelationalOperators/nonAssociativeRelationalOperatorsTestOutput.txt $(top_builddir)/projects/compass/nonAssociativeRelationalOperatorsTestOutput.txt.00 ;\
		grep "NonAssociativeRelationalOperators:" $(top_builddir)/projects/compass/nonAssociativeRelationalOperatorsTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/nonAssociativeRelationalOperators/nonAssociativeRelationalOperatorsTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/nonAssociativeRelationalOperatorsTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/nonAssociativeRelationalOperators/nonAssociativeRelationalOperatorsTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/nonAssociativeRelationalOperators/nonAssociativeRelationalOperatorsTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/nonAssociativeRelationalOperators/nonAssociativeRelationalOperatorsTestOutput.txt; then \
			if test -e $(srcdir)/nonAssociativeRelationalOperators/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/nonAssociativeRelationalOperators/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/nonAssociativeRelationalOperators/nonAssociativeRelationalOperatorsTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/nonAssociativeRelationalOperators/nonAssociativeRelationalOperatorsTest1.C $(TGUI) &> $(top_builddir)/projects/compass/nonAssociativeRelationalOperatorsTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/nonAssociativeRelationalOperatorsTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/nonAssociativeRelationalOperatorsTestOutput.txt $(top_builddir)/projects/compass/nonAssociativeRelationalOperatorsTestOutput.txt.00 ;\
		grep "NonAssociativeRelationalOperators:" $(top_builddir)/projects/compass/nonAssociativeRelationalOperatorsTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/nonAssociativeRelationalOperatorsTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/nonAssociativeRelationalOperatorsTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/nonAssociativeRelationalOperatorsTestOutput.txt || ! test -s $(srcdir)/nonAssociativeRelationalOperators/nonAssociativeRelationalOperatorsTestOutput.txt ; then \
			if test -e $(srcdir)/nonAssociativeRelationalOperators/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/nonAssociativeRelationalOperators/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/nonAssociativeRelationalOperatorsTestOutput.txt or $(srcdir)/nonAssociativeRelationalOperators/nonAssociativeRelationalOperatorsTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/nonAssociativeRelationalOperatorsTestOutput.txt $(srcdir)/nonAssociativeRelationalOperators/nonAssociativeRelationalOperatorsTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/nonAssociativeRelationalOperatorsTestOutput.txt and $(srcdir)/nonAssociativeRelationalOperators/nonAssociativeRelationalOperatorsTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/nonAssociativeRelationalOperatorsTestOutput.txt ;\
	fi
	@echo "done testnonAssociativeRelationalOperators"

testNonmemberFunctionInterfaceNamespace: compassMain
	@echo -e "\nRunning testNonmemberFunctionInterfaceNamespace"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/nonmemberFunctionInterfaceNamespace/nonmemberFunctionInterfaceNamespaceTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/nonmemberFunctionInterfaceNamespace/nonmemberFunctionInterfaceNamespaceTestOutput.txt; then \
		echo "creating $(srcdir)/nonmemberFunctionInterfaceNamespace/nonmemberFunctionInterfaceNamespaceTestOutput.txt" ;\
		./compassMain $(srcdir)/nonmemberFunctionInterfaceNamespace/nonmemberFunctionInterfaceNamespaceTest1.C $(TGUI) &> $(srcdir)/nonmemberFunctionInterfaceNamespace/nonmemberFunctionInterfaceNamespaceTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/nonmemberFunctionInterfaceNamespace/nonmemberFunctionInterfaceNamespaceTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/nonmemberFunctionInterfaceNamespace/nonmemberFunctionInterfaceNamespaceTestOutput.txt $(top_builddir)/projects/compass/nonmemberFunctionInterfaceNamespaceTestOutput.txt.00 ;\
		grep "NonmemberFunctionInterfaceNamespace:" $(top_builddir)/projects/compass/nonmemberFunctionInterfaceNamespaceTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/nonmemberFunctionInterfaceNamespace/nonmemberFunctionInterfaceNamespaceTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/nonmemberFunctionInterfaceNamespaceTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/nonmemberFunctionInterfaceNamespace/nonmemberFunctionInterfaceNamespaceTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/nonmemberFunctionInterfaceNamespace/nonmemberFunctionInterfaceNamespaceTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/nonmemberFunctionInterfaceNamespace/nonmemberFunctionInterfaceNamespaceTestOutput.txt; then \
			if test -e $(srcdir)/nonmemberFunctionInterfaceNamespace/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/nonmemberFunctionInterfaceNamespace/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/nonmemberFunctionInterfaceNamespace/nonmemberFunctionInterfaceNamespaceTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/nonmemberFunctionInterfaceNamespace/nonmemberFunctionInterfaceNamespaceTest1.C $(TGUI) &> $(top_builddir)/projects/compass/nonmemberFunctionInterfaceNamespaceTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/nonmemberFunctionInterfaceNamespaceTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/nonmemberFunctionInterfaceNamespaceTestOutput.txt $(top_builddir)/projects/compass/nonmemberFunctionInterfaceNamespaceTestOutput.txt.00 ;\
		grep "NonmemberFunctionInterfaceNamespace:" $(top_builddir)/projects/compass/nonmemberFunctionInterfaceNamespaceTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/nonmemberFunctionInterfaceNamespaceTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/nonmemberFunctionInterfaceNamespaceTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/nonmemberFunctionInterfaceNamespaceTestOutput.txt || ! test -s $(srcdir)/nonmemberFunctionInterfaceNamespace/nonmemberFunctionInterfaceNamespaceTestOutput.txt ; then \
			if test -e $(srcdir)/nonmemberFunctionInterfaceNamespace/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/nonmemberFunctionInterfaceNamespace/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/nonmemberFunctionInterfaceNamespaceTestOutput.txt or $(srcdir)/nonmemberFunctionInterfaceNamespace/nonmemberFunctionInterfaceNamespaceTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/nonmemberFunctionInterfaceNamespaceTestOutput.txt $(srcdir)/nonmemberFunctionInterfaceNamespace/nonmemberFunctionInterfaceNamespaceTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/nonmemberFunctionInterfaceNamespaceTestOutput.txt and $(srcdir)/nonmemberFunctionInterfaceNamespace/nonmemberFunctionInterfaceNamespaceTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/nonmemberFunctionInterfaceNamespaceTestOutput.txt ;\
	fi
	@echo "done testnonmemberFunctionInterfaceNamespace"

testNonStandardTypeRefArgs: compassMain
	@echo -e "\nRunning testNonStandardTypeRefArgs"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/nonStandardTypeRefArgs/nonStandardTypeRefArgsTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/nonStandardTypeRefArgs/nonStandardTypeRefArgsTestOutput.txt; then \
		echo "creating $(srcdir)/nonStandardTypeRefArgs/nonStandardTypeRefArgsTestOutput.txt" ;\
		./compassMain $(srcdir)/nonStandardTypeRefArgs/nonStandardTypeRefArgsTest1.C $(TGUI) &> $(srcdir)/nonStandardTypeRefArgs/nonStandardTypeRefArgsTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/nonStandardTypeRefArgs/nonStandardTypeRefArgsTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/nonStandardTypeRefArgs/nonStandardTypeRefArgsTestOutput.txt $(top_builddir)/projects/compass/nonStandardTypeRefArgsTestOutput.txt.00 ;\
		grep "NonStandardTypeRefArgs:" $(top_builddir)/projects/compass/nonStandardTypeRefArgsTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/nonStandardTypeRefArgs/nonStandardTypeRefArgsTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/nonStandardTypeRefArgsTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/nonStandardTypeRefArgs/nonStandardTypeRefArgsTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/nonStandardTypeRefArgs/nonStandardTypeRefArgsTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/nonStandardTypeRefArgs/nonStandardTypeRefArgsTestOutput.txt; then \
			if test -e $(srcdir)/nonStandardTypeRefArgs/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/nonStandardTypeRefArgs/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/nonStandardTypeRefArgs/nonStandardTypeRefArgsTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/nonStandardTypeRefArgs/nonStandardTypeRefArgsTest1.C $(TGUI) &> $(top_builddir)/projects/compass/nonStandardTypeRefArgsTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/nonStandardTypeRefArgsTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/nonStandardTypeRefArgsTestOutput.txt $(top_builddir)/projects/compass/nonStandardTypeRefArgsTestOutput.txt.00 ;\
		grep "NonStandardTypeRefArgs:" $(top_builddir)/projects/compass/nonStandardTypeRefArgsTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/nonStandardTypeRefArgsTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/nonStandardTypeRefArgsTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/nonStandardTypeRefArgsTestOutput.txt || ! test -s $(srcdir)/nonStandardTypeRefArgs/nonStandardTypeRefArgsTestOutput.txt ; then \
			if test -e $(srcdir)/nonStandardTypeRefArgs/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/nonStandardTypeRefArgs/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/nonStandardTypeRefArgsTestOutput.txt or $(srcdir)/nonStandardTypeRefArgs/nonStandardTypeRefArgsTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/nonStandardTypeRefArgsTestOutput.txt $(srcdir)/nonStandardTypeRefArgs/nonStandardTypeRefArgsTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/nonStandardTypeRefArgsTestOutput.txt and $(srcdir)/nonStandardTypeRefArgs/nonStandardTypeRefArgsTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/nonStandardTypeRefArgsTestOutput.txt ;\
	fi
	@echo "done testnonStandardTypeRefArgs"

testNonStandardTypeRefReturns: compassMain
	@echo -e "\nRunning testNonStandardTypeRefReturns"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/nonStandardTypeRefReturns/nonStandardTypeRefReturnsTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/nonStandardTypeRefReturns/nonStandardTypeRefReturnsTestOutput.txt; then \
		echo "creating $(srcdir)/nonStandardTypeRefReturns/nonStandardTypeRefReturnsTestOutput.txt" ;\
		./compassMain $(srcdir)/nonStandardTypeRefReturns/nonStandardTypeRefReturnsTest1.C $(TGUI) &> $(srcdir)/nonStandardTypeRefReturns/nonStandardTypeRefReturnsTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/nonStandardTypeRefReturns/nonStandardTypeRefReturnsTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/nonStandardTypeRefReturns/nonStandardTypeRefReturnsTestOutput.txt $(top_builddir)/projects/compass/nonStandardTypeRefReturnsTestOutput.txt.00 ;\
		grep "NonStandardTypeRefReturns:" $(top_builddir)/projects/compass/nonStandardTypeRefReturnsTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/nonStandardTypeRefReturns/nonStandardTypeRefReturnsTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/nonStandardTypeRefReturnsTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/nonStandardTypeRefReturns/nonStandardTypeRefReturnsTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/nonStandardTypeRefReturns/nonStandardTypeRefReturnsTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/nonStandardTypeRefReturns/nonStandardTypeRefReturnsTestOutput.txt; then \
			if test -e $(srcdir)/nonStandardTypeRefReturns/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/nonStandardTypeRefReturns/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/nonStandardTypeRefReturns/nonStandardTypeRefReturnsTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/nonStandardTypeRefReturns/nonStandardTypeRefReturnsTest1.C $(TGUI) &> $(top_builddir)/projects/compass/nonStandardTypeRefReturnsTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/nonStandardTypeRefReturnsTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/nonStandardTypeRefReturnsTestOutput.txt $(top_builddir)/projects/compass/nonStandardTypeRefReturnsTestOutput.txt.00 ;\
		grep "NonStandardTypeRefReturns:" $(top_builddir)/projects/compass/nonStandardTypeRefReturnsTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/nonStandardTypeRefReturnsTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/nonStandardTypeRefReturnsTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/nonStandardTypeRefReturnsTestOutput.txt || ! test -s $(srcdir)/nonStandardTypeRefReturns/nonStandardTypeRefReturnsTestOutput.txt ; then \
			if test -e $(srcdir)/nonStandardTypeRefReturns/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/nonStandardTypeRefReturns/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/nonStandardTypeRefReturnsTestOutput.txt or $(srcdir)/nonStandardTypeRefReturns/nonStandardTypeRefReturnsTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/nonStandardTypeRefReturnsTestOutput.txt $(srcdir)/nonStandardTypeRefReturns/nonStandardTypeRefReturnsTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/nonStandardTypeRefReturnsTestOutput.txt and $(srcdir)/nonStandardTypeRefReturns/nonStandardTypeRefReturnsTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/nonStandardTypeRefReturnsTestOutput.txt ;\
	fi
	@echo "done testnonStandardTypeRefReturns"

testNonVirtualRedefinition: compassMain
	@echo -e "\nRunning testNonVirtualRedefinition"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/nonVirtualRedefinition/nonVirtualRedefinitionTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/nonVirtualRedefinition/nonVirtualRedefinitionTestOutput.txt; then \
		echo "creating $(srcdir)/nonVirtualRedefinition/nonVirtualRedefinitionTestOutput.txt" ;\
		./compassMain $(srcdir)/nonVirtualRedefinition/nonVirtualRedefinitionTest1.C $(TGUI) &> $(srcdir)/nonVirtualRedefinition/nonVirtualRedefinitionTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/nonVirtualRedefinition/nonVirtualRedefinitionTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/nonVirtualRedefinition/nonVirtualRedefinitionTestOutput.txt $(top_builddir)/projects/compass/nonVirtualRedefinitionTestOutput.txt.00 ;\
		grep "NonVirtualRedefinition:" $(top_builddir)/projects/compass/nonVirtualRedefinitionTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/nonVirtualRedefinition/nonVirtualRedefinitionTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/nonVirtualRedefinitionTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/nonVirtualRedefinition/nonVirtualRedefinitionTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/nonVirtualRedefinition/nonVirtualRedefinitionTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/nonVirtualRedefinition/nonVirtualRedefinitionTestOutput.txt; then \
			if test -e $(srcdir)/nonVirtualRedefinition/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/nonVirtualRedefinition/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/nonVirtualRedefinition/nonVirtualRedefinitionTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/nonVirtualRedefinition/nonVirtualRedefinitionTest1.C $(TGUI) &> $(top_builddir)/projects/compass/nonVirtualRedefinitionTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/nonVirtualRedefinitionTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/nonVirtualRedefinitionTestOutput.txt $(top_builddir)/projects/compass/nonVirtualRedefinitionTestOutput.txt.00 ;\
		grep "NonVirtualRedefinition:" $(top_builddir)/projects/compass/nonVirtualRedefinitionTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/nonVirtualRedefinitionTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/nonVirtualRedefinitionTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/nonVirtualRedefinitionTestOutput.txt || ! test -s $(srcdir)/nonVirtualRedefinition/nonVirtualRedefinitionTestOutput.txt ; then \
			if test -e $(srcdir)/nonVirtualRedefinition/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/nonVirtualRedefinition/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/nonVirtualRedefinitionTestOutput.txt or $(srcdir)/nonVirtualRedefinition/nonVirtualRedefinitionTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/nonVirtualRedefinitionTestOutput.txt $(srcdir)/nonVirtualRedefinition/nonVirtualRedefinitionTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/nonVirtualRedefinitionTestOutput.txt and $(srcdir)/nonVirtualRedefinition/nonVirtualRedefinitionTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/nonVirtualRedefinitionTestOutput.txt ;\
	fi
	@echo "done testnonVirtualRedefinition"

testNoOverloadAmpersand: compassMain
	@echo -e "\nRunning testNoOverloadAmpersand"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/noOverloadAmpersand/noOverloadAmpersandTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/noOverloadAmpersand/noOverloadAmpersandTestOutput.txt; then \
		echo "creating $(srcdir)/noOverloadAmpersand/noOverloadAmpersandTestOutput.txt" ;\
		./compassMain $(srcdir)/noOverloadAmpersand/noOverloadAmpersandTest1.C $(TGUI) &> $(srcdir)/noOverloadAmpersand/noOverloadAmpersandTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/noOverloadAmpersand/noOverloadAmpersandTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/noOverloadAmpersand/noOverloadAmpersandTestOutput.txt $(top_builddir)/projects/compass/noOverloadAmpersandTestOutput.txt.00 ;\
		grep "NoOverloadAmpersand:" $(top_builddir)/projects/compass/noOverloadAmpersandTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/noOverloadAmpersand/noOverloadAmpersandTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/noOverloadAmpersandTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/noOverloadAmpersand/noOverloadAmpersandTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/noOverloadAmpersand/noOverloadAmpersandTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/noOverloadAmpersand/noOverloadAmpersandTestOutput.txt; then \
			if test -e $(srcdir)/noOverloadAmpersand/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/noOverloadAmpersand/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/noOverloadAmpersand/noOverloadAmpersandTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/noOverloadAmpersand/noOverloadAmpersandTest1.C $(TGUI) &> $(top_builddir)/projects/compass/noOverloadAmpersandTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/noOverloadAmpersandTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/noOverloadAmpersandTestOutput.txt $(top_builddir)/projects/compass/noOverloadAmpersandTestOutput.txt.00 ;\
		grep "NoOverloadAmpersand:" $(top_builddir)/projects/compass/noOverloadAmpersandTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/noOverloadAmpersandTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/noOverloadAmpersandTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/noOverloadAmpersandTestOutput.txt || ! test -s $(srcdir)/noOverloadAmpersand/noOverloadAmpersandTestOutput.txt ; then \
			if test -e $(srcdir)/noOverloadAmpersand/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/noOverloadAmpersand/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/noOverloadAmpersandTestOutput.txt or $(srcdir)/noOverloadAmpersand/noOverloadAmpersandTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/noOverloadAmpersandTestOutput.txt $(srcdir)/noOverloadAmpersand/noOverloadAmpersandTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/noOverloadAmpersandTestOutput.txt and $(srcdir)/noOverloadAmpersand/noOverloadAmpersandTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/noOverloadAmpersandTestOutput.txt ;\
	fi
	@echo "done testnoOverloadAmpersand"

testNoRand: compassMain
	@echo -e "\nRunning testNoRand"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/noRand/noRandTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/noRand/noRandTestOutput.txt; then \
		echo "creating $(srcdir)/noRand/noRandTestOutput.txt" ;\
		./compassMain $(srcdir)/noRand/noRandTest1.C $(TGUI) &> $(srcdir)/noRand/noRandTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/noRand/noRandTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/noRand/noRandTestOutput.txt $(top_builddir)/projects/compass/noRandTestOutput.txt.00 ;\
		grep "NoRand:" $(top_builddir)/projects/compass/noRandTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/noRand/noRandTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/noRandTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/noRand/noRandTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/noRand/noRandTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/noRand/noRandTestOutput.txt; then \
			if test -e $(srcdir)/noRand/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/noRand/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/noRand/noRandTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/noRand/noRandTest1.C $(TGUI) &> $(top_builddir)/projects/compass/noRandTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/noRandTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/noRandTestOutput.txt $(top_builddir)/projects/compass/noRandTestOutput.txt.00 ;\
		grep "NoRand:" $(top_builddir)/projects/compass/noRandTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/noRandTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/noRandTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/noRandTestOutput.txt || ! test -s $(srcdir)/noRand/noRandTestOutput.txt ; then \
			if test -e $(srcdir)/noRand/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/noRand/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/noRandTestOutput.txt or $(srcdir)/noRand/noRandTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/noRandTestOutput.txt $(srcdir)/noRand/noRandTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/noRandTestOutput.txt and $(srcdir)/noRand/noRandTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/noRandTestOutput.txt ;\
	fi
	@echo "done testnoRand"

testNoSecondTermSideEffects: compassMain
	@echo -e "\nRunning testNoSecondTermSideEffects"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/noSecondTermSideEffects/noSecondTermSideEffectsTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/noSecondTermSideEffects/noSecondTermSideEffectsTestOutput.txt; then \
		echo "creating $(srcdir)/noSecondTermSideEffects/noSecondTermSideEffectsTestOutput.txt" ;\
		./compassMain $(srcdir)/noSecondTermSideEffects/noSecondTermSideEffectsTest1.C $(TGUI) &> $(srcdir)/noSecondTermSideEffects/noSecondTermSideEffectsTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/noSecondTermSideEffects/noSecondTermSideEffectsTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/noSecondTermSideEffects/noSecondTermSideEffectsTestOutput.txt $(top_builddir)/projects/compass/noSecondTermSideEffectsTestOutput.txt.00 ;\
		grep "NoSecondTermSideEffects:" $(top_builddir)/projects/compass/noSecondTermSideEffectsTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/noSecondTermSideEffects/noSecondTermSideEffectsTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/noSecondTermSideEffectsTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/noSecondTermSideEffects/noSecondTermSideEffectsTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/noSecondTermSideEffects/noSecondTermSideEffectsTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/noSecondTermSideEffects/noSecondTermSideEffectsTestOutput.txt; then \
			if test -e $(srcdir)/noSecondTermSideEffects/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/noSecondTermSideEffects/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/noSecondTermSideEffects/noSecondTermSideEffectsTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/noSecondTermSideEffects/noSecondTermSideEffectsTest1.C $(TGUI) &> $(top_builddir)/projects/compass/noSecondTermSideEffectsTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/noSecondTermSideEffectsTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/noSecondTermSideEffectsTestOutput.txt $(top_builddir)/projects/compass/noSecondTermSideEffectsTestOutput.txt.00 ;\
		grep "NoSecondTermSideEffects:" $(top_builddir)/projects/compass/noSecondTermSideEffectsTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/noSecondTermSideEffectsTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/noSecondTermSideEffectsTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/noSecondTermSideEffectsTestOutput.txt || ! test -s $(srcdir)/noSecondTermSideEffects/noSecondTermSideEffectsTestOutput.txt ; then \
			if test -e $(srcdir)/noSecondTermSideEffects/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/noSecondTermSideEffects/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/noSecondTermSideEffectsTestOutput.txt or $(srcdir)/noSecondTermSideEffects/noSecondTermSideEffectsTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/noSecondTermSideEffectsTestOutput.txt $(srcdir)/noSecondTermSideEffects/noSecondTermSideEffectsTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/noSecondTermSideEffectsTestOutput.txt and $(srcdir)/noSecondTermSideEffects/noSecondTermSideEffectsTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/noSecondTermSideEffectsTestOutput.txt ;\
	fi
	@echo "done testnoSecondTermSideEffects"

testNoSideEffectInSizeof: compassMain
	@echo -e "\nRunning testNoSideEffectInSizeof"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/noSideEffectInSizeof/noSideEffectInSizeofTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/noSideEffectInSizeof/noSideEffectInSizeofTestOutput.txt; then \
		echo "creating $(srcdir)/noSideEffectInSizeof/noSideEffectInSizeofTestOutput.txt" ;\
		./compassMain $(srcdir)/noSideEffectInSizeof/noSideEffectInSizeofTest1.C $(TGUI) &> $(srcdir)/noSideEffectInSizeof/noSideEffectInSizeofTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/noSideEffectInSizeof/noSideEffectInSizeofTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/noSideEffectInSizeof/noSideEffectInSizeofTestOutput.txt $(top_builddir)/projects/compass/noSideEffectInSizeofTestOutput.txt.00 ;\
		grep "NoSideEffectInSizeof:" $(top_builddir)/projects/compass/noSideEffectInSizeofTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/noSideEffectInSizeof/noSideEffectInSizeofTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/noSideEffectInSizeofTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/noSideEffectInSizeof/noSideEffectInSizeofTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/noSideEffectInSizeof/noSideEffectInSizeofTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/noSideEffectInSizeof/noSideEffectInSizeofTestOutput.txt; then \
			if test -e $(srcdir)/noSideEffectInSizeof/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/noSideEffectInSizeof/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/noSideEffectInSizeof/noSideEffectInSizeofTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/noSideEffectInSizeof/noSideEffectInSizeofTest1.C $(TGUI) &> $(top_builddir)/projects/compass/noSideEffectInSizeofTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/noSideEffectInSizeofTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/noSideEffectInSizeofTestOutput.txt $(top_builddir)/projects/compass/noSideEffectInSizeofTestOutput.txt.00 ;\
		grep "NoSideEffectInSizeof:" $(top_builddir)/projects/compass/noSideEffectInSizeofTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/noSideEffectInSizeofTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/noSideEffectInSizeofTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/noSideEffectInSizeofTestOutput.txt || ! test -s $(srcdir)/noSideEffectInSizeof/noSideEffectInSizeofTestOutput.txt ; then \
			if test -e $(srcdir)/noSideEffectInSizeof/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/noSideEffectInSizeof/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/noSideEffectInSizeofTestOutput.txt or $(srcdir)/noSideEffectInSizeof/noSideEffectInSizeofTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/noSideEffectInSizeofTestOutput.txt $(srcdir)/noSideEffectInSizeof/noSideEffectInSizeofTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/noSideEffectInSizeofTestOutput.txt and $(srcdir)/noSideEffectInSizeof/noSideEffectInSizeofTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/noSideEffectInSizeofTestOutput.txt ;\
	fi
	@echo "done testnoSideEffectInSizeof"

testNoTemplateUsage: compassMain
	@echo -e "\nRunning testNoTemplateUsage"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/noTemplateUsage/noTemplateUsageTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/noTemplateUsage/noTemplateUsageTestOutput.txt; then \
		echo "creating $(srcdir)/noTemplateUsage/noTemplateUsageTestOutput.txt" ;\
		./compassMain $(srcdir)/noTemplateUsage/noTemplateUsageTest1.C $(TGUI) &> $(srcdir)/noTemplateUsage/noTemplateUsageTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/noTemplateUsage/noTemplateUsageTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/noTemplateUsage/noTemplateUsageTestOutput.txt $(top_builddir)/projects/compass/noTemplateUsageTestOutput.txt.00 ;\
		grep "NoTemplateUsage:" $(top_builddir)/projects/compass/noTemplateUsageTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/noTemplateUsage/noTemplateUsageTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/noTemplateUsageTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/noTemplateUsage/noTemplateUsageTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/noTemplateUsage/noTemplateUsageTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/noTemplateUsage/noTemplateUsageTestOutput.txt; then \
			if test -e $(srcdir)/noTemplateUsage/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/noTemplateUsage/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/noTemplateUsage/noTemplateUsageTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/noTemplateUsage/noTemplateUsageTest1.C $(TGUI) &> $(top_builddir)/projects/compass/noTemplateUsageTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/noTemplateUsageTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/noTemplateUsageTestOutput.txt $(top_builddir)/projects/compass/noTemplateUsageTestOutput.txt.00 ;\
		grep "NoTemplateUsage:" $(top_builddir)/projects/compass/noTemplateUsageTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/noTemplateUsageTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/noTemplateUsageTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/noTemplateUsageTestOutput.txt || ! test -s $(srcdir)/noTemplateUsage/noTemplateUsageTestOutput.txt ; then \
			if test -e $(srcdir)/noTemplateUsage/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/noTemplateUsage/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/noTemplateUsageTestOutput.txt or $(srcdir)/noTemplateUsage/noTemplateUsageTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/noTemplateUsageTestOutput.txt $(srcdir)/noTemplateUsage/noTemplateUsageTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/noTemplateUsageTestOutput.txt and $(srcdir)/noTemplateUsage/noTemplateUsageTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/noTemplateUsageTestOutput.txt ;\
	fi
	@echo "done testnoTemplateUsage"

testNoVfork: compassMain
	@echo -e "\nRunning testNoVfork"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/noVfork/noVforkTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/noVfork/noVforkTestOutput.txt; then \
		echo "creating $(srcdir)/noVfork/noVforkTestOutput.txt" ;\
		./compassMain $(srcdir)/noVfork/noVforkTest1.C $(TGUI) &> $(srcdir)/noVfork/noVforkTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/noVfork/noVforkTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/noVfork/noVforkTestOutput.txt $(top_builddir)/projects/compass/noVforkTestOutput.txt.00 ;\
		grep "NoVfork:" $(top_builddir)/projects/compass/noVforkTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/noVfork/noVforkTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/noVforkTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/noVfork/noVforkTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/noVfork/noVforkTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/noVfork/noVforkTestOutput.txt; then \
			if test -e $(srcdir)/noVfork/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/noVfork/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/noVfork/noVforkTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/noVfork/noVforkTest1.C $(TGUI) &> $(top_builddir)/projects/compass/noVforkTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/noVforkTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/noVforkTestOutput.txt $(top_builddir)/projects/compass/noVforkTestOutput.txt.00 ;\
		grep "NoVfork:" $(top_builddir)/projects/compass/noVforkTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/noVforkTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/noVforkTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/noVforkTestOutput.txt || ! test -s $(srcdir)/noVfork/noVforkTestOutput.txt ; then \
			if test -e $(srcdir)/noVfork/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/noVfork/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/noVforkTestOutput.txt or $(srcdir)/noVfork/noVforkTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/noVforkTestOutput.txt $(srcdir)/noVfork/noVforkTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/noVforkTestOutput.txt and $(srcdir)/noVfork/noVforkTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/noVforkTestOutput.txt ;\
	fi
	@echo "done testnoVfork"

testNullDeref: compassMain
	@echo -e "\nRunning testNullDeref"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/nullDeref/nullDerefTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/nullDeref/nullDerefTestOutput.txt; then \
		echo "creating $(srcdir)/nullDeref/nullDerefTestOutput.txt" ;\
		./compassMain $(srcdir)/nullDeref/nullDerefTest1.C $(TGUI) &> $(srcdir)/nullDeref/nullDerefTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/nullDeref/nullDerefTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/nullDeref/nullDerefTestOutput.txt $(top_builddir)/projects/compass/nullDerefTestOutput.txt.00 ;\
		grep "NullDeref:" $(top_builddir)/projects/compass/nullDerefTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/nullDeref/nullDerefTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/nullDerefTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/nullDeref/nullDerefTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/nullDeref/nullDerefTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/nullDeref/nullDerefTestOutput.txt; then \
			if test -e $(srcdir)/nullDeref/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/nullDeref/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/nullDeref/nullDerefTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/nullDeref/nullDerefTest1.C $(TGUI) &> $(top_builddir)/projects/compass/nullDerefTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/nullDerefTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/nullDerefTestOutput.txt $(top_builddir)/projects/compass/nullDerefTestOutput.txt.00 ;\
		grep "NullDeref:" $(top_builddir)/projects/compass/nullDerefTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/nullDerefTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/nullDerefTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/nullDerefTestOutput.txt || ! test -s $(srcdir)/nullDeref/nullDerefTestOutput.txt ; then \
			if test -e $(srcdir)/nullDeref/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/nullDeref/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/nullDerefTestOutput.txt or $(srcdir)/nullDeref/nullDerefTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/nullDerefTestOutput.txt $(srcdir)/nullDeref/nullDerefTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/nullDerefTestOutput.txt and $(srcdir)/nullDeref/nullDerefTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/nullDerefTestOutput.txt ;\
	fi
	@echo "done testnullDeref"

testOneLinePerDeclaration: compassMain
	@echo -e "\nRunning testOneLinePerDeclaration"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/oneLinePerDeclaration/oneLinePerDeclarationTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/oneLinePerDeclaration/oneLinePerDeclarationTestOutput.txt; then \
		echo "creating $(srcdir)/oneLinePerDeclaration/oneLinePerDeclarationTestOutput.txt" ;\
		./compassMain $(srcdir)/oneLinePerDeclaration/oneLinePerDeclarationTest1.C $(TGUI) &> $(srcdir)/oneLinePerDeclaration/oneLinePerDeclarationTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/oneLinePerDeclaration/oneLinePerDeclarationTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/oneLinePerDeclaration/oneLinePerDeclarationTestOutput.txt $(top_builddir)/projects/compass/oneLinePerDeclarationTestOutput.txt.00 ;\
		grep "OneLinePerDeclaration:" $(top_builddir)/projects/compass/oneLinePerDeclarationTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/oneLinePerDeclaration/oneLinePerDeclarationTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/oneLinePerDeclarationTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/oneLinePerDeclaration/oneLinePerDeclarationTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/oneLinePerDeclaration/oneLinePerDeclarationTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/oneLinePerDeclaration/oneLinePerDeclarationTestOutput.txt; then \
			if test -e $(srcdir)/oneLinePerDeclaration/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/oneLinePerDeclaration/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/oneLinePerDeclaration/oneLinePerDeclarationTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/oneLinePerDeclaration/oneLinePerDeclarationTest1.C $(TGUI) &> $(top_builddir)/projects/compass/oneLinePerDeclarationTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/oneLinePerDeclarationTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/oneLinePerDeclarationTestOutput.txt $(top_builddir)/projects/compass/oneLinePerDeclarationTestOutput.txt.00 ;\
		grep "OneLinePerDeclaration:" $(top_builddir)/projects/compass/oneLinePerDeclarationTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/oneLinePerDeclarationTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/oneLinePerDeclarationTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/oneLinePerDeclarationTestOutput.txt || ! test -s $(srcdir)/oneLinePerDeclaration/oneLinePerDeclarationTestOutput.txt ; then \
			if test -e $(srcdir)/oneLinePerDeclaration/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/oneLinePerDeclaration/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/oneLinePerDeclarationTestOutput.txt or $(srcdir)/oneLinePerDeclaration/oneLinePerDeclarationTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/oneLinePerDeclarationTestOutput.txt $(srcdir)/oneLinePerDeclaration/oneLinePerDeclarationTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/oneLinePerDeclarationTestOutput.txt and $(srcdir)/oneLinePerDeclaration/oneLinePerDeclarationTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/oneLinePerDeclarationTestOutput.txt ;\
	fi
	@echo "done testoneLinePerDeclaration"

testOperatorOverloading: compassMain
	@echo -e "\nRunning testOperatorOverloading"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/operatorOverloading/operatorOverloadingTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/operatorOverloading/operatorOverloadingTestOutput.txt; then \
		echo "creating $(srcdir)/operatorOverloading/operatorOverloadingTestOutput.txt" ;\
		./compassMain $(srcdir)/operatorOverloading/operatorOverloadingTest1.C $(TGUI) &> $(srcdir)/operatorOverloading/operatorOverloadingTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/operatorOverloading/operatorOverloadingTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/operatorOverloading/operatorOverloadingTestOutput.txt $(top_builddir)/projects/compass/operatorOverloadingTestOutput.txt.00 ;\
		grep "OperatorOverloading:" $(top_builddir)/projects/compass/operatorOverloadingTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/operatorOverloading/operatorOverloadingTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/operatorOverloadingTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/operatorOverloading/operatorOverloadingTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/operatorOverloading/operatorOverloadingTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/operatorOverloading/operatorOverloadingTestOutput.txt; then \
			if test -e $(srcdir)/operatorOverloading/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/operatorOverloading/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/operatorOverloading/operatorOverloadingTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/operatorOverloading/operatorOverloadingTest1.C $(TGUI) &> $(top_builddir)/projects/compass/operatorOverloadingTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/operatorOverloadingTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/operatorOverloadingTestOutput.txt $(top_builddir)/projects/compass/operatorOverloadingTestOutput.txt.00 ;\
		grep "OperatorOverloading:" $(top_builddir)/projects/compass/operatorOverloadingTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/operatorOverloadingTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/operatorOverloadingTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/operatorOverloadingTestOutput.txt || ! test -s $(srcdir)/operatorOverloading/operatorOverloadingTestOutput.txt ; then \
			if test -e $(srcdir)/operatorOverloading/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/operatorOverloading/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/operatorOverloadingTestOutput.txt or $(srcdir)/operatorOverloading/operatorOverloadingTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/operatorOverloadingTestOutput.txt $(srcdir)/operatorOverloading/operatorOverloadingTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/operatorOverloadingTestOutput.txt and $(srcdir)/operatorOverloading/operatorOverloadingTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/operatorOverloadingTestOutput.txt ;\
	fi
	@echo "done testoperatorOverloading"

testOtherArgument: compassMain
	@echo -e "\nRunning testOtherArgument"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/otherArgument/otherArgumentTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/otherArgument/otherArgumentTestOutput.txt; then \
		echo "creating $(srcdir)/otherArgument/otherArgumentTestOutput.txt" ;\
		./compassMain $(srcdir)/otherArgument/otherArgumentTest1.C $(TGUI) &> $(srcdir)/otherArgument/otherArgumentTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/otherArgument/otherArgumentTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/otherArgument/otherArgumentTestOutput.txt $(top_builddir)/projects/compass/otherArgumentTestOutput.txt.00 ;\
		grep "OtherArgument:" $(top_builddir)/projects/compass/otherArgumentTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/otherArgument/otherArgumentTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/otherArgumentTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/otherArgument/otherArgumentTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/otherArgument/otherArgumentTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/otherArgument/otherArgumentTestOutput.txt; then \
			if test -e $(srcdir)/otherArgument/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/otherArgument/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/otherArgument/otherArgumentTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/otherArgument/otherArgumentTest1.C $(TGUI) &> $(top_builddir)/projects/compass/otherArgumentTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/otherArgumentTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/otherArgumentTestOutput.txt $(top_builddir)/projects/compass/otherArgumentTestOutput.txt.00 ;\
		grep "OtherArgument:" $(top_builddir)/projects/compass/otherArgumentTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/otherArgumentTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/otherArgumentTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/otherArgumentTestOutput.txt || ! test -s $(srcdir)/otherArgument/otherArgumentTestOutput.txt ; then \
			if test -e $(srcdir)/otherArgument/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/otherArgument/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/otherArgumentTestOutput.txt or $(srcdir)/otherArgument/otherArgumentTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/otherArgumentTestOutput.txt $(srcdir)/otherArgument/otherArgumentTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/otherArgumentTestOutput.txt and $(srcdir)/otherArgument/otherArgumentTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/otherArgumentTestOutput.txt ;\
	fi
	@echo "done testotherArgument"

testPlaceConstantOnTheLhs: compassMain
	@echo -e "\nRunning testPlaceConstantOnTheLhs"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/placeConstantOnTheLhs/placeConstantOnTheLhsTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/placeConstantOnTheLhs/placeConstantOnTheLhsTestOutput.txt; then \
		echo "creating $(srcdir)/placeConstantOnTheLhs/placeConstantOnTheLhsTestOutput.txt" ;\
		./compassMain $(srcdir)/placeConstantOnTheLhs/placeConstantOnTheLhsTest1.C $(TGUI) &> $(srcdir)/placeConstantOnTheLhs/placeConstantOnTheLhsTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/placeConstantOnTheLhs/placeConstantOnTheLhsTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/placeConstantOnTheLhs/placeConstantOnTheLhsTestOutput.txt $(top_builddir)/projects/compass/placeConstantOnTheLhsTestOutput.txt.00 ;\
		grep "PlaceConstantOnTheLhs:" $(top_builddir)/projects/compass/placeConstantOnTheLhsTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/placeConstantOnTheLhs/placeConstantOnTheLhsTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/placeConstantOnTheLhsTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/placeConstantOnTheLhs/placeConstantOnTheLhsTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/placeConstantOnTheLhs/placeConstantOnTheLhsTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/placeConstantOnTheLhs/placeConstantOnTheLhsTestOutput.txt; then \
			if test -e $(srcdir)/placeConstantOnTheLhs/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/placeConstantOnTheLhs/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/placeConstantOnTheLhs/placeConstantOnTheLhsTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/placeConstantOnTheLhs/placeConstantOnTheLhsTest1.C $(TGUI) &> $(top_builddir)/projects/compass/placeConstantOnTheLhsTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/placeConstantOnTheLhsTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/placeConstantOnTheLhsTestOutput.txt $(top_builddir)/projects/compass/placeConstantOnTheLhsTestOutput.txt.00 ;\
		grep "PlaceConstantOnTheLhs:" $(top_builddir)/projects/compass/placeConstantOnTheLhsTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/placeConstantOnTheLhsTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/placeConstantOnTheLhsTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/placeConstantOnTheLhsTestOutput.txt || ! test -s $(srcdir)/placeConstantOnTheLhs/placeConstantOnTheLhsTestOutput.txt ; then \
			if test -e $(srcdir)/placeConstantOnTheLhs/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/placeConstantOnTheLhs/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/placeConstantOnTheLhsTestOutput.txt or $(srcdir)/placeConstantOnTheLhs/placeConstantOnTheLhsTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/placeConstantOnTheLhsTestOutput.txt $(srcdir)/placeConstantOnTheLhs/placeConstantOnTheLhsTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/placeConstantOnTheLhsTestOutput.txt and $(srcdir)/placeConstantOnTheLhs/placeConstantOnTheLhsTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/placeConstantOnTheLhsTestOutput.txt ;\
	fi
	@echo "done testplaceConstantOnTheLhs"

testPreferAlgorithms: compassMain
	@echo -e "\nRunning testPreferAlgorithms"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/preferAlgorithms/preferAlgorithmsTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/preferAlgorithms/preferAlgorithmsTestOutput.txt; then \
		echo "creating $(srcdir)/preferAlgorithms/preferAlgorithmsTestOutput.txt" ;\
		./compassMain $(srcdir)/preferAlgorithms/preferAlgorithmsTest1.C $(TGUI) &> $(srcdir)/preferAlgorithms/preferAlgorithmsTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/preferAlgorithms/preferAlgorithmsTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/preferAlgorithms/preferAlgorithmsTestOutput.txt $(top_builddir)/projects/compass/preferAlgorithmsTestOutput.txt.00 ;\
		grep "PreferAlgorithms:" $(top_builddir)/projects/compass/preferAlgorithmsTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/preferAlgorithms/preferAlgorithmsTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/preferAlgorithmsTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/preferAlgorithms/preferAlgorithmsTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/preferAlgorithms/preferAlgorithmsTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/preferAlgorithms/preferAlgorithmsTestOutput.txt; then \
			if test -e $(srcdir)/preferAlgorithms/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/preferAlgorithms/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/preferAlgorithms/preferAlgorithmsTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/preferAlgorithms/preferAlgorithmsTest1.C $(TGUI) &> $(top_builddir)/projects/compass/preferAlgorithmsTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/preferAlgorithmsTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/preferAlgorithmsTestOutput.txt $(top_builddir)/projects/compass/preferAlgorithmsTestOutput.txt.00 ;\
		grep "PreferAlgorithms:" $(top_builddir)/projects/compass/preferAlgorithmsTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/preferAlgorithmsTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/preferAlgorithmsTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/preferAlgorithmsTestOutput.txt || ! test -s $(srcdir)/preferAlgorithms/preferAlgorithmsTestOutput.txt ; then \
			if test -e $(srcdir)/preferAlgorithms/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/preferAlgorithms/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/preferAlgorithmsTestOutput.txt or $(srcdir)/preferAlgorithms/preferAlgorithmsTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/preferAlgorithmsTestOutput.txt $(srcdir)/preferAlgorithms/preferAlgorithmsTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/preferAlgorithmsTestOutput.txt and $(srcdir)/preferAlgorithms/preferAlgorithmsTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/preferAlgorithmsTestOutput.txt ;\
	fi
	@echo "done testpreferAlgorithms"

testPreferFseekToRewind: compassMain
	@echo -e "\nRunning testPreferFseekToRewind"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/preferFseekToRewind/preferFseekToRewindTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/preferFseekToRewind/preferFseekToRewindTestOutput.txt; then \
		echo "creating $(srcdir)/preferFseekToRewind/preferFseekToRewindTestOutput.txt" ;\
		./compassMain $(srcdir)/preferFseekToRewind/preferFseekToRewindTest1.C $(TGUI) &> $(srcdir)/preferFseekToRewind/preferFseekToRewindTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/preferFseekToRewind/preferFseekToRewindTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/preferFseekToRewind/preferFseekToRewindTestOutput.txt $(top_builddir)/projects/compass/preferFseekToRewindTestOutput.txt.00 ;\
		grep "PreferFseekToRewind:" $(top_builddir)/projects/compass/preferFseekToRewindTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/preferFseekToRewind/preferFseekToRewindTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/preferFseekToRewindTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/preferFseekToRewind/preferFseekToRewindTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/preferFseekToRewind/preferFseekToRewindTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/preferFseekToRewind/preferFseekToRewindTestOutput.txt; then \
			if test -e $(srcdir)/preferFseekToRewind/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/preferFseekToRewind/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/preferFseekToRewind/preferFseekToRewindTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/preferFseekToRewind/preferFseekToRewindTest1.C $(TGUI) &> $(top_builddir)/projects/compass/preferFseekToRewindTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/preferFseekToRewindTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/preferFseekToRewindTestOutput.txt $(top_builddir)/projects/compass/preferFseekToRewindTestOutput.txt.00 ;\
		grep "PreferFseekToRewind:" $(top_builddir)/projects/compass/preferFseekToRewindTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/preferFseekToRewindTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/preferFseekToRewindTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/preferFseekToRewindTestOutput.txt || ! test -s $(srcdir)/preferFseekToRewind/preferFseekToRewindTestOutput.txt ; then \
			if test -e $(srcdir)/preferFseekToRewind/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/preferFseekToRewind/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/preferFseekToRewindTestOutput.txt or $(srcdir)/preferFseekToRewind/preferFseekToRewindTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/preferFseekToRewindTestOutput.txt $(srcdir)/preferFseekToRewind/preferFseekToRewindTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/preferFseekToRewindTestOutput.txt and $(srcdir)/preferFseekToRewind/preferFseekToRewindTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/preferFseekToRewindTestOutput.txt ;\
	fi
	@echo "done testpreferFseekToRewind"

testPreferSetvbufToSetbuf: compassMain
	@echo -e "\nRunning testPreferSetvbufToSetbuf"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/preferSetvbufToSetbuf/preferSetvbufToSetbufTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/preferSetvbufToSetbuf/preferSetvbufToSetbufTestOutput.txt; then \
		echo "creating $(srcdir)/preferSetvbufToSetbuf/preferSetvbufToSetbufTestOutput.txt" ;\
		./compassMain $(srcdir)/preferSetvbufToSetbuf/preferSetvbufToSetbufTest1.C $(TGUI) &> $(srcdir)/preferSetvbufToSetbuf/preferSetvbufToSetbufTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/preferSetvbufToSetbuf/preferSetvbufToSetbufTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/preferSetvbufToSetbuf/preferSetvbufToSetbufTestOutput.txt $(top_builddir)/projects/compass/preferSetvbufToSetbufTestOutput.txt.00 ;\
		grep "PreferSetvbufToSetbuf:" $(top_builddir)/projects/compass/preferSetvbufToSetbufTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/preferSetvbufToSetbuf/preferSetvbufToSetbufTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/preferSetvbufToSetbufTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/preferSetvbufToSetbuf/preferSetvbufToSetbufTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/preferSetvbufToSetbuf/preferSetvbufToSetbufTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/preferSetvbufToSetbuf/preferSetvbufToSetbufTestOutput.txt; then \
			if test -e $(srcdir)/preferSetvbufToSetbuf/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/preferSetvbufToSetbuf/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/preferSetvbufToSetbuf/preferSetvbufToSetbufTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/preferSetvbufToSetbuf/preferSetvbufToSetbufTest1.C $(TGUI) &> $(top_builddir)/projects/compass/preferSetvbufToSetbufTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/preferSetvbufToSetbufTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/preferSetvbufToSetbufTestOutput.txt $(top_builddir)/projects/compass/preferSetvbufToSetbufTestOutput.txt.00 ;\
		grep "PreferSetvbufToSetbuf:" $(top_builddir)/projects/compass/preferSetvbufToSetbufTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/preferSetvbufToSetbufTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/preferSetvbufToSetbufTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/preferSetvbufToSetbufTestOutput.txt || ! test -s $(srcdir)/preferSetvbufToSetbuf/preferSetvbufToSetbufTestOutput.txt ; then \
			if test -e $(srcdir)/preferSetvbufToSetbuf/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/preferSetvbufToSetbuf/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/preferSetvbufToSetbufTestOutput.txt or $(srcdir)/preferSetvbufToSetbuf/preferSetvbufToSetbufTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/preferSetvbufToSetbufTestOutput.txt $(srcdir)/preferSetvbufToSetbuf/preferSetvbufToSetbufTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/preferSetvbufToSetbufTestOutput.txt and $(srcdir)/preferSetvbufToSetbuf/preferSetvbufToSetbufTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/preferSetvbufToSetbufTestOutput.txt ;\
	fi
	@echo "done testpreferSetvbufToSetbuf"

testProtectVirtualMethods: compassMain
	@echo -e "\nRunning testProtectVirtualMethods"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/protectVirtualMethods/protectVirtualMethodsTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/protectVirtualMethods/protectVirtualMethodsTestOutput.txt; then \
		echo "creating $(srcdir)/protectVirtualMethods/protectVirtualMethodsTestOutput.txt" ;\
		./compassMain $(srcdir)/protectVirtualMethods/protectVirtualMethodsTest1.C $(TGUI) &> $(srcdir)/protectVirtualMethods/protectVirtualMethodsTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/protectVirtualMethods/protectVirtualMethodsTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/protectVirtualMethods/protectVirtualMethodsTestOutput.txt $(top_builddir)/projects/compass/protectVirtualMethodsTestOutput.txt.00 ;\
		grep "ProtectVirtualMethods:" $(top_builddir)/projects/compass/protectVirtualMethodsTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/protectVirtualMethods/protectVirtualMethodsTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/protectVirtualMethodsTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/protectVirtualMethods/protectVirtualMethodsTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/protectVirtualMethods/protectVirtualMethodsTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/protectVirtualMethods/protectVirtualMethodsTestOutput.txt; then \
			if test -e $(srcdir)/protectVirtualMethods/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/protectVirtualMethods/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/protectVirtualMethods/protectVirtualMethodsTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/protectVirtualMethods/protectVirtualMethodsTest1.C $(TGUI) &> $(top_builddir)/projects/compass/protectVirtualMethodsTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/protectVirtualMethodsTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/protectVirtualMethodsTestOutput.txt $(top_builddir)/projects/compass/protectVirtualMethodsTestOutput.txt.00 ;\
		grep "ProtectVirtualMethods:" $(top_builddir)/projects/compass/protectVirtualMethodsTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/protectVirtualMethodsTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/protectVirtualMethodsTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/protectVirtualMethodsTestOutput.txt || ! test -s $(srcdir)/protectVirtualMethods/protectVirtualMethodsTestOutput.txt ; then \
			if test -e $(srcdir)/protectVirtualMethods/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/protectVirtualMethods/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/protectVirtualMethodsTestOutput.txt or $(srcdir)/protectVirtualMethods/protectVirtualMethodsTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/protectVirtualMethodsTestOutput.txt $(srcdir)/protectVirtualMethods/protectVirtualMethodsTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/protectVirtualMethodsTestOutput.txt and $(srcdir)/protectVirtualMethods/protectVirtualMethodsTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/protectVirtualMethodsTestOutput.txt ;\
	fi
	@echo "done testprotectVirtualMethods"

testPushBack: compassMain
	@echo -e "\nRunning testPushBack"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/pushBack/pushBackTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/pushBack/pushBackTestOutput.txt; then \
		echo "creating $(srcdir)/pushBack/pushBackTestOutput.txt" ;\
		./compassMain $(srcdir)/pushBack/pushBackTest1.C $(TGUI) &> $(srcdir)/pushBack/pushBackTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/pushBack/pushBackTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/pushBack/pushBackTestOutput.txt $(top_builddir)/projects/compass/pushBackTestOutput.txt.00 ;\
		grep "PushBack:" $(top_builddir)/projects/compass/pushBackTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/pushBack/pushBackTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/pushBackTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/pushBack/pushBackTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/pushBack/pushBackTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/pushBack/pushBackTestOutput.txt; then \
			if test -e $(srcdir)/pushBack/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/pushBack/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/pushBack/pushBackTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/pushBack/pushBackTest1.C $(TGUI) &> $(top_builddir)/projects/compass/pushBackTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/pushBackTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/pushBackTestOutput.txt $(top_builddir)/projects/compass/pushBackTestOutput.txt.00 ;\
		grep "PushBack:" $(top_builddir)/projects/compass/pushBackTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/pushBackTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/pushBackTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/pushBackTestOutput.txt || ! test -s $(srcdir)/pushBack/pushBackTestOutput.txt ; then \
			if test -e $(srcdir)/pushBack/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/pushBack/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/pushBackTestOutput.txt or $(srcdir)/pushBack/pushBackTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/pushBackTestOutput.txt $(srcdir)/pushBack/pushBackTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/pushBackTestOutput.txt and $(srcdir)/pushBack/pushBackTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/pushBackTestOutput.txt ;\
	fi
	@echo "done testpushBack"

testRightShiftMask: compassMain
	@echo -e "\nRunning testRightShiftMask"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/rightShiftMask/rightShiftMaskTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/rightShiftMask/rightShiftMaskTestOutput.txt; then \
		echo "creating $(srcdir)/rightShiftMask/rightShiftMaskTestOutput.txt" ;\
		./compassMain $(srcdir)/rightShiftMask/rightShiftMaskTest1.C $(TGUI) &> $(srcdir)/rightShiftMask/rightShiftMaskTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/rightShiftMask/rightShiftMaskTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/rightShiftMask/rightShiftMaskTestOutput.txt $(top_builddir)/projects/compass/rightShiftMaskTestOutput.txt.00 ;\
		grep "RightShiftMask:" $(top_builddir)/projects/compass/rightShiftMaskTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/rightShiftMask/rightShiftMaskTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/rightShiftMaskTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/rightShiftMask/rightShiftMaskTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/rightShiftMask/rightShiftMaskTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/rightShiftMask/rightShiftMaskTestOutput.txt; then \
			if test -e $(srcdir)/rightShiftMask/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/rightShiftMask/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/rightShiftMask/rightShiftMaskTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/rightShiftMask/rightShiftMaskTest1.C $(TGUI) &> $(top_builddir)/projects/compass/rightShiftMaskTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/rightShiftMaskTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/rightShiftMaskTestOutput.txt $(top_builddir)/projects/compass/rightShiftMaskTestOutput.txt.00 ;\
		grep "RightShiftMask:" $(top_builddir)/projects/compass/rightShiftMaskTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/rightShiftMaskTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/rightShiftMaskTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/rightShiftMaskTestOutput.txt || ! test -s $(srcdir)/rightShiftMask/rightShiftMaskTestOutput.txt ; then \
			if test -e $(srcdir)/rightShiftMask/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/rightShiftMask/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/rightShiftMaskTestOutput.txt or $(srcdir)/rightShiftMask/rightShiftMaskTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/rightShiftMaskTestOutput.txt $(srcdir)/rightShiftMask/rightShiftMaskTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/rightShiftMaskTestOutput.txt and $(srcdir)/rightShiftMask/rightShiftMaskTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/rightShiftMaskTestOutput.txt ;\
	fi
	@echo "done testrightShiftMask"

testSetPointersToNull: compassMain
	@echo -e "\nRunning testSetPointersToNull"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/setPointersToNull/setPointersToNullTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/setPointersToNull/setPointersToNullTestOutput.txt; then \
		echo "creating $(srcdir)/setPointersToNull/setPointersToNullTestOutput.txt" ;\
		./compassMain $(srcdir)/setPointersToNull/setPointersToNullTest1.C $(TGUI) &> $(srcdir)/setPointersToNull/setPointersToNullTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/setPointersToNull/setPointersToNullTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/setPointersToNull/setPointersToNullTestOutput.txt $(top_builddir)/projects/compass/setPointersToNullTestOutput.txt.00 ;\
		grep "SetPointersToNull:" $(top_builddir)/projects/compass/setPointersToNullTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/setPointersToNull/setPointersToNullTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/setPointersToNullTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/setPointersToNull/setPointersToNullTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/setPointersToNull/setPointersToNullTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/setPointersToNull/setPointersToNullTestOutput.txt; then \
			if test -e $(srcdir)/setPointersToNull/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/setPointersToNull/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/setPointersToNull/setPointersToNullTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/setPointersToNull/setPointersToNullTest1.C $(TGUI) &> $(top_builddir)/projects/compass/setPointersToNullTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/setPointersToNullTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/setPointersToNullTestOutput.txt $(top_builddir)/projects/compass/setPointersToNullTestOutput.txt.00 ;\
		grep "SetPointersToNull:" $(top_builddir)/projects/compass/setPointersToNullTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/setPointersToNullTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/setPointersToNullTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/setPointersToNullTestOutput.txt || ! test -s $(srcdir)/setPointersToNull/setPointersToNullTestOutput.txt ; then \
			if test -e $(srcdir)/setPointersToNull/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/setPointersToNull/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/setPointersToNullTestOutput.txt or $(srcdir)/setPointersToNull/setPointersToNullTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/setPointersToNullTestOutput.txt $(srcdir)/setPointersToNull/setPointersToNullTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/setPointersToNullTestOutput.txt and $(srcdir)/setPointersToNull/setPointersToNullTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/setPointersToNullTestOutput.txt ;\
	fi
	@echo "done testsetPointersToNull"

testSingleParameterConstructorExplicitModifier: compassMain
	@echo -e "\nRunning testSingleParameterConstructorExplicitModifier"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/singleParameterConstructorExplicitModifier/singleParameterConstructorExplicitModifierTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/singleParameterConstructorExplicitModifier/singleParameterConstructorExplicitModifierTestOutput.txt; then \
		echo "creating $(srcdir)/singleParameterConstructorExplicitModifier/singleParameterConstructorExplicitModifierTestOutput.txt" ;\
		./compassMain $(srcdir)/singleParameterConstructorExplicitModifier/singleParameterConstructorExplicitModifierTest1.C $(TGUI) &> $(srcdir)/singleParameterConstructorExplicitModifier/singleParameterConstructorExplicitModifierTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/singleParameterConstructorExplicitModifier/singleParameterConstructorExplicitModifierTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/singleParameterConstructorExplicitModifier/singleParameterConstructorExplicitModifierTestOutput.txt $(top_builddir)/projects/compass/singleParameterConstructorExplicitModifierTestOutput.txt.00 ;\
		grep "SingleParameterConstructorExplicitModifier:" $(top_builddir)/projects/compass/singleParameterConstructorExplicitModifierTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/singleParameterConstructorExplicitModifier/singleParameterConstructorExplicitModifierTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/singleParameterConstructorExplicitModifierTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/singleParameterConstructorExplicitModifier/singleParameterConstructorExplicitModifierTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/singleParameterConstructorExplicitModifier/singleParameterConstructorExplicitModifierTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/singleParameterConstructorExplicitModifier/singleParameterConstructorExplicitModifierTestOutput.txt; then \
			if test -e $(srcdir)/singleParameterConstructorExplicitModifier/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/singleParameterConstructorExplicitModifier/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/singleParameterConstructorExplicitModifier/singleParameterConstructorExplicitModifierTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/singleParameterConstructorExplicitModifier/singleParameterConstructorExplicitModifierTest1.C $(TGUI) &> $(top_builddir)/projects/compass/singleParameterConstructorExplicitModifierTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/singleParameterConstructorExplicitModifierTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/singleParameterConstructorExplicitModifierTestOutput.txt $(top_builddir)/projects/compass/singleParameterConstructorExplicitModifierTestOutput.txt.00 ;\
		grep "SingleParameterConstructorExplicitModifier:" $(top_builddir)/projects/compass/singleParameterConstructorExplicitModifierTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/singleParameterConstructorExplicitModifierTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/singleParameterConstructorExplicitModifierTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/singleParameterConstructorExplicitModifierTestOutput.txt || ! test -s $(srcdir)/singleParameterConstructorExplicitModifier/singleParameterConstructorExplicitModifierTestOutput.txt ; then \
			if test -e $(srcdir)/singleParameterConstructorExplicitModifier/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/singleParameterConstructorExplicitModifier/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/singleParameterConstructorExplicitModifierTestOutput.txt or $(srcdir)/singleParameterConstructorExplicitModifier/singleParameterConstructorExplicitModifierTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/singleParameterConstructorExplicitModifierTestOutput.txt $(srcdir)/singleParameterConstructorExplicitModifier/singleParameterConstructorExplicitModifierTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/singleParameterConstructorExplicitModifierTestOutput.txt and $(srcdir)/singleParameterConstructorExplicitModifier/singleParameterConstructorExplicitModifierTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/singleParameterConstructorExplicitModifierTestOutput.txt ;\
	fi
	@echo "done testsingleParameterConstructorExplicitModifier"

testSizeOfPointer: compassMain
	@echo -e "\nRunning testSizeOfPointer"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/sizeOfPointer/sizeOfPointerTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/sizeOfPointer/sizeOfPointerTestOutput.txt; then \
		echo "creating $(srcdir)/sizeOfPointer/sizeOfPointerTestOutput.txt" ;\
		./compassMain $(srcdir)/sizeOfPointer/sizeOfPointerTest1.C $(TGUI) &> $(srcdir)/sizeOfPointer/sizeOfPointerTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/sizeOfPointer/sizeOfPointerTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/sizeOfPointer/sizeOfPointerTestOutput.txt $(top_builddir)/projects/compass/sizeOfPointerTestOutput.txt.00 ;\
		grep "SizeOfPointer:" $(top_builddir)/projects/compass/sizeOfPointerTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/sizeOfPointer/sizeOfPointerTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/sizeOfPointerTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/sizeOfPointer/sizeOfPointerTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/sizeOfPointer/sizeOfPointerTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/sizeOfPointer/sizeOfPointerTestOutput.txt; then \
			if test -e $(srcdir)/sizeOfPointer/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/sizeOfPointer/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/sizeOfPointer/sizeOfPointerTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/sizeOfPointer/sizeOfPointerTest1.C $(TGUI) &> $(top_builddir)/projects/compass/sizeOfPointerTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/sizeOfPointerTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/sizeOfPointerTestOutput.txt $(top_builddir)/projects/compass/sizeOfPointerTestOutput.txt.00 ;\
		grep "SizeOfPointer:" $(top_builddir)/projects/compass/sizeOfPointerTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/sizeOfPointerTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/sizeOfPointerTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/sizeOfPointerTestOutput.txt || ! test -s $(srcdir)/sizeOfPointer/sizeOfPointerTestOutput.txt ; then \
			if test -e $(srcdir)/sizeOfPointer/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/sizeOfPointer/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/sizeOfPointerTestOutput.txt or $(srcdir)/sizeOfPointer/sizeOfPointerTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/sizeOfPointerTestOutput.txt $(srcdir)/sizeOfPointer/sizeOfPointerTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/sizeOfPointerTestOutput.txt and $(srcdir)/sizeOfPointer/sizeOfPointerTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/sizeOfPointerTestOutput.txt ;\
	fi
	@echo "done testsizeOfPointer"

testStringTokenToIntegerConverter: compassMain
	@echo -e "\nRunning testStringTokenToIntegerConverter"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/stringTokenToIntegerConverter/stringTokenToIntegerConverterTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/stringTokenToIntegerConverter/stringTokenToIntegerConverterTestOutput.txt; then \
		echo "creating $(srcdir)/stringTokenToIntegerConverter/stringTokenToIntegerConverterTestOutput.txt" ;\
		./compassMain $(srcdir)/stringTokenToIntegerConverter/stringTokenToIntegerConverterTest1.C $(TGUI) &> $(srcdir)/stringTokenToIntegerConverter/stringTokenToIntegerConverterTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/stringTokenToIntegerConverter/stringTokenToIntegerConverterTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/stringTokenToIntegerConverter/stringTokenToIntegerConverterTestOutput.txt $(top_builddir)/projects/compass/stringTokenToIntegerConverterTestOutput.txt.00 ;\
		grep "StringTokenToIntegerConverter:" $(top_builddir)/projects/compass/stringTokenToIntegerConverterTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/stringTokenToIntegerConverter/stringTokenToIntegerConverterTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/stringTokenToIntegerConverterTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/stringTokenToIntegerConverter/stringTokenToIntegerConverterTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/stringTokenToIntegerConverter/stringTokenToIntegerConverterTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/stringTokenToIntegerConverter/stringTokenToIntegerConverterTestOutput.txt; then \
			if test -e $(srcdir)/stringTokenToIntegerConverter/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/stringTokenToIntegerConverter/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/stringTokenToIntegerConverter/stringTokenToIntegerConverterTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/stringTokenToIntegerConverter/stringTokenToIntegerConverterTest1.C $(TGUI) &> $(top_builddir)/projects/compass/stringTokenToIntegerConverterTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/stringTokenToIntegerConverterTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/stringTokenToIntegerConverterTestOutput.txt $(top_builddir)/projects/compass/stringTokenToIntegerConverterTestOutput.txt.00 ;\
		grep "StringTokenToIntegerConverter:" $(top_builddir)/projects/compass/stringTokenToIntegerConverterTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/stringTokenToIntegerConverterTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/stringTokenToIntegerConverterTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/stringTokenToIntegerConverterTestOutput.txt || ! test -s $(srcdir)/stringTokenToIntegerConverter/stringTokenToIntegerConverterTestOutput.txt ; then \
			if test -e $(srcdir)/stringTokenToIntegerConverter/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/stringTokenToIntegerConverter/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/stringTokenToIntegerConverterTestOutput.txt or $(srcdir)/stringTokenToIntegerConverter/stringTokenToIntegerConverterTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/stringTokenToIntegerConverterTestOutput.txt $(srcdir)/stringTokenToIntegerConverter/stringTokenToIntegerConverterTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/stringTokenToIntegerConverterTestOutput.txt and $(srcdir)/stringTokenToIntegerConverter/stringTokenToIntegerConverterTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/stringTokenToIntegerConverterTestOutput.txt ;\
	fi
	@echo "done teststringTokenToIntegerConverter"

testSubExpressionEvaluationOrder: compassMain
	@echo -e "\nRunning testSubExpressionEvaluationOrder"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/subExpressionEvaluationOrder/subExpressionEvaluationOrderTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/subExpressionEvaluationOrder/subExpressionEvaluationOrderTestOutput.txt; then \
		echo "creating $(srcdir)/subExpressionEvaluationOrder/subExpressionEvaluationOrderTestOutput.txt" ;\
		./compassMain $(srcdir)/subExpressionEvaluationOrder/subExpressionEvaluationOrderTest1.C $(TGUI) &> $(srcdir)/subExpressionEvaluationOrder/subExpressionEvaluationOrderTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/subExpressionEvaluationOrder/subExpressionEvaluationOrderTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/subExpressionEvaluationOrder/subExpressionEvaluationOrderTestOutput.txt $(top_builddir)/projects/compass/subExpressionEvaluationOrderTestOutput.txt.00 ;\
		grep "SubExpressionEvaluationOrder:" $(top_builddir)/projects/compass/subExpressionEvaluationOrderTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/subExpressionEvaluationOrder/subExpressionEvaluationOrderTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/subExpressionEvaluationOrderTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/subExpressionEvaluationOrder/subExpressionEvaluationOrderTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/subExpressionEvaluationOrder/subExpressionEvaluationOrderTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/subExpressionEvaluationOrder/subExpressionEvaluationOrderTestOutput.txt; then \
			if test -e $(srcdir)/subExpressionEvaluationOrder/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/subExpressionEvaluationOrder/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/subExpressionEvaluationOrder/subExpressionEvaluationOrderTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/subExpressionEvaluationOrder/subExpressionEvaluationOrderTest1.C $(TGUI) &> $(top_builddir)/projects/compass/subExpressionEvaluationOrderTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/subExpressionEvaluationOrderTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/subExpressionEvaluationOrderTestOutput.txt $(top_builddir)/projects/compass/subExpressionEvaluationOrderTestOutput.txt.00 ;\
		grep "SubExpressionEvaluationOrder:" $(top_builddir)/projects/compass/subExpressionEvaluationOrderTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/subExpressionEvaluationOrderTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/subExpressionEvaluationOrderTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/subExpressionEvaluationOrderTestOutput.txt || ! test -s $(srcdir)/subExpressionEvaluationOrder/subExpressionEvaluationOrderTestOutput.txt ; then \
			if test -e $(srcdir)/subExpressionEvaluationOrder/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/subExpressionEvaluationOrder/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/subExpressionEvaluationOrderTestOutput.txt or $(srcdir)/subExpressionEvaluationOrder/subExpressionEvaluationOrderTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/subExpressionEvaluationOrderTestOutput.txt $(srcdir)/subExpressionEvaluationOrder/subExpressionEvaluationOrderTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/subExpressionEvaluationOrderTestOutput.txt and $(srcdir)/subExpressionEvaluationOrder/subExpressionEvaluationOrderTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/subExpressionEvaluationOrderTestOutput.txt ;\
	fi
	@echo "done testsubExpressionEvaluationOrder"

testTernaryOperator: compassMain
	@echo -e "\nRunning testTernaryOperator"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/ternaryOperator/ternaryOperatorTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/ternaryOperator/ternaryOperatorTestOutput.txt; then \
		echo "creating $(srcdir)/ternaryOperator/ternaryOperatorTestOutput.txt" ;\
		./compassMain $(srcdir)/ternaryOperator/ternaryOperatorTest1.C $(TGUI) &> $(srcdir)/ternaryOperator/ternaryOperatorTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/ternaryOperator/ternaryOperatorTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/ternaryOperator/ternaryOperatorTestOutput.txt $(top_builddir)/projects/compass/ternaryOperatorTestOutput.txt.00 ;\
		grep "TernaryOperator:" $(top_builddir)/projects/compass/ternaryOperatorTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/ternaryOperator/ternaryOperatorTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/ternaryOperatorTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/ternaryOperator/ternaryOperatorTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/ternaryOperator/ternaryOperatorTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/ternaryOperator/ternaryOperatorTestOutput.txt; then \
			if test -e $(srcdir)/ternaryOperator/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/ternaryOperator/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/ternaryOperator/ternaryOperatorTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/ternaryOperator/ternaryOperatorTest1.C $(TGUI) &> $(top_builddir)/projects/compass/ternaryOperatorTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/ternaryOperatorTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/ternaryOperatorTestOutput.txt $(top_builddir)/projects/compass/ternaryOperatorTestOutput.txt.00 ;\
		grep "TernaryOperator:" $(top_builddir)/projects/compass/ternaryOperatorTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/ternaryOperatorTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/ternaryOperatorTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/ternaryOperatorTestOutput.txt || ! test -s $(srcdir)/ternaryOperator/ternaryOperatorTestOutput.txt ; then \
			if test -e $(srcdir)/ternaryOperator/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/ternaryOperator/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/ternaryOperatorTestOutput.txt or $(srcdir)/ternaryOperator/ternaryOperatorTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/ternaryOperatorTestOutput.txt $(srcdir)/ternaryOperator/ternaryOperatorTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/ternaryOperatorTestOutput.txt and $(srcdir)/ternaryOperator/ternaryOperatorTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/ternaryOperatorTestOutput.txt ;\
	fi
	@echo "done testternaryOperator"

testTime_tDirectManipulation: compassMain
	@echo -e "\nRunning testTime_tDirectManipulation"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/time_tDirectManipulation/time_tDirectManipulationTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/time_tDirectManipulation/time_tDirectManipulationTestOutput.txt; then \
		echo "creating $(srcdir)/time_tDirectManipulation/time_tDirectManipulationTestOutput.txt" ;\
		./compassMain $(srcdir)/time_tDirectManipulation/time_tDirectManipulationTest1.C $(TGUI) &> $(srcdir)/time_tDirectManipulation/time_tDirectManipulationTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/time_tDirectManipulation/time_tDirectManipulationTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/time_tDirectManipulation/time_tDirectManipulationTestOutput.txt $(top_builddir)/projects/compass/time_tDirectManipulationTestOutput.txt.00 ;\
		grep "Time_tDirectManipulation:" $(top_builddir)/projects/compass/time_tDirectManipulationTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/time_tDirectManipulation/time_tDirectManipulationTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/time_tDirectManipulationTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/time_tDirectManipulation/time_tDirectManipulationTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/time_tDirectManipulation/time_tDirectManipulationTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/time_tDirectManipulation/time_tDirectManipulationTestOutput.txt; then \
			if test -e $(srcdir)/time_tDirectManipulation/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/time_tDirectManipulation/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/time_tDirectManipulation/time_tDirectManipulationTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/time_tDirectManipulation/time_tDirectManipulationTest1.C $(TGUI) &> $(top_builddir)/projects/compass/time_tDirectManipulationTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/time_tDirectManipulationTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/time_tDirectManipulationTestOutput.txt $(top_builddir)/projects/compass/time_tDirectManipulationTestOutput.txt.00 ;\
		grep "Time_tDirectManipulation:" $(top_builddir)/projects/compass/time_tDirectManipulationTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/time_tDirectManipulationTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/time_tDirectManipulationTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/time_tDirectManipulationTestOutput.txt || ! test -s $(srcdir)/time_tDirectManipulation/time_tDirectManipulationTestOutput.txt ; then \
			if test -e $(srcdir)/time_tDirectManipulation/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/time_tDirectManipulation/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/time_tDirectManipulationTestOutput.txt or $(srcdir)/time_tDirectManipulation/time_tDirectManipulationTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/time_tDirectManipulationTestOutput.txt $(srcdir)/time_tDirectManipulation/time_tDirectManipulationTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/time_tDirectManipulationTestOutput.txt and $(srcdir)/time_tDirectManipulation/time_tDirectManipulationTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/time_tDirectManipulationTestOutput.txt ;\
	fi
	@echo "done testtime_tDirectManipulation"

testTypeTypedef: compassMain
	@echo -e "\nRunning testTypeTypedef"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/typeTypedef/typeTypedefTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/typeTypedef/typeTypedefTestOutput.txt; then \
		echo "creating $(srcdir)/typeTypedef/typeTypedefTestOutput.txt" ;\
		./compassMain $(srcdir)/typeTypedef/typeTypedefTest1.C $(TGUI) &> $(srcdir)/typeTypedef/typeTypedefTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/typeTypedef/typeTypedefTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/typeTypedef/typeTypedefTestOutput.txt $(top_builddir)/projects/compass/typeTypedefTestOutput.txt.00 ;\
		grep "TypeTypedef:" $(top_builddir)/projects/compass/typeTypedefTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/typeTypedef/typeTypedefTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/typeTypedefTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/typeTypedef/typeTypedefTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/typeTypedef/typeTypedefTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/typeTypedef/typeTypedefTestOutput.txt; then \
			if test -e $(srcdir)/typeTypedef/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/typeTypedef/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/typeTypedef/typeTypedefTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/typeTypedef/typeTypedefTest1.C $(TGUI) &> $(top_builddir)/projects/compass/typeTypedefTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/typeTypedefTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/typeTypedefTestOutput.txt $(top_builddir)/projects/compass/typeTypedefTestOutput.txt.00 ;\
		grep "TypeTypedef:" $(top_builddir)/projects/compass/typeTypedefTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/typeTypedefTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/typeTypedefTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/typeTypedefTestOutput.txt || ! test -s $(srcdir)/typeTypedef/typeTypedefTestOutput.txt ; then \
			if test -e $(srcdir)/typeTypedef/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/typeTypedef/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/typeTypedefTestOutput.txt or $(srcdir)/typeTypedef/typeTypedefTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/typeTypedefTestOutput.txt $(srcdir)/typeTypedef/typeTypedefTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/typeTypedefTestOutput.txt and $(srcdir)/typeTypedef/typeTypedefTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/typeTypedefTestOutput.txt ;\
	fi
	@echo "done testtypeTypedef"

testUnaryMinus: compassMain
	@echo -e "\nRunning testUnaryMinus"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/unaryMinus/unaryMinusTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/unaryMinus/unaryMinusTestOutput.txt; then \
		echo "creating $(srcdir)/unaryMinus/unaryMinusTestOutput.txt" ;\
		./compassMain $(srcdir)/unaryMinus/unaryMinusTest1.C $(TGUI) &> $(srcdir)/unaryMinus/unaryMinusTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/unaryMinus/unaryMinusTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/unaryMinus/unaryMinusTestOutput.txt $(top_builddir)/projects/compass/unaryMinusTestOutput.txt.00 ;\
		grep "UnaryMinus:" $(top_builddir)/projects/compass/unaryMinusTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/unaryMinus/unaryMinusTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/unaryMinusTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/unaryMinus/unaryMinusTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/unaryMinus/unaryMinusTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/unaryMinus/unaryMinusTestOutput.txt; then \
			if test -e $(srcdir)/unaryMinus/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/unaryMinus/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/unaryMinus/unaryMinusTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/unaryMinus/unaryMinusTest1.C $(TGUI) &> $(top_builddir)/projects/compass/unaryMinusTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/unaryMinusTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/unaryMinusTestOutput.txt $(top_builddir)/projects/compass/unaryMinusTestOutput.txt.00 ;\
		grep "UnaryMinus:" $(top_builddir)/projects/compass/unaryMinusTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/unaryMinusTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/unaryMinusTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/unaryMinusTestOutput.txt || ! test -s $(srcdir)/unaryMinus/unaryMinusTestOutput.txt ; then \
			if test -e $(srcdir)/unaryMinus/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/unaryMinus/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/unaryMinusTestOutput.txt or $(srcdir)/unaryMinus/unaryMinusTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/unaryMinusTestOutput.txt $(srcdir)/unaryMinus/unaryMinusTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/unaryMinusTestOutput.txt and $(srcdir)/unaryMinus/unaryMinusTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/unaryMinusTestOutput.txt ;\
	fi
	@echo "done testunaryMinus"

testUninitializedDefinition: compassMain
	@echo -e "\nRunning testUninitializedDefinition"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/uninitializedDefinition/uninitializedDefinitionTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/uninitializedDefinition/uninitializedDefinitionTestOutput.txt; then \
		echo "creating $(srcdir)/uninitializedDefinition/uninitializedDefinitionTestOutput.txt" ;\
		./compassMain $(srcdir)/uninitializedDefinition/uninitializedDefinitionTest1.C $(TGUI) &> $(srcdir)/uninitializedDefinition/uninitializedDefinitionTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/uninitializedDefinition/uninitializedDefinitionTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/uninitializedDefinition/uninitializedDefinitionTestOutput.txt $(top_builddir)/projects/compass/uninitializedDefinitionTestOutput.txt.00 ;\
		grep "UninitializedDefinition:" $(top_builddir)/projects/compass/uninitializedDefinitionTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/uninitializedDefinition/uninitializedDefinitionTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/uninitializedDefinitionTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/uninitializedDefinition/uninitializedDefinitionTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/uninitializedDefinition/uninitializedDefinitionTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/uninitializedDefinition/uninitializedDefinitionTestOutput.txt; then \
			if test -e $(srcdir)/uninitializedDefinition/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/uninitializedDefinition/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/uninitializedDefinition/uninitializedDefinitionTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/uninitializedDefinition/uninitializedDefinitionTest1.C $(TGUI) &> $(top_builddir)/projects/compass/uninitializedDefinitionTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/uninitializedDefinitionTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/uninitializedDefinitionTestOutput.txt $(top_builddir)/projects/compass/uninitializedDefinitionTestOutput.txt.00 ;\
		grep "UninitializedDefinition:" $(top_builddir)/projects/compass/uninitializedDefinitionTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/uninitializedDefinitionTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/uninitializedDefinitionTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/uninitializedDefinitionTestOutput.txt || ! test -s $(srcdir)/uninitializedDefinition/uninitializedDefinitionTestOutput.txt ; then \
			if test -e $(srcdir)/uninitializedDefinition/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/uninitializedDefinition/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/uninitializedDefinitionTestOutput.txt or $(srcdir)/uninitializedDefinition/uninitializedDefinitionTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/uninitializedDefinitionTestOutput.txt $(srcdir)/uninitializedDefinition/uninitializedDefinitionTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/uninitializedDefinitionTestOutput.txt and $(srcdir)/uninitializedDefinition/uninitializedDefinitionTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/uninitializedDefinitionTestOutput.txt ;\
	fi
	@echo "done testuninitializedDefinition"

testUpperRangeLimit: compassMain
	@echo -e "\nRunning testUpperRangeLimit"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/upperRangeLimit/upperRangeLimitTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/upperRangeLimit/upperRangeLimitTestOutput.txt; then \
		echo "creating $(srcdir)/upperRangeLimit/upperRangeLimitTestOutput.txt" ;\
		./compassMain $(srcdir)/upperRangeLimit/upperRangeLimitTest1.C $(TGUI) &> $(srcdir)/upperRangeLimit/upperRangeLimitTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/upperRangeLimit/upperRangeLimitTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/upperRangeLimit/upperRangeLimitTestOutput.txt $(top_builddir)/projects/compass/upperRangeLimitTestOutput.txt.00 ;\
		grep "UpperRangeLimit:" $(top_builddir)/projects/compass/upperRangeLimitTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/upperRangeLimit/upperRangeLimitTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/upperRangeLimitTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/upperRangeLimit/upperRangeLimitTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/upperRangeLimit/upperRangeLimitTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/upperRangeLimit/upperRangeLimitTestOutput.txt; then \
			if test -e $(srcdir)/upperRangeLimit/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/upperRangeLimit/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/upperRangeLimit/upperRangeLimitTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/upperRangeLimit/upperRangeLimitTest1.C $(TGUI) &> $(top_builddir)/projects/compass/upperRangeLimitTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/upperRangeLimitTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/upperRangeLimitTestOutput.txt $(top_builddir)/projects/compass/upperRangeLimitTestOutput.txt.00 ;\
		grep "UpperRangeLimit:" $(top_builddir)/projects/compass/upperRangeLimitTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/upperRangeLimitTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/upperRangeLimitTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/upperRangeLimitTestOutput.txt || ! test -s $(srcdir)/upperRangeLimit/upperRangeLimitTestOutput.txt ; then \
			if test -e $(srcdir)/upperRangeLimit/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/upperRangeLimit/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/upperRangeLimitTestOutput.txt or $(srcdir)/upperRangeLimit/upperRangeLimitTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/upperRangeLimitTestOutput.txt $(srcdir)/upperRangeLimit/upperRangeLimitTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/upperRangeLimitTestOutput.txt and $(srcdir)/upperRangeLimit/upperRangeLimitTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/upperRangeLimitTestOutput.txt ;\
	fi
	@echo "done testupperRangeLimit"

testVariableNameEqualsDatabaseName: compassMain
	@echo -e "\nRunning testVariableNameEqualsDatabaseName"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/variableNameEqualsDatabaseName/variableNameEqualsDatabaseNameTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/variableNameEqualsDatabaseName/variableNameEqualsDatabaseNameTestOutput.txt; then \
		echo "creating $(srcdir)/variableNameEqualsDatabaseName/variableNameEqualsDatabaseNameTestOutput.txt" ;\
		./compassMain $(srcdir)/variableNameEqualsDatabaseName/variableNameEqualsDatabaseNameTest1.C $(TGUI) &> $(srcdir)/variableNameEqualsDatabaseName/variableNameEqualsDatabaseNameTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/variableNameEqualsDatabaseName/variableNameEqualsDatabaseNameTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/variableNameEqualsDatabaseName/variableNameEqualsDatabaseNameTestOutput.txt $(top_builddir)/projects/compass/variableNameEqualsDatabaseNameTestOutput.txt.00 ;\
		grep "VariableNameEqualsDatabaseName:" $(top_builddir)/projects/compass/variableNameEqualsDatabaseNameTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/variableNameEqualsDatabaseName/variableNameEqualsDatabaseNameTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/variableNameEqualsDatabaseNameTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/variableNameEqualsDatabaseName/variableNameEqualsDatabaseNameTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/variableNameEqualsDatabaseName/variableNameEqualsDatabaseNameTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/variableNameEqualsDatabaseName/variableNameEqualsDatabaseNameTestOutput.txt; then \
			if test -e $(srcdir)/variableNameEqualsDatabaseName/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/variableNameEqualsDatabaseName/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/variableNameEqualsDatabaseName/variableNameEqualsDatabaseNameTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/variableNameEqualsDatabaseName/variableNameEqualsDatabaseNameTest1.C $(TGUI) &> $(top_builddir)/projects/compass/variableNameEqualsDatabaseNameTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/variableNameEqualsDatabaseNameTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/variableNameEqualsDatabaseNameTestOutput.txt $(top_builddir)/projects/compass/variableNameEqualsDatabaseNameTestOutput.txt.00 ;\
		grep "VariableNameEqualsDatabaseName:" $(top_builddir)/projects/compass/variableNameEqualsDatabaseNameTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/variableNameEqualsDatabaseNameTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/variableNameEqualsDatabaseNameTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/variableNameEqualsDatabaseNameTestOutput.txt || ! test -s $(srcdir)/variableNameEqualsDatabaseName/variableNameEqualsDatabaseNameTestOutput.txt ; then \
			if test -e $(srcdir)/variableNameEqualsDatabaseName/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/variableNameEqualsDatabaseName/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/variableNameEqualsDatabaseNameTestOutput.txt or $(srcdir)/variableNameEqualsDatabaseName/variableNameEqualsDatabaseNameTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/variableNameEqualsDatabaseNameTestOutput.txt $(srcdir)/variableNameEqualsDatabaseName/variableNameEqualsDatabaseNameTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/variableNameEqualsDatabaseNameTestOutput.txt and $(srcdir)/variableNameEqualsDatabaseName/variableNameEqualsDatabaseNameTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/variableNameEqualsDatabaseNameTestOutput.txt ;\
	fi
	@echo "done testvariableNameEqualsDatabaseName"

testVoidStar: compassMain
	@echo -e "\nRunning testVoidStar"
	cp -f $(srcdir)/compass_parameters compass_parameters
	@echo -e "./compassMain $(srcdir)/voidStar/voidStarTest1.C $(TGUI)"
	@if ! test -f $(srcdir)/voidStar/voidStarTestOutput.txt; then \
		echo "creating $(srcdir)/voidStar/voidStarTestOutput.txt" ;\
		./compassMain $(srcdir)/voidStar/voidStarTest1.C $(TGUI) &> $(srcdir)/voidStar/voidStarTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(srcdir)/voidStar/voidStarTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(srcdir)/voidStar/voidStarTestOutput.txt $(top_builddir)/projects/compass/voidStarTestOutput.txt.00 ;\
		grep "VoidStar:" $(top_builddir)/projects/compass/voidStarTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(srcdir)/voidStar/voidStarTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/voidStarTestOutput.txt.00 ;\
		if ! test -e $(srcdir)/voidStar/voidStarTestOutput.txt; then  \
			echo "error in compass tests: failed to create $(srcdir)/voidStar/voidStarTestOutput.txt" ;\
			exit 1 ;\
		elif ! test -s $(srcdir)/voidStar/voidStarTestOutput.txt; then \
			if test -e $(srcdir)/voidStar/regressionException; then \
				echo "Warning this checker uses `cat $(srcdir)/voidStar/regressionException`" ;\
			else \
				echo "error in compass tests: Created empty file $(srcdir)/voidStar/voidStarTestOutput.txt" ;\
				exit 1 ;\
			fi ;\
		fi ;\
	else \
		./compassMain $(srcdir)/voidStar/voidStarTest1.C $(TGUI) &> $(top_builddir)/projects/compass/voidStarTestOutput.txt ;\
		if(($$?!=0)) ; then \
			echo -e "\nError: compassMain EXIT FAILURE, see $(top_builddir)/projects/compass/voidStarTestOutput.txt" ;\
			exit 1 ;\
		fi ;\
		cp -f $(top_builddir)/projects/compass/voidStarTestOutput.txt $(top_builddir)/projects/compass/voidStarTestOutput.txt.00 ;\
		grep "VoidStar:" $(top_builddir)/projects/compass/voidStarTestOutput.txt.00 | sed -e s@\/.*\/@@g > $(top_builddir)/projects/compass/voidStarTestOutput.txt ;\
		rm -f $(top_builddir)/projects/compass/voidStarTestOutput.txt.00 ;\
		if ! test -s $(top_builddir)/projects/compass/voidStarTestOutput.txt || ! test -s $(srcdir)/voidStar/voidStarTestOutput.txt ; then \
			if test -e $(srcdir)/voidStar/regressionException ; then \
				echo "Warning this checker uses `cat $(srcdir)/voidStar/regressionException`" ;\
			fi ;\
			echo "warning in compass tests: Comparison between empty file $(top_builddir)/projects/compass/voidStarTestOutput.txt or $(srcdir)/voidStar/voidStarTestOutput.txt" ;\
		fi ;\
		if ! diff -q $(top_builddir)/projects/compass/voidStarTestOutput.txt $(srcdir)/voidStar/voidStarTestOutput.txt ; then \
			echo "error in compass tests: $(top_builddir)/projects/compass/voidStarTestOutput.txt and $(srcdir)/voidStar/voidStarTestOutput.txt differ" ;\
			exit 1 ;\
		fi ;\
		rm $(top_builddir)/projects/compass/voidStarTestOutput.txt ;\
	fi
	@echo "done testvoidStar"


testAllCheckersSeparately: $(COMPASS_CHECKER_TEST_LABELS)
	@echo "$@ done"

#COMPASS CHECKER INCLUDES:


COMPASS_CHECKER_ARCHIVES = \
	archiveAllocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstraction \
	archiveAssignmentOperatorCheckSelf \
	archiveAssignmentReturnConstThis \
	archiveAsynchronousSignalHandler \
	archiveAvoidUsingTheSameHandlerForMultipleSignals \
	archiveBooleanIsHas \
	archiveBufferOverflowFunctions \
	archiveByteByByteStructureComparison \
	archiveCharStarForString \
	archiveCommaOperator \
	archiveComputationalFunctions \
	archiveConstCast \
	archiveConstructorDestructorCallsVirtualFunction \
	archiveConstStringLiterals \
	archiveControlVariableTestAgainstFunction \
	archiveCopyConstructorConstArg \
	archiveCppCallsSetjmpLongjmp \
	archiveCyclomaticComplexity \
	archiveDataMemberAccess \
	archiveDeepNesting \
	archiveDefaultCase \
	archiveDefaultConstructor \
	archiveDiscardAssignment \
	archiveDoNotCallPutenvWithAutoVar \
	archiveDoNotDeleteThis \
	archiveDoNotUseCstyleCasts \
	archiveDuffsDevice \
	archiveDynamicCast \
	archiveEmptyInsteadOfSize \
	archiveEnumDeclarationNamespaceClassScope \
	archiveExplicitCharSign \
	archiveExplicitCopy \
	archiveExplicitTestForNonBooleanValue \
	archiveFloatForLoopCounter \
	archiveFloatingPointExactComparison \
	archiveFopenFormatParameter \
	archiveForbiddenFunctions \
	archiveForLoopConstructionControlStmt \
	archiveForLoopCppIndexVariableDeclaration \
	archiveFriendDeclarationModifier \
	archiveFunctionCallAllocatesMultipleResources \
	archiveFunctionDefinitionPrototype \
	archiveFunctionDocumentation \
	archiveInductionVariableUpdate \
	archiveInternalDataSharing \
	archiveLocalizedVariables \
	archiveLocPerFunction \
	archiveLowerRangeLimit \
	archiveMagicNumber \
	archiveMallocReturnValueUsedInIfStmt \
	archiveMultiplePublicInheritance \
	archiveNameAllParameters \
	archiveNameConsistency \
	archiveNewDelete \
	archiveNoExceptions \
	archiveNoExitInMpiCode \
	archiveNoGoto \
	archiveNonAssociativeRelationalOperators \
	archiveNonmemberFunctionInterfaceNamespace \
	archiveNonStandardTypeRefArgs \
	archiveNonStandardTypeRefReturns \
	archiveNonVirtualRedefinition \
	archiveNoOverloadAmpersand \
	archiveNoRand \
	archiveNoSecondTermSideEffects \
	archiveNoSideEffectInSizeof \
	archiveNoTemplateUsage \
	archiveNoVfork \
	archiveNullDeref \
	archiveOneLinePerDeclaration \
	archiveOperatorOverloading \
	archiveOtherArgument \
	archivePlaceConstantOnTheLhs \
	archivePreferAlgorithms \
	archivePreferFseekToRewind \
	archivePreferSetvbufToSetbuf \
	archiveProtectVirtualMethods \
	archivePushBack \
	archiveRightShiftMask \
	archiveSetPointersToNull \
	archiveSingleParameterConstructorExplicitModifier \
	archiveSizeOfPointer \
	archiveStringTokenToIntegerConverter \
	archiveSubExpressionEvaluationOrder \
	archiveTernaryOperator \
	archiveTime_tDirectManipulation \
	archiveTypeTypedef \
	archiveUnaryMinus \
	archiveUninitializedDefinition \
	archiveUpperRangeLimit \
	archiveVariableNameEqualsDatabaseName \
	archiveVoidStar

archiveAllocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstraction:
	rm -f $(srcdir)/compassRepository/allocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstraction.*
	cd $(srcdir); tar -zcf compassRepository/allocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstraction.tar.gz allocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstraction
	@echo "updated allocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstraction archive"

archiveAssignmentOperatorCheckSelf:
	rm -f $(srcdir)/compassRepository/assignmentOperatorCheckSelf.*
	cd $(srcdir); tar -zcf compassRepository/assignmentOperatorCheckSelf.tar.gz assignmentOperatorCheckSelf
	@echo "updated assignmentOperatorCheckSelf archive"

archiveAssignmentReturnConstThis:
	rm -f $(srcdir)/compassRepository/assignmentReturnConstThis.*
	cd $(srcdir); tar -zcf compassRepository/assignmentReturnConstThis.tar.gz assignmentReturnConstThis
	@echo "updated assignmentReturnConstThis archive"

archiveAsynchronousSignalHandler:
	rm -f $(srcdir)/compassRepository/asynchronousSignalHandler.*
	cd $(srcdir); tar -zcf compassRepository/asynchronousSignalHandler.tar.gz asynchronousSignalHandler
	@echo "updated asynchronousSignalHandler archive"

archiveAvoidUsingTheSameHandlerForMultipleSignals:
	rm -f $(srcdir)/compassRepository/avoidUsingTheSameHandlerForMultipleSignals.*
	cd $(srcdir); tar -zcf compassRepository/avoidUsingTheSameHandlerForMultipleSignals.tar.gz avoidUsingTheSameHandlerForMultipleSignals
	@echo "updated avoidUsingTheSameHandlerForMultipleSignals archive"

archiveBooleanIsHas:
	rm -f $(srcdir)/compassRepository/booleanIsHas.*
	cd $(srcdir); tar -zcf compassRepository/booleanIsHas.tar.gz booleanIsHas
	@echo "updated booleanIsHas archive"

archiveBufferOverflowFunctions:
	rm -f $(srcdir)/compassRepository/bufferOverflowFunctions.*
	cd $(srcdir); tar -zcf compassRepository/bufferOverflowFunctions.tar.gz bufferOverflowFunctions
	@echo "updated bufferOverflowFunctions archive"

archiveByteByByteStructureComparison:
	rm -f $(srcdir)/compassRepository/byteByByteStructureComparison.*
	cd $(srcdir); tar -zcf compassRepository/byteByByteStructureComparison.tar.gz byteByByteStructureComparison
	@echo "updated byteByByteStructureComparison archive"

archiveCharStarForString:
	rm -f $(srcdir)/compassRepository/charStarForString.*
	cd $(srcdir); tar -zcf compassRepository/charStarForString.tar.gz charStarForString
	@echo "updated charStarForString archive"

archiveCommaOperator:
	rm -f $(srcdir)/compassRepository/commaOperator.*
	cd $(srcdir); tar -zcf compassRepository/commaOperator.tar.gz commaOperator
	@echo "updated commaOperator archive"

archiveComputationalFunctions:
	rm -f $(srcdir)/compassRepository/computationalFunctions.*
	cd $(srcdir); tar -zcf compassRepository/computationalFunctions.tar.gz computationalFunctions
	@echo "updated computationalFunctions archive"

archiveConstCast:
	rm -f $(srcdir)/compassRepository/constCast.*
	cd $(srcdir); tar -zcf compassRepository/constCast.tar.gz constCast
	@echo "updated constCast archive"

archiveConstructorDestructorCallsVirtualFunction:
	rm -f $(srcdir)/compassRepository/constructorDestructorCallsVirtualFunction.*
	cd $(srcdir); tar -zcf compassRepository/constructorDestructorCallsVirtualFunction.tar.gz constructorDestructorCallsVirtualFunction
	@echo "updated constructorDestructorCallsVirtualFunction archive"

archiveConstStringLiterals:
	rm -f $(srcdir)/compassRepository/constStringLiterals.*
	cd $(srcdir); tar -zcf compassRepository/constStringLiterals.tar.gz constStringLiterals
	@echo "updated constStringLiterals archive"

archiveControlVariableTestAgainstFunction:
	rm -f $(srcdir)/compassRepository/controlVariableTestAgainstFunction.*
	cd $(srcdir); tar -zcf compassRepository/controlVariableTestAgainstFunction.tar.gz controlVariableTestAgainstFunction
	@echo "updated controlVariableTestAgainstFunction archive"

archiveCopyConstructorConstArg:
	rm -f $(srcdir)/compassRepository/copyConstructorConstArg.*
	cd $(srcdir); tar -zcf compassRepository/copyConstructorConstArg.tar.gz copyConstructorConstArg
	@echo "updated copyConstructorConstArg archive"

archiveCppCallsSetjmpLongjmp:
	rm -f $(srcdir)/compassRepository/cppCallsSetjmpLongjmp.*
	cd $(srcdir); tar -zcf compassRepository/cppCallsSetjmpLongjmp.tar.gz cppCallsSetjmpLongjmp
	@echo "updated cppCallsSetjmpLongjmp archive"

archiveCyclomaticComplexity:
	rm -f $(srcdir)/compassRepository/cyclomaticComplexity.*
	cd $(srcdir); tar -zcf compassRepository/cyclomaticComplexity.tar.gz cyclomaticComplexity
	@echo "updated cyclomaticComplexity archive"

archiveDataMemberAccess:
	rm -f $(srcdir)/compassRepository/dataMemberAccess.*
	cd $(srcdir); tar -zcf compassRepository/dataMemberAccess.tar.gz dataMemberAccess
	@echo "updated dataMemberAccess archive"

archiveDeepNesting:
	rm -f $(srcdir)/compassRepository/deepNesting.*
	cd $(srcdir); tar -zcf compassRepository/deepNesting.tar.gz deepNesting
	@echo "updated deepNesting archive"

archiveDefaultCase:
	rm -f $(srcdir)/compassRepository/defaultCase.*
	cd $(srcdir); tar -zcf compassRepository/defaultCase.tar.gz defaultCase
	@echo "updated defaultCase archive"

archiveDefaultConstructor:
	rm -f $(srcdir)/compassRepository/defaultConstructor.*
	cd $(srcdir); tar -zcf compassRepository/defaultConstructor.tar.gz defaultConstructor
	@echo "updated defaultConstructor archive"

archiveDiscardAssignment:
	rm -f $(srcdir)/compassRepository/discardAssignment.*
	cd $(srcdir); tar -zcf compassRepository/discardAssignment.tar.gz discardAssignment
	@echo "updated discardAssignment archive"

archiveDoNotCallPutenvWithAutoVar:
	rm -f $(srcdir)/compassRepository/doNotCallPutenvWithAutoVar.*
	cd $(srcdir); tar -zcf compassRepository/doNotCallPutenvWithAutoVar.tar.gz doNotCallPutenvWithAutoVar
	@echo "updated doNotCallPutenvWithAutoVar archive"

archiveDoNotDeleteThis:
	rm -f $(srcdir)/compassRepository/doNotDeleteThis.*
	cd $(srcdir); tar -zcf compassRepository/doNotDeleteThis.tar.gz doNotDeleteThis
	@echo "updated doNotDeleteThis archive"

archiveDoNotUseCstyleCasts:
	rm -f $(srcdir)/compassRepository/doNotUseCstyleCasts.*
	cd $(srcdir); tar -zcf compassRepository/doNotUseCstyleCasts.tar.gz doNotUseCstyleCasts
	@echo "updated doNotUseCstyleCasts archive"

archiveDuffsDevice:
	rm -f $(srcdir)/compassRepository/duffsDevice.*
	cd $(srcdir); tar -zcf compassRepository/duffsDevice.tar.gz duffsDevice
	@echo "updated duffsDevice archive"

archiveDynamicCast:
	rm -f $(srcdir)/compassRepository/dynamicCast.*
	cd $(srcdir); tar -zcf compassRepository/dynamicCast.tar.gz dynamicCast
	@echo "updated dynamicCast archive"

archiveEmptyInsteadOfSize:
	rm -f $(srcdir)/compassRepository/emptyInsteadOfSize.*
	cd $(srcdir); tar -zcf compassRepository/emptyInsteadOfSize.tar.gz emptyInsteadOfSize
	@echo "updated emptyInsteadOfSize archive"

archiveEnumDeclarationNamespaceClassScope:
	rm -f $(srcdir)/compassRepository/enumDeclarationNamespaceClassScope.*
	cd $(srcdir); tar -zcf compassRepository/enumDeclarationNamespaceClassScope.tar.gz enumDeclarationNamespaceClassScope
	@echo "updated enumDeclarationNamespaceClassScope archive"

archiveExplicitCharSign:
	rm -f $(srcdir)/compassRepository/explicitCharSign.*
	cd $(srcdir); tar -zcf compassRepository/explicitCharSign.tar.gz explicitCharSign
	@echo "updated explicitCharSign archive"

archiveExplicitCopy:
	rm -f $(srcdir)/compassRepository/explicitCopy.*
	cd $(srcdir); tar -zcf compassRepository/explicitCopy.tar.gz explicitCopy
	@echo "updated explicitCopy archive"

archiveExplicitTestForNonBooleanValue:
	rm -f $(srcdir)/compassRepository/explicitTestForNonBooleanValue.*
	cd $(srcdir); tar -zcf compassRepository/explicitTestForNonBooleanValue.tar.gz explicitTestForNonBooleanValue
	@echo "updated explicitTestForNonBooleanValue archive"

archiveFloatForLoopCounter:
	rm -f $(srcdir)/compassRepository/floatForLoopCounter.*
	cd $(srcdir); tar -zcf compassRepository/floatForLoopCounter.tar.gz floatForLoopCounter
	@echo "updated floatForLoopCounter archive"

archiveFloatingPointExactComparison:
	rm -f $(srcdir)/compassRepository/floatingPointExactComparison.*
	cd $(srcdir); tar -zcf compassRepository/floatingPointExactComparison.tar.gz floatingPointExactComparison
	@echo "updated floatingPointExactComparison archive"

archiveFopenFormatParameter:
	rm -f $(srcdir)/compassRepository/fopenFormatParameter.*
	cd $(srcdir); tar -zcf compassRepository/fopenFormatParameter.tar.gz fopenFormatParameter
	@echo "updated fopenFormatParameter archive"

archiveForbiddenFunctions:
	rm -f $(srcdir)/compassRepository/forbiddenFunctions.*
	cd $(srcdir); tar -zcf compassRepository/forbiddenFunctions.tar.gz forbiddenFunctions
	@echo "updated forbiddenFunctions archive"

archiveForLoopConstructionControlStmt:
	rm -f $(srcdir)/compassRepository/forLoopConstructionControlStmt.*
	cd $(srcdir); tar -zcf compassRepository/forLoopConstructionControlStmt.tar.gz forLoopConstructionControlStmt
	@echo "updated forLoopConstructionControlStmt archive"

archiveForLoopCppIndexVariableDeclaration:
	rm -f $(srcdir)/compassRepository/forLoopCppIndexVariableDeclaration.*
	cd $(srcdir); tar -zcf compassRepository/forLoopCppIndexVariableDeclaration.tar.gz forLoopCppIndexVariableDeclaration
	@echo "updated forLoopCppIndexVariableDeclaration archive"

archiveFriendDeclarationModifier:
	rm -f $(srcdir)/compassRepository/friendDeclarationModifier.*
	cd $(srcdir); tar -zcf compassRepository/friendDeclarationModifier.tar.gz friendDeclarationModifier
	@echo "updated friendDeclarationModifier archive"

archiveFunctionCallAllocatesMultipleResources:
	rm -f $(srcdir)/compassRepository/functionCallAllocatesMultipleResources.*
	cd $(srcdir); tar -zcf compassRepository/functionCallAllocatesMultipleResources.tar.gz functionCallAllocatesMultipleResources
	@echo "updated functionCallAllocatesMultipleResources archive"

archiveFunctionDefinitionPrototype:
	rm -f $(srcdir)/compassRepository/functionDefinitionPrototype.*
	cd $(srcdir); tar -zcf compassRepository/functionDefinitionPrototype.tar.gz functionDefinitionPrototype
	@echo "updated functionDefinitionPrototype archive"

archiveFunctionDocumentation:
	rm -f $(srcdir)/compassRepository/functionDocumentation.*
	cd $(srcdir); tar -zcf compassRepository/functionDocumentation.tar.gz functionDocumentation
	@echo "updated functionDocumentation archive"

archiveInductionVariableUpdate:
	rm -f $(srcdir)/compassRepository/inductionVariableUpdate.*
	cd $(srcdir); tar -zcf compassRepository/inductionVariableUpdate.tar.gz inductionVariableUpdate
	@echo "updated inductionVariableUpdate archive"

archiveInternalDataSharing:
	rm -f $(srcdir)/compassRepository/internalDataSharing.*
	cd $(srcdir); tar -zcf compassRepository/internalDataSharing.tar.gz internalDataSharing
	@echo "updated internalDataSharing archive"

archiveLocalizedVariables:
	rm -f $(srcdir)/compassRepository/localizedVariables.*
	cd $(srcdir); tar -zcf compassRepository/localizedVariables.tar.gz localizedVariables
	@echo "updated localizedVariables archive"

archiveLocPerFunction:
	rm -f $(srcdir)/compassRepository/locPerFunction.*
	cd $(srcdir); tar -zcf compassRepository/locPerFunction.tar.gz locPerFunction
	@echo "updated locPerFunction archive"

archiveLowerRangeLimit:
	rm -f $(srcdir)/compassRepository/lowerRangeLimit.*
	cd $(srcdir); tar -zcf compassRepository/lowerRangeLimit.tar.gz lowerRangeLimit
	@echo "updated lowerRangeLimit archive"

archiveMagicNumber:
	rm -f $(srcdir)/compassRepository/magicNumber.*
	cd $(srcdir); tar -zcf compassRepository/magicNumber.tar.gz magicNumber
	@echo "updated magicNumber archive"

archiveMallocReturnValueUsedInIfStmt:
	rm -f $(srcdir)/compassRepository/mallocReturnValueUsedInIfStmt.*
	cd $(srcdir); tar -zcf compassRepository/mallocReturnValueUsedInIfStmt.tar.gz mallocReturnValueUsedInIfStmt
	@echo "updated mallocReturnValueUsedInIfStmt archive"

archiveMultiplePublicInheritance:
	rm -f $(srcdir)/compassRepository/multiplePublicInheritance.*
	cd $(srcdir); tar -zcf compassRepository/multiplePublicInheritance.tar.gz multiplePublicInheritance
	@echo "updated multiplePublicInheritance archive"

archiveNameAllParameters:
	rm -f $(srcdir)/compassRepository/nameAllParameters.*
	cd $(srcdir); tar -zcf compassRepository/nameAllParameters.tar.gz nameAllParameters
	@echo "updated nameAllParameters archive"

archiveNameConsistency:
	rm -f $(srcdir)/compassRepository/nameConsistency.*
	cd $(srcdir); tar -zcf compassRepository/nameConsistency.tar.gz nameConsistency
	@echo "updated nameConsistency archive"

archiveNewDelete:
	rm -f $(srcdir)/compassRepository/newDelete.*
	cd $(srcdir); tar -zcf compassRepository/newDelete.tar.gz newDelete
	@echo "updated newDelete archive"

archiveNoExceptions:
	rm -f $(srcdir)/compassRepository/noExceptions.*
	cd $(srcdir); tar -zcf compassRepository/noExceptions.tar.gz noExceptions
	@echo "updated noExceptions archive"

archiveNoExitInMpiCode:
	rm -f $(srcdir)/compassRepository/noExitInMpiCode.*
	cd $(srcdir); tar -zcf compassRepository/noExitInMpiCode.tar.gz noExitInMpiCode
	@echo "updated noExitInMpiCode archive"

archiveNoGoto:
	rm -f $(srcdir)/compassRepository/noGoto.*
	cd $(srcdir); tar -zcf compassRepository/noGoto.tar.gz noGoto
	@echo "updated noGoto archive"

archiveNonAssociativeRelationalOperators:
	rm -f $(srcdir)/compassRepository/nonAssociativeRelationalOperators.*
	cd $(srcdir); tar -zcf compassRepository/nonAssociativeRelationalOperators.tar.gz nonAssociativeRelationalOperators
	@echo "updated nonAssociativeRelationalOperators archive"

archiveNonmemberFunctionInterfaceNamespace:
	rm -f $(srcdir)/compassRepository/nonmemberFunctionInterfaceNamespace.*
	cd $(srcdir); tar -zcf compassRepository/nonmemberFunctionInterfaceNamespace.tar.gz nonmemberFunctionInterfaceNamespace
	@echo "updated nonmemberFunctionInterfaceNamespace archive"

archiveNonStandardTypeRefArgs:
	rm -f $(srcdir)/compassRepository/nonStandardTypeRefArgs.*
	cd $(srcdir); tar -zcf compassRepository/nonStandardTypeRefArgs.tar.gz nonStandardTypeRefArgs
	@echo "updated nonStandardTypeRefArgs archive"

archiveNonStandardTypeRefReturns:
	rm -f $(srcdir)/compassRepository/nonStandardTypeRefReturns.*
	cd $(srcdir); tar -zcf compassRepository/nonStandardTypeRefReturns.tar.gz nonStandardTypeRefReturns
	@echo "updated nonStandardTypeRefReturns archive"

archiveNonVirtualRedefinition:
	rm -f $(srcdir)/compassRepository/nonVirtualRedefinition.*
	cd $(srcdir); tar -zcf compassRepository/nonVirtualRedefinition.tar.gz nonVirtualRedefinition
	@echo "updated nonVirtualRedefinition archive"

archiveNoOverloadAmpersand:
	rm -f $(srcdir)/compassRepository/noOverloadAmpersand.*
	cd $(srcdir); tar -zcf compassRepository/noOverloadAmpersand.tar.gz noOverloadAmpersand
	@echo "updated noOverloadAmpersand archive"

archiveNoRand:
	rm -f $(srcdir)/compassRepository/noRand.*
	cd $(srcdir); tar -zcf compassRepository/noRand.tar.gz noRand
	@echo "updated noRand archive"

archiveNoSecondTermSideEffects:
	rm -f $(srcdir)/compassRepository/noSecondTermSideEffects.*
	cd $(srcdir); tar -zcf compassRepository/noSecondTermSideEffects.tar.gz noSecondTermSideEffects
	@echo "updated noSecondTermSideEffects archive"

archiveNoSideEffectInSizeof:
	rm -f $(srcdir)/compassRepository/noSideEffectInSizeof.*
	cd $(srcdir); tar -zcf compassRepository/noSideEffectInSizeof.tar.gz noSideEffectInSizeof
	@echo "updated noSideEffectInSizeof archive"

archiveNoTemplateUsage:
	rm -f $(srcdir)/compassRepository/noTemplateUsage.*
	cd $(srcdir); tar -zcf compassRepository/noTemplateUsage.tar.gz noTemplateUsage
	@echo "updated noTemplateUsage archive"

archiveNoVfork:
	rm -f $(srcdir)/compassRepository/noVfork.*
	cd $(srcdir); tar -zcf compassRepository/noVfork.tar.gz noVfork
	@echo "updated noVfork archive"

archiveNullDeref:
	rm -f $(srcdir)/compassRepository/nullDeref.*
	cd $(srcdir); tar -zcf compassRepository/nullDeref.tar.gz nullDeref
	@echo "updated nullDeref archive"

archiveOneLinePerDeclaration:
	rm -f $(srcdir)/compassRepository/oneLinePerDeclaration.*
	cd $(srcdir); tar -zcf compassRepository/oneLinePerDeclaration.tar.gz oneLinePerDeclaration
	@echo "updated oneLinePerDeclaration archive"

archiveOperatorOverloading:
	rm -f $(srcdir)/compassRepository/operatorOverloading.*
	cd $(srcdir); tar -zcf compassRepository/operatorOverloading.tar.gz operatorOverloading
	@echo "updated operatorOverloading archive"

archiveOtherArgument:
	rm -f $(srcdir)/compassRepository/otherArgument.*
	cd $(srcdir); tar -zcf compassRepository/otherArgument.tar.gz otherArgument
	@echo "updated otherArgument archive"

archivePlaceConstantOnTheLhs:
	rm -f $(srcdir)/compassRepository/placeConstantOnTheLhs.*
	cd $(srcdir); tar -zcf compassRepository/placeConstantOnTheLhs.tar.gz placeConstantOnTheLhs
	@echo "updated placeConstantOnTheLhs archive"

archivePreferAlgorithms:
	rm -f $(srcdir)/compassRepository/preferAlgorithms.*
	cd $(srcdir); tar -zcf compassRepository/preferAlgorithms.tar.gz preferAlgorithms
	@echo "updated preferAlgorithms archive"

archivePreferFseekToRewind:
	rm -f $(srcdir)/compassRepository/preferFseekToRewind.*
	cd $(srcdir); tar -zcf compassRepository/preferFseekToRewind.tar.gz preferFseekToRewind
	@echo "updated preferFseekToRewind archive"

archivePreferSetvbufToSetbuf:
	rm -f $(srcdir)/compassRepository/preferSetvbufToSetbuf.*
	cd $(srcdir); tar -zcf compassRepository/preferSetvbufToSetbuf.tar.gz preferSetvbufToSetbuf
	@echo "updated preferSetvbufToSetbuf archive"

archiveProtectVirtualMethods:
	rm -f $(srcdir)/compassRepository/protectVirtualMethods.*
	cd $(srcdir); tar -zcf compassRepository/protectVirtualMethods.tar.gz protectVirtualMethods
	@echo "updated protectVirtualMethods archive"

archivePushBack:
	rm -f $(srcdir)/compassRepository/pushBack.*
	cd $(srcdir); tar -zcf compassRepository/pushBack.tar.gz pushBack
	@echo "updated pushBack archive"

archiveRightShiftMask:
	rm -f $(srcdir)/compassRepository/rightShiftMask.*
	cd $(srcdir); tar -zcf compassRepository/rightShiftMask.tar.gz rightShiftMask
	@echo "updated rightShiftMask archive"

archiveSetPointersToNull:
	rm -f $(srcdir)/compassRepository/setPointersToNull.*
	cd $(srcdir); tar -zcf compassRepository/setPointersToNull.tar.gz setPointersToNull
	@echo "updated setPointersToNull archive"

archiveSingleParameterConstructorExplicitModifier:
	rm -f $(srcdir)/compassRepository/singleParameterConstructorExplicitModifier.*
	cd $(srcdir); tar -zcf compassRepository/singleParameterConstructorExplicitModifier.tar.gz singleParameterConstructorExplicitModifier
	@echo "updated singleParameterConstructorExplicitModifier archive"

archiveSizeOfPointer:
	rm -f $(srcdir)/compassRepository/sizeOfPointer.*
	cd $(srcdir); tar -zcf compassRepository/sizeOfPointer.tar.gz sizeOfPointer
	@echo "updated sizeOfPointer archive"

archiveStringTokenToIntegerConverter:
	rm -f $(srcdir)/compassRepository/stringTokenToIntegerConverter.*
	cd $(srcdir); tar -zcf compassRepository/stringTokenToIntegerConverter.tar.gz stringTokenToIntegerConverter
	@echo "updated stringTokenToIntegerConverter archive"

archiveSubExpressionEvaluationOrder:
	rm -f $(srcdir)/compassRepository/subExpressionEvaluationOrder.*
	cd $(srcdir); tar -zcf compassRepository/subExpressionEvaluationOrder.tar.gz subExpressionEvaluationOrder
	@echo "updated subExpressionEvaluationOrder archive"

archiveTernaryOperator:
	rm -f $(srcdir)/compassRepository/ternaryOperator.*
	cd $(srcdir); tar -zcf compassRepository/ternaryOperator.tar.gz ternaryOperator
	@echo "updated ternaryOperator archive"

archiveTime_tDirectManipulation:
	rm -f $(srcdir)/compassRepository/time_tDirectManipulation.*
	cd $(srcdir); tar -zcf compassRepository/time_tDirectManipulation.tar.gz time_tDirectManipulation
	@echo "updated time_tDirectManipulation archive"

archiveTypeTypedef:
	rm -f $(srcdir)/compassRepository/typeTypedef.*
	cd $(srcdir); tar -zcf compassRepository/typeTypedef.tar.gz typeTypedef
	@echo "updated typeTypedef archive"

archiveUnaryMinus:
	rm -f $(srcdir)/compassRepository/unaryMinus.*
	cd $(srcdir); tar -zcf compassRepository/unaryMinus.tar.gz unaryMinus
	@echo "updated unaryMinus archive"

archiveUninitializedDefinition:
	rm -f $(srcdir)/compassRepository/uninitializedDefinition.*
	cd $(srcdir); tar -zcf compassRepository/uninitializedDefinition.tar.gz uninitializedDefinition
	@echo "updated uninitializedDefinition archive"

archiveUpperRangeLimit:
	rm -f $(srcdir)/compassRepository/upperRangeLimit.*
	cd $(srcdir); tar -zcf compassRepository/upperRangeLimit.tar.gz upperRangeLimit
	@echo "updated upperRangeLimit archive"

archiveVariableNameEqualsDatabaseName:
	rm -f $(srcdir)/compassRepository/variableNameEqualsDatabaseName.*
	cd $(srcdir); tar -zcf compassRepository/variableNameEqualsDatabaseName.tar.gz variableNameEqualsDatabaseName
	@echo "updated variableNameEqualsDatabaseName archive"

archiveVoidStar:
	rm -f $(srcdir)/compassRepository/voidStar.*
	cd $(srcdir); tar -zcf compassRepository/voidStar.tar.gz voidStar
	@echo "updated voidStar archive"

archiveCheckers: $(COMPASS_CHECKER_ARCHIVES)
