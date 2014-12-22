#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif
/*
 * Method:    cactionInsertImportedPackageOnDemand
 * Signature: (Ljava/lang/String;LX10Token;)V
 */
JNIEXPORT void JNICALL cactionInsertImportedPackageOnDemand
  (JNIEnv *, jclass, jstring, jobject);

/*
 * Method:    cactionInsertImportedTypeOnDemand
 * Signature: (Ljava/lang/String;Ljava/lang/String;LX10Token;)V
 */
JNIEXPORT void JNICALL cactionInsertImportedTypeOnDemand
  (JNIEnv *, jclass, jstring, jstring, jobject);

/*
 * Method:    cactionInsertImportedType
 * Signature: (Ljava/lang/String;Ljava/lang/String;LX10Token;)V
 */
JNIEXPORT void JNICALL cactionInsertImportedType
  (JNIEnv *, jclass, jstring, jstring, jobject);

/*
 * Method:    cactionInsertImportedStaticField
 * Signature: (Ljava/lang/String;LX10Token;)V
 */
JNIEXPORT void JNICALL cactionInsertImportedStaticField
  (JNIEnv *, jclass, jstring, jobject);

/*
 * Method:    cactionPushPackage
 * Signature: (Ljava/lang/String;LX10Token;)V
 */
JNIEXPORT void JNICALL cactionPushPackage
  (JNIEnv *, jclass, jstring, jobject);

/*
 * Method:    cactionUpdatePushPackage
 * Signature: (Ljava/lang/String;LX10Token;)V
 */
JNIEXPORT void JNICALL cactionUpdatePushPackage
  (JNIEnv *, jclass, jstring, jobject);

/*
 * Method:    cactionPopPackage
 * Signature: ()V
 */
JNIEXPORT void JNICALL cactionPopPackage
  (JNIEnv *, jclass);

/*
 * Method:    cactionPushTypeScope
 * Signature: (Ljava/lang/String;Ljava/lang/String;LX10Token;)V
 */
JNIEXPORT void JNICALL cactionPushTypeScope
  (JNIEnv *, jclass, jstring, jstring, jobject);

/*
 * Method:    cactionPopTypeScope
 * Signature: ()V
 */
JNIEXPORT void JNICALL cactionPopTypeScope
  (JNIEnv *, jclass);

/*
 * Method:    cactionPushTypeParameterScope
 * Signature: (Ljava/lang/String;Ljava/lang/String;LX10Token;)V
 */
JNIEXPORT void JNICALL cactionPushTypeParameterScope
  (JNIEnv *, jclass, jstring, jstring, jobject);

/*
 * Method:    cactionPopTypeParameterScope
 * Signature: (LX10Token;)V
 */
JNIEXPORT void JNICALL cactionPopTypeParameterScope
  (JNIEnv *, jclass, jobject);

/*
 * Method:    cactionTest
 * Signature: ()V
 */
JNIEXPORT void JNICALL cactionTest
  (JNIEnv *, jclass);

/*
 * Method:    cactionCompilationUnitList
 * Signature: ()V
 */
JNIEXPORT void JNICALL cactionCompilationUnitList
  (JNIEnv *, jclass);

/*
 * Method:    cactionIsSpecifiedSourceFile
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL cactionIsSpecifiedSourceFile
  (JNIEnv *, jclass, jstring);

/*
 * Method:    cactionEcjFatalCompilationErrors
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL cactionEcjFatalCompilationErrors
  (JNIEnv *, jclass, jstring);

/*
 * Method:    cactionCompilationUnitListEnd
 * Signature: ()V
 */
JNIEXPORT void JNICALL cactionCompilationUnitListEnd
  (JNIEnv *, jclass);

/*
 * Method:    cactionSetupBasicTypes
 * Signature: ()V
 */
JNIEXPORT void JNICALL cactionSetupBasicTypes
  (JNIEnv *, jclass);

/*
 * Method:    cactionParenthesizedExpression
 * Signature: (I)V
 */
JNIEXPORT void JNICALL cactionParenthesizedExpression
  (JNIEnv *, jclass, jint);

/*
 * Method:    cactionPackageAnnotations
 * Signature: (ILX10Token;)V
 */
JNIEXPORT void JNICALL cactionPackageAnnotations
  (JNIEnv *, jclass, jint, jobject);

/*
 * Method:    cactionCompilationUnitDeclaration
 * Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;LX10Token;)V
 */
JNIEXPORT void JNICALL cactionCompilationUnitDeclaration
  (JNIEnv *, jclass, jstring, jstring, jstring, jobject);

/*
 * Method:    cactionCompilationUnitDeclarationEnd
 * Signature: (LX10Token;)V
 */
JNIEXPORT void JNICALL cactionCompilationUnitDeclarationEnd
  (JNIEnv *, jclass, jobject);

/*
 * Method:    cactionCompilationUnitDeclarationError
 * Signature: (Ljava/lang/String;LX10Token;)V
 */
JNIEXPORT void JNICALL cactionCompilationUnitDeclarationError
  (JNIEnv *, jclass, jstring, jobject);

/*
 * Method:    cactionTypeDeclaration
 * Signature: (Ljava/lang/String;Ljava/lang/String;IZZZZZZZZZZZLX10Token;)V
 */
JNIEXPORT void JNICALL cactionTypeDeclaration
  (JNIEnv *, jclass, jstring, jstring, jint, jboolean, jboolean, jboolean, jboolean, jboolean, jboolean, jboolean, jboolean, jboolean, jboolean, jboolean, jobject);

