// This is the file containing the C++ versions of the JNI functions written to interface with ECJ.
// These functions are called from the JAVA code (within the Java based traversal over the ECJ AST) 
// and construct the ROSE AST.

// Support for calling the Java <--> C/C++ code.
#include <jni.h>

// What was this?
#include "JavaParser.h"

// Support for the internal ROSE IR classes.
#include "sage3basic.h"

// DQ (10/14/2010):  This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
// Interestingly it must be at the top of the list of include files.
#include "rose_config.h"

// Support functions so that this file can be restricted to be just parser (AST traversal) rules.
#include "java_support.h"

/*
 * Class:     JavaParser
 * Method:    cactionCompilationUnitList
 * Signature: (java/lang/String;)
 */
JNIEXPORT void JNICALL Java_JavaParser_cactionCompilationUnitList (JNIEnv *env, jobject, jint, jobjectArray)
   {
     printf ("Inside of Java_JavaParser_cactionCompilationUnitList \n");

  // This is already setup by ROSE as part of basic file initialization before calling ECJ.
     ROSE_ASSERT(OpenFortranParser_globalFilePointer != NULL);
     printf ("OpenFortranParser_globalFilePointer = %s \n",OpenFortranParser_globalFilePointer->class_name().c_str());

     SgSourceFile* sourceFile = isSgSourceFile(OpenFortranParser_globalFilePointer);
     ROSE_ASSERT(sourceFile != NULL);

     printf ("sourceFile->getFileName() = %s \n",sourceFile->getFileName().c_str());

  // We don't use the SgProject but since it should have already been built, we can verify that it is present.
     SgProject* project = sourceFile->get_project();
     ROSE_ASSERT(project != NULL);

  // Get the pointer to the global scope and push it onto the astJavaScopeStack.
     SgGlobal* globalScope = sourceFile->get_globalScope();
     ROSE_ASSERT(globalScope != NULL);

  // Push the global scope onto the stack (each file has a single global scope).
     ROSE_ASSERT(astJavaScopeStack.empty() == true);
     astJavaScopeStack.push_front(globalScope);
     ROSE_ASSERT(astJavaScopeStack.empty() == false);

  // Verify that the parent is set, these AST nodes are already setup by ROSE before calling this function.
     ROSE_ASSERT(astJavaScopeStack.front()->get_parent() != NULL);
   }


/*
 * Class:     JavaParser
 * Method:    cactionCompilationUnitDeclaration
 * Signature: (java/lang/String;)
 */
