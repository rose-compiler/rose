#include<string>

#include "jni_x10_token.h"
#include "jni_x10_utils.h"
#include "jni_x10SourceCodePosition.h"
#include <assert.h>

X10_Token_t *convert_X10_token(JNIEnv *env, jobject token) {
    jclass cls;
    jstring java_string;
    const char *text = NULL;
    jmethodID method_id;
    X10_Token_t *our_token = NULL;
    X10SourceCodePosition * pos_info = NULL;
    jobject jpos_info;

    assert(env != NULL);

    // Need to get the object class so we can use it to retrieve the methods.
    // printf ("Need to get the object class so we can use it to retrieve the methods. \n");  
    cls = get_class(env, token);

    // Get the method ID for the getText() method.
    // printf ("Get the method ID for the getText() method. \n");
    method_id = get_method(env, cls, "getText", "()Ljava/lang/String;");

    // Call getText() to get the Java String object (note that the cast is critical!).
    // printf ("Call getText() to get the Java String object. \n");
    java_string = (jstring) env->CallObjectMethod(token, method_id);

    // Get a C char string for the Java String if the Java String exists.
    // printf ("Get a C char string for the Java String if the Java String exists. \n");
    if (java_string != NULL) {
        text = env -> GetStringUTFChars(java_string, NULL);
        if (text == NULL) {
            fprintf(stderr, "Unable to retrieve text string from String object\n");
            assert(false);
        }
    }
    else {
        text = NULL;
    }

    // Get the method ID for the getJavaSourcePositionInformation() method.
    method_id = get_method(env, cls, "getX10SourcePositionInformation", "()LX10SourcePositionInformation;");
    jpos_info = (jobject) env->CallObjectMethod(token, method_id);

#if 0
    // DQ (2/14/2013): Missing X10 function.
    // Convert position to its C-based representation
    pos_info = convert_X10_SourcePosition(env, jpos_info);
#endif

    // Build a C-based representation of the JavaToken
    our_token = new X10_Token_t(text, pos_info);

    // printf ("returning from convert_Java_token()\n");
    return our_token;
}