/*
 * Method:    cactionTypeDeclarationHeader
 * Signature: (ZIILX10Token;)V
 */
JNIEXPORT void JNICALL cactionTypeDeclarationHeader
  (JNIEnv *, jclass, jboolean, jint, jint, jobject);

/*
 * Method:    cactionTypeDeclarationEnd
 * Signature: (LX10Token;)V
 */
JNIEXPORT void JNICALL cactionTypeDeclarationEnd
  (JNIEnv *, jclass, jobject);

/*
 * Method:    cactionConstructorDeclaration
 * Signature: (Ljava/lang/String;ILX10Token;)V
 */
JNIEXPORT void JNICALL cactionConstructorDeclaration
  (JNIEnv *, jclass, jstring, jint, jobject);

/*
 * Method:    cactionConstructorDeclarationHeader
 * Signature: (Ljava/lang/String;ZZZIIILX10Token;)V
 */
JNIEXPORT void JNICALL cactionConstructorDeclarationHeader
  (JNIEnv *, jclass, jstring, jboolean, jboolean, jboolean, jint, jint, jint, jobject);

/*
 * Method:    cactionConstructorDeclarationEnd
 * Signature: (IILX10Token;)V
 */
JNIEXPORT void JNICALL cactionConstructorDeclarationEnd
  (JNIEnv *, jclass, jint, jint, jobject);

/*
 * Method:    cactionExplicitConstructorCall
 * Signature: (LX10Token;)V
 */
JNIEXPORT void JNICALL cactionExplicitConstructorCall
  (JNIEnv *, jclass, jobject);

/*
 * Method:    cactionExplicitConstructorCallEnd
 * Signature: (ZZZLjava/lang/String;Ljava/lang/String;IIILX10Token;)V
 */
JNIEXPORT void JNICALL cactionExplicitConstructorCallEnd
  (JNIEnv *, jclass, jboolean, jboolean, jboolean, jstring, jstring, jint, jint, jint, jobject);

/*
 * Method:    cactionMethodDeclaration
 * Signature: (Ljava/lang/String;ILX10Token;)V
 */
JNIEXPORT void JNICALL cactionMethodDeclaration
  (JNIEnv *, jclass, jstring, jint, jobject);

/*
 * Method:    cactionMethodDeclarationHeader
 * Signature: (Ljava/lang/String;ZZZZZZZZZIIILX10Token;)V
 */
JNIEXPORT void JNICALL cactionMethodDeclarationHeader
  (JNIEnv *, jclass, jstring, jboolean, jboolean, jboolean, jboolean, jboolean, jboolean, jboolean, jboolean, jboolean, jint, jint, jint, jobject);

/*
 * Method:    cactionMethodDeclarationEnd
 * Signature: (IILX10Token;)V
 */
JNIEXPORT void JNICALL cactionMethodDeclarationEnd
  (JNIEnv *, jclass, jint, jint, jobject);

/*
 * Method:    cactionTypeParameterReference
 * Signature: (Ljava/lang/String;Ljava/lang/String;ILjava/lang/String;LX10Token;)V
 */
JNIEXPORT void JNICALL cactionTypeParameterReference
  (JNIEnv *, jclass, jstring, jstring, jint, jstring, jobject);

/*
 * Method:    cactionTypeReference
 * Signature: (Ljava/lang/String;Ljava/lang/String;LX10Token;)V
 */
JNIEXPORT void JNICALL cactionTypeReference
  (JNIEnv *, jclass, jstring, jstring, jobject);

/*
 * Method:    cactionQualifiedTypeReference
 * Signature: (Ljava/lang/String;Ljava/lang/String;LX10Token;)V
 */
JNIEXPORT void JNICALL cactionQualifiedTypeReference
  (JNIEnv *, jclass, jstring, jstring, jobject);

/*
 * Method:    cactionArgument
 * Signature: (Ljava/lang/String;LX10Token;)V
 */
JNIEXPORT void JNICALL cactionArgument
  (JNIEnv *, jclass, jstring, jobject);

/*
 * Method:    cactionArgumentEnd
 * Signature: (ILjava/lang/String;LX10Token;)V
 */
JNIEXPORT void JNICALL cactionArgumentEnd
  (JNIEnv *, jclass, jint, jstring, jobject);

/*
 * Method:    cactionCatchArgument
 * Signature: (Ljava/lang/String;LX10Token;)V
 */
JNIEXPORT void JNICALL cactionCatchArgument
  (JNIEnv *, jclass, jstring, jobject);

/*
 * Method:    cactionCatchArgumentEnd
 * Signature: (ILjava/lang/String;IZLX10Token;)V
 */
JNIEXPORT void JNICALL cactionCatchArgumentEnd
  (JNIEnv *, jclass, jint, jstring, jint, jboolean, jobject);

/*
 * Method:    cactionArrayTypeReference
 * Signature: (ILX10Token;)V
 */
JNIEXPORT void JNICALL cactionArrayTypeReference
  (JNIEnv *, jclass, jint, jobject);

/*
 * Method:    cactionArrayTypeReferenceEnd
 * Signature: (Ljava/lang/String;ILX10Token;)V
 */
JNIEXPORT void JNICALL cactionArrayTypeReferenceEnd
  (JNIEnv *, jclass, jstring, jint, jobject);

/*
 * Method:    cactionMessageSend
 * Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;LX10Token;)V
 */
JNIEXPORT void JNICALL cactionMessageSend
  (JNIEnv *, jclass, jstring, jstring, jstring, jobject);

