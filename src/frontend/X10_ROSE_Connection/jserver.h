#ifndef __X10_JSERVER_H__
#define __X10_JSERVER_H__

#include "jni.h"
#include <list>

#define STATIC_METHOD 1
#define REGULAR_METHOD 0

#define xDEBUG(flag,code)  { if(flag) {code; fflush(stdout);}}

class SgClassDefinition;
class SgClassType;
class SgClassType;
class SgClassType;
class SgVariableSymbol;
class SgSourceFile;

namespace Rose {
    namespace Frontend {
        namespace X10 {

            extern SgClassDefinition *x10LangPackageDefinition;
            extern SgClassType *ObjectClassType;
            extern SgClassType *StringClassType;
            extern SgClassType *ClassClassType;
            extern SgVariableSymbol *lengthSymbol;

            namespace X10c {

                extern SgSourceFile* X10c_globalFilePointer;
                extern std::list<std::string> classNames;
                jstring      jserver_getJavaString(const char*);
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

                extern jclass currentX10TraversalClass;
                extern JNIEnv *currentEnvironment;
                extern jmethodID mainMethod;
                extern jmethodID hasConflictsMethod;
                extern jmethodID getTempDirectoryMethod;
                extern jmethodID createTempFileMethod;
                extern jmethodID createTempNamedFileMethod;
                extern jmethodID createTempNamedDirectoryMethod;

            }// Rose::Frontend::X10::X10c
        }// Rose::Frontend::X10
    }// Rose::Frontend
}// Rose

#endif
