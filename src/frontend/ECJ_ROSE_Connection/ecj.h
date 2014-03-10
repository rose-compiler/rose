#ifndef __ECJ_H__
#define __ECJ_H__

#include "jserver.h"

extern jclass currentJavaTraversalClass;
extern JNIEnv *currentEnvironment;
extern jmethodID mainMethod;
extern jmethodID hasConflictsMethod;
extern jmethodID getTempDirectoryMethod;
extern jmethodID createTempFileMethod;
extern jmethodID createTempNamedFileMethod;
extern jmethodID createTempNamedDirectoryMethod;
extern int jvm_ecj_processing(int argc,char** argv);

#endif