/*
 * Method:    cactionMessageSendEnd
 * Signature: (ZZLjava/lang/String;Ljava/lang/String;Ljava/lang/String;IIILjava/lang/Object;LX10Token;)V
 */
JNIEXPORT void JNICALL cactionMessageSendEnd
  (JNIEnv *, jclass, jboolean, jboolean, jstring, jstring, jstring, jint, jint, jint, jobject, jobject);

/*
 * Method:    cactionStringLiteral
 * Signature: (Ljava/lang/String;LX10Token;)V
 */
JNIEXPORT void JNICALL cactionStringLiteral
  (JNIEnv *, jclass, jstring, jobject);

/*
 * Method:    cactionAllocationExpression
 * Signature: (LX10Token;)V
 */
JNIEXPORT void JNICALL cactionAllocationExpression
  (JNIEnv *, jclass, jobject);

/*
 * Method:    cactionAllocationExpressionEnd
 * Signature: (ZILX10Token;)V
 */
JNIEXPORT void JNICALL cactionAllocationExpressionEnd
  (JNIEnv *, jclass, jboolean, jint, jobject);

/*
 * Method:    cactionANDANDExpression
 * Signature: (LX10Token;)V
 */
JNIEXPORT void JNICALL cactionANDANDExpression
  (JNIEnv *, jclass, jobject);

/*
 * Method:    cactionANDANDExpressionEnd
 * Signature: (LX10Token;)V
 */
JNIEXPORT void JNICALL cactionANDANDExpressionEnd
  (JNIEnv *, jclass, jobject);

/*
 * Method:    cactionAnnotationMethodDeclaration
 * Signature: (Ljava/lang/String;ILX10Token;)V
 */
JNIEXPORT void JNICALL cactionAnnotationMethodDeclaration
  (JNIEnv *, jclass, jstring, jint, jobject);

/*
 * Method:    cactionAnnotationMethodDeclarationEnd
 * Signature: (Ljava/lang/String;IIZLX10Token;)V
 */
JNIEXPORT void JNICALL cactionAnnotationMethodDeclarationEnd
  (JNIEnv *, jclass, jstring, jint, jint, jboolean, jobject);

/*
 * Method:    cactionArrayAllocationExpression
 * Signature: (LX10Token;)V
 */
JNIEXPORT void JNICALL cactionArrayAllocationExpression
  (JNIEnv *, jclass, jobject);

/*
 * Method:    cactionArrayAllocationExpressionEnd
 * Signature: (IZLX10Token;)V
 */
JNIEXPORT void JNICALL cactionArrayAllocationExpressionEnd
  (JNIEnv *, jclass, jint, jboolean, jobject);

/*
 * Method:    cactionArrayInitializer
 * Signature: (LX10Token;)V
 */
JNIEXPORT void JNICALL cactionArrayInitializer
  (JNIEnv *, jclass, jobject);

/*
 * Method:    cactionArrayInitializerEnd
 * Signature: (ILX10Token;)V
 */
JNIEXPORT void JNICALL cactionArrayInitializerEnd
  (JNIEnv *, jclass, jint, jobject);

/*
 * Method:    cactionArrayReference
 * Signature: (LX10Token;)V
 */
JNIEXPORT void JNICALL cactionArrayReference
  (JNIEnv *, jclass, jobject);

/*
 * Method:    cactionArrayReferenceEnd
 * Signature: (LX10Token;)V
 */
JNIEXPORT void JNICALL cactionArrayReferenceEnd
  (JNIEnv *, jclass, jobject);

/*
 * Method:    cactionAssertStatement
 * Signature: (LX10Token;)V
 */
JNIEXPORT void JNICALL cactionAssertStatement
  (JNIEnv *, jclass, jobject);

/*
 * Method:    cactionAssertStatementEnd
 * Signature: (ZLX10Token;)V
 */
JNIEXPORT void JNICALL cactionAssertStatementEnd
  (JNIEnv *, jclass, jboolean, jobject);

/*
 * Method:    cactionAssignment
 * Signature: (LX10Token;)V
 */
JNIEXPORT void JNICALL cactionAssignment
  (JNIEnv *, jclass, jobject);

/*
 * Method:    cactionAssignmentEnd
 * Signature: (LX10Token;)V
 */
JNIEXPORT void JNICALL cactionAssignmentEnd
  (JNIEnv *, jclass, jobject);

/*
 * Method:    cactionBinaryExpression
 * Signature: (LX10Token;)V
 */
JNIEXPORT void JNICALL cactionBinaryExpression
  (JNIEnv *, jclass, jobject);

/*
 * Method:    cactionBinaryExpressionEnd
 * Signature: (ILX10Token;)V
 */
JNIEXPORT void JNICALL cactionBinaryExpressionEnd
  (JNIEnv *, jclass, jint, jobject);

/*
 * Method:    cactionBlock
 * Signature: (LX10Token;)V
 */
JNIEXPORT void JNICALL cactionBlock
  (JNIEnv *, jclass, jobject);

/*
 * Method:    cactionBlockEnd
 * Signature: (ILX10Token;)V
 */
JNIEXPORT void JNICALL cactionBlockEnd
  (JNIEnv *, jclass, jint, jobject);

/*
 * Method:    cactionBreakStatement
 * Signature: (Ljava/lang/String;LX10Token;)V
 */
JNIEXPORT void JNICALL cactionBreakStatement
  (JNIEnv *, jclass, jstring, jobject);