JNIEXPORT void JNICALL Java_JavaParser_cactionCompilationUnitDeclaration (JNIEnv *env, jobject xxx, jstring compilationUnitFilename)
   {
  // Example of how to get the string...but we don't really use the absolutePathFilename in this function.
     const char* absolutePathFilename = env->GetStringUTFChars(compilationUnitFilename, NULL);
     ROSE_ASSERT(absolutePathFilename != NULL);
     printf ("Inside of Java_JavaParser_cactionCompilationUnitDeclaration absolutePathFilename = %s \n",absolutePathFilename);
     env->ReleaseStringUTFChars(compilationUnitFilename, absolutePathFilename);

  // This is already setup by ROSE as part of basic file initialization before calling ECJ.
     ROSE_ASSERT(OpenFortranParser_globalFilePointer != NULL);

     ROSE_ASSERT(astJavaScopeStack.empty() == false);
     ROSE_ASSERT(astJavaScopeStack.front()->get_parent() != NULL);
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionTypeDeclaration (JNIEnv *env, jobject xxx, jstring java_string)
   {
     printf ("Build a SgClassDeclaration \n");

  // We could provide a constructor for "SgName" that takes a "jstring".  This might help support a simpler interface.
     SgName name = convertJavaStringToCxxString(env,java_string);

     printf ("Build class type: name = %s \n",name.str());

     outputJavaState("At TOP of cactionTypeDeclaration");

#if 1
     buildClass(name);
#else
     ROSE_ASSERT(astJavaScopeStack.empty() == false);
     SgClassDeclaration* declaration = SageBuilder::buildDefiningClassDeclaration ( name, astJavaScopeStack.front() );

     ROSE_ASSERT(declaration->get_type() != NULL);

  // Set the source code position...
  // setSourcePosition(declaration);
  // setSourcePositionCompilerGenerated(declaration);

  // void setSourcePosition  ( SgLocatedNode* locatedNode );
  // void setSourcePositionCompilerGenerated( SgLocatedNode* locatedNode );

     ROSE_ASSERT(astJavaScopeStack.empty() == false);
     SgClassDefinition* definition = SageBuilder::buildClassDefinition(declaration);

  // Set the source code position...
  // setSourcePosition(definition);
  // setSourcePositionCompilerGenerated(definition);

     astJavaScopeStack.push_front(definition);
     ROSE_ASSERT(astJavaScopeStack.front()->get_parent() != NULL);

  // Add "super()" member function.
     SgMemberFunctionDeclaration* functionDeclaration = buildSimpleMemberFunction("super");
     ROSE_ASSERT(functionDeclaration != NULL);
#endif

     ROSE_ASSERT(astJavaScopeStack.front() != NULL);
     astJavaScopeStack.front()->get_file_info()->display("source position in Java_JavaParser_cactionTypeDeclaration(): debug");

     outputJavaState("At BOTTOM of cactionTypeDeclaration");

     printf ("Leaving Java_JavaParser_cactionTypeDeclaration() \n");
   }




JNIEXPORT void JNICALL Java_JavaParser_cactionConstructorDeclaration (JNIEnv *env, jobject xxx, jstring java_string)
   {
     printf ("Build a SgMemberFunctionDeclaration (constructor) \n");

#if 1
  // Comment out to test simpler useage.

  // SgMemberFunctionDeclaration* buildDefiningMemberFunctionDeclaration (const SgName & name, SgType* return_type, SgFunctionParameterList *parlist, SgScopeStatement* scope=NULL);
     SgName name = "abc";

#if 1
     SgMemberFunctionDeclaration* functionDeclaration = buildSimpleMemberFunction(name);
#else
     SgClassDefinition* classDefinition = isSgClassDefinition(astJavaScopeStack.front());
     ROSE_ASSERT(classDefinition != NULL);

     SgFunctionParameterTypeList* typeList = SageBuilder::buildFunctionParameterTypeList();
     ROSE_ASSERT(typeList != NULL);

  // Specify if this is const, volatile, or restrict (0 implies normal member function).
     unsigned int mfunc_specifier = 0;
     SgMemberFunctionType* return_type = SageBuilder::buildMemberFunctionType(SgTypeVoid::createType(), typeList, classDefinition, mfunc_specifier);
     ROSE_ASSERT(return_type != NULL);

     SgFunctionParameterList* parameterlist = SageBuilder::buildFunctionParameterList(typeList);
     ROSE_ASSERT(parameterlist != NULL);

     SgMemberFunctionDeclaration* functionDeclaration = SageBuilder::buildDefiningMemberFunctionDeclaration (name, return_type, parameterlist, astJavaScopeStack.front() );
     ROSE_ASSERT(functionDeclaration != NULL);

     ROSE_ASSERT(functionDeclaration->get_definingDeclaration() != NULL);

  // non-defining declaration not built yet.
     ROSE_ASSERT(functionDeclaration->get_firstNondefiningDeclaration() == NULL);
#endif

     SgFunctionDefinition* functionDefinition = functionDeclaration->get_definition();
     ROSE_ASSERT(functionDefinition != NULL);

     astJavaScopeStack.push_front(functionDefinition);
     ROSE_ASSERT(astJavaScopeStack.front()->get_parent() != NULL);

     ROSE_ASSERT(functionDefinition->get_body() != NULL);
     astJavaScopeStack.push_front(functionDefinition->get_body());
     ROSE_ASSERT(astJavaScopeStack.front()->get_parent() != NULL);
#endif
   }

JNIEXPORT void JNICALL Java_JavaParser_cactionConstructorDeclarationEnd (JNIEnv *env, jobject xxx)
   {
     printf ("End of SgMemberFunctionDeclaration (constructor) \n");

  // Pop the constructor body...
     ROSE_ASSERT(astJavaScopeStack.empty() == false);
     astJavaScopeStack.pop_front();
 
  // Pop the fuction definition...
     ROSE_ASSERT(astJavaScopeStack.empty() == false);
     astJavaScopeStack.pop_front();
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionExplicitConstructorCall (JNIEnv *env, jobject xxx, jstring java_string)
   {
  // Build a member function call...
     printf ("Build a constructor function call \n");

  // Should this be a SgBasicBlock or just a SgScopeStatement?
     SgBasicBlock* basicBlock = isSgBasicBlock(astJavaScopeStack.front());
     ROSE_ASSERT(basicBlock != NULL);
     ROSE_ASSERT(basicBlock->get_parent() != NULL);

     SgName name = convertJavaStringToCxxString(env,java_string);

     printf ("buiding function call: name = %s \n",name.str());

     SgExprListExp* parameters = NULL;
     SgExprStatement* expressionStatement = SageBuilder::buildFunctionCallStmt(name,SgTypeVoid::createType(),parameters,astJavaScopeStack.front());
     ROSE_ASSERT(expressionStatement != NULL);

     ROSE_ASSERT(astJavaScopeStack.empty() == false);
     astJavaScopeStack.front()->append_statement(expressionStatement);
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionMethodDeclaration (JNIEnv *env, jobject, jstring java_string)
   {
     printf ("Build a SgMemberFunctionDeclaration \n");

     SgName name = convertJavaStringToCxxString(env,java_string);
     SgMemberFunctionDeclaration* functionDeclaration = buildSimpleMemberFunction(name);

     astJavaScopeStack.front()->append_statement(functionDeclaration);

  // Push the declaration onto the declaration stack.
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionSingleTypeReference (JNIEnv *, jobject, jstring)
   {
     printf ("Build a type \n");

  // Build a type and put it onto the type stack.
  // ...OR...
  // Build a type and add it to the declaration on the declaration stack.
   }

JNIEXPORT void JNICALL Java_JavaParser_cactionArgument (JNIEnv *, jobject, jstring)
   {
     printf ("Build a function argument \n");
   }

JNIEXPORT void JNICALL Java_JavaParser_cactionArrayTypeReference (JNIEnv *, jobject, jstring)
   {
     printf ("Build a array type \n");
   }

JNIEXPORT void JNICALL Java_JavaParser_cactionMessageSend (JNIEnv *, jobject, jstring)
   {
     printf ("Build a member function call \n");
   }

JNIEXPORT void JNICALL Java_JavaParser_cactionQualifiedNameReference (JNIEnv *, jobject, jstring)
   {
     printf ("Build a qualified name reference \n");
   }

JNIEXPORT void JNICALL Java_JavaParser_cactionStringLiteral (JNIEnv *, jobject, jstring)
   {
     printf ("Build a SgStringVal \n");
   }





// DQ: Note that the function signature is abby-normal...jclass instead of jobject (because they are 
// declared "public static native" instead of "public native" in the Java side of the JNI interface.
JNIEXPORT void JNICALL Java_JavaParser_cactionBuildImplicitClassSupportStart (JNIEnv* env, jclass xxx, jstring java_string)
   {
     printf ("Build support for implicit class (start) \n");

     outputJavaState("At TOP of cactionBuildImplicitClassSupportStart");

#if 1
     SgName name = convertJavaStringToCxxString(env,java_string);
     buildClass(name);
#endif

     outputJavaState("At BOTTOM of cactionBuildImplicitClassSupportStart");
   }


// DQ: Note that the function signature is abby-normal...jclass instead of jobject (because they are 
// declared "public static native" instead of "public native" in the Java side of the JNI interface.
JNIEXPORT void JNICALL Java_JavaParser_cactionBuildImplicitClassSupportEnd (JNIEnv* env, jclass xxx, jstring java_string)
   {
     printf ("Build support for implicit class (end) \n");

     outputJavaState("cactionBuildImplicitClassSupportEnd");

  // Experiment with ERROR on C++ side...communicated to Java...and back to C++ side where the JVM is called by ROSE...
  // ROSE_ASSERT(false);

  // Pop the class definition off the scope stack...
     ROSE_ASSERT(astJavaScopeStack.empty() == false);
     astJavaScopeStack.pop_front();
   }


// DQ: Note that the function signature is abby-normal...jclass instead of jobject (because they are 
// declared "public static native" instead of "public native" in the Java side of the JNI interface.
JNIEXPORT void JNICALL Java_JavaParser_cactionBuildImplicitMethodSupport (JNIEnv* env, jclass xxx, jstring java_string)
   {
  // printf ("Build support for implicit class member function (method) \n");

     SgName name = convertJavaStringToCxxString(env,java_string);
     SgMemberFunctionDeclaration* functionDeclaration = buildSimpleMemberFunction(name);
     ROSE_ASSERT(functionDeclaration != NULL);
   }


// DQ: Note that the function signature is abby-normal...jclass instead of jobject (because they are 
// declared "public static native" instead of "public native" in the Java side of the JNI interface.
JNIEXPORT void JNICALL Java_JavaParser_cactionBuildImplicitFieldSupport (JNIEnv* env, jclass xxx, jstring java_string)
   {
  // printf ("Build support for implicit class data member (field) \n");

     SgName name = convertJavaStringToCxxString(env,java_string);
     SgVariableDeclaration* variableDeclaration = buildSimpleVariableDeclaration(name);
     ROSE_ASSERT(variableDeclaration != NULL);

     variableDeclaration->get_file_info()->display("source position in Java_JavaParser_cactionBuildImplicitFieldSupport(): debug");
   }


JNIEXPORT void cactionAllocationExpression(JNIEnv *env, jobject xxx)
   {
   }

JNIEXPORT void cactionAND_AND_Expression(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionAnnotationMethodDeclaration(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionArgumentClassScope(JNIEnv *, jobject, jstring)
   {
   }


JNIEXPORT void cactionArrayAllocationExpression(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionArrayInitializer(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionArrayQualifiedTypeReference(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionArrayQualifiedTypeReferenceClassScope(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionArrayReference(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionArrayTypeReferenceClassScope(JNIEnv *, jobject, jstring)
   {
   }


JNIEXPORT void cactionAssertStatement(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionAssignment(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionBinaryExpression(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionBlock(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionBreakStatement(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionCaseStatement(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionCastExpression(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionCharLiteral(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionClassLiteralAccess(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionClinit(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionConditionalExpression(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionContinueStatement(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionCompoundAssignment(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionDoStatement(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionDoubleLiteral(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionEmptyStatement(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionEqualExpression(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionExtendedStringLiteral(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionFalseLiteral(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionFieldDeclaration(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionFieldReference(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionFieldReferenceClassScope(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionFloatLiteral(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionForeachStatement(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionForStatement(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionIfStatement(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionImportReference(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionInitializer(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionInstanceOfExpression(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionIntLiteral(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionJavadoc(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionJavadocClassScope(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionJavadocAllocationExpression(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionJavadocAllocationExpressionClassScope(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionJavadocArgumentExpression(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionJavadocArgumentExpressionClassScope(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionJavadocArrayQualifiedTypeReference(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionJavadocArrayQualifiedTypeReferenceClassScope(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionJavadocArraySingleTypeReference(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionJavadocArraySingleTypeReferenceClassScope(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionJavadocFieldReference(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionJavadocFieldReferenceClassScope(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionJavadocImplicitTypeReference(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionJavadocImplicitTypeReferenceClassScope(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionJavadocMessageSend(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionJavadocMessageSendClassScope(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionJavadocQualifiedTypeReference(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionJavadocQualifiedTypeReferenceClassScope(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionJavadocReturnStatement(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionJavadocReturnStatementClassScope(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionJavadocSingleNameReference(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionJavadocSingleNameReferenceClassScope(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionJavadocSingleTypeReference(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionJavadocSingleTypeReferenceClassScope(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionLabeledStatement(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionLocalDeclaration(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionLongLiteral(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionMarkerAnnotation(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionMemberValuePair(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionStringLiteralConcatenation(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionNormalAnnotation(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionNullLiteral(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionOR_OR_Expression(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionParameterizedQualifiedTypeReference(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionParameterizedQualifiedTypeReferenceClassScope(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionParameterizedSingleTypeReference(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionParameterizedSingleTypeReferenceClassScope(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionPostfixExpression(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionPrefixExpression(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionQualifiedAllocationExpression(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionQualifiedSuperReference(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionQualifiedSuperReferenceClassScope(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionQualifiedThisReference(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionQualifiedThisReferenceClassScope(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionQualifiedTypeReference(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionQualifiedTypeReferenceClassScope(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionReturnStatement(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionSingleMemberAnnotation(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionSingleNameReference(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionSingleNameReferenceClassScope(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionSuperReference(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionSwitchStatement(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionSynchronizedStatement(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionThisReference(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionThisReferenceClassScope(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionThrowStatement(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionTrueLiteral(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionTryStatement(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionTypeParameter(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionTypeParameterClassScope(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionUnaryExpression(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionWhileStatement(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionWildcard(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void cactionWildcardClassScope(JNIEnv *env, jobject xxx)
   {
   }


