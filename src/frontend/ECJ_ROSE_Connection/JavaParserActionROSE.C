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
JNIEXPORT void JNICALL Java_JavaParser_cactionCompilationUnitList (JNIEnv *, jobject, jint, jobjectArray)
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
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionTypeDeclaration (JNIEnv *env, jobject xxx, jstring java_string)
   {
     printf ("Build a SgClassDeclaration \n");

     const char* str = env->GetStringUTFChars(java_string, NULL);
     ROSE_ASSERT(str != NULL);
     printf ("Inside of Java_JavaParser_cactionTypeDeclaration s = %s \n",str);
     env->ReleaseStringUTFChars(java_string, str);

     SgName name = str;
     ROSE_ASSERT(astJavaScopeStack.empty() == false);
     SgClassDeclaration* declaration = SageBuilder::buildDefiningClassDeclaration ( name, astJavaScopeStack.front() );

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
   }

