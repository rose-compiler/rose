#ifndef __JSERVER_H__
#define __JSERVER_H__

#include "jni.h"

#define STATIC_METHOD 1
#define REGULAR_METHOD 0

#define xDEBUG(flag,code)  { if(flag) {code; fflush(stdout);}}

extern jstring      jserver_getJavaString(const char*);
extern jobject       jserver_getNewObject(jclass,jmethodID, jobjectArray,jstring,jstring);
extern jclass       jserver_FindClass(const char *name);
extern jobjectArray jserver_getJavaStringArray(int argc, char **argv);
extern void         jserver_callMethod(jclass, jmethodID, jobjectArray);
extern int          jserver_callBooleanMethod(jobject, jmethodID);
extern jmethodID    jserver_GetMethodID(int, jclass, const char*, const char*);
extern void         jserver_handleException();
extern void         jserver_init();
extern void         jserver_finish();
extern JNIEnv* get_env(); //tps
#endif