/*
 * Method:    cactionCaseStatement
 * Signature: (ZLX10Token;)V
 */
JNIEXPORT void JNICALL cactionCaseStatement
  (JNIEnv *, jclass, jboolean, jobject);

/*
 * Method:    cactionCaseStatementEnd
 * Signature: (ZLX10Token;)V
 */
JNIEXPORT void JNICALL cactionCaseStatementEnd
  (JNIEnv *, jclass, jboolean, jobject);

/*
 * Method:    cactionCastExpression
 * Signature: (LX10Token;)V
 */
JNIEXPORT void JNICALL cactionCastExpression
  (JNIEnv *, jclass, jobject);

/*
 * Method:    cactionCastExpressionEnd
 * Signature: (LX10Token;)V
 */
JNIEXPORT void JNICALL cactionCastExpressionEnd
  (JNIEnv *, jclass, jobject);

/*
 * Method:    cactionCharLiteral
 * Signature: (CLX10Token;)V
 */
JNIEXPORT void JNICALL cactionCharLiteral
  (JNIEnv *, jclass, jchar, jobject);

/*
 * Method:    cactionClassLiteralAccess
 * Signature: (LX10Token;)V
 */
JNIEXPORT void JNICALL cactionClassLiteralAccess
  (JNIEnv *, jclass, jobject);

/*
 * Method:    cactionClassLiteralAccessEnd
 * Signature: (LX10Token;)V
 */
JNIEXPORT void JNICALL cactionClassLiteralAccessEnd
  (JNIEnv *, jclass, jobject);

/*
 * Method:    cactionClinit
 * Signature: (LX10Token;)V
 */
JNIEXPORT void JNICALL cactionClinit
  (JNIEnv *, jclass, jobject);

/*
 * Method:    cactionConditionalExpression
 * Signature: (LX10Token;)V
 */
JNIEXPORT void JNICALL cactionConditionalExpression
  (JNIEnv *, jclass, jobject);

/*
 * Method:    cactionConditionalExpressionEnd
 * Signature: (LX10Token;)V
 */
JNIEXPORT void JNICALL cactionConditionalExpressionEnd
  (JNIEnv *, jclass, jobject);

/*
 * Method:    cactionContinueStatement
 * Signature: (Ljava/lang/String;LX10Token;)V
 */
JNIEXPORT void JNICALL cactionContinueStatement
  (JNIEnv *, jclass, jstring, jobject);

/*
 * Method:    cactionCompoundAssignment
 * Signature: (LX10Token;)V
 */
JNIEXPORT void JNICALL cactionCompoundAssignment
  (JNIEnv *, jclass, jobject);

/*
 * Method:    cactionCompoundAssignmentEnd
 * Signature: (ILX10Token;)V
 */
JNIEXPORT void JNICALL cactionCompoundAssignmentEnd
  (JNIEnv *, jclass, jint, jobject);

/*
 * Method:    cactionDoStatement
 * Signature: (LX10Token;)V
 */
JNIEXPORT void JNICALL cactionDoStatement
  (JNIEnv *, jclass, jobject);

/*
 * Method:    cactionDoStatementEnd
 * Signature: (LX10Token;)V
 */
JNIEXPORT void JNICALL cactionDoStatementEnd
  (JNIEnv *, jclass, jobject);

/*
 * Method:    cactionDoubleLiteral
 * Signature: (DLjava/lang/String;LX10Token;)V
 */
JNIEXPORT void JNICALL cactionDoubleLiteral
  (JNIEnv *, jclass, jdouble, jstring, jobject);

/*
 * Method:    cactionEmptyStatement
 * Signature: (LX10Token;)V
 */
JNIEXPORT void JNICALL cactionEmptyStatement
  (JNIEnv *, jclass, jobject);

/*
 * Method:    cactionEmptyStatementEnd
 * Signature: (LX10Token;)V
 */
JNIEXPORT void JNICALL cactionEmptyStatementEnd
  (JNIEnv *, jclass, jobject);

/*
 * Method:    cactionEqualExpression
 * Signature: (LX10Token;)V
 */
JNIEXPORT void JNICALL cactionEqualExpression
  (JNIEnv *, jclass, jobject);

/*
 * Method:    cactionEqualExpressionEnd
 * Signature: (ILX10Token;)V
 */
JNIEXPORT void JNICALL cactionEqualExpressionEnd
  (JNIEnv *, jclass, jint, jobject);

/*
 * Method:    cactionExtendedStringLiteral
 * Signature: (Ljava/lang/String;LX10Token;)V
 */
JNIEXPORT void JNICALL cactionExtendedStringLiteral
  (JNIEnv *, jclass, jstring, jobject);

/*
 * Method:    cactionFalseLiteral
 * Signature: (LX10Token;)V
 */
JNIEXPORT void JNICALL cactionFalseLiteral
  (JNIEnv *, jclass, jobject);

/*
 * Method:    cactionFieldDeclarationEnd
 * Signature: (Ljava/lang/String;IZZZZZZZZZZLX10Token;)V
 */
JNIEXPORT void JNICALL cactionFieldDeclarationEnd
  (JNIEnv *, jclass, jstring, jint, jboolean, jboolean, jboolean, jboolean, jboolean, jboolean, jboolean, jboolean, jboolean, jboolean, jobject);

/*
 * Method:    cactionFieldReference
 * Signature: (Ljava/lang/String;LX10Token;)V
 */
JNIEXPORT void JNICALL cactionFieldReference
  (JNIEnv *, jclass, jstring, jobject);

