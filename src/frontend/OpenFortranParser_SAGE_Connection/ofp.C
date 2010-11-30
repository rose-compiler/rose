#include <string.h>
#include <string>
#include <stdlib.h>
#include <stdio.h>

#include "jserver.h"
#include "ofp.h"

using std::string;

static jclass    jofp_get_class(const char* name);
static jmethodID jofp_get_method(jclass xclass, int static_method, const char* name, const char* arg);
static jclass    jofp_get_frontEnd_class();
static jmethodID jofp_get_frontEnd_main_method() ;
static jmethodID jofp_get_frontEnd_getError_method();


int 
jvm_ofp_processing(int argc, char **argv)
{  
    jobjectArray args = jserver_getJavaStringArray(argc, argv);
    if (args == NULL) jserver_handleException();

    jserver_callMethod(jofp_get_frontEnd_class(), jofp_get_frontEnd_main_method(), args);
    int retval = jserver_callBooleanMethod(jofp_get_frontEnd_class(), jofp_get_frontEnd_getError_method());

    return retval;
}


static jclass 
jofp_get_class(const char* name)
{
    jclass xclass = jserver_FindClass(name);
    if (xclass == NULL) jserver_handleException();
    return xclass;
}


static jmethodID
jofp_get_method(jclass xclass, int static_method, const char* name, const char* arg)
{
    jmethodID method = jserver_GetMethodID(static_method, xclass, name, arg);
    if (method == NULL) jserver_handleException();
	return method;
}


static jclass
jofp_get_frontEnd_class()
{
    static jclass frontEndClass;
    if (frontEndClass == NULL) frontEndClass = jofp_get_class("fortran/ofp/FrontEnd");
    return frontEndClass;
}


static jmethodID 
jofp_get_frontEnd_main_method()
{
   static jmethodID  mainMethod;
   if (mainMethod == NULL)
        mainMethod = jofp_get_method(jofp_get_frontEnd_class(), STATIC_METHOD, "main", "([Ljava/lang/String;)V");
   return mainMethod;
}


static jmethodID 
jofp_get_frontEnd_getError_method()
{
    static  jmethodID getErrorMethod;
    if (getErrorMethod == NULL)
    	getErrorMethod = jofp_get_method(jofp_get_frontEnd_class(), STATIC_METHOD, "getError", "()Z");
    return getErrorMethod;
}



