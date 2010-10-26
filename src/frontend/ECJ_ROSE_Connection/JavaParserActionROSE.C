// This is the file containing the C++ versions of the JNI functions written to interface with ECJ.


#include <jni.h>
#include <stdio.h>
#include "JavaParser.h"
#include <iostream>
#include <fstream> 

#include "sage3basic.h"

// DQ (10/14/2010):  This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
// Interestingly it must be at the top of the list of include files.
#include "rose_config.h"


extern SgSourceFile* OpenFortranParser_globalFilePointer;


#if 0
JNIEXPORT void JNICALL 
Java_JavaTraversal_invokeINIT(JNIEnv *env, jobject obj)
 {
   if (myfile.is_open())
     {
       myfile << "Digraph G {\n";
     }
 }

 JNIEXPORT void JNICALL 
 Java_JavaTraversal_invokeEND(JNIEnv *env, jobject obj)
 {
  if (myfile.is_open())
     {
       myfile << "}\n";
       myfile.close();
     }
  }

 JNIEXPORT void JNICALL 
 Java_JavaTraversal_invokeNODE(JNIEnv *env, jobject obj, jstring str, jint nr)
 {
   jboolean iscopy;
   const char *classname = (env)->GetStringUTFChars( str, &iscopy);
   //printf("     C-SIDE: Found Node %s (%d)\n",classname,(int)nr);
   myfile << "\"" <<nr << classname << "\" [label=\"" << classname << "\"];\n";
   return;
 }

 JNIEXPORT void JNICALL 
 Java_JavaTraversal_invokeEDGE(JNIEnv *env, jobject obj, jstring str1, jint nr1,jstring str2, jint nr2)
 {
   jboolean iscopy;
   const char *classname1 = (env)->GetStringUTFChars( str1, &iscopy);
   const char *classname2 = (env)->GetStringUTFChars( str2, &iscopy);
   //printf("     C-SIDE: Found Edge %s(%d)->%s(%d)  \n",classname1,(int)nr1,classname2,(int)nr2);
   myfile << "\"" << nr1 << classname1 << "\" -> \"" << nr2 << classname2 << "\";\n";
   return; 
 }
#endif



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
   }


/*
 * Class:     JavaParser
 * Method:    cactionCompilationUnitDeclaration
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_JavaParser_cactionCompilationUnitDeclaration (JNIEnv *, jobject, jstring)
   {
     printf ("Inside of Java_JavaParser_cactionCompilationUnitDeclaration \n");

  // This is already setup by ROSE as part of basic file initialization before calling ECJ.
     ROSE_ASSERT(OpenFortranParser_globalFilePointer != NULL);
   }