/*
 * Method:    cactionFieldReferenceEnd
 * Signature: (ZLjava/lang/String;LX10Token;)V
 */
JNIEXPORT void JNICALL cactionFieldReferenceEnd
  (JNIEnv *, jclass, jboolean, jstring, jobject);

/*
 * Method:    cactionFloatLiteral
 * Signature: (FLjava/lang/String;LX10Token;)V
 */
JNIEXPORT void JNICALL cactionFloatLiteral
  (JNIEnv *, jclass, jfloat, jstring, jobject);

/*
 * Method:    cactionForeachStatement
 * Signature: (LX10Token;)V
 */
JNIEXPORT void JNICALL cactionForeachStatement
  (JNIEnv *, jclass, jobject);

/*
 * Method:    cactionForeachStatementEnd
 * Signature: (LX10Token;)V
 */
JNIEXPORT void JNICALL cactionForeachStatementEnd
  (JNIEnv *, jclass, jobject);

/*
 * Method:    cactionForStatement
 * Signature: (LX10Token;)V
 */
JNIEXPORT void JNICALL cactionForStatement
  (JNIEnv *, jclass, jobject);

/*
 * Method:    cactionForStatementEnd
 * Signature: (IZILX10Token;)V
 */
JNIEXPORT void JNICALL cactionForStatementEnd
  (JNIEnv *, jclass, jint, jboolean, jint, jobject);

/*
 * Method:    cactionIfStatement
 * Signature: (ZLX10Token;)V
 */
JNIEXPORT void JNICALL cactionIfStatement
  (JNIEnv *, jclass, jboolean, jobject);

/*
 * Method:    cactionIfStatementEnd
 * Signature: (ZLX10Token;)V
 */
JNIEXPORT void JNICALL cactionIfStatementEnd
  (JNIEnv *, jclass, jboolean, jobject);

/*
 * Method:    cactionImportReference
 * Signature: (ZLjava/lang/String;ZLX10Token;)V
 */
JNIEXPORT void JNICALL cactionImportReference
  (JNIEnv *, jclass, jboolean, jstring, jboolean, jobject);

/*
 * Method:    cactionInitializer
 * Signature: (ZLjava/lang/String;ILX10Token;)V
 */
JNIEXPORT void JNICALL cactionInitializer
  (JNIEnv *, jclass, jboolean, jstring, jint, jobject);

/*
 * Method:    cactionInitializerEnd
 * Signature: (LX10Token;)V
 */
JNIEXPORT void JNICALL cactionInitializerEnd
  (JNIEnv *, jclass, jobject);

/*
 * Method:    cactionInstanceOfExpression
 * Signature: (LX10Token;)V
 */
JNIEXPORT void JNICALL cactionInstanceOfExpression
  (JNIEnv *, jclass, jobject);

/*
 * Method:    cactionInstanceOfExpressionEnd
 * Signature: (LX10Token;)V
 */
JNIEXPORT void JNICALL cactionInstanceOfExpressionEnd
  (JNIEnv *, jclass, jobject);

/*
 * Method:    cactionIntLiteral
 * Signature: (ILjava/lang/String;LX10Token;)V
 */
JNIEXPORT void JNICALL cactionIntLiteral
  (JNIEnv *, jclass, jint, jstring, jobject);

/*
 * Method:    cactionLabeledStatement
 * Signature: (Ljava/lang/String;LX10Token;)V
 */
JNIEXPORT void JNICALL cactionLabeledStatement
  (JNIEnv *, jclass, jstring, jobject);

/*
 * Method:    cactionLabeledStatementEnd
 * Signature: (LX10Token;)V
 */
JNIEXPORT void JNICALL cactionLabeledStatementEnd
  (JNIEnv *, jclass, jobject);

/*
 * Method:    cactionLocalDeclaration
 * Signature: (ILjava/lang/String;ZLX10Token;)V
 */
JNIEXPORT void JNICALL cactionLocalDeclaration
  (JNIEnv *, jclass, jint, jstring, jboolean, jobject);

/*
 * Method:    cactionLocalDeclarationEnd
 * Signature: (Ljava/lang/String;ZLX10Token;)V
 */
JNIEXPORT void JNICALL cactionLocalDeclarationEnd
  (JNIEnv *, jclass, jstring, jboolean, jobject);

/*
 * Method:    cactionLongLiteral
 * Signature: (JLjava/lang/String;LX10Token;)V
 */
JNIEXPORT void JNICALL cactionLongLiteral
  (JNIEnv *, jclass, jlong, jstring, jobject);

/*
 * Method:    cactionMarkerAnnotationEnd
 * Signature: (LX10Token;)V
 */
JNIEXPORT void JNICALL cactionMarkerAnnotationEnd
  (JNIEnv *, jclass, jobject);

/*
 * Method:    cactionMemberValuePairEnd
 * Signature: (Ljava/lang/String;LX10Token;)V
 */
JNIEXPORT void JNICALL cactionMemberValuePairEnd
  (JNIEnv *, jclass, jstring, jobject);

/*
 * Method:    cactionStringLiteralConcatenation
 * Signature: (LX10Token;)V
 */
JNIEXPORT void JNICALL cactionStringLiteralConcatenation
  (JNIEnv *, jclass, jobject);

/*
 * Method:    cactionNormalAnnotationEnd
 * Signature: (ILX10Token;)V
 */
JNIEXPORT void JNICALL cactionNormalAnnotationEnd
  (JNIEnv *, jclass, jint, jobject);

