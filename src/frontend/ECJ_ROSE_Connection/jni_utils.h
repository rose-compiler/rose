#include <jni.h>

bool jni_to_c_bool(jboolean arg);
char jni_to_c_char(jchar arg);
int jni_to_c_int(jint arg);
long jni_to_c_long(jlong arg);
float jni_to_c_float(jfloat arg);
double jni_to_c_double(jdouble arg);

jclass get_class(JNIEnv *env, jobject jobj);
jmethodID get_method(JNIEnv *env, jclass cls, const char * mthName, const char * mthSig);
