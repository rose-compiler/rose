// This file contains low level JNI functions to build a DOT file of the 
// ECJ AST from a C++ program.  It was an early demo of the use of JNI 
// functions to support the ECJ AST translation to build the ROSE AST.
// None of these functions are used as part of the ECJ ROSE AST translation.

 #include <jni.h>
 #include <stdio.h>
 #include "JavaTraversal.h"
 #include <iostream>
 #include <fstream> 

// #error "This JavaTraversal.C file is no longer used."

#if 1
// DQ (8/15/2011): We can't build this file every time we run a ROSE translator...(also causes make distcheck to fail).
// std::ofstream myfile ("astC.dot");

 JNIEXPORT void JNICALL 
 Java_JavaTraversal_invokeINIT(JNIEnv *env, jobject obj)
 {
#if 0
// DQ (8/15/2011): since we don't build the file, avoid writing to it.
   if (myfile.is_open())
     {
       myfile << "Digraph G {\n";
     }
#endif
 }

 JNIEXPORT void JNICALL 
 Java_JavaTraversal_invokeEND(JNIEnv *env, jobject obj)
 {
#if 0
// DQ (8/15/2011): since we don't build the file, avoid writing to it.
  if (myfile.is_open())
     {
       myfile << "}\n";
       myfile.close();
     }
#endif
  }

 JNIEXPORT void JNICALL 
 Java_JavaTraversal_invokeNODE(JNIEnv *env, jobject obj, jstring str, jint nr)
 {
#if 0
// DQ (8/15/2011): since we don't build the file, avoid writing to it.
   jboolean iscopy;
   const char *classname = (env)->GetStringUTFChars( str, &iscopy);
   //printf("     C-SIDE: Found Node %s (%d)\n",classname,(int)nr);
   myfile << "\"" <<nr << classname << "\" [label=\"" << classname << "\"];\n";
#endif
   return;
 }

 JNIEXPORT void JNICALL 
 Java_JavaTraversal_invokeEDGE(JNIEnv *env, jobject obj, jstring str1, jint nr1,jstring str2, jint nr2)
 {
#if 0
// DQ (8/15/2011): since we don't build the file, avoid writing to it.
   jboolean iscopy;
   const char *classname1 = (env)->GetStringUTFChars( str1, &iscopy);
   const char *classname2 = (env)->GetStringUTFChars( str2, &iscopy);
   //printf("     C-SIDE: Found Edge %s(%d)->%s(%d)  \n",classname1,(int)nr1,classname2,(int)nr2);
   myfile << "\"" << nr1 << classname1 << "\" -> \"" << nr2 << classname2 << "\";\n";
#endif
   return; 
 }
#endif
