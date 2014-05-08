#ifndef __jni_x10_token_h__
#define __jni_x10_token_h__

#include <jni.h>
#include "x10_token.h"
#include "jni_token.h"

X10_Token_t* convert_X10_token(JNIEnv *env, jobject token);

Token_t* convert_Java_token(JNIEnv *env, jobject token);

#endif
