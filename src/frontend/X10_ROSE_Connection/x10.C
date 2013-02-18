#include <string.h>
#include <string>
#include <stdlib.h>
#include <stdio.h>

#include "jserver.h"
#include "x10.h"

using std::string;

static jmethodID jofp_get_method(int, const char*, const char*);

// DQ (4/17/2011): This is not used.
// static jmethodID jofp_get_main() ;

static jmethodID jofp_get_error_method();
static jmethodID jofp_get_cons_method();
static jclass    jofp_get_class();
static jobject   jofp_get_new_object(jmethodID, jobjectArray, jstring, jstring);

// DQ (10/11/2010): Make this static to avoid conflict with ofp.C.
static int       jofp_invoke(int, char **);
// int       jofp_invoke(int, char **);


int jvm_x10_processing(int argc, char **argv){  
    /* call Open Fortran Parser */   
    return jofp_invoke(argc, argv);
}


#if 1
// DQ (2/14/2013): I needed to copy this functions from jserver.C to resolve 
// undefined references in the link stage.  This should not have been required.

// I think that this should be moved to a header file (e.g. jserver.h).
typedef struct {
   JavaVM * jvm;
   JNIEnv * env;
} JvmT;

JNIEnv*  get_env();
JavaVM*  get_jvm();
JvmT*    get_jvmEnv();
void     jserver_start(JvmT* je);


// I am unclear why these three functions have to be defined in order to avoid unresolved references at link time.
jobject
jserver_getNewObject(jclass cls,jmethodID method, jobjectArray args,
                                        jstring name,jstring type)
{
    JNIEnv* env = get_env();
    return env->NewObject(cls,method, args, name, type);
}

int
jserver_callBooleanMethod(jobject obj_class, jmethodID method)
{
    int retv = 0; 
    JNIEnv *env = get_env();

    retv = env->CallBooleanMethod(obj_class, method);
    return retv;
}

jmethodID
jserver_GetMethodID(int static_method, jclass obj_class, 
                           const char* name, const char* path)
{
   JNIEnv *env = get_env();
   if (static_method==STATIC_METHOD)   
     return  env->GetStaticMethodID(obj_class,name, path);
   else
     return  env->GetMethodID(obj_class,name, path);
}
#endif

static jclass jofp_get_class() { 
    static jclass ofp_class ;
    if (ofp_class == NULL) {
        // ofp_class = jserver_FindClass("fortran/ofp/FrontEnd");
        ofp_class = jserver_FindClass("JavaTraversal");
        // ofp_class = jserver_FindClass("org.eclipse.jdt.internal.compiler.batch.Main");

        if (ofp_class == NULL)  jserver_handleException();
    }
    return ofp_class;
}


static jobject jofp_get_new_object(jmethodID method, jobjectArray args, jstring name, jstring type) {
    return jserver_getNewObject(jofp_get_class(),method, args, name, type);
}


static int jofp_invoke(int argc, char **argv) {
    int retval= 0;

    jobjectArray args;
 
    /* Create a Java String[] out of argv (everything after the first arg).  */
    args = jserver_getJavaStringArray(argc, argv);

    /* The filename of the Fortran source to parse MUST be the LAST
     * argument!  The setting of this and the type below are probably not
     * needed since we call main() and give it all args to parse itself.  
     */
     jstring fileName = jserver_getJavaString(argv[argc-1]);
    // printf ("In jofp_invoke(): fileName = %s \n",argv[argc-1]);

    // jstring type = jserver_getJavaString("fortran.ofp.parser.java.FortranParserActionNull");
    // jstring type = jserver_getJavaString("JavaTraversal");
    jstring type = jserver_getJavaString("JavaTraversal");

    if (fileName == NULL || args == NULL || type == NULL) jserver_handleException(); 

    // tps : this code is more transparent and easier to read
    jclass cls = jserver_FindClass("JavaTraversal");
    jmethodID  mainMethod = jserver_GetMethodID(STATIC_METHOD, cls, "main",  "([Ljava/lang/String;)V");
    JNIEnv* env = getEnv();
    (*env).CallStaticVoidMethod(cls, mainMethod,args);

    jmethodID errorMethod = jofp_get_method(STATIC_METHOD, "getError", "()Z");
    retval = (*env).CallBooleanMethod(cls, errorMethod);
    if (retval != 0)  {
        fprintf(stderr, "C++ side : Error detected ---------------------------------.\n");
        abort();
    }      

    // printf("We are done -----------------------------------------\n");

    // DQ (10/12/2010): This function is not implemented in the ECJ parser (only in OFP).
    jobject new_ofp_class = jofp_get_new_object(jofp_get_cons_method(),args, fileName, type);

    // DQ (10/12/2010): This function is not implemented in the ECJ parser (only in OFP).
    retval = jserver_callBooleanMethod(new_ofp_class, jofp_get_error_method());

    if (retval != 0) {
        fprintf(stderr, "Warning: jserver_callBooleanMethod return non-zero result.\n");
    }

    return retval;
}


static jmethodID jofp_get_method(int static_method, const char* name, const char* arg) {
    jmethodID result = NULL;

    /* Get the constructor for FortranMain(String[] args, String fileName,
     * String type).  
     */
    result = jserver_GetMethodID(static_method, jofp_get_class(), name, arg);

    if (result == NULL) jserver_handleException();
        return result;
}


static jmethodID jofp_get_cons_method() {
    static jmethodID  consMethod;

    if (consMethod == NULL) {
        // DQ (10/12/2010): These methods are not defined properly...(OK for OFP, but not for JavaTraversal; I don't know why).
        // consMethod = jofp_get_method(REGULAR_METHOD, "<init>","([Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
        // consMethod = jofp_get_method(REGULAR_METHOD, "FrontEnd","([Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
        // consMethod = jofp_get_method(REGULAR_METHOD, "printToDOT","([Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
        // consMethod = jofp_get_method(REGULAR_METHOD, "getLast","([Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
        // consMethod = jofp_get_method(REGULAR_METHOD, "getError","([Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");

        // DQ (10/12/2010): This is the original method used in this function and which fails (but works for OFP).
        // consMethod = jofp_get_method(REGULAR_METHOD, "<init>","([Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");

        // DQ (10/12/2010): At least this method appears to exist...
        consMethod = jofp_get_method(STATIC_METHOD, "getError", "()Z");
    }
    return consMethod;
}


static jmethodID jofp_get_error_method() {
    static  jmethodID errorMethod;
    if (errorMethod ==NULL) 
        errorMethod = jofp_get_method(STATIC_METHOD, "getError", "()Z");
    return errorMethod;
}