/*
 * Method:    cactionNullLiteral
 * Signature: (LX10Token;)V
 */
JNIEXPORT void JNICALL cactionNullLiteral
  (JNIEnv *, jclass, jobject);

/*
 * Method:    cactionORORExpression
 * Signature: (LX10Token;)V
 */
JNIEXPORT void JNICALL cactionORORExpression
  (JNIEnv *, jclass, jobject);

/*
 * Method:    cactionORORExpressionEnd
 * Signature: (LX10Token;)V
 */
JNIEXPORT void JNICALL cactionORORExpressionEnd
  (JNIEnv *, jclass, jobject);

/*
 * Method:    cactionParameterizedTypeReference
 * Signature: (LX10Token;)V
 */
JNIEXPORT void JNICALL cactionParameterizedTypeReference
  (JNIEnv *, jclass, jobject);

/*
 * Method:    cactionParameterizedTypeReferenceEnd
 * Signature: (Ljava/lang/String;Ljava/lang/String;ZILX10Token;)V
 */
JNIEXPORT void JNICALL cactionParameterizedTypeReferenceEnd
  (JNIEnv *, jclass, jstring, jstring, jboolean, jint, jobject);

/*
 * Method:    cactionParameterizedQualifiedTypeReferenceEnd
 * Signature: (Ljava/lang/String;ZILX10Token;)V
 */
JNIEXPORT void JNICALL cactionParameterizedQualifiedTypeReferenceEnd
  (JNIEnv *, jclass, jstring, jboolean, jint, jobject);

/*
 * Method:    cactionPostfixExpression
 * Signature: (LX10Token;)V
 */
JNIEXPORT void JNICALL cactionPostfixExpression
  (JNIEnv *, jclass, jobject);

/*
 * Method:    cactionPostfixExpressionEnd
 * Signature: (ILX10Token;)V
 */
JNIEXPORT void JNICALL cactionPostfixExpressionEnd
  (JNIEnv *, jclass, jint, jobject);

/*
 * Method:    cactionPrefixExpression
 * Signature: (LX10Token;)V
 */
JNIEXPORT void JNICALL cactionPrefixExpression
  (JNIEnv *, jclass, jobject);

/*
 * Method:    cactionPrefixExpressionEnd
 * Signature: (ILX10Token;)V
 */
JNIEXPORT void JNICALL cactionPrefixExpressionEnd
  (JNIEnv *, jclass, jint, jobject);

/*
 * Method:    cactionQualifiedAllocationExpression
 * Signature: (LX10Token;)V
 */
JNIEXPORT void JNICALL cactionQualifiedAllocationExpression
  (JNIEnv *, jclass, jobject);

/*
 * Method:    cactionQualifiedAllocationExpressionEnd
 * Signature: (ZZIZLX10Token;)V
 */
JNIEXPORT void JNICALL cactionQualifiedAllocationExpressionEnd
  (JNIEnv *, jclass, jboolean, jboolean, jint, jboolean, jobject);

/*
 * Method:    cactionQualifiedSuperReference
 * Signature: (LX10Token;)V
 */
JNIEXPORT void JNICALL cactionQualifiedSuperReference
  (JNIEnv *, jclass, jobject);

/*
 * Method:    cactionQualifiedSuperReferenceEnd
 * Signature: (LX10Token;)V
 */
JNIEXPORT void JNICALL cactionQualifiedSuperReferenceEnd
  (JNIEnv *, jclass, jobject);

/*
 * Method:    cactionQualifiedThisReference
 * Signature: (LX10Token;)V
 */
JNIEXPORT void JNICALL cactionQualifiedThisReference
  (JNIEnv *, jclass, jobject);

/*
 * Method:    cactionQualifiedThisReferenceEnd
 * Signature: (LX10Token;)V
 */
JNIEXPORT void JNICALL cactionQualifiedThisReferenceEnd
  (JNIEnv *, jclass, jobject);

/*
 * Method:    cactionReturnStatement
 * Signature: (LX10Token;)V
 */
JNIEXPORT void JNICALL cactionReturnStatement
  (JNIEnv *, jclass, jobject);

/*
 * Method:    cactionReturnStatementEnd
 * Signature: (ZLX10Token;)V
 */
JNIEXPORT void JNICALL cactionReturnStatementEnd
  (JNIEnv *, jclass, jboolean, jobject);

/*
 * Method:    cactionSingleMemberAnnotationEnd
 * Signature: (LX10Token;)V
 */
JNIEXPORT void JNICALL cactionSingleMemberAnnotationEnd
  (JNIEnv *, jclass, jobject);

/*
 * Method:    cactionSingleNameReference
 * Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;LX10Token;)V
 */
JNIEXPORT void JNICALL cactionSingleNameReference
  (JNIEnv *, jclass, jstring, jstring, jstring, jobject);

/*
 * Method:    cactionSuperReference
 * Signature: (LX10Token;)V
 */
JNIEXPORT void JNICALL cactionSuperReference
  (JNIEnv *, jclass, jobject);

/*
 * Method:    cactionSwitchStatement
 * Signature: (LX10Token;)V
 */
JNIEXPORT void JNICALL cactionSwitchStatement
  (JNIEnv *, jclass, jobject);

/*
 * Method:    cactionSwitchStatementEnd
 * Signature: (IZLX10Token;)V
 */
JNIEXPORT void JNICALL cactionSwitchStatementEnd
  (JNIEnv *, jclass, jint, jboolean, jobject);

