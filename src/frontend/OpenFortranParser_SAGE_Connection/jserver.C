/* Based on examples/docs from:
 * http://java.sun.com/j2se/1.4.2/docs/guide/jni/spec/invocation.html#wp9502
 * http://java.sun.com/j2se/1.4.2/docs/guide/jni/spec/jniTOC.html
 * http://java.sun.com/docs/books/jni/html/invoke.html
 */
#include <string.h>
#include <string>
#include <stdlib.h>
#include <stdio.h>

#include "commandline_processing.h"

#include "jserver.h"

// DQ (10/14/2010):  This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
// Interestingly it must be at the top of the list of include files.
#include "rose_config.h"

using namespace std;

typedef struct {
   JavaVM * jvm;
   JNIEnv * env;
} JvmT;


static JvmT je;

#if 0  
static JNIEnv*  get_env();
static JavaVM*  get_jvm();
static JvmT*    get_jvmEnv();
static void     jserver_start(JvmT* je);
static void     jserver_destroy();
static jclass   jserver_getJavaStringClass();
#else //gdb cannot stop at a static funciton
JNIEnv*  get_env();
JavaVM*  get_jvm();
JvmT*    get_jvmEnv();
void     jserver_start(JvmT* je);
void     jserver_destroy();
jclass   jserver_getJavaStringClass();
#endif

// static jchar*   convertToJChar(const char *c_string);

/* These  should be defined in jni.h */
#ifndef JNI_VERSION_1_6
#define JNI_VERSION_1_6 6
#endif
#ifndef JNI_VERSION_1_4
#define JNI_VERSION_1_4 4
#endif
#ifndef JNI_VERSION_1_2
#define JNI_VERSION_1_2 2
#endif
#ifndef JNI_VERSION_1_1
#define JNI_VERSION_1_1 1
#endif


/* 
 * This function does nothing since Java VM will
 * be loaded and started automatically when it needed. 
 *
 */
void 
jserver_init()
{
  return ;
}


void 
jserver_finish()
{
  return jserver_destroy();
}


jclass 
jserver_FindClass(const char *name)
{
   JNIEnv *env = get_env();
   return env->FindClass(name);
};


jmethodID
jserver_GetMethodID(int static_method, jclass obj_class, 
                           const char* name, const char* path)
{
   JNIEnv *env = get_env();
   if (static_method==STATIC_METHOD)   
     return  env->GetStaticMethodID(obj_class,name, path);
   else
     return  env->GetMethodID(obj_class,name, path);
};

void
jserver_callMethod(jclass obj_class, jmethodID method, jobjectArray args)
{  
    JNIEnv *env = get_env();
    env->CallStaticVoidMethod(obj_class, method, args);
    return ;
};

int
jserver_callBooleanMethod(jobject obj_class, jmethodID method)
{
    int retv = 0; 
    JNIEnv *env = get_env();

    retv = env->CallBooleanMethod(obj_class, method);
    return retv;
};


//! Java VM loaded here
/*! This function return <jvm, env> if Java VM is already load/run
 *  otherwise load/run Java VM
 */
//static JvmT* 
JvmT* 
get_jvmEnv()
{
#if 0
  // DQ (3/11/2010): Updating to new Fortran OFP version 0.7.2 with Craig.
     printf ("In get_jvmEnv(): This code is not called and I think it can be removed... \n");
     ROSE_ASSERT(false);
#endif

  //static JvmT je;
  if (je.jvm == NULL ) {
       jserver_start(&je);
  }
  return &je; 
}

//static JavaVM* 
JavaVM* 
get_jvm()
{
   return (get_jvmEnv())->jvm;
}

//static JNIEnv* 
JNIEnv* 
get_env()
{
   return (get_jvmEnv())->env;
}

// DQ (4/5/2010): Centralize the specification of the class path.
extern string global_build_classpath();

