#include <iostream>
#include <string>
#include <assert.h>
#include "jni_x10SourceCodePosition.h"
#include "jni_x10_utils.h"

using namespace std;

//
// Replace newline character by its escape character sequence.
//
// TODO: PC Question: Shouldn't the unparser be doing this as it is already processing
//                    other escape sequences such as \" and \'.
//
string normalize(string source) {
    string target = "";
    for (string::iterator it = source.begin(); it < source.end(); it++) {
        switch(*it) {
            case '\0':
                target += "\\0";
                break;
            case '\n':
                target += "\\n";
                break;
            case '\r':
                target += "\\r";
                break;
            case '\\':
                target += "\\\\";
                break;
            default:
                target += (*it);
                break;
        }
    }

    return target;
}



X10SourceCodePosition *convert_X10_SourcePosition(JNIEnv *env, jobject sourcePosition) {
    jclass cls;
    jmethodID method_id;
    X10SourceCodePosition *pos_info = NULL;

    int line_start = -1;
    int line_end   = -1;
    int col_start  = -1;
    int col_end    = -1;

    assert(env != NULL);

    // Need to get the object class so we can use it to retrieve the methods.
    cls = get_class(env, sourcePosition);

    // ************************
    // Get the line information
    // ************************
    method_id = get_method(env, cls, "getLineStart", "()I");
    line_start = (int) env->CallIntMethod(sourcePosition, method_id);

    method_id = get_method(env, cls, "getLineEnd", "()I");
    line_end = (int) env->CallIntMethod(sourcePosition, method_id);

    // **************************
    // Get the column information
    // **************************
    method_id = get_method(env, cls, "getColumnStart", "()I");
    col_start = (int) env->CallIntMethod(sourcePosition, method_id);

    method_id = get_method(env, cls, "getColumnEnd", "()I");
    col_end = (int) env->CallIntMethod(sourcePosition, method_id);

    // build a Token of our own.
    pos_info = new X10SourceCodePosition(line_start,line_end,col_start,col_end);

    return pos_info;
}

std::string convertJavaStringToCxxString(JNIEnv *env, const jstring &java_string) {
     // Note that "env" can't be passed into this function as "const".
    const char *str = env -> GetStringUTFChars(java_string, NULL);
    assert(str != NULL);

    string returnString = str;

    // printf ("Inside of convertJavaStringToCxxString s = %s \n", str);

    // Note that str is not set to NULL.
    env -> ReleaseStringUTFChars(java_string, str);

    return normalize(returnString);
}
