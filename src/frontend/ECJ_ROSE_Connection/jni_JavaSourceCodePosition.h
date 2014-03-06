#ifndef __jni_source_code_position_h__
#define __jni_source_code_position_h__

#include <string>
#include <jni.h>
#include "JavaSourceCodePosition.h"

std::string normalize(std::string str);

extern std::string convertJavaStringToCxxString(JNIEnv *env, const jstring &java_string);

JavaSourceCodePosition *convert_Java_SourcePosition(JNIEnv *env, jobject sourcePosition);

#endif
