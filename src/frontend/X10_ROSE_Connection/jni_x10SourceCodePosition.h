#ifndef __jni_source_code_position_h__
#define __jni_source_code_position_h__

#include <jni.h>
#include "x10SourceCodePosition.h"

X10SourceCodePosition * convert_X10_SourcePosition(JNIEnv *env, jobject sourcePosition);

std::string normalize(std::string str);

extern std::string convertJavaStringToCxxString(JNIEnv *env, const jstring &java_string);

#endif
