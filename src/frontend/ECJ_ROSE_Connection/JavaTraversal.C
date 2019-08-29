// This file contains low level JNI functions to build a DOT file of the 
// ECJ AST from a C++ program.  It was an early demo of the use of JNI 
// functions to support the ECJ AST translation to build the ROSE AST.
// None of these functions are used as part of the ECJ ROSE AST translation.

#include <jni.h>
#include <stdio.h>
//#include "JavaTraversal.h"
#include <iostream>
#include <fstream> 

//#error "This JavaTraversal.C file is no longer used."

#if 0

// DQ (8/15/2011): We can't build this file every time we run a ROSE translator...(also causes make distcheck to fail).
// std::ofstream myfile ("astC.dot");

JNIEXPORT void JNICALL Java_JavaTraversal_invokeINIT(JNIEnv *env, jobject obj) {
    // Do Nothing !
}

JNIEXPORT void JNICALL Java_JavaTraversal_invokeEND(JNIEnv *env, jobject obj) {
    // Do Nothing !
}

JNIEXPORT void JNICALL Java_JavaTraversal_invokeNODE(JNIEnv *env, jobject obj, jstring str, jint nr) {
    // Do Nothing !
}

JNIEXPORT void JNICALL Java_JavaTraversal_invokeEDGE(JNIEnv *env, jobject obj, jstring str1, jint nr1,jstring str2, jint nr2) {
    // Do Nothing !
}

#endif
