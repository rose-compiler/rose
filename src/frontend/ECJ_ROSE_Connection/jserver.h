#ifndef __JAVA_JSERVER_H__
#define __JAVA_JSERVER_H__

#include "jni.h"
#include "rosedll.h"

#define STATIC_METHOD 1
#define REGULAR_METHOD 0

#define xDEBUG(flag,code)  { if(flag) {code; fflush(stdout);}}

class SgClassDefinition;
class SgClassType;
class SgClassType;
class SgClassType;
class SgVariableSymbol;
class SgProject;

namespace Rose {
    namespace Frontend {
        namespace Java {

            extern ROSE_DLL_API SgClassDefinition *javaLangPackageDefinition;
            extern ROSE_DLL_API SgClassType *ObjectClassType;
            extern ROSE_DLL_API SgClassType *StringClassType;
            extern ROSE_DLL_API SgClassType *ClassClassType;
            extern ROSE_DLL_API SgVariableSymbol *lengthSymbol;

            namespace Ecj {

                extern ROSE_DLL_API SgProject* Ecj_globalProjectPointer;
                jstring ROSE_DLL_API jserver_getJavaString(const char*);
                jobject      jserver_getNewObject(jclass,jmethodID, jobjectArray,jstring,jstring);
                jclass       jserver_FindClass(const char *name);
                jobjectArray jserver_getJavaStringArray(int argc, char **argv);
                void         jserver_callMethod(jclass, jmethodID, jobjectArray);
                int          jserver_callBooleanMethod(jobject, jmethodID);
                jmethodID    jserver_GetMethodID(int, jclass, const char*, const char*);
                void         jserver_handleException();
                void         jserver_init();
                void         jserver_finish();
                JNIEnv* getEnv();

                extern ROSE_DLL_API jclass currentJavaTraversalClass;
                extern ROSE_DLL_API JNIEnv *currentEnvironment;
                extern jmethodID mainMethod;
                extern ROSE_DLL_API jmethodID hasConflictsMethod;
                extern jmethodID getTempDirectoryMethod;
                extern jmethodID createTempFileMethod;
                extern jmethodID createTempNamedFileMethod;
                extern jmethodID createTempNamedDirectoryMethod;

            }// Rose::Frontend::Java::Ecj
        }// Rose::Frontend::Java
    }// Rose::Frontend
}// Rose

#endif