/*
 * Method:    cactionSynchronizedStatement
 * Signature: (LX10Token;)V
 */
JNIEXPORT void JNICALL cactionSynchronizedStatement
  (JNIEnv *, jclass, jobject);

/*
 * Method:    cactionSynchronizedStatementEnd
 * Signature: (LX10Token;)V
 */
JNIEXPORT void JNICALL cactionSynchronizedStatementEnd
  (JNIEnv *, jclass, jobject);

/*
 * Method:    cactionThisReference
 * Signature: (LX10Token;)V
 */
JNIEXPORT void JNICALL cactionThisReference
  (JNIEnv *, jclass, jobject);

/*
 * Method:    cactionThrowStatement
 * Signature: (LX10Token;)V
 */
JNIEXPORT void JNICALL cactionThrowStatement
  (JNIEnv *, jclass, jobject);

/*
 * Method:    cactionThrowStatementEnd
 * Signature: (LX10Token;)V
 */
JNIEXPORT void JNICALL cactionThrowStatementEnd
  (JNIEnv *, jclass, jobject);

/*
 * Method:    cactionTrueLiteral
 * Signature: (LX10Token;)V
 */
JNIEXPORT void JNICALL cactionTrueLiteral
  (JNIEnv *, jclass, jobject);

/*
 * Method:    cactionTryStatement
 * Signature: (IZLX10Token;)V
 */
JNIEXPORT void JNICALL cactionTryStatement
  (JNIEnv *, jclass, jint, jboolean, jobject);

/*
 * Method:    cactionTryStatementEnd
 * Signature: (IIZLX10Token;)V
 */
JNIEXPORT void JNICALL cactionTryStatementEnd
  (JNIEnv *, jclass, jint, jint, jboolean, jobject);

/*
 * Method:    cactionCatchBlockEnd
 * Signature: (LX10Token;)V
 */
JNIEXPORT void JNICALL cactionCatchBlockEnd
  (JNIEnv *, jclass, jobject);

/*
 * Method:    cactionUnaryExpression
 * Signature: (LX10Token;)V
 */
JNIEXPORT void JNICALL cactionUnaryExpression
  (JNIEnv *, jclass, jobject);

/*
 * Method:    cactionUnaryExpressionEnd
 * Signature: (ILX10Token;)V
 */
JNIEXPORT void JNICALL cactionUnaryExpressionEnd
  (JNIEnv *, jclass, jint, jobject);

/*
 * Method:    cactionWhileStatement
 * Signature: (LX10Token;)V
 */
JNIEXPORT void JNICALL cactionWhileStatement
  (JNIEnv *, jclass, jobject);

/*
 * Method:    cactionWhileStatementEnd
 * Signature: (LX10Token;)V
 */
JNIEXPORT void JNICALL cactionWhileStatementEnd
  (JNIEnv *, jclass, jobject);

/*
 * Method:    cactionWildcard
 * Signature: (LX10Token;)V
 */
JNIEXPORT void JNICALL cactionWildcard
  (JNIEnv *, jclass, jobject);

/*
 * Method:    cactionWildcardEnd
 * Signature: (ZZZLX10Token;)V
 */
JNIEXPORT void JNICALL cactionWildcardEnd
  (JNIEnv *, jclass, jboolean, jboolean, jboolean, jobject);

/*
 * Method:    cactionSetupSourceFilename
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL cactionSetupSourceFilename
  (JNIEnv *, jclass, jstring);

/*
 * Method:    cactionClearSourceFilename
 * Signature: ()V
 */
JNIEXPORT void JNICALL cactionClearSourceFilename
  (JNIEnv *, jclass);

/*
 * Method:    cactionInsertClassStart
 * Signature: (Ljava/lang/String;ZZZLX10Token;)V
 */
JNIEXPORT void JNICALL cactionInsertClassStart
  (JNIEnv *, jclass, jstring, jboolean, jboolean, jboolean, jobject);

/*
 * Method:    cactionInsertClassEnd
 * Signature: (Ljava/lang/String;LX10Token;)V
 */
JNIEXPORT void JNICALL cactionInsertClassEnd
  (JNIEnv *, jclass, jstring, jobject);

/*
 * Method:    cactionBuildClassSupportStart
 * Signature: (Ljava/lang/String;Ljava/lang/String;ZZZZLX10Token;)V
 */
JNIEXPORT void JNICALL cactionBuildClassSupportStart
  (JNIEnv *, jclass, jstring, jstring, jboolean, jboolean, jboolean, jboolean, jobject);

/*
 * Method:    cactionUpdateClassSupportStart
 * Signature: (Ljava/lang/String;LX10Token;)V
 */
JNIEXPORT void JNICALL cactionUpdateClassSupportStart
  (JNIEnv *, jclass, jstring, jobject);

/*
 * Method:    cactionInsertTypeParameter
 * Signature: (Ljava/lang/String;LX10Token;)V
 */
JNIEXPORT void JNICALL cactionInsertTypeParameter
  (JNIEnv *, jclass, jstring, jobject);

/*
 * Method:    cactionBuildTypeParameterSupport
 * Signature: (Ljava/lang/String;Ljava/lang/String;ILjava/lang/String;ILX10Token;)V
 */
JNIEXPORT void JNICALL cactionBuildTypeParameterSupport
  (JNIEnv *, jclass, jstring, jstring, jint, jstring, jint, jobject);