//static void 
void 
jserver_start(JvmT* je)
{ 
  JavaVMInitArgs jvm_args;  /* VM initialization args.  */
  // char *jni_version = NULL;
  int res;

  // char *classpathEnvVar = NULL;
  // char *classpath;

#if 0
     string classpath = "-Djava.class.path=";
  // DQ (3/11/2010): Updating to new Fortran OFP version 0.7.2 with Craig.
  // classpath += findRoseSupportPathFromBuild("/src/3rdPartyLibraries/fortran-parser/OpenFortranParser.jar", "lib/OpenFortranParser.jar") + ":";
  // classpath += findRoseSupportPathFromSource("/src/3rdPartyLibraries/antlr-jars/antlr-2.7.7.jar", "lib/antlr-2.7.7.jar") + ":";
  // classpath += findRoseSupportPathFromSource("/src/3rdPartyLibraries/antlr-jars/antlr-3.0.1.jar", "lib/antlr-3.0.1.jar") + ":";
  // classpath += findRoseSupportPathFromSource("/src/3rdPartyLibraries/antlr-jars/antlr-runtime-3.0.1.jar", "lib/antlr-runtime-3.0.1.jar") + ":";
  // classpath += findRoseSupportPathFromSource("/src/3rdPartyLibraries/antlr-jars/stringtemplate-3.1b1.jar", "lib/stringtemplate-3.1b1.jar") + ":";
     classpath += findRoseSupportPathFromSource("/src/3rdPartyLibraries/antlr-jars/antlr-3.2.jar", "lib/antlr-3.2.jar") + ":";

  // DQ (3/12/2010): Moved OpenFortranParser-0.7.2.jar to fortran-parser from fortran-parser/lib
  // classpath += findRoseSupportPathFromSource("/src/3rdPartyLibraries/fortran-parser/lib/OpenFortranParser-0.7.2.jar", "lib/OpenFortranParser-0.7.2.jar") + ":";
#if ROSE_OFP_MINOR_VERSION_NUMBER >= 8 & ROSE_OFP_PATCH_VERSION_NUMBER >= 0
     classpath += findRoseSupportPathFromSource("/src/3rdPartyLibraries/fortran-parser/OpenFortranParser-0.8.0.jar", "OpenFortranParser-0.8.0.jar") + ":";
#else
   #if ROSE_OFP_MINOR_VERSION_NUMBER == 7 & ROSE_OFP_PATCH_VERSION_NUMBER == 2
     classpath += findRoseSupportPathFromSource("/src/3rdPartyLibraries/fortran-parser/OpenFortranParser-0.7.2.jar", "OpenFortranParser-0.7.2.jar") + ":";
   #else
     printf ("Error: Unknown version of OFP \n");
     ROSE_ASSERT(false);
   #endif
#endif

     classpath += ".";
#else
  // DQ (3/12/2010): We can't use this since we don't include the ROSE header files 
  // (however, this might simplify the design).
  // string classpath = SgSourceFile::build_classpath();
     string classpath = global_build_classpath();
#endif

#if 0
  // DQ (3/11/2010): Updating to new Fortran OFP version 0.7.2 with Craig.
     printf ("In jserver_start(): This code is not called and I think it can be removed... \n");
     ROSE_ASSERT(false);
#endif

  //const char *javaCPOption = "-Djava.class.path=";
  //int len;


  //classpathEnvVar = getenv("CLASSPATH");

  //len = strlen(classpathEnvVar) + strlen(javaCPOption) + 1;
 
  //classpath = malloc(sizeof(char)*len);
  //snprintf(classpath, len, "%s%s\0", javaCPOption, classpathEnvVar);

   /* Set up the VM initialization args.  */
  //jni_version = getenv("JNI_VERSION");
#if 0
  if(jni_version)
  {
  if(strcmp(jni_version, "1.6") == 0)
    jvm_args.version = JNI_VERSION_1_6;
  else if(strcmp(jni_version, "1.4") == 0)
    jvm_args.version = JNI_VERSION_1_4;
  else if(strcmp(jni_version, "1.2") == 0)
    jvm_args.version = JNI_VERSION_1_2;
  else if(strcmp(jni_version, "1.1") == 0)
    jvm_args.version = JNI_VERSION_1_1;
  else
    {
    /* We got an *invalid* JNI_VERSION.  Try defaulting to 1_4.  */
    fprintf(stderr, "Warning: Invalid JNI_VERSION.\nDefaulting to 1.4!\n");
    jvm_args.version = JNI_VERSION_1_4;
    }
   } else  {
     /* The user did not set JNI_VERSION in their environment.  
      * Warn them and then try defaulting to 1_4.  
      */
     fprintf(stderr, "Warning: JNI_VERSION environment variable is not set! Defaulting to 1.4!\n");
     jvm_args.version = JNI_VERSION_1_4;
    }

 
   jvm_args.options = malloc(sizeof(JavaVMOption));

   xDEBUG(jvm_args.options==NULL, printf("Fatal Error: cannot allocate memory for \"jvm_args.options\". \n"));

   jvm_args.nOptions = 1;

   jvm_args.options->optionString = strdup(classpath);

   jvm_args.ignoreUnrecognized = JNI_FALSE;
#endif

     jvm_args.version = JNI_VERSION_1_4;
     jvm_args.nOptions = 1;
     jvm_args.options = new JavaVMOption[jvm_args.nOptions];
     jvm_args.options[0].optionString = strdup(classpath.c_str());
     jvm_args.ignoreUnrecognized = JNI_FALSE;

  // JNI_CreateJavaVM(&jvm, (void **)&env, &jvm_args);
  // handleExceptionMaybe(jvm, env);

  // printf ("Create and load the Java VM: Calling JNI_CreateJavaVM(); \n");
     
  /* Create and load the Java VM.  */
     res = JNI_CreateJavaVM(&(je->jvm), (void **)&(je->env), &jvm_args);

     if (res<0 || je->jvm==NULL || je->env==NULL)
          exit(1);  
   return ;
}



