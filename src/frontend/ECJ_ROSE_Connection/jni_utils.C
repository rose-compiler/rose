#include <assert.h>
#include "jni_utils.h"


char jni_to_c_char(jchar arg)
{
    return (char) arg;
}

int jni_to_c_int(jint arg)
{
    return (int) arg;
}

long jni_to_c_long(jlong arg)
{
    return (long) arg;
}

float jni_to_c_float(jfloat arg)
{
    return (float) arg;
}

double jni_to_c_double(jdouble arg)
{
    return (double) arg;
}

bool jni_to_c_bool(jboolean arg)
{
    return arg == JNI_TRUE;
}

jmethodID get_method(JNIEnv *env, jclass cls, const char * mthName, const char * mthSig) {
        jmethodID method_id = env->GetMethodID(cls, mthName, mthSig);
    if ( method_id == NULL )
       {
         printf ("Error: jmethodID not found %s %s\n", mthName, mthSig);
         assert(false);
       }
    return method_id;
}

jclass get_class(JNIEnv *env, jobject jobj) {
        jclass cls = env->GetObjectClass(jobj);
    if ( cls == NULL )
       {
         printf ("Error: jclass cls == NULL! \n");
         assert(false);
       }
    return cls;
}