/*
 * Method:    cactionUpdatePushMethodParameterScope
 * Signature: (Ljava/lang/String;Ljava/lang/String;LX10Token;)V
 */
JNIEXPORT void JNICALL cactionUpdatePushMethodParameterScope
  (JNIEnv *, jclass, jint, jobject);

/*
 * Method:    cactionUpdateTypeParameterSupport
 * Signature: (Ljava/lang/String;IILX10Token;)V
 */
JNIEXPORT void JNICALL cactionUpdateTypeParameterSupport
  (JNIEnv *, jclass, jstring, jint, jint, jobject);

/*
 * Method:    cactionUpdatePopMethodParameterScope
 * Signature: (LX10Token;)V
 */
JNIEXPORT void JNICALL cactionUpdatePopMethodParameterScope
  (JNIEnv *, jclass, jobject);

/*
 * Method:    cactionBuildClassExtendsAndImplementsSupport
 * Signature: (IZILX10Token;)V
 */
JNIEXPORT void JNICALL cactionBuildClassExtendsAndImplementsSupport
#if 0
  (JNIEnv *, jclass, jint, jboolean, jint, jobject);
#else
  (JNIEnv *, jclass, jint, jobjectArray, jboolean, jstring, jint, jobjectArray, jobject);
#endif

/*
 * Method:    cactionBuildClassSupportEnd
 * Signature: (Ljava/lang/String;ILX10Token;)V
 */
JNIEXPORT void JNICALL cactionBuildClassSupportEnd
  (JNIEnv *, jclass, jstring, jint, jobject);

/*
 * Method:    cactionUpdateClassSupportEnd
 * Signature: (Ljava/lang/String;ZIILX10Token;)V
 */
JNIEXPORT void JNICALL cactionUpdateClassSupportEnd
  (JNIEnv *, jclass, jstring, jboolean, jint, jint, jobject);

/*
 * Method:    cactionBuildInnerTypeSupport
 * Signature: (Ljava/lang/String;Ljava/lang/String;LX10Token;)V
 */
JNIEXPORT void JNICALL cactionBuildInnerTypeSupport
  (JNIEnv *, jclass, jstring, jstring, jobject);

/*
 * Method:    cactionUpdateInnerTypeSupport
 * Signature: (Ljava/lang/String;Ljava/lang/String;LX10Token;)V
 */
JNIEXPORT void JNICALL cactionUpdateInnerTypeSupport
  (JNIEnv *, jclass, jstring, jstring, jobject);

/*
 * Method:    cactionBuildArgumentSupport
 * Signature: (Ljava/lang/String;Ljava/lang/String;ZZLX10Token;)V
 */
JNIEXPORT void JNICALL cactionBuildArgumentSupport
  (JNIEnv *, jclass, jstring, jstring, jboolean, jboolean, jobject);

/*
 * Method:    cactionUpdateArgumentSupport
 * Signature: (ILjava/lang/String;Ljava/lang/String;ZZLX10Token;)V
 */
JNIEXPORT void JNICALL cactionUpdateArgumentSupport
  (JNIEnv *, jclass, jint, jstring, jstring, jboolean, jboolean, jobject);

/*
 * Method:    cactionBuildMethodSupportStart
 * Signature: (Ljava/lang/String;ILX10Token;)V
 */
JNIEXPORT void JNICALL cactionBuildMethodSupportStart
  (JNIEnv *, jclass, jstring, jint, jobject);

/*
 * Method:    cactionUpdateMethodSupportStart
 * Signature: (Ljava/lang/String;IILX10Token;)V
 */
JNIEXPORT void JNICALL cactionUpdateMethodSupportStart
  (JNIEnv *, jclass, jstring, jint, jint, jobject);

/*
 * Method:    cactionBuildMethodSupportEnd
 * Signature: (Ljava/lang/String;IZZZIIZLX10Token;LX10Token;)V
 */
JNIEXPORT void JNICALL cactionBuildMethodSupportEnd
  (JNIEnv *, jclass, jstring, jint, jboolean, jboolean, jboolean, jint, jint, jboolean, jobject, jobject);

/*
 * Method:    cactionUpdateMethodSupportEnd
 * Signature: (Ljava/lang/String;IZILX10Token;LX10Token;)V
 */
JNIEXPORT void JNICALL cactionUpdateMethodSupportEnd
  (JNIEnv *, jclass, jstring, jint, jboolean, jint, jobject, jobject);

/*
 * Method:    cactionBuildFieldSupport
 * Signature: (Ljava/lang/String;LX10Token;)V
 */
JNIEXPORT void JNICALL cactionBuildFieldSupport
  (JNIEnv *, jclass, jstring, jobject);

/*
 * Method:    cactionUpdateFieldSupport
 * Signature: (Ljava/lang/String;LX10Token;)V
 */
JNIEXPORT void JNICALL cactionUpdateFieldSupport
  (JNIEnv *, jclass, jstring, jobject);

/*
 * Method:    cactionBuildInitializerSupport
 * Signature: (ZLjava/lang/String;ILX10Token;)V
 */
JNIEXPORT void JNICALL cactionBuildInitializerSupport
  (JNIEnv *, jclass, jboolean, jstring, jint, jobject);

/*
 * Method:    cactionArgumentName
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL cactionArgumentName
  (JNIEnv *, jclass, jstring);

/*
 * Method:    cactionArgumentModifiers
 * Signature: (I)V
 */
JNIEXPORT void JNICALL cactionArgumentModifiers
  (JNIEnv *, jclass, jint);

#ifdef __cplusplus
}
#endif