//static void 
void 
jserver_destroy()
{
    
    JavaVM*  jvm = get_jvm();
    if (jvm != NULL) {
         /* Destroy the Java VM.  */
         jvm->DestroyJavaVM();
    }
}


void 
jserver_handleException() 
{
    JNIEnv* env = get_env();
     if(env->ExceptionOccurred())
        env->ExceptionDescribe();

     jserver_destroy();

     /* Exit since the exception should mean we can't recover.  */
     exit(1);
}


jobject
jserver_getNewObject(jclass cls,jmethodID method, jobjectArray args,
                                        jstring name,jstring type)
{
    JNIEnv* env = get_env();
    return env->NewObject(cls,method, args, name, type);

}
  
jstring 
jserver_getJavaString(const char *inString)
{
#if 0
	 /* Based on example from:
	  * http://java.sun.com/docs/books/jni/html/fldmeth.html#11202  */
	 jclass stringClass;
	 jmethodID consID;
	 jstring javaString;
	 jcharArray elemArray;
         JNIEnv*  env = get_env();

	 /* Get the java.lang.String class.  */
	 stringClass = jserver_getJavaStringClass();
	 if(stringClass == NULL)
		return NULL;

	 /* Get the String(char[]) constructor.  */
	 consID = env->GetMethodID(stringClass, "<init>", "([C)V");

	 if(consID == NULL)
		return NULL;

	 /* Create the char[] that holds the chars in 'string'.  */
	 elemArray = env->NewCharArray(strlen(string));

	 if(elemArray == NULL)
		return NULL;

	 (*env)->SetCharArrayRegion(elemArray, 0, strlen(string),
	                                            convertToJChar(string));

	 /* Create the java.lang.String object by invoking the given constructor:
	  * String(char[]).  
	  */
	 javaString = env->NewObject(stringClass, consID, elemArray);

	 /* Free local references.  */
	 env->DeleteLocalRef(elemArray);

	 env->DeleteLocalRef(stringClass);

	 return javaString;
#endif
         JNIEnv*  env = get_env();
         return env->NewStringUTF(inString);
}


jobjectArray 
jserver_getJavaStringArray(int argc, char **argv)
{
	 jobjectArray argsStringArray = NULL;
	 jclass stringClass;
	 int i;
         JNIEnv * env = get_env();

	 /* We need the String class because that is the underlying type of
		 the array.  */
	 stringClass = jserver_getJavaStringClass();
	 if(stringClass == NULL)
		return NULL;

	 /* Build a new object array.  Params are: env, length, class type of the 
		 array, initial object(?).  */
	 argsStringArray = env->NewObjectArray((argc-1), stringClass, NULL);

	 if(argsStringArray == NULL)
		return NULL;

	 /* Put all args from argv, after the first (which is this program's
		 name) into the array of Strings for FortranMain.  The args array
		 for Java does not include the program name.  */
	 for(i = 1; i < argc; i++)
 	   env->SetObjectArrayElement(argsStringArray, (jsize)i-1,
						jserver_getJavaString(argv[i]));

	 return argsStringArray;
}


#if 0
static jchar *
convertToJChar(const char *c_string)
{
	 int i;
	 int len;
	 jchar *jchar_string;

	 len = strlen(c_string);
	 jchar_string = malloc(sizeof(jchar)*len);

	 for(i = 0; i < len; i++)
		jchar_string[i] = (jchar)(*(c_string+i));

	 return jchar_string;
}

#endif

//static jclass 
jclass 
jserver_getJavaStringClass()
{
      JNIEnv * env = get_env();
       return env->FindClass("java/lang/String");
}

