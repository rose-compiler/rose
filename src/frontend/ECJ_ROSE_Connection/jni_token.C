#include "jni_token.h"
#include "jni_utils.h"
#include "jni_JavaSourceCodePosition.h"
#include <assert.h>
#include <iostream>

using namespace std;

Token_t *convert_Java_token(JNIEnv *env, jobject token) {
    assert(env != NULL);

    // Need to get the object class so we can use it to retrieve the methods.
    // printf ("Need to get the object class so we can use it to retrieve the methods. \n");  
    jclass cls = get_class(env, token);

    // ************************
    // Get the file information
    // ************************
    jmethodID method_id = get_method(env, cls, "getFileName", "()Ljava/lang/String;");
    string filename = convertJavaStringToCxxString(env, (jstring) env->CallObjectMethod(token, method_id));

// TODO: Remove this !!!
/*
    jstring java_string;
    const char *text = NULL;
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
*/

    // Get the method ID for the getJavaSourcePositionInformation() method.
    method_id = get_method(env, cls, "getJavaSourcePositionInformation", "()LJavaSourcePositionInformation;");
    jobject jpos_info = (jobject) env->CallObjectMethod(token, method_id);

    // Convert position to its C-based representation
    JavaSourceCodePosition *pos_info = convert_Java_SourcePosition(env, jpos_info);

    // Build a C-based representation of the JavaToken
    Token_t *our_token = new Token_t(filename, pos_info);

    // printf ("returning from convert_Java_token()\n");
    return our_token;
}

