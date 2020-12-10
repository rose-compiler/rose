#ifndef __JSERVER_H__
#define __JSERVER_H__

#include "jni.h"

#define STATIC_METHOD 1
#define REGULAR_METHOD 0

#define xDEBUG(flag,code)  { if(flag) {code; fflush(stdout);}}

namespace Rose {
namespace Frontend {
namespace Fortran {
namespace Ofp {
  jstring      jserver_getJavaString(const char*);
  jobject       jserver_getNewObject(jclass,jmethodID, jobjectArray,jstring,jstring);
  jclass       jserver_FindClass(const char *name);
  jobjectArray jserver_getJavaStringArray(int argc, char **argv);
  void         jserver_callMethod(jclass, jmethodID, jobjectArray);
  int          jserver_callStaticBooleanMethod(jclass, jmethodID);
  jmethodID    jserver_GetMethodID(int, jclass, const char*, const char*);
  void         jserver_handleException();
  void         jserver_init();
  void         jserver_finish();
  JNIEnv* getEnv();
}// ::Rose::Frontend::Fortran::Ofp
}// ::Rose::Frontend::Fortran
}// ::Rose::Frontend
}// ::Rose

#endif

