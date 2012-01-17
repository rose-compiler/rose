#ifndef __jni_source_code_position_h__
#define __jni_source_code_position_h__

#include <jni.h>
#include "JavaSourceCodePosition.h"

JavaSourceCodePosition * convert_Java_SourcePosition(JNIEnv *env, jobject sourcePosition);

#endif
