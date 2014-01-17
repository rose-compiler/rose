#ifndef __ECJ_H__
#define __ECJ_H__

#include "jserver.h"

extern jclass currentJavaTraversalClass;
extern JNIEnv *currentEnvironment;
extern jmethodID classHasConflictsMethod;
extern int jvm_ecj_processing(int argc,char** argv);

#endif
