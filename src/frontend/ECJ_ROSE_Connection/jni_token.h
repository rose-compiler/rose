#ifndef __jni_token_h__
#define __jni_token_h__

#include "jni.h"
#include "token.h"

// Token_t *convert_Java_token(JNIEnv *env, jobject token);
JNIEXPORT Token_t* JNICALL convert_Java_token(JNIEnv *env, jobject token);

#endif
