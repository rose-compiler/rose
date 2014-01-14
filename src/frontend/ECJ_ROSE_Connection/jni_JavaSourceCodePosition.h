#ifndef __jni_source_code_position_h__
#define __jni_source_code_position_h__

#include <jni.h>
#include "JavaSourceCodePosition.h"

using namespace std;

string normalize(string str);

extern string convertJavaStringToCxxString(JNIEnv *env, const jstring &java_string);

JavaSourceCodePosition *convert_Java_SourcePosition(JNIEnv *env, jobject sourcePosition);

#endif
