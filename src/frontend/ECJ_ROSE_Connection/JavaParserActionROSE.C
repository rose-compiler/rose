// This is the file containing the C++ versions of the JNI functions written to interface with ECJ.


#include <jni.h>
#include "JavaParser.h"

// #include <stdio.h>
// #include <iostream>
// #include <fstream> 

#include "sage3basic.h"

// DQ (10/14/2010):  This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
// Interestingly it must be at the top of the list of include files.
#include "rose_config.h"

// Support functions so that this file can be restricted to be just parser (AST traversal) rules.
#include "java_support.h"

// #include "fortran_support.h"


// extern SgSourceFile* OpenFortranParser_globalFilePointer;


/*
 * Class:     JavaParser
 * Method:    cactionCompilationUnitList
 * Signature: (I[Ljava/lang/String;)V
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

     SgProject* project = sourceFile->get_project();
     ROSE_ASSERT(project != NULL);

     SgGlobal* globalScope = sourceFile->get_globalScope();
     ROSE_ASSERT(globalScope != NULL);

  // Push the global scope onto the stack (each file has a single global scope).
     ROSE_ASSERT(astJavaScopeStack.empty() == true);
     astJavaScopeStack.push_front(globalScope);
     ROSE_ASSERT(astJavaScopeStack.empty() == false);
     ROSE_ASSERT(astJavaScopeStack.front()->get_parent() != NULL);
   }


/*
 * Class:     JavaParser
 * Method:    cactionCompilationUnitDeclaration
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_JavaParser_cactionCompilationUnitDeclaration (JNIEnv *env, jobject xxx, jstring java_string)
   {
  // Example of how to get the string...
     const char* str = env->GetStringUTFChars(java_string, NULL);
     ROSE_ASSERT(str != NULL);
     printf ("Inside of Java_JavaParser_cactionCompilationUnitDeclaration s = %s \n",str);
     env->ReleaseStringUTFChars(java_string, str);

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
   }




JNIEXPORT void JNICALL Java_JavaParser_cactionConstructorDeclaration (JNIEnv *env, jobject xxx, jstring java_string)
   {
     printf ("Build a SgMemberFunctionDeclaration (constructor) \n");

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
