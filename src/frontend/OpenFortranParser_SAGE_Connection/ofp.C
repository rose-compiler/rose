#include <string.h>
#include <string>
#include <stdlib.h>
#include <stdio.h>

#include "jserver.h"
#include "ofp.h"

using std::string;

static jmethodID jofp_get_method(int, const char*, const char*);
static jmethodID jofp_get_main() ;
static jmethodID jofp_get_error_method();
static jmethodID jofp_get_cons_method();
static jclass    jofp_get_class();
static jobject   jofp_get_new_object(jmethodID, jobjectArray, jstring, jstring);
//static int       jofp_invoke(int, char **);
int       jofp_invoke(int, char **);


int 
jvm_ofp_processing(int argc,char** argv)
{  
    /* call Open Fortran Parser */   
    return jofp_invoke(argc, argv);

}


static jclass 
jofp_get_class() 
{ 
    static jclass ofp_class ;
    if (ofp_class == NULL)  {
        ofp_class = jserver_FindClass("fortran/ofp/FrontEnd");
        if(ofp_class == NULL)  jserver_handleException();
    }
    return ofp_class;
}


static jobject
jofp_get_new_object(jmethodID method, jobjectArray args, jstring name, 
                                                                  jstring type)
{
    return jserver_getNewObject(jofp_get_class(),method, args, name, type);
}


//static int 
int
jofp_invoke(int argc, char **argv)
{
    int retval= 0;

    jobjectArray args;
 
    /* Create a Java String[] out of argv (everything after the first arg).  */
    args = jserver_getJavaStringArray(argc, argv);

    /* The filename of the Fortran source to parse MUST be the LAST
     * argument!  The setting of this and the type below are probably not
     * needed since we call main() and give it all args to parse itself.  
     */
     jstring fileName = jserver_getJavaString(argv[argc-1]);

     jstring type = jserver_getJavaString("fortran.ofp.parser.java.FortranParserActionNull");


     if (fileName == NULL || args == NULL || type == NULL) jserver_handleException(); 

     jserver_callMethod(jofp_get_class(), jofp_get_main(), args);

     jobject new_ofp_class =jofp_get_new_object(jofp_get_cons_method(),args, fileName, type);

     retval = jserver_callBooleanMethod(new_ofp_class, jofp_get_error_method());

     if (retval != 0)  {
         fprintf(stderr, "Warning: jserver_callBooleanMethod return non-zero result.\n");
     }      

    return retval;
}


static jmethodID
jofp_get_method(int static_method, const char* name, const char* arg) 
{
    jmethodID result = NULL;

    /* Get the constructor for FortranMain(String[] args, String fileName,
     * String type).  
     */
    result = jserver_GetMethodID(static_method, jofp_get_class(), name, arg);

    if (result == NULL) jserver_handleException();
	return result;
}


static jmethodID 
jofp_get_cons_method()
{
    static jmethodID  consMethod;

    if (consMethod == NULL) 
         consMethod = jofp_get_method(REGULAR_METHOD, "<init>","([Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
    return consMethod;
}


static jmethodID 
jofp_get_error_method()
{
    static  jmethodID errorMethod;
    if (errorMethod ==NULL) 
         errorMethod = jofp_get_method(STATIC_METHOD, "getError", "()Z");
    return errorMethod;
}


static jmethodID 
jofp_get_main() 
{
   static jmethodID  mainMethod;
   if (mainMethod == NULL) 
        mainMethod = jofp_get_method(STATIC_METHOD, "main", "([Ljava/lang/String;)V");
   return mainMethod;
}



